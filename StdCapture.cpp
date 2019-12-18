#include "StdCapture.h"

#ifdef _MSC_VER
#include <io.h>
#else
#include <unistd.h>
#include <thread>
#endif
#include <fcntl.h>
#include <stdio.h>
#include <iostream>

// actually define vars.
int StdCapture::m_pipe[2];
int StdCapture::m_oldStdOut;
int StdCapture::m_oldStdErr;
bool StdCapture::m_capturing;
std::mutex StdCapture::m_mutex;
std::string StdCapture::m_captured;
const int StdCapture::m_sleepTime = 10;

//-----------------------------------------------------------------------------------------------------------------------------------
void StdCapture::init(void)
//-----------------------------------------------------------------------------------------------------------------------------------
{
    // make stdout and stderr stream unbuffered
    // so that we don't need to flush the streams
    // before capture and after capture 
    // (fflush can cause a deadlock if the stream is currently being 
    std::lock_guard<std::mutex> lock(m_mutex);
    setvbuf(stdout, nullptr, _IONBF, 0);
    setvbuf(stderr, nullptr, _IONBF, 0);
}

//-----------------------------------------------------------------------------------------------------------------------------------
void StdCapture::beginCapture(const int& p_bits)
//-----------------------------------------------------------------------------------------------------------------------------------
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_capturing)
        return;

    secure_pipe(m_pipe, p_bits);
    m_oldStdOut = secure_dup(stdout_fileno());
    m_oldStdErr = secure_dup(stderr_fileno());
    secure_dup2(m_pipe[WRITE], stdout_fileno());
    secure_dup2(m_pipe[WRITE], stderr_fileno());
    m_capturing = true;
#ifndef _MSC_VER
    secure_close(m_pipe[WRITE]);
#endif
}

//-----------------------------------------------------------------------------------------------------------------------------------
bool StdCapture::isCapturing(void)
//-----------------------------------------------------------------------------------------------------------------------------------
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_capturing;
}

//-----------------------------------------------------------------------------------------------------------------------------------
void StdCapture::endCapture(void)
//-----------------------------------------------------------------------------------------------------------------------------------
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_capturing)
        return;

    m_captured.clear();
    secure_dup2(m_oldStdOut, stdout_fileno());
    secure_dup2(m_oldStdErr, stderr_fileno());

    const int bufSize = 1025;
    char buf[bufSize];
    int bytesRead = 0;
    bool fd_blocked(false);
    do
    {
        bytesRead = 0;
        fd_blocked = false;
#ifdef _MSC_VER
        if (!_eof(m_pipe[READ]))
            bytesRead = _read(m_pipe[READ], buf, bufSize - 1);
#else
        bytesRead = read(m_pipe[READ], buf, bufSize - 1);
#endif
        if (bytesRead > 0)
        {
            buf[bytesRead] = 0;
            m_captured += buf;
        }
        else if (bytesRead < 0)
        {
            fd_blocked = (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR);
            if (fd_blocked)
                std::this_thread::sleep_for(std::chrono::milliseconds(m_sleepTime));
        }
    } while (fd_blocked || bytesRead == (bufSize - 1));

    secure_close(m_oldStdOut);
    secure_close(m_oldStdErr);
    secure_close(m_pipe[READ]);
#ifdef _MSC_VER
    secure_close(m_pipe[WRITE]);
#endif
    m_capturing = false;
}

//-----------------------------------------------------------------------------------------------------------------------------------
std::string StdCapture::capture(void)
//-----------------------------------------------------------------------------------------------------------------------------------
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_captured;
}

//-----------------------------------------------------------------------------------------------------------------------------------
int StdCapture::secure_dup(int p_src)
//-----------------------------------------------------------------------------------------------------------------------------------
{
    int ret = -1;
    bool fd_blocked = false;
    do
    {
#ifdef _MSC_VER
        ret = _dup(p_src);
#else
        ret = dup(p_src);
#endif
        fd_blocked = (errno == EINTR || errno == EBUSY);
        if (fd_blocked)
            std::this_thread::sleep_for(std::chrono::milliseconds(m_sleepTime));
    } while (ret < 0);
    return ret;
}

//-----------------------------------------------------------------------------------------------------------------------------------
void StdCapture::secure_pipe(int* p_pipes, const int& p_bits)
//-----------------------------------------------------------------------------------------------------------------------------------
{
    int ret = -1;
    bool fd_blocked = false;
    do
    {
#ifdef _MSC_VER
        ret = _pipe(p_pipes, p_bits, O_BINARY);
#else
        ret = pipe(p_pipes) == -1;
#ifdef linux
        fcntl(p_pipes[WRITE], F_SETPIPE_SZ, p_bits);
#else
        (void)p_bits; // nothing to do on macOS
#endif
#endif
        fd_blocked = (errno == EINTR || errno == EBUSY);
        if (fd_blocked)
            std::this_thread::sleep_for(std::chrono::milliseconds(m_sleepTime));
    } while (ret < 0);
}

//-----------------------------------------------------------------------------------------------------------------------------------
void StdCapture::secure_dup2(int p_src, int p_dest)
//-----------------------------------------------------------------------------------------------------------------------------------
{
    int ret = -1;
    bool fd_blocked = false;
    do
    {
#ifdef _MSC_VER
        ret = _dup2(p_src, p_dest);
#else
        ret = dup2(p_src, p_dest);
#endif
        fd_blocked = (errno == EINTR || errno == EBUSY);
        if (fd_blocked)
            std::this_thread::sleep_for(std::chrono::milliseconds(m_sleepTime));
    } while (ret < 0);
}

//-----------------------------------------------------------------------------------------------------------------------------------
void StdCapture::secure_close(int& p_fd)
//-----------------------------------------------------------------------------------------------------------------------------------
{
    int ret = -1;
    bool fd_blocked = false;
    do
    {
#ifdef _MSC_VER
        ret = _close(p_fd);
#else
        ret = close(p_fd);
#endif
        fd_blocked = (errno == EINTR);
        if (fd_blocked)
            std::this_thread::sleep_for(std::chrono::milliseconds(m_sleepTime));
    } while (ret < 0);

    p_fd = -1;
}

//-----------------------------------------------------------------------------------------------------------------------------------
int StdCapture::std_fileno(FILE* p_fileno)
//-----------------------------------------------------------------------------------------------------------------------------------
{
#ifdef _MSC_VER
    return _fileno(p_fileno);
#else
    return fileno(p_fileno);
#endif
}

//-----------------------------------------------------------------------------------------------------------------------------------
int StdCapture::stdout_fileno(void)
//-----------------------------------------------------------------------------------------------------------------------------------
{
    return std_fileno(stdout);
}

//-----------------------------------------------------------------------------------------------------------------------------------
int StdCapture::stderr_fileno(void)
//-----------------------------------------------------------------------------------------------------------------------------------
{
    return std_fileno(stderr);
}
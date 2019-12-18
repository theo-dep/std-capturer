#pragma once

#include <mutex>

/*!
* \class StdCapture
* \brief Static class to pipe stdout and stderr to a string buffer
*/
class StdCapture
{
public:
    /*!
    * \brief init the class (to do one time, in the constructor for example)
    */
    static void init(void);

    /*!
    * \brief Begin the capture (to do before stdout or stderr writing)
    * \param p_bits: the size of the pipe's buffer. If the app freeze, increase the size.
    */
    static void beginCapture(const int& p_bits = 2048);

    /*!
    * \brief Check if a capture has began
    * \return the result
    */
    static bool isCapturing(void);

    /*!
    * \brief Stop the capture and flush the pipe's buffer to the class capture buffer
    */
    static void endCapture(void);

    /*
    * \brief Get the class capture buffer (after capture ending)
    * \return the string which contains the stdout and stderr writing
    */
    static std::string capture(void);

protected:
    enum PIPES { READ, WRITE }; //<! enum to in/out pipe

    /*!
    * \brief Make a dup with thread concurency
    * \param p_src: a file numero
    * \return the old descriptor
    */
    static int secure_dup(int p_src);

    /*!
    * \brief Make a pipe with thread concurency
    * \param p_pipes: a two int table
    * \param p_bits: the size of the pipe's buffer
    */
    static void secure_pipe(int* p_pipes, const int& p_bits = 2048);

    /*!
    * \brief Make a dup2 with thread concurency
    * \param p_src: a file numero
    * \param p_dest: the new file numero
    */
    static void secure_dup2(int p_src, int p_dest);

    /*!
    * \brief Make a close with thread concurency
    * \param p_fd: a file numero
    */
    static void secure_close(int& p_fd);

    /*!
    * \brief Get the file numero of stdout/stderr
    * \param p_fileno: the file numero
    * \return the file numero
    */
    ///{@
    static int std_fileno(FILE* p_fileno);
    static int stdout_fileno(void);
    static int stderr_fileno(void);
    ///@}

    static int         m_pipe[2];   //<! the pipe file descriptors
    static int         m_oldStdOut; //<! the old stdout file descriptor (to be resetted)
    static int         m_oldStdErr; //<! the old stderr file descriptor (to be resetted)
    static bool        m_capturing; //<! the current capturing status
    static std::mutex  m_mutex;     //<! the thread mutex
    static std::string m_captured;  //<! the string after capturing
    static const int   m_sleepTime; //<! the sleep time to wait if a secure function fails

private:
    StdCapture(void) = delete;
    StdCapture(const StdCapture&) = delete;
    StdCapture& operator=(const StdCapture&) = delete;
};

#include "StdCapture.h"

#include <iostream>

int main(void)
{
    StdCapture::init();
    StdCapture::beginCapture();

    if (!StdCapture::isCapturing())
        return EXIT_FAILURE;

    std::cout << "This message goes to stdout" << std::endl;
    std::cerr << "This message goes to stderr" << std::endl;

    StdCapture::endCapture();

    std::string capture = StdCapture::capture();

    if (capture != "This message goes to stdout\nThis message goes to stderr\n")
        return EXIT_FAILURE;

    std::cout << "Success!" << std::endl;

    return EXIT_SUCCESS;
}

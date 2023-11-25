//////////////////////////////////////////////////////////////////////////////
//
//  Check.h
//
//  A simple C-preprocessor macro for verifying a system call.s success.
//    If the system call returns a minus one (-1) value to indicate an
//    error state, this macro will print a simple diagnostic message
//    and exit the application

#ifndef __CHECK_H__
#define __CHECK_H__

extern "C" {

#include <stdio.h>
#include <stdlib.h>

#define CHECK(f) \
    do { if ((int)(f) < 0) {\
        char msg[1024];\
        sprintf(msg, "[%s:%d] " #f " failed", __FILE__, __LINE__);\
        perror(msg);\
        exit(EXIT_FAILURE);\
    }} while(0)

}

#endif // __CHECK_H__

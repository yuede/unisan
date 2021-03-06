//===------------------------- cxa_exception.hpp --------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//  
//  This file implements the "Exception Handling APIs"
//  http://mentorembedded.github.io/cxx-abi/abi-eh.html
//  
//===----------------------------------------------------------------------===//

#ifndef _CXA_EXCEPTION_H
#define _CXA_EXCEPTION_H

#include <exception> // for std::unexpected_handler and std::terminate_handler
#include <cxxabi.h>
#include "unwind.h"

namespace __cxxabiv1 {

#pragma GCC visibility push(hidden)

static const uint64_t kOurExceptionClass          = 0x434C4E47432B2B00; // CLNGC++\0
static const uint64_t kOurDependentExceptionClass = 0x434C4E47432B2B01; // CLNGC++\1
static const uint64_t get_vendor_and_language     = 0xFFFFFFFFFFFFFF00; // mask for CLNGC++

struct __cxa_exception {
#if defined(__LP64__) || LIBCXXABI_ARM_EHABI
    // This is a new field to support C++ 0x exception_ptr.
    // For binary compatibility it is at the start of this
    // struct which is prepended to the object thrown in
    // __cxa_allocate_exception.
    size_t referenceCount;
#endif

    //  Manage the exception object itself.
    std::type_info *exceptionType;
    void (*exceptionDestructor)(void *);
    std::unexpected_handler unexpectedHandler;
    std::terminate_handler  terminateHandler;

    __cxa_exception *nextException;

    int handlerCount;

#if LIBCXXABI_ARM_EHABI
    __cxa_exception* nextPropagatingException;
    int propagationCount;
#else
    int handlerSwitchValue;
    const unsigned char *actionRecord;
    const unsigned char *languageSpecificData;
    void *catchTemp;
    void *adjustedPtr;
#endif

#if !defined(__LP64__) && !LIBCXXABI_ARM_EHABI
    // This is a new field to support C++ 0x exception_ptr.
    // For binary compatibility it is placed where the compiler
    // previously adding padded to 64-bit align unwindHeader.
    size_t referenceCount;
#endif

    _Unwind_Exception unwindHeader;
};

// http://sourcery.mentor.com/archives/cxx-abi-dev/msg01924.html
// The layout of this structure MUST match the layout of __cxa_exception, with
// primaryException instead of referenceCount.
struct __cxa_dependent_exception {
#if defined(__LP64__) || LIBCXXABI_ARM_EHABI
    void* primaryException;
#endif

    std::type_info *exceptionType;
    void (*exceptionDestructor)(void *);
    std::unexpected_handler unexpectedHandler;
    std::terminate_handler terminateHandler;

    __cxa_exception *nextException;

    int handlerCount;

#if LIBCXXABI_ARM_EHABI
    __cxa_exception* nextPropagatingException;
    int propagationCount;
#else
    int handlerSwitchValue;
    const unsigned char *actionRecord;
    const unsigned char *languageSpecificData;
    void * catchTemp;
    void *adjustedPtr;
#endif

#if !defined(__LP64__) && !LIBCXXABI_ARM_EHABI
    void* primaryException;
#endif

    _Unwind_Exception unwindHeader;
};

struct __cxa_eh_globals {
    __cxa_exception *   caughtExceptions;
    unsigned int        uncaughtExceptions;
#if LIBCXXABI_ARM_EHABI
    __cxa_exception* propagatingExceptions;
#endif
};

#pragma GCC visibility pop
#pragma GCC visibility push(default)

extern "C" __cxa_eh_globals * __cxa_get_globals      ();
extern "C" __cxa_eh_globals * __cxa_get_globals_fast ();

extern "C" void * __cxa_allocate_dependent_exception ();
extern "C" void __cxa_free_dependent_exception (void * dependent_exception);

#pragma GCC visibility pop

}  // namespace __cxxabiv1

#endif  // _CXA_EXCEPTION_H

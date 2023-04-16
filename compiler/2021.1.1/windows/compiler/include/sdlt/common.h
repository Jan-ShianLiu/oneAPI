//
// Copyright (C) 2012-2016 Intel Corporation. All Rights Reserved.
//
// The source code contained or described herein and all documents related
// to the source code ("Material") are owned by Intel Corporation or its
// suppliers or licensors.  Title to the Material remains with Intel
// Corporation or its suppliers and licensors.  The Material is protected
// by worldwide copyright laws and treaty provisions.  No part of the
// Material may be used, copied, reproduced, modified, published, uploaded,
// posted, transmitted, distributed, or disclosed in any way without
// Intel's prior express written permission.
//
// No license under any patent, copyright, trade secret or other intellectual
// property right is granted to or conferred upon you by disclosure or
// delivery of the Materials, either expressly, by implication, inducement,
// estoppel or otherwise. Any license under such intellectual property rights
// must be express and approved by Intel in writing.
//
// Unless otherwise agreed by Intel in writing, you may not remove or alter
// this notice or any other notice embedded in Materials by Intel or Intel's
// suppliers or licensors in any way.
//

#ifndef SDLT_COMMON_H
#define SDLT_COMMON_H

#include <cassert>
#include <cstddef>

#define __SDLT_VERSION_MAJOR              2
#define __SDLT_VERSION_MINOR              6
#define __SDLT_VERSION                    __SDLT_VERSION_MAJOR * 1000 + __SDLT_VERSION_MINOR

// Namespace __SDLT_IMPL should be <= the __SDLT_VERSION
#define __SDLT_IMPL v2_6

#ifndef SDLT_DEBUG
    #ifdef NDEBUG
        #define SDLT_DEBUG 0
    #else
        #ifdef _DEBUG
            #define SDLT_DEBUG _DEBUG
        #else
            #define SDLT_DEBUG 0
        #endif
    #endif
#endif // SDLT_DEBUG

#ifndef __SDLT_ON_WINDOWS
    #if defined(_WIN32) || defined(_WIN64)
        #define __SDLT_ON_WINDOWS 1
    #else
        #define __SDLT_ON_WINDOWS 0
    #endif // _WIN32 || _WIN64
#endif // __SDLT_ON_WINDOWS

#ifndef SDLT_ON_LINUX
    #if defined(__linux__) && (__linux__ == 1)
        #define SDLT_ON_LINUX 1
    #else
        #define SDLT_ON_LINUX 0
    #endif
#endif //SDLT_ON_LINUX

// Verify compiler supports c++11 features
#ifdef __INTEL_COMPILER
    #ifndef __INTEL_CXX11_MODE__
        #if __SDLT_ON_WINDOWS
            #error SDLT requires C++11 be enabled for the compiler, add the command line option /Qstd=c++11
        #else
            #error SDLT requires C++11 be enabled for the compiler, add the command line option -std=c++11
        #endif // __SDLT_ON_WINDOWS
    #endif // __INTEL_CXX11_MODE__
#endif // __INTEL_COMPILER

#if __SDLT_ON_WINDOWS
    // Windows headers like to define macros that interfere with std::min and std::max 
    // or any scope's min or max functions
    // By defining NOMINMAX those macro definitions should be avoided
    #ifndef __SDLT_ALLOW_MINMAX_ON_WINDOWS
        #ifndef NOMINMAX
            #define NOMINMAX
        #endif

        // if max is a macro it interferes with max as a pragma parameter or std::max
        #ifdef max
            #undef max
        #endif

        // if min is a macro it interferes with min as a pragma parameter or std::min
        #ifdef min
            #undef min
        #endif
    #endif

    #define __SDLT_MAYBE_UNUSED
#endif

#ifdef SDLT_ON_LINUX
    //#define __SDLT_MAYBE_UNUSED __attribute__((unused))
    #define __SDLT_MAYBE_UNUSED
#endif

#ifndef __SDLT_ON_64BIT
    #if defined(__x86_64__) || defined(_WIN64)
        #define __SDLT_ON_64BIT 1
    #else
        #define __SDLT_ON_64BIT 0
    #endif // __x86_64__ || _WIN64
#endif // __SDLT_ON_64BIT

#if SDLT_DEBUG
    #ifndef __SDLT_ASSERT
        #define __SDLT_ASSERT(expression) assert(expression)
    #endif
    #ifndef __SDLT_VERIFY
        #define __SDLT_VERIFY(expression) assert(expression)
    #endif
    #ifndef __SDLT_REQUIRE
        #define __SDLT_REQUIRE(expression) assert(expression)
    #endif
    #ifndef SDLT_INLINE
        #define SDLT_INLINE inline
    #endif


#else
    #ifndef __SDLT_ASSERT
        #define __SDLT_ASSERT(expression)
    #endif
    #ifndef __SDLT_VERIFY
        #define __SDLT_VERIFY(expression) (expression)
    #endif
    #ifndef __SDLT_REQUIRE
        #define __SDLT_REQUIRE(expression) assert(expression)
    #endif
    #ifndef SDLT_INLINE
        #if __INTEL_COMPILER >= 1100 || __SDLT_ON_WINDOWS
            #define SDLT_INLINE __forceinline
        #else
            //#define SDLT_INLINE inline
            #define SDLT_INLINE inline __attribute__((always_inline))
        #endif
    #endif

#endif

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace {

    // Helper meant for only __SDLT_UNUSED_VAR to call
    template <typename T>
    SDLT_INLINE
    void no_op(const T & a_variable)
    {
        (void)a_variable;
    }

} // namespace
} // internal
} // __SDLT_IMPL
} // namespace sdlt

#ifndef SDLT_BRANCH
    #define SDLT_BRANCH(expression) __builtin_expect(expression, 0)
#endif

#ifndef SDLT_SKIP_MASKING
    #define SDLT_SKIP_MASKING(expression) __builtin_expect(expression, 1)
#endif

// Handy to have the actual assertion emitted as part of the message as it can
// contain the types involved, if those types are templatized, consider using
// internal::indirect_assertion
#define __SDLT_STATIC_ASSERT(ASSERTION, MESSAGE) \
static_assert(ASSERTION, "("#ASSERTION ", \"" MESSAGE "\")")

#define __SDLT_EXPAND_TOKEN(token) token

#define __SDLT_QUOTE_TOKEN(token) #token
#define __SDLT_QUOTE_TOKEN_INDIRECT(s) __SDLT_QUOTE_TOKEN(s)

#define __SDLT_CONCATENATE_TOKENS(LEFT, RIGHT) LEFT##RIGHT
#define __SDLT_CONCATENATE_TOKENS_INDIRECT(LEFT, RIGHT) __SDLT_CONCATENATE_TOKENS(LEFT, RIGHT)

// Utilize our own offset of calculation vs. offetof or __builtin_offsetof
// to avoid Intel(r) C++ compiler warning 1875 about calculating offsetof on
// non-pod data types.  However these types are still trivial and offsetof
// should be perfectly fine and legal
#ifdef __INTEL_COMPILER
#define __SDLT_OFFSETOF(TYPE, MEMBER) ( ((char *)(&(static_cast<TYPE *>(nullptr)->MEMBER) )) - static_cast<char *>(nullptr))
#else
#define __SDLT_OFFSETOF(TYPE, MEMBER) offsetof(TYPE, MEMBER)
#endif

#define __SDLT_ALIGN(BYTE_BOUNDARY) alignas(BYTE_BOUNDARY)

// Supports Intel compiler 12 (experimental support for 11.1)
#if __INTEL_COMPILER >= 1100
        #define __SDLT_ASSUME(aCondition) __assume(aCondition)
        #define __SDLT_ASSUME_CACHE_LINE_ALIGNED(pointer) __assume_aligned(pointer, 64)
        #define SDLT_INTEL_PRAGMA(aQuotedPragma) _Pragma(aQuotedPragma)
    #if __INTEL_COMPILER >= 1200
        // ICC version 12 specific features

        #define SDLT_INLINE_BLOCK \
            _Pragma("forceinline recursive")
    #else

        #define SDLT_INLINE_BLOCK
    #endif
    // ICC common features
    #define SDLT_NO_VECTOR \
            _Pragma("novector")

    #define __SDLT_MAX_LOOP_COUNT(maxIterationCount) \
        _Pragma(__SDLT_QUOTE_TOKEN(loop_count max(maxIterationCount)))

    #ifdef __GNUC__
        // icc: warning #2196:  routine is both "inline" and "noinline" ("noinline" assumed)
        // when template methods defined in the class body are attributed as noinline
        // choose to keep simplicity of defining methods in the body versus pulling them out
        // so choose to disable the warning
        #pragma warning (disable:2196)
    #endif


        #if __INTEL_COMPILER < 1600
                #define __SDLT_VARIADIC_TEMPLATE_CALL_SITE_BEGIN _Pragma("warning(push)") _Pragma("warning(disable:869)")
                #define __SDLT_VARIADIC_TEMPLATE_CALL_SITE_END _Pragma("warning(pop)")

        #else
                #define __SDLT_VARIADIC_TEMPLATE_CALL_SITE_BEGIN
                #define __SDLT_VARIADIC_TEMPLATE_CALL_SITE_END

        #endif

        #define __SDLT_FLOATING_POINT_EQUALITY_BEGIN _Pragma("warning(push)") _Pragma("warning(disable:1572)")
        #define __SDLT_FLOATING_POINT_EQUALITY_END _Pragma("warning(pop)")

        #define __SDLT_INHERIT_IMPL_BEGIN _Pragma("warning(push)")
        #define __SDLT_INHERIT_IMPL_DESTRUCTOR _Pragma("warning(disable:444)")
        #define __SDLT_INHERIT_IMPL_END _Pragma("warning(pop)")

        #define __SDLT_ALWAYS_EVALUATE_BOTH_ARGUMENTS_BEGIN
        #define __SDLT_ALWAYS_EVALUATE_BOTH_ARGUMENTS_END

        #define __SDLT_UNITIALIZED_POD_BEGIN
        #define __SDLT_UNITIALIZED_POD_END

    //#define SDLT_RESTRICT restrict
    #define SDLT_RESTRICT 

#elif defined(__clang__)
    #define __SDLT_ASSUME(aCondition) if (!(aCondition)) __builtin_unreachable()
    #define __SDLT_ASSUME_CACHE_LINE_ALIGNED(pointer)
    #define SDLT_INTEL_PRAGMA(aQuotedPragma)

    // Non Intel compilers (add pragmas if they exist here)
    #define SDLT_NO_VECTOR
    #define SDLT_INLINE_BLOCK

    #define __SDLT_VARIADIC_TEMPLATE_CALL_SITE_BEGIN
    #define __SDLT_VARIADIC_TEMPLATE_CALL_SITE_END

    #define __SDLT_FLOATING_POINT_EQUALITY_BEGIN
    #define __SDLT_FLOATING_POINT_EQUALITY_END

    #define __SDLT_INHERIT_IMPL_BEGIN _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Weffc++\"")

    #define __SDLT_INHERIT_IMPL_DESTRUCTOR
    #define __SDLT_INHERIT_IMPL_END _Pragma("GCC diagnostic pop")

    #define __SDLT_ALWAYS_EVALUATE_BOTH_ARGUMENTS_BEGIN _Pragma("GCC diagnostic push")
    #define __SDLT_ALWAYS_EVALUATE_BOTH_ARGUMENTS_END _Pragma("GCC diagnostic pop")

    #define __SDLT_UNITIALIZED_POD_BEGIN _Pragma("GCC diagnostic push")
    #define __SDLT_UNITIALIZED_POD_END _Pragma("GCC diagnostic pop")

    #define SDLT_RESTRICT __restrict
#else
    #define __SDLT_ASSUME(aCondition) if (!(aCondition)) __builtin_unreachable()
    #define __SDLT_ASSUME_CACHE_LINE_ALIGNED(pointer)
    #define SDLT_INTEL_PRAGMA(aQuotedPragma)

    // Non Intel compilers (add pragmas if they exist here)
    #define SDLT_NO_VECTOR
    #define SDLT_INLINE_BLOCK

    #define __SDLT_VARIADIC_TEMPLATE_CALL_SITE_BEGIN
    #define __SDLT_VARIADIC_TEMPLATE_CALL_SITE_END

    #define __SDLT_FLOATING_POINT_EQUALITY_BEGIN
    #define __SDLT_FLOATING_POINT_EQUALITY_END

    #define __SDLT_INHERIT_IMPL_BEGIN _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Weffc++\"")

    #define __SDLT_INHERIT_IMPL_DESTRUCTOR
    #define __SDLT_INHERIT_IMPL_END _Pragma("GCC diagnostic pop")

    #define __SDLT_ALWAYS_EVALUATE_BOTH_ARGUMENTS_BEGIN _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
    #define __SDLT_ALWAYS_EVALUATE_BOTH_ARGUMENTS_END _Pragma("GCC diagnostic pop")

    #define __SDLT_UNITIALIZED_POD_BEGIN _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
    #define __SDLT_UNITIALIZED_POD_END _Pragma("GCC diagnostic pop")

    #define SDLT_RESTRICT __restrict

#endif



// without noinline some compilers tend to inline too much from outside the SIMD loop
// so purposely introduce a noinline function to prevent simple array accesses with
// complex data accessess from outside the loop.
// Also creates a nice starting point for the compiler for register assignments
// giving it as many as possible
#ifdef __GNUC__
    #define SDLT_NOINLINE __attribute__ ((noinline))
#else
    #define SDLT_NOINLINE __declspec(noinline)
#endif

#if SDLT_DEBUG
    #define __SDLT_DEBUG_ONLY(CODE) CODE
#else
    #define __SDLT_DEBUG_ONLY(CODE) /* CODE */
#endif

// Use for to consider parameters unused and avoid the warnings
// Appears to work on most platforms
#define __SDLT_UNUSED_PARAM(NAME) (void)(NAME)

// ICC doesn't want to consider local variables as unused just
// because they were cast to void.  So we need to call
// an inlined no-op function
#define __SDLT_UNUSED_VAR(NAME) sdlt::__SDLT_IMPL::internal::no_op(NAME)

#if SDLT_DEBUG
    // Dont force vectorization if we are in debug mode
    #define __SDLT_INTERNAL_SIMD_LOOP_OPTIONS \
            SDLT_INTEL_PRAGMA("loop_count min(_sdlt_simdlane_count), max(_sdlt_simdlane_count), avg(_sdlt_simdlane_count)")
#else
    #define __SDLT_INTERNAL_SIMD_LOOP_OPTIONS \
            SDLT_INTEL_PRAGMA("ivdep")   \
            SDLT_INTEL_PRAGMA("simd assert vectorlength(_sdlt_simdlane_count)") \
            SDLT_INTEL_PRAGMA("loop_count min(_sdlt_simdlane_count), max(_sdlt_simdlane_count), avg(_sdlt_simdlane_count)")
#endif

#if __SDLT_ON_WINDOWS
    #define __SDLT_FUNCTION_SIG __FUNCSIG__
#else
    #define __SDLT_FUNCTION_SIG __PRETTY_FUNCTION__
#endif // __SDLT_ON_WINDOWS

// Code level macros that declare local variables avoid interaction with
// variables in the outer scope by prefixing _sdlt_
#define SDLT_SIMD_LOOP(INDEX_NAME, START_AT, END_BEFORE, SIMD_LANE_COUNT) \
    SDLT_INLINE_BLOCK    \
    {   \
        static const int _sdlt_simdlane_count = (SIMD_LANE_COUNT);    \
        const int _sdlt_start_at = (START_AT);    \
        const int _sdlt_end_before = (END_BEFORE);    \
        __SDLT_ASSERT(_sdlt_start_at%_sdlt_simdlane_count == 0);   \
        __SDLT_ASSERT(_sdlt_end_before%_sdlt_simdlane_count == 0); \
        \
        const int _sdlt_start_at_block = _sdlt_start_at/_sdlt_simdlane_count;  \
        const int _sdlt_end_before_block = _sdlt_end_before/_sdlt_simdlane_count;  \
        SDLT_NO_VECTOR \
        for(int _sdlt_simd_block_index=_sdlt_start_at_block; _sdlt_simd_block_index < _sdlt_end_before_block; ++_sdlt_simd_block_index) \
        {   \
            __SDLT_INTERNAL_SIMD_LOOP_OPTIONS \
            for(int _sdlt_simdlane_index = 0; _sdlt_simdlane_index < _sdlt_simdlane_count; ++_sdlt_simdlane_index)  \
            {   \
                const sdlt::simd_index<_sdlt_simdlane_count> INDEX_NAME(_sdlt_simd_block_index, _sdlt_simdlane_index);

#define SDLT_END_SIMD_LOOP   \
            }   \
        }   \
    }

#define SDLT_SIMD_LOOP_BEGIN(INDEX_NAME, START_AT, END_BEFORE, SIMD_LANE_COUNT) SDLT_SIMD_LOOP(INDEX_NAME, START_AT, END_BEFORE, SIMD_LANE_COUNT)
#define SDLT_SIMD_LOOP_END   SDLT_END_SIMD_LOOP

#endif // SDLT_COMMON_H

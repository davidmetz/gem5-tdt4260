/*
 * Copyright (c) 2012,2017-2018 ARM Limited
 * All rights reserved
 *
 * The license below extends only to copyright in the software and shall
 * not be construed as granting a license to any other intellectual
 * property including but not limited to intellectual property relating
 * to a hardware implementation of the functionality of the software
 * licensed hereunder.  You may use the software subject to the license
 * terms below provided that you ensure that this notice is replicated
 * unmodified and in its entirety in all distributions of the software,
 * modified or unmodified, in source code or in binary form.
 *
 * Copyright (c) 2006 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __BASE_COMPILER_HH__
#define __BASE_COMPILER_HH__

#include <memory>

// http://gcc.gnu.org/onlinedocs/gcc/Function-Attributes.html


/*
 * Attributes that become standard in later versions of c++.
 */

// Use GEM5_FALLTHROUGH to mark when you're intentionally falling through from
// one case to another in a switch statement.
#if __has_cpp_attribute(fallthrough) // Standard in c++17.
#  define GEM5_FALLTHROUGH [[fallthrough]]
#else
// Not supported, so it's not necessary to avoid warnings.
#  define GEM5_FALLTHROUGH
#endif

// When the return value of a function should not be discarded, mark it with
// GEM5_NO_DISCARD.
#if __has_cpp_attribute(nodiscard) // Standard in c++17, with message in c++20.
#  define GEM5_NO_DISCARD [[nodiscard]]
#else
// Not supported, but it's optional so we can just omit it.
#  define GEM5_NO_DISCARD
#endif

// When a variable may purposefully not be used, for instance if it's only used
// in debug statements which might be disabled, mark it with GEM5_VAR_USED.
#if __has_cpp_attribute(maybe_unused) // Standard in c++17.
#  define GEM5_VAR_USED [[maybe_unused]]
#elif defined(__GNUC__)
// gcc and clang support a custom attribute which is essentially the same
// thing.
#  define GEM5_VAR_USED [[gnu::unused]]
#else
#  error "Don't know what to do for your compiler."
#endif


/*
 * Compiler specific features.
 */

#if defined(__GNUC__) // clang or gcc.
// Mark a structure as packed, so that no padding is added to its layout. This
// padding might be added to, for instance, ensure certain fields have certain
// alignment.
#  define GEM5_PACKED [[gnu::packed]]

// Prevent a function from being inlined.
#  define GEM5_NO_INLINE [[gnu::noinline]]

// Set the visibility of a symbol.
#  define GEM5_PUBLIC [[gnu:visibility("default")]]
#  define GEM5_LOCAL [[gnu::visibility("hidden")]]
#  define GEM5_WEAK [[gnu::weak]]

// Force an alignment for a variable.
#  define GEM5_ALIGNED(alignment) [[gnu::aligned(alignment)]]

// Marker for what should be an unreachable point in the code.
#  define GEM5_UNREACHABLE __builtin_unreachable()

// To mark a branch condition as likely taken, wrap it's condition with
// GEM5_LIKELY. To mark it as likely not taken, wrap it's condition with
// GEM5_UNLIKELY. These can be replaced with the standard attributes [[likely]]
// and [[unlikely]] in c++20, although the syntax is different enough that
// we can't do that with direct substitution.
#  define GEM5_LIKELY(cond) __builtin_expect(!!(cond), 1)
#  define GEM5_UNLIKELY(cond) __builtin_expect(!!(cond), 0)

// Mark a c++ declaration as deprecated, with a message explaining what to do
// to update to a non-deprecated alternative.
#  define GEM5_DEPRECATED(message) [[gnu::deprecated(message)]]
// Mark a C++ emum value as deprecated, with a message explaining what to do
// to update to a non-deprecated alternative. This wraps GEM5_DEPRECATED but
// is guarded by a preprocessor if directive to ensure it is not included
// when compiled in GCC < 6, as deprecation of enum values was introduced in
// GCC 6. All supported clang compilers allow enum value deprecation.
#  if defined(__clang__) || __GNUC__ >= 6
#    define GEM5_DEPRECATED_ENUM_VAL(message) GEM5_DEPRECATED(message)
#  else
#    define GEM5_DEPRECATED_ENUM_VAL(message)
#  endif
// Mark an expression-like macro as deprecated by wrapping it in some code
// which declares and uses a deprecated variable with the same name as the
// macro. The wrapping macro evaluates to the same thing as the original macro.
// The definition must be an c++ expression and not a statement because of how
// the original macro is wrapped.
#  define GEM5_DEPRECATED_MACRO(name, definition, message) \
     ([](){GEM5_DEPRECATED(message) int name{}; return name;}, (definition))
// This version is for macros which are statement-like, which frequently use
// "do {} while (0)" to make their syntax look more like normal c++ statements.
#  define GEM5_DEPRECATED_MACRO_STMT(name, definition, message) \
     do {{definition;} GEM5_DEPRECATED_MACRO(name, {}, message);} while (0)

// Evaluate an expanded parameter pack in order. Multiple arguments can be
// passed in which be evaluated in order relative to each other as a group.
// The argument(s) must include a parameter pack to expand. This works because
// the elements of a brace inclosed initializer list are evaluated in order,
// as are the arguments to the comma operator, which evaluates to the last
// value. This is compiler specific because it uses variadic macros.
#define GEM5_FOR_EACH_IN_PACK(...) \
do { GEM5_VAR_USED int i[] = { 0, ((void)(__VA_ARGS__), 0)... }; } while (0)

#else
#  error "Don't know what to do for your compiler."
#endif

// When a member variable may be unused, mark it with GEM5_CLASS_VAR_USED. This
// needs to be limitted to clang only since clang warns on these unused
// variables, and g++ will actually warn if you use this attribute since it
// won't do anything there.
#if defined(__clang__) // clang only.
#  define GEM5_CLASS_VAR_USED GEM5_VAR_USED
#else
#  define GEM5_CLASS_VAR_USED
#endif

// Aliases for macros using the deprecated M5 prefix.
#define M5_VAR_USED GEM5_VAR_USED
#define M5_NODISCARD GEM5_NO_DISCARD
#define M5_FALLTHROUGH GEM5_FALLTHROUGH
#define M5_ATTR_PACKED GEM5_PACKED
#define M5_NO_INLINE GEM5_NO_INLINE
#define M5_PUBLIC GEM5_PUBLIC
#define M5_LOCAL GEM5_LOCAL
#define M5_WEAK GEM5_WEAK
#define M5_ALIGNED(x) GEM5_ALIGNED(x)
#define M5_UNREACHABLE GEM5_UNREACHABLE
#define M5_LIKELY(x) GEM5_LIKELY(x)
#define M5_UNLIKELY(x) GEM5_UNLIKELY(x)
#define M5_FOR_EACH_IN_PACK(...) GEM5_FOR_EACH_IN_PACK(__VA_ARGS__)
#define M5_CLASS_VAR_USED GEM5_CLASS_VAR_USED

#endif // __BASE_COMPILER_HH__

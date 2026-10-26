#ifndef SPFLIB_H
#define SPFLIB_H

#if defined(__cplusplus)
#   error "This library does not support C++."
#endif

#if __STDC_VERSION__ < 201112L
#if !defined(SPFLIB_DONT_CARE)
#   pragma message("This library was designed to be used with C11 some features maybe be disabled. To disable this warning define SPFLIB_DONT_CARE.")
#endif
#endif

#if defined(SPFLIB_IMPL)
#   define SPFLIB_STRING_IMPL
#   define SPFLIB_VEC2_IMPL
#   define SPFLIB_VEC3_IMPL
#   define SPFLIB_PRINT_IMPL
#   define SPFLIB_MEMORY_IMPL
#endif

// -- general utils -- //

#include <stddef.h>   // size_t
#include <stdint.h>

// TODO:
// - is nan anf infinity checks
// - string to int, string to float etc
// - window creation (GLFW) and opengl context
// - matrix 3x3
// - good rand num gen
// - customizable lerp
// - profiler
// - platform specific actions like clipboard, opening url etc
// - dynamic array, hashmap, more data structures
// - unit testing

// detect operating system
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#   define OS_WINDOWS
#   define WIN32_LEAN_AND_MEAN
#   if defined(_WIN64)
#       define OS_WINDOWS_64
#   elif !defined(_WIN64) && defined(_WIN32)
#       define OS_WINDOWS_32
#   elif !defined(_WIN64) && !defined(_WIN32) && defined(_WIN16)
#       define OS_WINDOWS_16
#   else
#       error "Unknown Windows version."
#   endif
#elif defined(__linux__)
#	define OS_LINUX
#	define OS_POSIX
#   if defined(__ANDROID__)
#	    define OS_ANDROID
#   endif
#elif defined(__APPLE__)
#	define OS_APPLE
#	define OS_POSIX
#   include TargetConditionals.h
#   if defined(TARGET_OS_MAC)
#       define OS_MACOS
#   elif defined(TARGET_OS_IPHONE) && defined(TARGET_OS_IOS)
#       define OS_IOS
#   elif defined(TARGET_OS_TV) 
#       define OS_APPLE_TV
#   elif defined(TARGET_OS_WATCH) 
#       define OS_APPLE_WATCH
#   else
#       error "Unknown Apple device."
#   endif
#else
#   error "Unknown OS."
#endif

// detect cpu architecture
#if defined(OS_WINDOWS)
#   if defined(_M_X64) || defined(_M_AMD64)
#       define ARCH_x64
#   elif defined(_M_IX86) || defined(_X86_)
#       define ARCH_x86
#   elif defined(_M_ARM64)
#       define ARCH_ARM64
#   elif defined(_M_ARM) || defined(_M_ARMT)
#       define ARCH_ARM86
#   else
#       error "Unknown Windows CPU architecture."
#   endif
#elif defined(OS_LINUX)
#   if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#       define ARCH_x64
#   elif defined(i386) || defined(__i386) || defined(__i386__)
#       define ARCH_x86
#   elif defined(__aarch64__)
#       define ARCH_ARM64
#   elif defined(__arm__)
#       define ARCH_ARM86
#   else
#       error "Unknown Linux CPU architecture."
#   endif
#elif defined(OS_APPLE)
#   if defined(TARGET_CPU_X86_64)
#       define ARCH_x64
#   elif defined(TARGET_CPU_X86)
#       define ARCH_x86
#   elif defined(TARGET_CPU_ARM)
#       define ARCH_ARM86
#   elif defined(TARGET_CPU_ARM64)
#       define ARCH_ARM64
#   else
#       error "Unknown Apple CPU architecture."
#   endif
#endif

// detect compiler
#if defined(__GNUC__) && !defined(__clang__)
#	define COMPILER_GCC
#elif defined(__clang__) && !defined(_MSC_VER)
#	define COMPILER_CLANG
#elif defined(_MSC_VER) && !defined(__clang__)
#	define COMPILER_MSVC
#elif defined(__TINYC__)
#	define COMPILER_TCC
#elif defined(__chibicc__)
#	define COMPILER_CHIBI
#else
#   pragma message("Unkown C compiler.")
#endif

#if defined(__STDC_VERSION__)
#   if __STDC_VERSION__   == 199901L
#       define STDC_VER 99
#   elif __STDC_VERSION__ == 201112L
#       define STDC_VER 11
#   elif __STDC_VERSION__ == 201710L
#       define STDC_VER 17
#   elif __STDC_VERSION__ == 202311L
#       define STDC_VER 23
#   else
#       define STDC_UNKNOWN
#       pragma message("Unkown C standard.")
#   endif
#else
#   error "Unsupported C standard."
#endif

#if defined(BUILD_RELEASE)
#   define NDEBUG
#endif

#if defined(NDEBUG)
#   define BUILD_RELEASE
#else
#   define BUILD_DEBUG
#endif

#if STDC_VER < 23
#	define nullptr (void*)0
#endif

#if !defined(bool) && STDC_VER < 23
#	define bool  _Bool
#	define true  1
#	define false 0
#endif

#define UNUSED(u) (void)u
#define cast(t) (t)
#define fallthrough

#if 0

#if STDC_VER < 11
#   if defined(COMPILER_GCC) || defined(COMPILER_CLANG)
#       define unreachable() (__builtin_unreachable())
#   elif defined(COMPILER_MSVC)
#       define unreachable() (__assume(0))
#   else
#       define unreachable()
#   endif
#elif STDC_VER >= 11 && STDC_VER < 23
#   define unreachable() (unreachable_impl())
#   if defined(SPFLIB_IMPL)
        _Noreturn inline void unreachable_impl() { abort(); }
#   else // defined(SPFLIB_IMPL)
        _Noreturn inline void unreachable_impl();
#   endif
#elif STDC_VER >= 23
#   // stddef header includes unreachable
#endif

#endif

#if STDC_VER >= 11 && STDC_VER < 23
#   if !defined(static_assert)
#       define static_assert  _Static_assert
#   endif
#   if !defined(alignas)
#       define alignas        _Alignas
#   endif
#   if !defined(alignof)
#       define alignof        _Alignof
#   endif
#   if !defined(thread_local)
#       define thread_local   _Thread_local
#   endif
#endif

#if STDC_VER >= 11 && !defined(atomic)
#   define atomic _Atomic
#endif

#if !defined(offsetof)
#   define offsetof(_s, _m) (size_t)&(((_s*)0)->_m)
#endif

#define countof(_arr) (sizeof(_arr) / sizeof(_arr[0]))

#define __fori4(_type, _i, _from, _to) for (_type (_i) = (_from); (_i) _to; (_i)++)
#define __forir4(_type, _i, _from, _to) for (_type (_i) = (_to); (_i)-- > (_from);)

#define __fori_args(_1, _2, _3, _4, x, ...) x
#define fori(...) __fori_args(__VA_ARGS__, __fori4(__VA_ARGS__), __fori4(s64, __VA_ARGS__), __fori4(s64, i, __VA_ARGS__), "Invalid arguments should be fori (type (optional), i (optional), from, (< | <=) to)")
#define fori_rev(...) __fori_args(__VA_ARGS__, __forir4(__VA_ARGS__), __forir4(s64, __VA_ARGS__), __forir4(s64, i, __VA_ARGS__), "Invalid arguments should be fori_r (type (optional), i (optional), from, to)")

typedef uint8_t  u8;
typedef int8_t   s8;

typedef uint16_t u16;
typedef int16_t  s16;

typedef uint32_t u32;
typedef int32_t  s32;

typedef uint64_t u64;
typedef int64_t  s64;

typedef float    f32;
typedef double   f64;

#include <limits.h>

#if 0

#if SIZE_MAX == ULLONG_MAX
    typedef signed long long ssize_t;
#elif SIZE_MAX == ULONG_MAX
    typedef signed long int  ssize_t;
#elif SIZE_MAX == UINT_MAX
    typedef signed int       ssize_t;
#elif SIZE_MAX == USHRT_MAX
    typedef signed short     ssize_t;
#elif SIZE_MAX == UCHAR_MAX
    typedef signed char      ssize_t;
#else
    #error "Unspecified underlying size_t type"
#endif

#endif

// TODO: add nan and inf checks

#define SPF_PI        3.14159265358979323846264338327950288
#define SPF_PI_2      1.57079632679489661923132169163975144
#define SPF_PI_4      0.78539816339744830961566084581987572

#define TO_RADIANS(_ang_deg)  ((_ang_deg) * SPF_PI / 180.)
#define TO_DEGREES(_ang_rad)  ((_ang_rad) * 180. / SPF_PI)

#define KiB(_n)   ((_n) * 1024)
#define MiB(_n)   ((_n) * 1024 * 1024)
#define GiB(_n)   ((_n) * 1024 * 1024 * 1024)
#define TiB(_n)   ((_n) * 1024 * 1024 * 1024 * 1024)

// --- macro utils --- //

// TODO: add INC and DEC macros

#define SPF_PAIR_AT_1(_1, _2) _1
#define SPF_PAIR_AT_2(_1, _2) _2

#define SPF_CHECK_AT_1(_1, _2, ...) _2
#define SPF_CHECK(...) SPF_CHECK_AT_1(__VA_ARGS__,)

#define SPF_EMPTY()

#define SPF_REMOVE_PARENS(...) __VA_ARGS__

#define SPF_EXPAND(...) __VA_ARGS__
// #define SPF_EXPAND(...) SPF_EXPAND_(__VA_ARGS__)

#define SPF_EXPAND_NO_PARENS(...) SPF_EXPAND __VA_ARGS__

#define SPF_CONCAT_(a, b) a ## b
#define SPF_CONCAT(a, b) SPF_CONCAT_(a, b)

// https://gustedt.wordpress.com/2010/06/08/detect-empty-macro-arguments/
#define __SPF_ARG16(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, ...) _15
#define __SPF_HAS_COMMA(...) __SPF_ARG16(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0)
#define __SPF_TRIGGER_PARENTHESIS_(...) ,
 
#define __SPF_ISEMPTY(...)                                                    \
__SPF_ISEMPTY_(                                                               \
          /* test if there is just one argument, eventually an empty    \
             one */                                                     \
          __SPF_HAS_COMMA(__VA_ARGS__),                                       \
          /* test if _TRIGGER_PARENTHESIS_ together with the argument   \
             adds a comma */                                            \
          __SPF_HAS_COMMA(__SPF_TRIGGER_PARENTHESIS_ __VA_ARGS__),                 \
          /* test if the argument together with a parenthesis           \
             adds a comma */                                            \
          __SPF_HAS_COMMA(__VA_ARGS__ (/*empty*/)),                           \
          /* test if placing it between _TRIGGER_PARENTHESIS_ and the   \
             parenthesis adds a comma */                                \
          __SPF_HAS_COMMA(__SPF_TRIGGER_PARENTHESIS_ __VA_ARGS__ (/*empty*/))      \
          )
 
#define __SPF_PASTE5(_0, _1, _2, _3, _4) _0 ## _1 ## _2 ## _3 ## _4
#define __SPF_ISEMPTY_(_0, _1, _2, _3) __SPF_HAS_COMMA(__SPF_PASTE5(__SPF_IS_EMPTY_CASE_, _0, _1, _2, _3))
#define __SPF_IS_EMPTY_CASE_0001 ,
 

// vaopt
#define __SPF_EXPAND_NON_EMPTY_0(...) __VA_ARGS__
#define __SPF_EXPAND_NON_EMPTY_1(...)

#define __SPF_EXPAND_EMPTY_0(...) 
#define __SPF_EXPAND_EMPTY_1(...) __VA_ARGS__

#define VA_OPT_(_e, ...) SPF_CONCAT(__SPF_EXPAND_NON_EMPTY_, __SPF_ISEMPTY(__VA_ARGS__)) (_e)
#define VA_OPT(_e, ...) VA_OPT_(SPF_EXPAND_NO_PARENS(_e), __VA_ARGS__)

#define VA_NOPT_(_e, ...) SPF_CONCAT(__SPF_EXPAND_EMPTY_, __SPF_ISEMPTY(__VA_ARGS__)) (_e)
#define VA_NOPT(_e, ...) VA_NOPT_(SPF_EXPAND_NO_PARENS(_e), __VA_ARGS__)

#define VA_OPT_IF_ELSE(_if, _else, ...) VA_OPT((_if), __VA_ARGS__) VA_NOPT((_else), __VA_ARGS__)
//

#define VA_COUNT_(_64, _63, _62, _61, _60, _59, _58, _57, _56, _55, _54, _53,  \
                   _52, _51, _50, _49, _48, _47, _46, _45, _44, _43, _42, _41, \
                   _40, _39, _38, _37, _36, _35, _34, _33, _32, _31, _30, _29, \
                   _28, _27, _26, _25, _24, _23, _22, _21, _20, _19, _18, _17, \
                   _16, _15, _14, _13, _12, _11, _10, _9, _8, _7, _6, _5, _4,  \
                   _3, _2, _1, n, ...) n
#define VA_COUNT__(_64, _63, _62, _61, _60, _59, _58, _57, _56, _55, _54,      \
                    _53, _52, _51, _50, _49, _48, _47, _46, _45, _44, _43,     \
                    _42, _41, _40, _39, _38, _37, _36, _35, _34, _33, _32,     \
                    _31, _30, _29, _28, _27, _26, _25, _24, _23, _22, _21,     \
                    _20, _19, _18, _17, _16, _15, _14, _13, _12, _11, _10, _9, \
                    _8, _7, _6, _5, _4, _3, _2, _1, n, ...)                    \
  VA_COUNT_(_64, _63, _62, _61, _60, _59, _58, _57, _56, _55, _54, _53, _52,   \
             _51, _50, _49, _48, _47, _46, _45, _44, _43, _42, _41, _40, _39,  \
             _38, _37, _36, _35, _34, _33, _32, _31, _30, _29, _28, _27, _26,  \
             _25, _24, _23, _22, _21, _20, _19, _18, _17, _16, _15, _14, _13,  \
             _12, _11, _10, _9, _8, _7, _6, _5, _4, _3, _2, _1, n,             \
             __VA_ARGS__)
#define VA_COUNT(...)                                                          \
  VA_COUNT__(VA_OPT((__VA_ARGS__, ), __VA_ARGS__) 64, 63, 62,                  \
              61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46,  \
              45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30,  \
              29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14,  \
              13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

// foreach
#define EVAL2(...) __VA_ARGS__
#define EVAL1(...) EVAL2(EVAL2(EVAL2(EVAL2(EVAL2(EVAL2(EVAL2(__VA_ARGS__)))))))
#define EVAL(...)  EVAL1(EVAL1(EVAL1(EVAL1(EVAL1(EVAL1(EVAL1(__VA_ARGS__)))))))

#define __SPF_LOOP_IND() __SPF_LOOP
#define __SPF_LOOP_(f, x, c, ...) SPF_CONCAT(__SPF_LOOP, c) (f, x, __VA_ARGS__)
#define __SPF_LOOP(f, x, ...) __SPF_LOOP_(f, x, VA_NOPT((0), __VA_ARGS__) VA_OPT((1), __VA_ARGS__), __VA_ARGS__)
#define __SPF_LOOP1(f, x, ...) f(x) __SPF_LOOP_IND SPF_EMPTY()() (f, __VA_ARGS__)
#define __SPF_LOOP0(f, x, ...) f(x)

#define SPF_FOREACH(_fn, ...) EVAL(__SPF_LOOP(_fn, __VA_ARGS__))

// foreach arg
#define __SPF_ARG_LOOP_IND() __SPF_ARG_LOOP
#define __SPF_ARG_LOOP_(f, a, x, c, ...) SPF_CONCAT(__SPF_ARG_LOOP, c) (f, a, x, __VA_ARGS__)
#define __SPF_ARG_LOOP(f, a, x, ...) __SPF_ARG_LOOP_(f, a, x, VA_NOPT((0), __VA_ARGS__) VA_OPT((1), __VA_ARGS__), __VA_ARGS__)
#define __SPF_ARG_LOOP1(f, a, x, ...) f(a, x) __SPF_ARG_LOOP_IND SPF_EMPTY()() (f, a, __VA_ARGS__)
#define __SPF_ARG_LOOP0(f, a, x, ...) f(a, x)

#define SPF_FOREACH_ARG(_fn, _arg, ...) EVAL(__SPF_ARG_LOOP(_fn, _arg, __VA_ARGS__))

// foreach 2 args
#define __SPF_ARG2_LOOP_IND() __SPF_ARG2_LOOP
#define __SPF_ARG2_LOOP_(f, a, a2, x, c, ...) SPF_CONCAT(__SPF_ARG2_LOOP, c) (f, a, a2, x, __VA_ARGS__)
#define __SPF_ARG2_LOOP(f, a, a2, x, ...) __SPF_ARG2_LOOP_(f, a, a2, x, VA_NOPT((0), __VA_ARGS__) VA_OPT((1), __VA_ARGS__), __VA_ARGS__)
#define __SPF_ARG2_LOOP1(f, a, a2, x, ...) f(a, a2, x) __SPF_ARG2_LOOP_IND SPF_EMPTY()() (f, a, a2, __VA_ARGS__)
#define __SPF_ARG2_LOOP0(f, a, a2, x, ...) f(a, a2, x)

#define SPF_FOREACH_ARG2(_fn, _arg, _arg2, ...) EVAL(__SPF_ARG2_LOOP(_fn, _arg, _arg2, __VA_ARGS__))

// --- assert --- //
#if !defined(SPFLIB_NO_ASSERT)

#if defined(BUILD_RELEASE) && defined(SPFLIB_ASSERT_IN_RELEASE) || !defined(BUILD_RELEASE)

#include <stdio.h>    // printf
#include <stdlib.h>   // abort

#define SPF_WILL_ASSERT 1

#define SPF_ASSERT(_cond)                                                     \
    do {                                                                      \
        if (!(_cond)) {                                                       \
            printf("Assertion failed in '%s' at '%d'\n", __FILE__, __LINE__); \
            abort();                                                          \
        }                                                                     \
    } while (0)

#else // defined(BUILD_RELEASE) && defined(SPFLIB_ASSERT_IN_RELEASE) || !defined(BUILD_RELEASE)
#   define SPF_ASSERT(_cond)
#endif // defined(BUILD_RELEASE) && defined(SPFLIB_ASSERT_IN_RELEASE) || !defined(BUILD_RELEASE)

#else // defined(SPFLIB_NO_ASSERT)
#   define SPF_ASSERT(_cond)
#endif // !defined(SPFLIB_NO_ASSERT)

// --- type info --- //

// TODO: make type names their actual type without typedef alias

#if defined(SPFLIB_TYPE_INFO)

#if STDC_VER < 11
#   error "Type info is not allowed for C versions below C11."
#endif

/*
-- settings

#define SPFLIB_TYPE_INFO_DISABLE

*/

/*
-- example

STRUCT(Player,
    (s32, health),
    (float, breath),
    (s32 inventory, 5),
    (string, name)
)
*/

typedef struct
{
    struct {
        const char* name;
    } type;

    const char* name;

    size_t size;
    size_t offset;
    size_t arr_size;

    bool is_signed;
    bool is_array;
    bool is_pointer;
    bool is_integral;
    bool is_floating_point;
    bool is_struct;
} FieldInfo;

typedef struct
{
    const char* name;
    FieldInfo fields[64];
    size_t field_count;
} TypeInfo;

#define IS_SIGNED(_type) _Generic(_type,  \
    char              : (((char)-1) < 0), \
    signed char       : 1,                \
    signed short      : 1,                \
    signed int        : 1,                \
    signed long       : 1,                \
    signed long long  : 1,                \
    default           : 0                 \
)

#define IS_UNSIGNED(_type) _Generic(_type,  \
    _Bool               : 1,                \
    char                : (((char)-1) < 0), \
    unsigned char       : 1,                \
    unsigned short      : 1,                \
    unsigned int        : 1,                \
    unsigned long       : 1,                \
    unsigned long long  : 1,                \
    default             : 0                 \
)

#define IS_INTEGRAL(_type) (IS_SIGNED(_type) || IS_UNSIGNED(_type))

#define IS_FLOATING_POINT(_type) _Generic(_type, \
    float  : 1,                                  \
    double : 1,                                  \
    default: 0                                   \
)

#define IS_POINTER(_type) #_type[sizeof(#_type) - 2] == '*'
#define IS_NOT_POINTER(_type) #_type[sizeof(#_type) - 2] != '*'

#define __DECLARE_FIELD2(_type, _name) _type _name;
#define __DECLARE_FIELD3(_type, _name, _arr_sz) _type _name[_arr_sz];

#define __DECLARE_FIELD_ARGS(_1, _2, _3, x, ...) x
#define __DECLARE_FIELD(...) __DECLARE_FIELD_ARGS(__VA_ARGS__, __DECLARE_FIELD3, __DECLARE_FIELD2, ERROR) (__VA_ARGS__)

#define __DECLARE_FIELD_INFO(_struct, _type, _name, ...) \
{ \
    .type = { .name = #_type }, \
    .name = #_name, \
    .size = sizeof(_type), \
    .offset = offsetof(_struct, _name), \
    .arr_size = VA_OPT_IF_ELSE(__VA_ARGS__, 0, __VA_ARGS__), \
    .is_signed = IS_SIGNED(_type), \
    .is_array = VA_OPT_IF_ELSE(1, 0, __VA_ARGS__), \
    .is_pointer = IS_POINTER(_type), \
    .is_integral = IS_INTEGRAL(_type), \
    .is_floating_point = IS_FLOATING_POINT(_type), \
    .is_struct = (!IS_INTEGRAL(_type) && !IS_FLOATING_POINT(_type) && IS_NOT_POINTER(_type) && VA_OPT_IF_ELSE(0, 1, __VA_ARGS__)), \
},

#define __CALL_FN_WITH_ARGS(_fn, _args) _fn _args

#define __OP (
#define __CP )
#define __CALL_FN_WITH_ARGS2(_fn, _extra, _args) _fn __OP _extra, SPF_REMOVE_PARENS _args __CP

#if !defined(SPFLIB_TYPE_INFO_DISABLE)

#define STRUCT(_name, ...) \
    typedef struct \
    { \
        SPF_FOREACH_ARG(__CALL_FN_WITH_ARGS, __DECLARE_FIELD, __VA_ARGS__) \
    } _name; \
    TypeInfo __type_info_##_name = \
    { \
        .name = #_name, \
        .fields = { \
            SPF_FOREACH_ARG2(__CALL_FN_WITH_ARGS2, __DECLARE_FIELD_INFO, _name, __VA_ARGS__) \
        }, \
        .field_count = VA_COUNT(__VA_ARGS__) \
    };

#define type_info(_s)   __type_info_##_s

#else // !defined(SPFLIB_TYPE_INFO_DISABLE)

#define STRUCT(_name, ...) \
    typedef struct \
    { \
        SPF_FOREACH_ARG(__CALL_FN_WITH_ARGS, __DECLARE_FIELD, __VA_ARGS__) \
    } _name;

#define type_info(_s)   (TypeInfo){0}

#endif // !defined(SPFLIB_TYPE_INFO_DISABLE)

#define field_value_of(_struct, _field_type, _field)   *(_field_type*)((u8*)&_struct + _field->offset)
#define field_ptr_of(_struct, _field_type, _field)   (_field_type*)((u8*)&_struct + _field->offset)

#define field_arr_value_at(_struct, _field_type, _field, _index)  *((_field_type*)((u8*)&_struct + _field->offset) + _index)

#define field_is_f32(_field) (_field->is_floating_point && _field->size == sizeof(float))
#define field_is_f64(_field) (_field->is_floating_point && _field->size == sizeof(double))

#define field_is_u8(_field)  (_field->is_integral && !_field->is_signed && _field->size == sizeof(u8))
#define field_is_s8(_field)  (_field->is_integral && _field->is_signed && _field->size == sizeof(s8))

#define field_is_u16(_field) (_field->is_integral && !_field->is_signed && _field->size == sizeof(u16))
#define field_is_s16(_field) (_field->is_integral && _field->is_signed && _field->size == sizeof(s16))

#define field_is_u32(_field) (_field->is_integral && !_field->is_signed && _field->size == sizeof(u32))
#define field_is_s32(_field) (_field->is_integral && _field->is_signed && _field->size == sizeof(s32))

#define field_is_u64(_field) (_field->is_integral && !_field->is_signed && _field->size == sizeof(u64))
#define field_is_s64(_field) (_field->is_integral && _field->is_signed && _field->size == sizeof(s64))

// #define field_is(_field, _name) (strcmp(_field->type.name, #_name) == 0 && sizeof(_name) == _field->size)

#endif // defined(SPFLIB_TYPE_INFO)

// --- string --- //

// TODO: support utf-8, utf-16, utf-32

#if !defined(SPFLIB_NO_STRING)

#include <stddef.h>     // size_t

typedef struct
{
    const char* data;
    size_t      len;
} string;

#define STRING_FMT "%.*s"
#define STRING_ARGS(s) (s32)(s).len, (s).data

// #define string(str) str_create((str))
#define string(_str)  \
    (sizeof(_str) != sizeof(void*) ?  \
            str_create_sub(_str, sizeof(_str) - 1) :  \
            str_create(_str))

#if STDC_VER >= 11 
    #define str_compare(str1, str2) _Generic((str2), \
        string:      str_compare_str,                \
        const char*: str_compare_cstr,               \
        char*:       str_compare_cstr                \
    ) ((str1), (str2))
#endif

// TODO: add str_to_cstr

#define str_len(_str) (sizeof(_str) != sizeof(void*) ? sizeof(_str) - 1 : str_length(_str))

#if !defined(SPFLIB_STRING_IMPL)

size_t str_length(const char* str);
void str_cpy(char* restrict dest, char* restrict src, size_t num);
string str_create(const char* cstr);
string str_create_sub(const char* cstr, size_t length);
bool str_is_valid(string str);
bool str_compare_str(string str1, string str2);
bool str_compare_cstr(string str, const char* cstr);
bool str_starts_with(string str, const char* cstr);
bool str_ends_with(string str, const char* cstr);
bool str_contains(string str, const char* cstr);
string str_substr(string str, size_t pos, size_t count);
string str_clip_prefix(string str, size_t count);
string str_clip_suffix(string str, size_t count);

#else // defined(SPFLIB_STRING_IMPL)

size_t str_length(const char* str)
{
    size_t c = 0;
    while (*str++) { c++; }
    return c;
}

void str_cpy(char* restrict dest, char* restrict src, size_t num)
{
    for (size_t i = 0; i < num; i++) {
        dest[i] = src[i];
    }
    dest[num] = '\0';
}

string str_create(const char* cstr)
{
	return (string){
        .data = cstr,
        .len  = str_len(cstr)
    };
}

string str_create_sub(const char* cstr, size_t length)
{
	return (string){
        .data = cstr,
        .len  = length
    };
}

bool str_is_valid(string str)
{
	if (str.data == nullptr) return false;
	if (str.len  <= 0)       return false;
	return true;
}

bool str_compare_str(string str1, string str2)
{
	if (!str_is_valid(str1))  return false;
	if (!str_is_valid(str2))  return false;

    if (str1.len != str2.len) return false;

    size_t c = 0;
    while (c < str1.len) {
        if (str1.data[c] != str2.data[c])
            return false;
        c++;
    }

    return true;
}

bool str_compare_cstr(string str, const char* cstr)
{
	if (!str_is_valid(str))  return false;

    size_t cstr_len = str_len(cstr);
	if (cstr_len == 0)       return false;
    if (str.len  != cstr_len) return false;

    size_t c = 0;
    while (c < str.len) {
        if (str.data[c] != cstr[c])
            return false;
        c++;
    }

    return true;
}

bool str_starts_with(string str, const char* cstr)
{
	if (!str_is_valid(str)) return false;

	size_t cstr_len = str_len(cstr);
	if (cstr_len == 0)      return false;

    // string is shorter than sub str
    if (str.len < cstr_len) return false;

    size_t c = 0;
    while (c < cstr_len) {
        if (str.data[c] != cstr[c])
            return false;
        c++;
    }

    return true;
}

bool str_ends_with(string str, const char* cstr)
{
	if (!str_is_valid(str)) return false;

	size_t cstr_len = str_len(cstr);
	if (cstr_len == 0)      return false;

    // string is shorter than the sub str
    if (str.len < cstr_len) return false;

    size_t c = 0;
    while (c < cstr_len) {
        if (cstr[c] != str.data[c + (str.len - cstr_len)]) {
            return false;
        }
        c++;
    }

    return true;
}

bool str_contains(string str, const char* cstr)
{
	if (!str_is_valid(str)) return false;

	size_t cstr_len = str_len(cstr);
	if (cstr_len == 0)      return false;

    // string is shorter than the sub str
    if (str.len < cstr_len) return false;

	size_t i = 0;
	while (i < str.len) {
		char str_char = str.data[i];

		if (str_char == cstr[0]) {
			string substr = str_create_sub(&str.data[i], cstr_len); 

			if (str_compare_cstr(substr, cstr))
				return true;
			else {
				i++;
				continue;
			}
		}
		i++;
	}

	return false;
}

string str_substr(string str, size_t pos, size_t count)
{
	if (!str_is_valid(str))                                 return str;
	if (pos   >= str.len)                                   return str;
	if (count >= str.len || ((pos - 1) + count) >= str.len) return str;

	return (string){
        .data = str.data + pos,
        .len  = count
    };
}

string str_clip_prefix(string str, size_t count)
{
	if (!str_is_valid(str)) return str;
	if (count >= str.len)   return str;

	return (string){
        .data = str.data + count,
        .len  = str.len - count
    };
}

string str_clip_suffix(string str, size_t count)
{
	if (!str_is_valid(str))     return str;
	if ((str.len - count) <= 0) return str;

	return (string){
        .data = str.data,
        .len  = str.len - count
    };
}

#endif // !defined(SPFLIB_STRING_IMPL)

#endif // SPFLIB_NO_STRING

// --- vector 2 --- //

#if defined(SPFLIB_VEC2)

#include <math.h>

typedef struct
{
    union {
        struct {
            float x, y;
        };
        float v[2];
    };
} Vec2;

#define __Vec2_Make(_x, _y) (Vec2){ .x = _x, .y = _y }
#define __Vec2_Args(_1, _2, x, ...) x
#define Vec2(...) __Vec2_Args(__VA_ARGS__, __Vec2_Make(__VA_ARGS__), __Vec2_Make(__VA_ARGS__, __VA_ARGS__))

#if STDC_VER >= 11
    #define vec2_add(v, s) _Generic((s),   \
        Vec2: vec2_addv,                   \
        float: vec2_adds                   \
    ) ((v), (s))
    #define vec2_sub(v, s) _Generic((s),   \
        Vec2: vec2_subv,                   \
        float: vec2_subs                   \
    ) ((v), (s))
    #define vec2_scale(v, s) _Generic((s), \
        Vec2: vec2_mulv,                   \
        float: vec2_muls                   \
    ) ((v), (s))
    #define vec2_div(v, s) _Generic((s),   \
        Vec2: vec2_divv,                   \
        float: vec2_divs                   \
    ) ((v), (s))
#endif

#if !defined(SPFLIB_VEC2_IMPL)

Vec2 vec2_addv(Vec2 a, Vec2 b);
Vec2 vec2_adds(Vec2 v, float s);
Vec2 vec2_subv(Vec2 a, Vec2 b);
Vec2 vec2_subs(Vec2 v, float s);
Vec2 vec2_mulv(Vec2 a, Vec2 b);
Vec2 vec2_muls(Vec2 v, float s);
Vec2 vec2_divv(Vec2 a, Vec2 b);
Vec2 vec2_divs(Vec2 v, float s);
float vec2_dot(Vec2 a, Vec2 b);
float vec2_cross(Vec2 a, Vec2 b);
float vec2_mag(Vec2 v);
Vec2 vec2_norm(Vec2 v);
Vec2 vec2_lerp(Vec2 a, Vec2 b, float s);

#else // defined(SPFLIB_VEC2_IMPL)

Vec2 vec2_addv(Vec2 a, Vec2 b)
{
    return (Vec2){ .x = a.x + b.x, .y = a.y + b.y };
}

Vec2 vec2_adds(Vec2 v, float s)
{
    return (Vec2){ .x = v.x + s, .y = v.y + s };
}

Vec2 vec2_subv(Vec2 a, Vec2 b)
{
    return (Vec2){ .x = a.x - b.x, .y = a.y - b.y };
}

Vec2 vec2_subs(Vec2 v, float s)
{
    return (Vec2){ .x = v.x - s, .y = v.y - s };
}

Vec2 vec2_mulv(Vec2 a, Vec2 b)
{
    return (Vec2){ .x = a.x * b.x, .y = a.y * b.y };
}

Vec2 vec2_muls(Vec2 v, float s)
{
    return (Vec2){ .x = v.x * s, .y = v.y * s };
}

Vec2 vec2_divv(Vec2 a, Vec2 b)
{
    return (Vec2){ .x = a.x / b.x, .y = a.y / b.y };
}

Vec2 vec2_divs(Vec2 v, float s)
{
    return (Vec2){ .x = v.x / s, .y = v.y / s };
}

float vec2_dot(Vec2 a, Vec2 b)
{
    return a.x * b.x + a.y * b.y;
}

float vec2_cross(Vec2 a, Vec2 b)
{
    return a.x * b.y - a.y * b.x;
}

float vec2_mag(Vec2 v)
{
    return sqrtf((v.x * v.x) + (v.y * v.y));
}

Vec2 vec2_norm(Vec2 v)
{
    float len = vec2_mag(v);
    if (len <= 0.f) return (Vec2){0};

    return (Vec2){ .x = v.x * (1.f / len), .y = v.y * (1.f / len) };
}

Vec2 vec2_lerp(Vec2 a, Vec2 b, float s)
{
    return (Vec2){
        .x = a.x + s * (b.x - a.x),
        .y = a.y + s * (b.y - a.y)
    };
}

#endif // defined(SPFLIB_VEC2_IMPL)

#endif // defined(SPFLIB_VEC2)

// --- vector 3 --- //

#if defined(SPFLIB_VEC3)

#include <math.h>

typedef struct
{
    union {
        struct {
            float x, y, z;
        };
        float v[3];
    };
} Vec3;

#define __Vec3_Make(_x, _y, _z) (Vec3){ .x = _x, .y = _y, .z = _z }
#define __Vec3_Args(_1, _2, _3, x, ...) x
#define Vec3(...) __Vec3_Args(__VA_ARGS__,           \
        __Vec3_Make(__VA_ARGS__),                    \
        __Vec3_Make(__VA_ARGS__, 0.f),               \
        __Vec3_Make(__VA_ARGS__, __VA_ARGS__, __VA_ARGS__))

#if STDC_VER >= 11
    #define vec3_add(v, s) _Generic((s),   \
        Vec3: vec3_addv,                   \
        float: vec3_adds                   \
    ) ((v), (s))
    #define vec3_sub(v, s) _Generic((s),   \
        Vec3: vec3_subv,                   \
        float: vec3_subs                   \
    ) ((v), (s))
    #define vec3_scale(v, s) _Generic((s), \
        Vec3: vec3_mulv,                   \
        float: vec3_muls                   \
    ) ((v), (s))
    #define vec3_div(v, s) _Generic((s),   \
        Vec3: vec3_divv,                   \
        float: vec3_divs                   \
    ) ((v), (s))
#endif

#if !defined(SPFLIB_VEC3_IMPL)

Vec3 vec3_addv(Vec3 a, Vec3 b);
Vec3 vec3_adds(Vec3 v, float s);
Vec3 vec3_subv(Vec3 a, Vec3 b);
Vec3 vec3_subs(Vec3 v, float s);
Vec3 vec3_mulv(Vec3 a, Vec3 b);
Vec3 vec3_muls(Vec3 v, float s);
Vec3 vec3_divv(Vec3 a, Vec3 b);
Vec3 vec3_divs(Vec3 v, float s);
float vec3_dot(Vec3 a, Vec3 b);
Vec3 vec3_cross(Vec3 a, Vec3 b);
float vec3_mag(Vec3 v);
Vec3 vec3_norm(Vec3 v);
Vec3 vec3_lerp(Vec3 a, Vec3 b, float s);

#else // defined(SPFLIB_VEC3_IMPL) 

Vec3 vec3_addv(Vec3 a, Vec3 b)
{
    return (Vec3){
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z
    };
}

Vec3 vec3_adds(Vec3 v, float s)
{
    return (Vec3){
        .x = v.x + s,
        .y = v.y + s,
        .z = v.z + s
    };
}

Vec3 vec3_subv(Vec3 a, Vec3 b)
{
    return (Vec3){
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z
    };
}

Vec3 vec3_subs(Vec3 v, float s)
{
    return (Vec3){
        .x = v.x - s,
        .y = v.y - s,
        .z = v.z - s
    };
}

Vec3 vec3_mulv(Vec3 a, Vec3 b)
{
    return (Vec3){
        .x = a.x * b.x,
        .y = a.y * b.y,
        .z = a.z * b.z
    };
}

Vec3 vec3_muls(Vec3 v, float s)
{
    return (Vec3){
        .x = v.x * s,
        .y = v.y * s,
        .z = v.z * s
    };
}

Vec3 vec3_divv(Vec3 a, Vec3 b)
{
    return (Vec3){
        .x = a.x / b.x,
        .y = a.y / b.y,
        .z = a.z / b.z
    };
}

Vec3 vec3_divs(Vec3 v, float s)
{
    return (Vec3){
        .x = v.x / s,
        .y = v.y / s,
        .z = v.z / s
    };
}

//

float vec3_dot(Vec3 a, Vec3 b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

Vec3 vec3_cross(Vec3 a, Vec3 b)
{
    return (Vec3){
        .x = a.y * b.z - a.z * b.y,
        .y = a.z * b.x - a.x * b.z,
        .z = a.x * b.y - a.y * b.x
    };
}

float vec3_mag(Vec3 v)
{
    return sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

Vec3 vec3_norm(Vec3 v)
{
    float len = vec3_mag(v);
    if (len == 0.f) return (Vec3){0};

    return (Vec3){
        .x = v.x * (1.f / len),
        .y = v.y * (1.f / len),
        .z = v.z * (1.f / len)
    };
}

Vec3 vec3_lerp(Vec3 a, Vec3 b, float s)
{
    return (Vec3){
        .x = a.x + s * (b.x - a.x),
        .y = a.y + s * (b.y - a.y),
        .z = a.z + s * (b.z - a.z)
    };
}

#endif // !defined(SPFLIB_VEC3_IMPL)

#endif // defined(SPFLIB_VEC3)

// --- print --- //

// TODO:
// - print _Bool values as true and false instead of 0 and 1
// - add tsprint function that will format a string to temporary storage

#if !defined(SPFLIB_NO_PRINT)

#if STDC_VER < 11
#   error "Printing is not allowed for C versions below C11. Define SPFLIB_NO_PRINT to disable it."
#endif

#define SPF_MAX_PRINT_ARGS 64

#define __SPF_COMMA

#define __ITER_TYPES(_fn, ...)                                                  \
    _fn(BOOL,   _Bool,              int,                "%u",   ...)__SPF_COMMA \
    _fn(FLOAT,  float,              double,             "%f",   ...)__SPF_COMMA \
    _fn(DOUBLE, double,             double,             "%f",   ...)__SPF_COMMA \
    _fn(CSTR,   const char*,        const char*,        "%s",   ...)__SPF_COMMA \
    _fn(STR,    char*,              char*,              "%s",   ...)__SPF_COMMA \
    _fn(CHAR,   char,               int,                "%c",   ...)__SPF_COMMA \
    _fn(SCHAR,  signed char,        int,                "%d",   ...)__SPF_COMMA \
    _fn(UCHAR,  unsigned char,      int,                "%u",   ...)__SPF_COMMA \
    _fn(SINT16, signed short,       int,                "%d",   ...)__SPF_COMMA \
    _fn(UINT16, unsigned short,     int,                "%u",   ...)__SPF_COMMA \
    _fn(SINT32, signed int,         signed int,         "%d",   ...)__SPF_COMMA \
    _fn(UINT32, unsigned int,       unsigned int,       "%u",   ...)__SPF_COMMA \
    _fn(SLONG,  signed long,        signed long,        "%ld",  ...)__SPF_COMMA \
    _fn(ULONG,  unsigned long,      unsigned long,      "%lu",  ...)__SPF_COMMA \
    _fn(SLLONG, signed long long,   signed long long,   "%lld", ...)__SPF_COMMA \
    _fn(ULLONG, unsigned long long, unsigned long long, "%llu", ...)__SPF_COMMA \

// custom types

#define __SPF_HAS_CUSTOM_TYPE_0(_fn, ...)
#define __SPF_HAS_CUSTOM_TYPE_1(_fn, ...)
#define __SPF_HAS_CUSTOM_TYPE_2(_fn, ...) _fn(__VA_ARGS__,,)
#define __SPF_HAS_CUSTOM_TYPE_3(_fn, ...) _fn(__VA_ARGS__,)
#define __SPF_HAS_CUSTOM_TYPE_4(_fn, ...) _fn(__VA_ARGS__)

#define __SPF_IS_CUSTOM_TYPE_DEFINED(_fn, ...) SPF_CONCAT(__SPF_HAS_CUSTOM_TYPE_, VA_COUNT(__VA_ARGS__)) (_fn, __VA_ARGS__)

#define __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT_(_m, _fn) __SPF_IS_CUSTOM_TYPE_DEFINED(_fn, _m)
#define __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(_n, _fn) __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT_(SPF_CUSTOM_PRINT_TYPE_ ## _n, _fn)

/* --- examples ---

note: 
- unfortunately you have to keep track of the number of defined custom print types

-- basic:
typedef struct
{
    float x, y;
} Vec2;

#define SPF_CUSTOM_PRINT_TYPE_0  Vec2, ("{ x = %f, y = %f }", t.x, t.y)  // <-- parenthesis are required for the 2nd arg

-- advanced:
#include <stddef.h>     // size_t

typedef struct
{
    const char* data;
    size_t      len;
} WrapperString;

// - first block is for regular printing
// - second block is for sprint formatting

#define SPF_CUSTOM_PRINT_TYPE_0  WrapperString,,                \
{                                                               \
    for (size_t i = 0; i < t.len; i++) {                        \
        putc(t.data[i], stream);                                \
    }                                                           \
},                                                              \
{                                                               \
    if ((buff_pos + t.len) < buff_sz - 1) {                     \
        strncpy(buff + buff_pos, t.data, t.len);                \
        buff_pos += t.len;                                      \
    }                                                           \
    else {                                                      \
        strncpy(buff + buff_pos, t.data, (buff_sz - buff_pos)); \
        buff_pos += (buff_sz - buff_pos);                       \
    }                                                           \
}

*/

// #define SPF_NEXT_EMPTY_CUSTOM_SLOT 0
// #if defined(SPF_CUSTOM_PRINT_TYPE_0)
// #   define SPF_NEXT_EMPTY_CUSTOM_SLOT 1
// #endif

// built-in types
#if !defined(SPFLIB_NO_STRING)
#   define __SPF_BUILTIN_TYPE_STRING(_fn) _fn(string,,              \
    {                                                               \
        for (size_t i = 0; i < t.len; i++) {                        \
            putc(t.data[i], stream);                                \
        }                                                           \
    },                                                              \
    {                                                               \
        if ((buff_pos + t.len) < buff_sz - 1) {                     \
            strncpy(buff + buff_pos, t.data, t.len);                \
            buff_pos += t.len;                                      \
        }                                                           \
        else {                                                      \
            strncpy(buff + buff_pos, t.data, (buff_sz - buff_pos)); \
            buff_pos += (buff_sz - buff_pos);                       \
        }                                                           \
    })
#else
#   define __SPF_BUILTIN_TYPE_STRING(_fn)
#endif

#if defined(SPFLIB_VEC2)
#   define __SPF_BUILTIN_TYPE_VEC2(_fn) _fn(Vec2, ("{ x = %.4f, y = %.4f }", t.x, t.y),,)
#else
#   define __SPF_BUILTIN_TYPE_VEC2(_fn)
#endif
#if defined(SPFLIB_VEC3)
#   define __SPF_BUILTIN_TYPE_VEC3(_fn) _fn(Vec3, ("{ x = %.4f, y = %.4f, z = %.4f }", t.x, t.y, t.z),,)
#else
#   define __SPF_BUILTIN_TYPE_VEC3(_fn)
#endif

#define __ITER_CUSTOM_TYPES(_fn)                 \
    __SPF_BUILTIN_TYPE_STRING(_fn)               \
    __SPF_BUILTIN_TYPE_VEC2(_fn)                 \
    __SPF_BUILTIN_TYPE_VEC3(_fn)                 \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(0, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(1, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(2, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(3, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(4, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(5, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(6, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(7, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(8, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(9, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(10, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(11, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(12, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(13, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(14, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(15, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(16, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(17, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(18, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(19, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(20, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(21, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(22, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(23, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(24, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(25, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(26, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(27, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(28, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(29, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(30, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(31, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(32, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(33, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(34, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(35, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(36, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(37, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(38, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(39, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(40, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(41, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(42, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(43, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(44, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(45, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(46, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(47, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(48, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(49, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(50, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(51, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(52, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(53, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(54, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(55, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(56, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(57, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(58, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(59, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(60, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(61, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(62, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(63, _fn)  \
    __SPF_DEFINE_CUSTOM_PRINT_TYPE_SLOT(64, _fn)  \

enum
{
    #define __TYPE_TO_ENUM_VAL(_up, ...) __SPF_ARG_TYPE_ ## _up,
    __ITER_TYPES(__TYPE_TO_ENUM_VAL)
    #undef __TYPE_TO_ENUM_VAL

    #define __GET_NAME(_name, _fmt, ...) __SPF_ARG_TYPE_CUSTOM_ ## _name,
    __ITER_CUSTOM_TYPES(__GET_NAME)
    #undef __GET_NAME

    __SPF_ARG_TYPE_UNKNOWN
};

#define __TYPE_TO_ENUM_TYPE(_up, _lw, ...)            _lw: __SPF_ARG_TYPE_ ## _up
#define __TYPE_TO_ENUM_CUSTOM_TYPE(_name, _fmt, ...)  _name: __SPF_ARG_TYPE_CUSTOM_ ## _name,

#undef __SPF_COMMA
#define __SPF_COMMA ,

#define __GET_ENUM_TYPE_FROM_TYPE(_t) _Generic((_t), \
    __ITER_TYPES(__TYPE_TO_ENUM_TYPE)                \
    __ITER_CUSTOM_TYPES(__TYPE_TO_ENUM_CUSTOM_TYPE)  \
    default: __SPF_ARG_TYPE_UNKNOWN                  \
),

#define print_stream(_stream, _fmt, ...)                                           \
    __spf_print(                                                                   \
        _stream,                                                                   \
        _fmt,                                                                      \
        VA_NOPT((0), __VA_ARGS__) VA_OPT((VA_COUNT(__VA_ARGS__),), __VA_ARGS__)    \
        VA_OPT((SPF_FOREACH(__GET_ENUM_TYPE_FROM_TYPE, __VA_ARGS__)), __VA_ARGS__) \
        __VA_ARGS__                                                                \
    )

#define print(_fmt, ...) print_stream(stdout, _fmt, __VA_ARGS__)

#define println_stream(_stream, _fmt, ...) \
    do { print_stream(_stream, _fmt, __VA_ARGS__); putc('\n', _stream); } while (0)

#define println(_fmt, ...) println_stream(stdout, _fmt, __VA_ARGS__)

#define sprint(_buff, _buff_sz, _fmt, ...) \
    __spf_sprint(                                                                  \
        _buff,                                                                     \
        _buff_sz,                                                                  \
        _fmt,                                                                      \
        VA_NOPT((0), __VA_ARGS__) VA_OPT((VA_COUNT(__VA_ARGS__),), __VA_ARGS__)    \
        VA_OPT((SPF_FOREACH(__GET_ENUM_TYPE_FROM_TYPE, __VA_ARGS__)), __VA_ARGS__) \
        __VA_ARGS__                                                                \
    )

#include <stdio.h>      // putc, fputs, fprintf

#if !defined(SPFLIB_PRINT_IMPL)

void __spf_print(FILE* stream, const char* fmt, size_t arg_count, ...);
void __spf_sprint(char* buff, size_t buff_sz, const char* fmt, size_t arg_count, ...);

#else // !defined(SPFLIB_PRINT_IMPL)

#include <stdarg.h>     // va_list, va_start, va_end
#include <string.h>     // strlen, strncpy TODO: potentially replace with standalone functions

void __spf_print(FILE* stream, const char* fmt, size_t arg_count, ...)
{
    if (arg_count == 0) {
        fputs(fmt, stream);
        return;
    }

    if (arg_count >= SPF_MAX_PRINT_ARGS) {
        fputs("Too many arguments passed into the print function.", stream);
        return;
    }

    va_list list;
    va_start(list, arg_count);

    size_t fmt_len = strlen(fmt);

    static int arg_types[SPF_MAX_PRINT_ARGS];
    for (size_t i = 0; i < arg_count; i++) {
        arg_types[i] = va_arg(list, int);
    }

    size_t current_arg_type = 0;

    size_t c = 0;
    while (c < fmt_len) {
        if (fmt[c] == '%') {
            if (current_arg_type >= arg_count) goto rest;

            char nextc = '\0';
            if ((c + 1) <= fmt_len)
                nextc = fmt[c + 1];

            int type = arg_types[current_arg_type];
            current_arg_type++;

            
            #define TYPE__Bool \
                ,int v = va_arg(list, int); \
                fprintf(stream, "%s", v ? "true" : "false");

            #define NOT_BOOL(_fmt, _prm) fprintf(stream, _fmt, va_arg(list, _prm));

            #define IS_BOOL(_lw, _fmt, _prm) SPF_CHECK(TYPE_ ## _lw, NOT_BOOL(_fmt, _prm))

            // fprintf(stream, _fmt, va_arg(list, _prm));

            #define __TYPE_CASE(_up, _lw, _prm, _fmt, ...) \
                case __SPF_ARG_TYPE_ ## _up: {             \
                    IS_BOOL(_lw, _fmt, _prm);              \
                    break;                                 \
                }

            #define __CUSTOM_TYPE_CASE(_type, _fmt, _adv_fmt, ...)    \
                case __SPF_ARG_TYPE_CUSTOM_ ## _type: {               \
                    _type t = va_arg(list, _type);                    \
                    VA_OPT((_adv_fmt), _adv_fmt)                      \
                    VA_OPT((fprintf(stream, SPF_EXPAND _fmt);), _fmt) \
                    break;                                            \
                }

            #undef __SPF_COMMA
            #define __SPF_COMMA

            switch (type) {
                __ITER_TYPES(__TYPE_CASE)
                __ITER_CUSTOM_TYPES(__CUSTOM_TYPE_CASE)

                default: {
                    fprintf(stream, "%p", va_arg(list, void*));
                    break;
                }
            }

            if (nextc == '%') {
                putc(fmt[c], stream);
                c++;
            }

            #undef __TYPE_CASE
            #undef __CUSTOM_TYPE_CASE

            #undef TYPE__Bool 
            #undef NOT_BOOL
            #undef IS_BOOL

            #undef __SPF_COMMA
            #define __SPF_COMMA ,

            c++;
        }
        else {
rest:
            putc(fmt[c], stream);
            c++;
        }
    }

// end:
    va_end(list);
}

void __spf_sprint(char* buff, size_t buff_sz, const char* fmt, size_t arg_count, ...)
{
    if (arg_count == 0) {
        size_t len = strlen(fmt);
        if (len == 0) return;

        if (len > buff_sz) {
            strncpy(buff, fmt, buff_sz);
            buff[buff_sz - 1] = '\0';
        }
        else {
            strncpy(buff, fmt, len);
            buff[len - 1] = '\0';
        }

        return;
    }

    if (arg_count >= SPF_MAX_PRINT_ARGS) {
        puts("Too many arguments passed into the sprint function.");
        return;
    }

    va_list list;
    va_start(list, arg_count);

    size_t fmt_len = strlen(fmt);

    static int arg_types[SPF_MAX_PRINT_ARGS];
    for (size_t i = 0; i < arg_count; i++) {
        arg_types[i] = va_arg(list, int);
    }

    size_t buff_pos = 0;
    size_t current_arg_type = 0;

    size_t c = 0;
    while (c < fmt_len) {
        if (fmt[c] == '%') {
            if (current_arg_type >= arg_count) goto rest;
            if (buff_pos >= buff_sz - 1) goto end;

            char nextc = '\0';
            if ((c + 1) <= fmt_len)
                nextc = fmt[c + 1];

            int type = arg_types[current_arg_type];
            current_arg_type++;


            #define TYPE__Bool \
                ,int v = va_arg(list, int); \
                int n = snprintf(buff + buff_pos, (buff_sz - buff_pos), "%s", v ? "true" : "false"); \

            #define NOT_BOOL(_fmt, _prm) \
                int n = snprintf(buff + buff_pos, (buff_sz - buff_pos), _fmt, va_arg(list, _prm));

            #define IS_BOOL(_lw, _fmt, _prm) SPF_CHECK(TYPE_ ## _lw, NOT_BOOL(_fmt, _prm))

            // int n = snprintf(buff + buff_pos, (buff_sz - buff_pos), _fmt, va_arg(list, _prm));

            #define __TYPE_CASE(_up, _lw, _prm, _fmt, ...) \
                case __SPF_ARG_TYPE_ ## _up: {             \
                    if (buff_pos >= buff_sz - 1) goto end; \
                    IS_BOOL(_lw, _fmt, _prm);              \
                    buff_pos += n;                         \
                    break;                                 \
                }

            #define __CUSTOM_TYPE_CASE(_type, _fmt, _adv_fmt, _adv_sfmt, ...)                     \
                case __SPF_ARG_TYPE_CUSTOM_ ## _type: {                                           \
                    if (buff_pos >= buff_sz - 1) goto end;                                        \
                    _type t = va_arg(list, _type);                                                \
                    VA_OPT((_adv_sfmt), _adv_sfmt)                                                \
                    VA_OPT((                                                                      \
                        if (buff_pos >= buff_sz - 1) goto end;                                    \
                        int n = snprintf(buff + buff_pos, (buff_sz - buff_pos), SPF_EXPAND _fmt); \
                        buff_pos += n;                                                            \
                    ), _fmt)                                                                      \
                    break;                                                                        \
                }

            #undef __SPF_COMMA
            #define __SPF_COMMA

            switch (type) {
                __ITER_TYPES(__TYPE_CASE)
                __ITER_CUSTOM_TYPES(__CUSTOM_TYPE_CASE)

                default: {
                    if (buff_pos >= buff_sz - 1) goto end; 
                    int n = snprintf(buff + buff_pos, (buff_sz - buff_pos), "%p", va_arg(list, void*)); 
                    buff_pos += n; 
                    break;
                }
            }

            if (nextc == '%') {
                if (buff_pos >= buff_sz - 1) goto end;
                buff[buff_pos] = fmt[c];
                buff_pos++;

                c++;
            }

            #undef __TYPE_CASE
            #undef __CUSTOM_TYPE_CASE

            #undef TYPE__Bool 
            #undef NOT_BOOL
            #undef IS_BOOL

            #undef __SPF_COMMA
            #define __SPF_COMMA ,

            c++;
        }
        else {
rest:
            if (buff_pos >= buff_sz - 1) goto end;

            buff[buff_pos] = fmt[c];
            buff_pos++;
            c++;
        }
    }

end:
    va_end(list);

    if (buff_pos < buff_sz)
        buff[buff_pos] = '\0';
    else
        buff[buff_sz - 1] = '\0';
}

#endif // !defined(SPFLIB_PRINT_IMPL)

#endif // !defined(SPFLIB_NO_PRINT)

// --- memory --- //

#if !defined(SPFLIB_NO_MEMORY)

// TODO: 
// - implement rpmalloc
// - add more allocators
// - add functions like mem_cpy, mem_set

// - temp - //
struct __spf_TempStorage;

#define temporary_storage __spf_temp_get_state()

#if !defined(SPFLIB_TEMP_CAPACITY)
#   define SPFLIB_TEMP_CAPACITY 2097152 /* 2 MB */
#endif

#if !defined(SPFLIB_MEMORY_NO_TEMP)
    #include <stdlib.h>     // atexit

    s32 __real_main(s32 argc, char** argv);

    #define main(...)                             \
        main(s32 argc, char** argv)               \
        {                                         \
           __spf_temp_init(SPFLIB_TEMP_CAPACITY); \
           atexit(__spf_temp_storage_delete);     \
           return __real_main(argc, argv);        \
        }                                         \
        s32 __real_main(s32 argc, char** argv)
#endif

// - linear - //

typedef struct
{
    void*  data;
    size_t capacity;
    size_t pos_bytes;
} LinearAllocator;

// - managament - //

#if STDC_VER < 11

#define new_linear(_type, _sz, _alloc) __spf_new_alloc_linear(sizeof(_type), _sz, _alloc)
#define new_temp(_type, _sz) __spf_new_alloc_temp(sizeof(_type), _sz, temporary_storage)
#define new(_type, _sz) __spf_new_alloc_default(sizeof(_type), _sz, 0)

#define delete_linear(_la) __spf_linear_delete(la)
#define delete(_ptr) free(la)

#else // STDC_VER < 11

#define __call_new(_t_sz, _sz, _alloc) _Generic((_alloc), \
    LinearAllocator*         : __spf_new_alloc_linear,    \
    struct __spf_TempStorage*: __spf_new_alloc_temp,      \
    default                  : __spf_new_alloc_default    \
) (_t_sz, _sz, _alloc)

#define __sz_or_alloc(a, b, _) _Generic((b),            \
    int                      : __spf_new_alloc_default, \
    LinearAllocator*         : __spf_new_dummy_la,      \
    struct __spf_TempStorage*: __spf_new_dummy_tmp      \
) (a, b, _)

#define __new2(_t, _sz, _alloc, ...) __call_new(sizeof(_t), _sz, _alloc)
#define __new1(_t, _sz_or_alloc, ...) \
    __spf_is_char_numeric(#_sz_or_alloc[0]) ?  \
                    __sz_or_alloc(sizeof(_t), _sz_or_alloc, 0) :  \
                    __call_new(sizeof(_t), 1, _sz_or_alloc)
#define __new0(_t) __spf_new_alloc_default(sizeof(_t), 1, 0)

#define new(_t, ...)                                                             \
  SPF_EXPAND(SPF_CONCAT(__new, VA_COUNT(__VA_ARGS__))                            \
          VA_OPT(((_t, __VA_ARGS__)), __VA_ARGS__) VA_NOPT(((_t)), __VA_ARGS__))

// TODO: make it so freeing a pointer sets it to null after
#define delete(_a) _Generic((_a),          \
    LinearAllocator*: __spf_linear_delete, \
    default         : free                 \
) (_a)

#endif // STDC_VER < 11

#if !defined(SPFLIB_MEMORY_IMPL)

// - temp - //
struct __spf_TempStorage* __spf_temp_get_state();
void __spf_temp_init(size_t capacity);
void* __spf_temp_alloc(size_t size);
void temp_storage_reset(void);
void __spf_temp_storage_delete(void);

// - linear - //
LinearAllocator allocator_linear_new(size_t capacity);
void* __spf_linear_alloc(LinearAllocator* la, size_t size);
void allocator_linear_reset(LinearAllocator* la);
void __spf_linear_delete(LinearAllocator* la);

// - management - //
bool __spf_is_char_numeric(char c);
void* __spf_new_alloc_default(size_t type_sz, size_t size, int _);
void* __spf_new_alloc_linear(size_t type_sz, size_t size, LinearAllocator* alloc);
void* __spf_new_alloc_temp(size_t type_sz, size_t size, struct __spf_TempStorage* alloc);

// dummy functions for the macro to work corectly
// these will never get called
void* __spf_new_dummy_la(size_t _, LinearAllocator* __, int ___);
void* __spf_new_dummy_tmp(size_t _, struct __spf_TempStorage* __, int ___);

// - misc - //
void mem_zero(void* dest, size_t count);
bool mem_equal(void* mem, void* mem2, size_t count);

#else // !defined(SPFLIB_MEMORY_IMPL)

// - temp - //
struct __spf_TempStorage
{
    void*  data;
    size_t capacity;
    size_t pos_bytes;
};
// struct __spf_TempStorage __temp_storage;

struct __spf_TempStorage* __spf_temp_get_state()
{
    static struct __spf_TempStorage __temp_storage;
    return &__temp_storage;
}

void __spf_temp_init(size_t capacity)
{
    struct __spf_TempStorage* __temp_storage = __spf_temp_get_state();
    __temp_storage->data = malloc(capacity);

    SPF_ASSERT(__temp_storage->data && "Temporary storage failed to initialize.");

    __temp_storage->capacity  = capacity;
    __temp_storage->pos_bytes = 0;
}

void* __spf_temp_alloc(size_t size)
{
    struct __spf_TempStorage* __temp_storage = __spf_temp_get_state();
    if (__temp_storage->data == nullptr) return nullptr;
    SPF_ASSERT(((size + __temp_storage->pos_bytes) < __temp_storage->capacity) 
                && "Temporary storage capacity exceeded. Did you forget to call 'temp_storage_reset()'?");

    __temp_storage->data      += size;
    __temp_storage->pos_bytes += size;

    return __temp_storage->data;
}

void temp_storage_reset(void)
{
    struct __spf_TempStorage* __temp_storage = __spf_temp_get_state();
    __temp_storage->data      -= __temp_storage->pos_bytes;
    __temp_storage->pos_bytes  = 0;
}

void __spf_temp_storage_delete(void)
{
    temp_storage_reset();

    struct __spf_TempStorage* __temp_storage = __spf_temp_get_state();
    free(__temp_storage->data);
}

// - linear - //

LinearAllocator allocator_linear_new(size_t capacity)
{
    LinearAllocator la;
    la.data = malloc(capacity);

    SPF_ASSERT(la.data && "Linear allocator failed to initialize.");

    la.capacity  = capacity;
    la.pos_bytes = 0;

    return la;
}

void* __spf_linear_alloc(LinearAllocator* la, size_t size)
{
    if (la->data == nullptr) return nullptr;
    SPF_ASSERT(((size + la->pos_bytes) < la->capacity) && "Linear allocator capacity exceeded.");

    la->data      += size;
    la->pos_bytes += size;

    return la->data;
}

void allocator_linear_reset(LinearAllocator* la)
{
    la->data      -= la->pos_bytes;
    la->pos_bytes  = 0;
}

void __spf_linear_delete(LinearAllocator* la)
{
    allocator_linear_reset(la);

    free(la->data);
    la->data = nullptr;
    la->capacity = 0;
}

// - management - //

bool __spf_is_char_numeric(char c)
{
    return c >= '0' && c <= '9';
}

void* __spf_new_alloc_default(size_t type_sz, size_t size, int _)
{
    return malloc(type_sz * size);
}

void* __spf_new_alloc_linear(size_t type_sz, size_t size, LinearAllocator* alloc)
{
    return __spf_linear_alloc(alloc, type_sz * size);
}

void* __spf_new_alloc_temp(size_t type_sz, size_t size, struct __spf_TempStorage* _)
{
    return __spf_temp_alloc(type_sz * size);
}

// dummy functions for the macro to work corectly
// these will never get called
void* __spf_new_dummy_la(size_t _, LinearAllocator* __, int ___)
{
    return (void*)0xDEADBEEF;
}
void* __spf_new_dummy_tmp(size_t _, struct __spf_TempStorage* __, int ___)
{
    return (void*)0xDEADBEEF;
}

// - misc - //
void mem_zero(void* dest, size_t count)
{
    while (count--) {
        *(u8*)dest++ = 0;
    }
}

bool mem_equal(void* mem, void* mem2, size_t count)
{
    while (count--) {
        if (*(u8*)mem++ != *(u8*)mem2++) return false;
    }
    return true;
}

#endif // !defined(SPFLIB_MEMORY_IMPL)

#endif // !defined(SPFLIB_NO_MEMORY)

#endif // SPFLIB_H

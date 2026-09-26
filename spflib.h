#ifndef SPFLIB_H
#define SPFLIB_H

#if defined(_MSC_VER) && _MSC_VER < 1944
#   error("MSVC version at least 19.44 is required.")
#elif defined(__GNUC__) && __GNUC__ < 8
#   error("GCC version at least 8.1 is required.")
#elif defined(__clang__) && __clang_major__ < 12
#   error("Clang version at least 12 is required.")
#endif

#if defined(__GNUC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wunused-function"
#elif defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunused-function"
#elif defined(_MSC_VER)
#   pragma warning( push )
#   pragma warning( disable : 4005 4505 5045 5110 )
#endif

// -- utils -- //

#include <stdint.h>

// TODO:
// - scratch buffer(linear allocator) for temporary allocation
// - is nan anf infinity checks
// - custom snprintf
// - string to int, string to float etc
// - add c++ support
// - support more compilers
// - reflection
// - window creation (RGFW) and opengl context
// - inline vec2 and other small functions 
// - file io
// - matrices

#if defined(_WIN32)
#   define OS_WINDOWS
#elif defined(__linux__)
#	define OS_LINUX
#	define OS_POSIX
#elif defined(__APPLE__)
#	define OS_APPLE
#	define OS_POSIX
#else
#	define OS_UNKOWN
#   pragma message("Unknown OS.")
#endif

#if defined(__GNUC__) && !defined(__clang__)
#	define COMPILER_GCC
#elif defined(__clang__)
#	define COMPILER_CLANG
#elif defined(_MSC_VER) && !defined(__clang__)
#	define COMPILER_MSVC
#else
#	define COMPILER_UNKNOWN
#   pragma message("Unkown C compiler.")
#endif

#if defined(__STDC_VERSION__)
#   if __STDC_VERSION__   == 199901L
#	    define STDC_VER 99
#   elif __STDC_VERSION__ == 201112L
#	    define STDC_VER 11
#   elif __STDC_VERSION__ == 201710L
#	    define STDC_VER 17
#   elif __STDC_VERSION__ == 202311L
#	    define STDC_VER 23
#   else
#	    define STDC_UNKNOWN
#       pragma message("Unkown C standard.")
#   endif
#endif

#if STDC_VER < 11
#   pragma message("At least version C11 is recommended for most features to work correctly.")
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

#if STDC_VER >= 11 && STDC_VER < 23
#   define static_assert  _Static_assert
#   define alignas        _Alignas
#   define alignof        _Alignof
#   define thread_local   _Thread_local
#endif

#if STDC_VER >= 11
#   define atomic         _Atomic
#endif

#if STDC_VER < 23
#   if defined(COMPILER_GCC) || defined(COMPILER_CLANG)
#       define typeof(x)        __typeof(x)
#       define typeof_unqual(x) __typeof_unqual(x)
#   elif defined(COMPILER_MSVC)
#       define typeof(x)        __typeof__(x)
#       define typeof_unqual(x) __typeof_unqual__(x)
#   endif
#endif

#if !defined(offsetof)
#   define offsetof(_s, _m) (size_t)&(((_s*)0)->_m)
#endif

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

#define SPF_PI        3.14159265358979323846264338327950288
#define SPF_PI_2      1.57079632679489661923132169163975144
#define SPF_PI_4      0.78539816339744830961566084581987572

#if STDC_VER >= 11

#define SWAP_TYPES(_fn) \
    _fn(f32) \
    _fn(f64) \
    _fn(s8) \
    _fn(u8) \
    _fn(s32) \
    _fn(u32) \
    _fn(s64) \
    _fn(u64) \

#define INVALID_MATCH default: (void(*)(void))0
// #define DEFINE_MATCH(_type, b) _type*: _Generic((b), _type* swap_##_type, INVALID_MATCH),

#define swap(a, b) _Generic((a), \
    float*:              _Generic((b), float*:              swap_float,  INVALID_MATCH), \
    double*:             _Generic((b), double*:             swap_double, INVALID_MATCH), \
    signed char*:        _Generic((b), signed char*:        swap_s8,     INVALID_MATCH), \
    unsigned char*:      _Generic((b), unsigned char*:      swap_u8,     INVALID_MATCH), \
    signed int*:         _Generic((b), signed int*:         swap_s32,    INVALID_MATCH), \
    unsigned int*:       _Generic((b), unsigned int*:       swap_u32,    INVALID_MATCH), \
    signed long long*:   _Generic((b), signed long long*:   swap_s64,    INVALID_MATCH), \
    unsigned long long*: _Generic((b), unsigned long long*: swap_u64,    INVALID_MATCH), \
    INVALID_MATCH \
) (a, b)

#define MAKE_SWAP_FN(_type) \
    static void swap_##_type(_type* a, _type* b) \
    { \
        _type temp = *a; \
        *a = *b; \
        *b = temp; \
    }

SWAP_TYPES(MAKE_SWAP_FN)

#endif // STDC_VER >= 11

// --- assert --- //

#if !defined(SPFLIB_NO_ASSERT)
#if defined(NDEBUG) && defined(SPFLIB_ASSERT_IN_RELEASE) || !defined(NDEBUG)

#include <stdio.h>

#define SPF_WILL_ASSERT 1

#define SPF_ASSERT(_cond, _msg) \
    do { \
        if (!(_cond)) { \
            printf("Assertion failed in '%s' at '%d', with message '%s'\n", __FILE__, __LINE__, _msg); \
            abort(); \
        } \
    } while (0)

#else // defined(NDEBUG) && defined(SPFLIB_ASSERT_IN_RELEASE) || !defined(NDEBUG)
#   define SPF_ASSERT(_cond, _msg)
#endif // defined(NDEBUG) && defined(SPFLIB_ASSERT_IN_RELEASE) || !defined(NDEBUG)
#endif // !defined(SPFLIB_NO_ASSERT)

// --- allocators --- //

#if defined(SPFLIB_ALLOCATORS)

// TODO: replace with rpmalloc
#include <stdlib.h>    // malloc, free

// bump

typedef struct
{
    void*  data;
    size_t capacity;
    size_t pos_bytes;
} BumpAllocator;

static BumpAllocator bump_new(size_t capacity)
{
    return (BumpAllocator){
        .data      = malloc(capacity),
        .capacity  = capacity,
        .pos_bytes = 0
    };
}

static void* bump_alloc(BumpAllocator* ac, size_t size)
{
    if (ac->data == nullptr)                    return nullptr;
    if ((size + ac->pos_bytes) >= ac->capacity) {
#if defined(SPF_WILL_ASSERT)
        SPF_ASSERT(0, "Bump allocator capacity exceeded.");
#endif
        return nullptr; 
    }

    ac->data      += size;
    ac->pos_bytes += size;

    return ac->data;
}

static void bump_reset(BumpAllocator* ac)
{
    ac->data      -= ac->pos_bytes;
    ac->pos_bytes  = 0;
}

static void bump_delete(BumpAllocator* ac)
{
    ac->pos_bytes = 0;
    ac->capacity  = 0;

    free(ac->data);
    ac->data      = nullptr;
}

#endif // defined(SPFLIB_ALLOCATORS)

// --- string --- //

#if !defined(SPFLIB_NO_STRING)

#include <stddef.h>     // size_t

typedef struct
{
    const char* data;
    size_t      len;
} string;

#define STRING_FMT "%.*s"
#define STRING_ARGS(s) (int)(s).len, (s).data

#define string(str) str_create((str))

#if STDC_VER >= 11 
    #define str_compare(str1, str2) _Generic((str2), \
        string:      str_compare_str,                \
        const char*: str_compare_cstr,               \
        char*:       str_compare_cstr                \
    ) ((str1), (str2))
#endif

// TODO: add str_to_cstr

static size_t str_len(const char* str)
{
    size_t c = 0;
    while (*str++) { c++; }
    return c;
}

static string str_create(const char* cstr)
{
	return (string){
        .data = cstr,
        .len  = str_len(cstr)
    };
}

static string str_create_sub(const char* cstr, size_t length)
{
	return (string){ 
        .data = cstr,
        .len  = length
    };
}

static bool str_is_valid(string str)
{
	if (str.data == nullptr) return false;
	if (str.len  <= 0)       return false;
	return true;
}

static bool str_compare_str(string str1, string str2)
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

static bool str_compare_cstr(string str, const char* cstr)
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

static bool str_starts_with(string str, const char* cstr)
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

static bool str_ends_with(string str, const char* cstr)
{
	if (!str_is_valid(str)) return false;

	size_t cstr_len = str_len(cstr);
	if (cstr_len == 0)      return false;

    // string is shorter than the sub str
    if (str.len < cstr_len) return false;

    size_t c = 0;
    while (c < cstr_len) {
        if (cstr[c] != str.data[c + (str.len - cstr_len)]) 
            return false;
        c++;
    }

    return true;
}

static bool str_contains(string str, const char* cstr)
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

			if (str_compare_str(substr, str))
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

static string str_substr(string str, size_t pos, size_t count)
{
	if (!str_is_valid(str))                                 return str;
	if (pos   >= str.len)                                   return str;
	if (count >= str.len || ((pos - 1) + count) >= str.len) return str;

	return (string){
        .data = str.data + pos,
        .len  = count
    };
}

static string str_clip_prefix(string str, size_t count)
{
	if (!str_is_valid(str)) return str;
	if (count >= str.len)   return str;

	return (string){
        .data = str.data + count,
        .len  = str.len - count
    };
}

static string str_clip_suffix(string str, size_t count)
{
	if (!str_is_valid(str))     return str;
	if ((str.len - count) <= 0) return str;

	return (string){
        .data = str.data,
        .len  = str.len - count
    };
}

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

#define __Vec2_Args(_1, _2, x, ...) x
#define Vec2(...) __Vec2_Args(__VA_ARGS__, vec2_init(__VA_ARGS__), vec2_make(__VA_ARGS__))

Vec2 vec2_make(float xy)
{
    return (Vec2){ .x = xy, .y = xy };
}

Vec2 vec2_init(float x, float y)
{
    return (Vec2){ .x = x, .y = y };
}

#if STDC_VER >= 11
    #define vec2_add(v, s) _Generic((s), \
        Vec2: vec2_addv, \
        float: vec2_adds \
    ) ((v), (s))
    #define vec2_sub(v, s) _Generic((s), \
        Vec2: vec2_subv, \
        float: vec2_subs \
    ) ((v), (s))
    #define vec2_scale(v, s) _Generic((s), \
        Vec2: vec2_mulv, \
        float: vec2_muls \
    ) ((v), (s))
    #define vec2_div(v, s) _Generic((s), \
        Vec2: vec2_divv, \
        float: vec2_divs \
    ) ((v), (s))
#endif

Vec2 vec2_addv(Vec2 a, Vec2 b)
{
    return (Vec2){ .x = a.x + b.x, .y = a.y + b.y };
}

Vec2 vec2_adds(Vec2 a, float s)
{
    return (Vec2){ .x = a.x + s, .y = a.y + s };
}

Vec2 vec2_subv(Vec2 a, Vec2 b)
{
    return (Vec2){ .x = a.x - b.x, .y = a.y - b.y };
}

Vec2 vec2_subs(Vec2 a, float s)
{
    return (Vec2){ .x = a.x - s, .y = a.y - s };
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

Vec2 vec2_divs(Vec2 a, float s)
{
    return (Vec2){ .x = a.x / s, .y = a.y / s };
}

//

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

#endif // defined(SPFLIB_VEC2)

// --- io --- //

#if !defined(SPFLIB_NO_IO)
#if STDC_VER >= 11

// TODO:
// - make registering custom print types better by not having to remember the last index
// - make '%' printable
// - add support for formatting e.g. (%.*, %02 etc.)
// - change printing format from '%' to '{}'

#include <stddef.h>   // size_t
#include <stdio.h>    // fprintf, putc
#include <stdarg.h>   // va_list
#include <string.h>   // strncat

#define __N_VA_ARGS_(_100,_99,_98,_97,_96,_95,_94,_93,_92,_91,_90,_89,_88,_87,_86,_85,_84,_83,_82,_81,_80,_79,_78,_77,_76,_75,_74,_73,_72,_71,_70,_69,_68,_67,_66,_65,_64,_63,_62,_61,_60,_59,_58,_57,_56,_55,_54,_53,_52,_51,_50,_49,_48,_47,_46,_45,_44,_43,_42,_41,_40,_39,_38,_37,_36,_35,_34,_33,_32,_31,_30,_29,_28,_27,_26,_25,_24,_23,_22,_21,_20,_19,_18,_17,_16,_15,_14,_13,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1, N, ...) N
#define __N_VA_ARGS(...) __N_VA_ARGS_(__VA_ARGS__ __VA_OPT__(,) 100,99,98,97,96,95,94,93,92,91,90,89,88,87,86,85,84,83,82,81,80,79,78,77,76,75,74,73,72,71,70,69,68,67,66,65,64,63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)

#define __FOREACH_0(FN, ...)
#define __FOREACH_1(FN, E, ARG, ...)  FN(E)
#define __FOREACH_2(FN, E, ARG, ...)  FN(E) __FOREACH_1(FN, ARG, __VA_ARGS__)
#define __FOREACH_3(FN, E, ARG, ...)  FN(E) __FOREACH_2(FN, ARG, __VA_ARGS__)
#define __FOREACH_4(FN, E, ARG, ...)  FN(E) __FOREACH_3(FN, ARG, __VA_ARGS__)
#define __FOREACH_5(FN, E, ARG, ...)  FN(E) __FOREACH_4(FN, ARG, __VA_ARGS__)
#define __FOREACH_6(FN, E, ARG, ...)  FN(E) __FOREACH_5(FN, ARG, __VA_ARGS__)
#define __FOREACH_7(FN, E, ARG, ...)  FN(E) __FOREACH_6(FN, ARG, __VA_ARGS__)
#define __FOREACH_8(FN, E, ARG, ...)  FN(E) __FOREACH_7(FN, ARG, __VA_ARGS__)
#define __FOREACH_9(FN, E, ARG, ...)  FN(E) __FOREACH_8(FN, ARG, __VA_ARGS__)
#define __FOREACH_10(FN, E, ARG, ...)  FN(E) __FOREACH_9(FN, ARG, __VA_ARGS__)
#define __FOREACH_11(FN, E, ARG, ...)  FN(E) __FOREACH_10(FN, ARG, __VA_ARGS__)
#define __FOREACH_12(FN, E, ARG, ...)  FN(E) __FOREACH_11(FN, ARG, __VA_ARGS__)
#define __FOREACH_13(FN, E, ARG, ...)  FN(E) __FOREACH_12(FN, ARG, __VA_ARGS__)
#define __FOREACH_14(FN, E, ARG, ...)  FN(E) __FOREACH_13(FN, ARG, __VA_ARGS__)
#define __FOREACH_15(FN, E, ARG, ...)  FN(E) __FOREACH_14(FN, ARG, __VA_ARGS__)
#define __FOREACH_16(FN, E, ARG, ...)  FN(E) __FOREACH_15(FN, ARG, __VA_ARGS__)
#define __FOREACH_17(FN, E, ARG, ...)  FN(E) __FOREACH_16(FN, ARG, __VA_ARGS__)
#define __FOREACH_18(FN, E, ARG, ...)  FN(E) __FOREACH_17(FN, ARG, __VA_ARGS__)
#define __FOREACH_19(FN, E, ARG, ...)  FN(E) __FOREACH_18(FN, ARG, __VA_ARGS__)
#define __FOREACH_20(FN, E, ARG, ...)  FN(E) __FOREACH_19(FN, ARG, __VA_ARGS__)
#define __FOREACH_21(FN, E, ARG, ...)  FN(E) __FOREACH_20(FN, ARG, __VA_ARGS__)
#define __FOREACH_22(FN, E, ARG, ...)  FN(E) __FOREACH_21(FN, ARG, __VA_ARGS__)
#define __FOREACH_23(FN, E, ARG, ...)  FN(E) __FOREACH_22(FN, ARG, __VA_ARGS__)
#define __FOREACH_24(FN, E, ARG, ...)  FN(E) __FOREACH_23(FN, ARG, __VA_ARGS__)
#define __FOREACH_25(FN, E, ARG, ...)  FN(E) __FOREACH_24(FN, ARG, __VA_ARGS__)
#define __FOREACH_26(FN, E, ARG, ...)  FN(E) __FOREACH_25(FN, ARG, __VA_ARGS__)
#define __FOREACH_27(FN, E, ARG, ...)  FN(E) __FOREACH_26(FN, ARG, __VA_ARGS__)
#define __FOREACH_28(FN, E, ARG, ...)  FN(E) __FOREACH_27(FN, ARG, __VA_ARGS__)
#define __FOREACH_29(FN, E, ARG, ...)  FN(E) __FOREACH_28(FN, ARG, __VA_ARGS__)
#define __FOREACH_30(FN, E, ARG, ...)  FN(E) __FOREACH_29(FN, ARG, __VA_ARGS__)
#define __FOREACH_31(FN, E, ARG, ...)  FN(E) __FOREACH_30(FN, ARG, __VA_ARGS__)
#define __FOREACH_32(FN, E, ARG, ...)  FN(E) __FOREACH_31(FN, ARG, __VA_ARGS__)
#define __FOREACH_33(FN, E, ARG, ...)  FN(E) __FOREACH_32(FN, ARG, __VA_ARGS__)
#define __FOREACH_34(FN, E, ARG, ...)  FN(E) __FOREACH_33(FN, ARG, __VA_ARGS__)
#define __FOREACH_35(FN, E, ARG, ...)  FN(E) __FOREACH_34(FN, ARG, __VA_ARGS__)
#define __FOREACH_36(FN, E, ARG, ...)  FN(E) __FOREACH_35(FN, ARG, __VA_ARGS__)
#define __FOREACH_37(FN, E, ARG, ...)  FN(E) __FOREACH_36(FN, ARG, __VA_ARGS__)
#define __FOREACH_38(FN, E, ARG, ...)  FN(E) __FOREACH_37(FN, ARG, __VA_ARGS__)
#define __FOREACH_39(FN, E, ARG, ...)  FN(E) __FOREACH_38(FN, ARG, __VA_ARGS__)
#define __FOREACH_40(FN, E, ARG, ...)  FN(E) __FOREACH_39(FN, ARG, __VA_ARGS__)
#define __FOREACH_41(FN, E, ARG, ...)  FN(E) __FOREACH_40(FN, ARG, __VA_ARGS__)
#define __FOREACH_42(FN, E, ARG, ...)  FN(E) __FOREACH_41(FN, ARG, __VA_ARGS__)
#define __FOREACH_43(FN, E, ARG, ...)  FN(E) __FOREACH_42(FN, ARG, __VA_ARGS__)
#define __FOREACH_44(FN, E, ARG, ...)  FN(E) __FOREACH_43(FN, ARG, __VA_ARGS__)
#define __FOREACH_45(FN, E, ARG, ...)  FN(E) __FOREACH_44(FN, ARG, __VA_ARGS__)
#define __FOREACH_46(FN, E, ARG, ...)  FN(E) __FOREACH_45(FN, ARG, __VA_ARGS__)
#define __FOREACH_47(FN, E, ARG, ...)  FN(E) __FOREACH_46(FN, ARG, __VA_ARGS__)
#define __FOREACH_48(FN, E, ARG, ...)  FN(E) __FOREACH_47(FN, ARG, __VA_ARGS__)
#define __FOREACH_49(FN, E, ARG, ...)  FN(E) __FOREACH_48(FN, ARG, __VA_ARGS__)
#define __FOREACH_50(FN, E, ARG, ...)  FN(E) __FOREACH_49(FN, ARG, __VA_ARGS__)
#define __FOREACH_51(FN, E, ARG, ...)  FN(E) __FOREACH_50(FN, ARG, __VA_ARGS__)
#define __FOREACH_52(FN, E, ARG, ...)  FN(E) __FOREACH_51(FN, ARG, __VA_ARGS__)
#define __FOREACH_53(FN, E, ARG, ...)  FN(E) __FOREACH_52(FN, ARG, __VA_ARGS__)
#define __FOREACH_54(FN, E, ARG, ...)  FN(E) __FOREACH_53(FN, ARG, __VA_ARGS__)
#define __FOREACH_55(FN, E, ARG, ...)  FN(E) __FOREACH_54(FN, ARG, __VA_ARGS__)
#define __FOREACH_56(FN, E, ARG, ...)  FN(E) __FOREACH_55(FN, ARG, __VA_ARGS__)
#define __FOREACH_57(FN, E, ARG, ...)  FN(E) __FOREACH_56(FN, ARG, __VA_ARGS__)
#define __FOREACH_58(FN, E, ARG, ...)  FN(E) __FOREACH_57(FN, ARG, __VA_ARGS__)
#define __FOREACH_59(FN, E, ARG, ...)  FN(E) __FOREACH_58(FN, ARG, __VA_ARGS__)
#define __FOREACH_60(FN, E, ARG, ...)  FN(E) __FOREACH_59(FN, ARG, __VA_ARGS__)
#define __FOREACH_61(FN, E, ARG, ...)  FN(E) __FOREACH_60(FN, ARG, __VA_ARGS__)
#define __FOREACH_62(FN, E, ARG, ...)  FN(E) __FOREACH_61(FN, ARG, __VA_ARGS__)
#define __FOREACH_63(FN, E, ARG, ...)  FN(E) __FOREACH_62(FN, ARG, __VA_ARGS__)
#define __FOREACH_64(FN, E, ARG, ...)  FN(E) __FOREACH_63(FN, ARG, __VA_ARGS__)
#define __FOREACH_65(FN, E, ARG, ...)  FN(E) __FOREACH_64(FN, ARG, __VA_ARGS__)
#define __FOREACH_66(FN, E, ARG, ...)  FN(E) __FOREACH_65(FN, ARG, __VA_ARGS__)
#define __FOREACH_67(FN, E, ARG, ...)  FN(E) __FOREACH_66(FN, ARG, __VA_ARGS__)
#define __FOREACH_68(FN, E, ARG, ...)  FN(E) __FOREACH_67(FN, ARG, __VA_ARGS__)
#define __FOREACH_69(FN, E, ARG, ...)  FN(E) __FOREACH_68(FN, ARG, __VA_ARGS__)
#define __FOREACH_70(FN, E, ARG, ...)  FN(E) __FOREACH_69(FN, ARG, __VA_ARGS__)
#define __FOREACH_71(FN, E, ARG, ...)  FN(E) __FOREACH_70(FN, ARG, __VA_ARGS__)
#define __FOREACH_72(FN, E, ARG, ...)  FN(E) __FOREACH_71(FN, ARG, __VA_ARGS__)
#define __FOREACH_73(FN, E, ARG, ...)  FN(E) __FOREACH_72(FN, ARG, __VA_ARGS__)
#define __FOREACH_74(FN, E, ARG, ...)  FN(E) __FOREACH_73(FN, ARG, __VA_ARGS__)
#define __FOREACH_75(FN, E, ARG, ...)  FN(E) __FOREACH_74(FN, ARG, __VA_ARGS__)
#define __FOREACH_76(FN, E, ARG, ...)  FN(E) __FOREACH_75(FN, ARG, __VA_ARGS__)
#define __FOREACH_77(FN, E, ARG, ...)  FN(E) __FOREACH_76(FN, ARG, __VA_ARGS__)
#define __FOREACH_78(FN, E, ARG, ...)  FN(E) __FOREACH_77(FN, ARG, __VA_ARGS__)
#define __FOREACH_79(FN, E, ARG, ...)  FN(E) __FOREACH_78(FN, ARG, __VA_ARGS__)
#define __FOREACH_80(FN, E, ARG, ...)  FN(E) __FOREACH_79(FN, ARG, __VA_ARGS__)
#define __FOREACH_81(FN, E, ARG, ...)  FN(E) __FOREACH_80(FN, ARG, __VA_ARGS__)
#define __FOREACH_82(FN, E, ARG, ...)  FN(E) __FOREACH_81(FN, ARG, __VA_ARGS__)
#define __FOREACH_83(FN, E, ARG, ...)  FN(E) __FOREACH_82(FN, ARG, __VA_ARGS__)
#define __FOREACH_84(FN, E, ARG, ...)  FN(E) __FOREACH_83(FN, ARG, __VA_ARGS__)
#define __FOREACH_85(FN, E, ARG, ...)  FN(E) __FOREACH_84(FN, ARG, __VA_ARGS__)
#define __FOREACH_86(FN, E, ARG, ...)  FN(E) __FOREACH_85(FN, ARG, __VA_ARGS__)
#define __FOREACH_87(FN, E, ARG, ...)  FN(E) __FOREACH_86(FN, ARG, __VA_ARGS__)
#define __FOREACH_88(FN, E, ARG, ...)  FN(E) __FOREACH_87(FN, ARG, __VA_ARGS__)
#define __FOREACH_89(FN, E, ARG, ...)  FN(E) __FOREACH_88(FN, ARG, __VA_ARGS__)
#define __FOREACH_90(FN, E, ARG, ...)  FN(E) __FOREACH_89(FN, ARG, __VA_ARGS__)
#define __FOREACH_91(FN, E, ARG, ...)  FN(E) __FOREACH_90(FN, ARG, __VA_ARGS__)
#define __FOREACH_92(FN, E, ARG, ...)  FN(E) __FOREACH_91(FN, ARG, __VA_ARGS__)
#define __FOREACH_93(FN, E, ARG, ...)  FN(E) __FOREACH_92(FN, ARG, __VA_ARGS__)
#define __FOREACH_94(FN, E, ARG, ...)  FN(E) __FOREACH_93(FN, ARG, __VA_ARGS__)
#define __FOREACH_95(FN, E, ARG, ...)  FN(E) __FOREACH_94(FN, ARG, __VA_ARGS__)
#define __FOREACH_96(FN, E, ARG, ...)  FN(E) __FOREACH_95(FN, ARG, __VA_ARGS__)
#define __FOREACH_97(FN, E, ARG, ...)  FN(E) __FOREACH_96(FN, ARG, __VA_ARGS__)
#define __FOREACH_98(FN, E, ARG, ...)  FN(E) __FOREACH_97(FN, ARG, __VA_ARGS__)
#define __FOREACH_99(FN, E, ARG, ...)  FN(E) __FOREACH_98(FN, ARG, __VA_ARGS__)
#define __FOREACH_100(FN, E, ARG, ...)  FN(E) __FOREACH_99(FN, ARG, __VA_ARGS__)

#define __FOREACH__(FN, NARGS, ARG, ...) __FOREACH_##NARGS(FN, ARG, __VA_ARGS__)
#define __FOREACH_(FN, NARGS, ARG, ...) __FOREACH__(FN, NARGS, ARG, __VA_ARGS__)
#define __FOREACH(FN, ARG, ...) __FOREACH_(FN, __N_VA_ARGS(__VA_ARGS__), ARG, __VA_ARGS__)

#define __EXPAND_ARGS(...) __VA_ARGS__
#define __COMMA


#if !defined(SPFLIB_NO_STRING)
#   define SPF_STRING_TYPE(_fn) _fn(STRING, string, "", ...)__COMMA
#else
#   define SPF_STRING_TYPE(_fn)
#endif

#if defined(SPFLIB_VEC2)
#   define SPF_VEC2_TYPE(_fn) _fn(VEC2, Vec2, "", ...)__COMMA
#else
#   define SPF_VEC2_TYPE(_fn)
#endif


#if defined(SPF_CUSTOM_PRINT_TYPE_0)
#   define SPF_CUSTOM_PRINT_TYPE_0_DECLARED
#   define REGISTER_CUSTOM_TYPE_0(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define SPF_CUSTOM_PRINT_TYPE_0
#   define SPF_CUSTOM_PRINT_TYPE_FMT_0
#   define REGISTER_CUSTOM_TYPE_0(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_1)
#   define SPF_CUSTOM_PRINT_TYPE_1_DECLARED
#   define REGISTER_CUSTOM_TYPE_1(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define SPF_CUSTOM_PRINT_TYPE_1
#   define SPF_CUSTOM_PRINT_TYPE_FMT_1
#   define REGISTER_CUSTOM_TYPE_1(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_2)
#   define SPF_CUSTOM_PRINT_TYPE_2_DECLARED
#   define REGISTER_CUSTOM_TYPE_2(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define SPF_CUSTOM_PRINT_TYPE_2
#   define SPF_CUSTOM_PRINT_TYPE_FMT_2
#   define REGISTER_CUSTOM_TYPE_2(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_3)
#   define SPF_CUSTOM_PRINT_TYPE_3_DECLARED
#   define REGISTER_CUSTOM_TYPE_3(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define SPF_CUSTOM_PRINT_TYPE_3
#   define SPF_CUSTOM_PRINT_TYPE_FMT_3
#   define REGISTER_CUSTOM_TYPE_3(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_4)
#   define SPF_CUSTOM_PRINT_TYPE_4_DECLARED
#   define REGISTER_CUSTOM_TYPE_4(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define SPF_CUSTOM_PRINT_TYPE_4
#   define SPF_CUSTOM_PRINT_TYPE_FMT_4
#   define REGISTER_CUSTOM_TYPE_4(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_5)
#   define SPF_CUSTOM_PRINT_TYPE_5_DECLARED
#   define REGISTER_CUSTOM_TYPE_5(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define SPF_CUSTOM_PRINT_TYPE_5
#   define SPF_CUSTOM_PRINT_TYPE_FMT_5
#   define REGISTER_CUSTOM_TYPE_5(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_6)
#   define SPF_CUSTOM_PRINT_TYPE_6_DECLARED
#   define REGISTER_CUSTOM_TYPE_6(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define SPF_CUSTOM_PRINT_TYPE_6
#   define SPF_CUSTOM_PRINT_TYPE_FMT_6
#   define REGISTER_CUSTOM_TYPE_6(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_7)
#   define SPF_CUSTOM_PRINT_TYPE_7_DECLARED
#   define REGISTER_CUSTOM_TYPE_7(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define SPF_CUSTOM_PRINT_TYPE_7
#   define SPF_CUSTOM_PRINT_TYPE_FMT_7
#   define REGISTER_CUSTOM_TYPE_7(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_8)
#   define SPF_CUSTOM_PRINT_TYPE_8_DECLARED
#   define REGISTER_CUSTOM_TYPE_8(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define SPF_CUSTOM_PRINT_TYPE_8
#   define SPF_CUSTOM_PRINT_TYPE_FMT_8
#   define REGISTER_CUSTOM_TYPE_8(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_9)
#   define SPF_CUSTOM_PRINT_TYPE_9_DECLARED
#   define REGISTER_CUSTOM_TYPE_9(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define SPF_CUSTOM_PRINT_TYPE_9
#   define SPF_CUSTOM_PRINT_TYPE_FMT_9
#   define REGISTER_CUSTOM_TYPE_9(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_10)
#   define SPF_CUSTOM_PRINT_TYPE_10_DECLARED
#   define REGISTER_CUSTOM_TYPE_10(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define SPF_CUSTOM_PRINT_TYPE_10
#   define SPF_CUSTOM_PRINT_TYPE_FMT_10
#   define REGISTER_CUSTOM_TYPE_10(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_11)
#   define SPF_CUSTOM_PRINT_TYPE_11_DECLARED
#   define REGISTER_CUSTOM_TYPE_11(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define SPF_CUSTOM_PRINT_TYPE_11
#   define SPF_CUSTOM_PRINT_TYPE_FMT_11
#   define REGISTER_CUSTOM_TYPE_11(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_12)
#   define SPF_CUSTOM_PRINT_TYPE_12_DECLARED
#   define REGISTER_CUSTOM_TYPE_12(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define SPF_CUSTOM_PRINT_TYPE_12
#   define SPF_CUSTOM_PRINT_TYPE_FMT_12
#   define REGISTER_CUSTOM_TYPE_12(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_13)
#   define SPF_CUSTOM_PRINT_TYPE_13_DECLARED
#   define REGISTER_CUSTOM_TYPE_13(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define SPF_CUSTOM_PRINT_TYPE_13
#   define SPF_CUSTOM_PRINT_TYPE_FMT_13
#   define REGISTER_CUSTOM_TYPE_13(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_14)
#   define SPF_CUSTOM_PRINT_TYPE_14_DECLARED
#   define REGISTER_CUSTOM_TYPE_14(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define SPF_CUSTOM_PRINT_TYPE_14
#   define SPF_CUSTOM_PRINT_TYPE_FMT_14
#   define REGISTER_CUSTOM_TYPE_14(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_15)
#   define SPF_CUSTOM_PRINT_TYPE_15_DECLARED
#   define REGISTER_CUSTOM_TYPE_15(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define SPF_CUSTOM_PRINT_TYPE_15
#   define SPF_CUSTOM_PRINT_TYPE_FMT_15
#   define REGISTER_CUSTOM_TYPE_15(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_16)
#   define SPF_CUSTOM_PRINT_TYPE_16_DECLARED
#   define REGISTER_CUSTOM_TYPE_16(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define SPF_CUSTOM_PRINT_TYPE_16
#   define SPF_CUSTOM_PRINT_TYPE_FMT_16
#   define REGISTER_CUSTOM_TYPE_16(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_17)
#error "No more custom print type slots available."
#endif

#define CAT(a, b) CAT_(a, b)
#define CAT_(a, b) a ## b

#define SPF_CUSTOM_CASE(_n) \
    case CAT(__ARG_TYPE_, SPF_CUSTOM_PRINT_TYPE_##_n): { \
        SPF_CUSTOM_PRINT_TYPE_##_n t = va_arg(list, SPF_CUSTOM_PRINT_TYPE_##_n); \
        SPF_CUSTOM_PRINT_TYPE_FMT_##_n; \
        break; \
    }

// #define SPF_CUSTOM_PRINT_TYPE_0      Entity
// #define SPF_CUSTOM_PRINT_TYPE_FMT_0  fprintf(stream, "{ health: %d, max_health: %d }", t.health, t.max_health)


#define __ITER_TYPES(_fn, ...)                                                \
    _fn(BOOL,                _Bool,                           "", ...)__COMMA \
    _fn(FLOAT,               float,                           "", ...)__COMMA \
    _fn(DOUBLE,              double,                          "", ...)__COMMA \
    _fn(CSTR,                const char*,                     "", ...)__COMMA \
    _fn(STR,                 char*,                           "", ...)__COMMA \
    _fn(CHAR,                char,                            "", ...)__COMMA \
    _fn(SCHAR,               signed char,                     "", ...)__COMMA \
    _fn(UCHAR,               unsigned char,                   "", ...)__COMMA \
    _fn(SINT16,              signed short,                    "", ...)__COMMA \
    _fn(UINT16,              unsigned short,                  "", ...)__COMMA \
    _fn(SINT32,              signed int,                      "", ...)__COMMA \
    _fn(UINT32,              unsigned int,                    "", ...)__COMMA \
    _fn(SLONG,               signed long,                     "", ...)__COMMA \
    _fn(ULONG,               unsigned long,                   "", ...)__COMMA \
    _fn(SLLONG,              signed long long,                "", ...)__COMMA \
    _fn(ULLONG,              unsigned long long,              "", ...)__COMMA \
    SPF_STRING_TYPE(_fn) \
    SPF_VEC2_TYPE(_fn) \
    REGISTER_CUSTOM_TYPE_0(_fn, SPF_CUSTOM_PRINT_TYPE_0, SPF_CUSTOM_PRINT_TYPE_FMT_0) \
    REGISTER_CUSTOM_TYPE_1(_fn, SPF_CUSTOM_PRINT_TYPE_1, SPF_CUSTOM_PRINT_TYPE_FMT_1) \
    REGISTER_CUSTOM_TYPE_2(_fn, SPF_CUSTOM_PRINT_TYPE_2, SPF_CUSTOM_PRINT_TYPE_FMT_2) \
    REGISTER_CUSTOM_TYPE_3(_fn, SPF_CUSTOM_PRINT_TYPE_3, SPF_CUSTOM_PRINT_TYPE_FMT_3) \
    REGISTER_CUSTOM_TYPE_4(_fn, SPF_CUSTOM_PRINT_TYPE_4, SPF_CUSTOM_PRINT_TYPE_FMT_4) \
    REGISTER_CUSTOM_TYPE_5(_fn, SPF_CUSTOM_PRINT_TYPE_5, SPF_CUSTOM_PRINT_TYPE_FMT_5) \
    REGISTER_CUSTOM_TYPE_6(_fn, SPF_CUSTOM_PRINT_TYPE_6, SPF_CUSTOM_PRINT_TYPE_FMT_6) \
    REGISTER_CUSTOM_TYPE_7(_fn, SPF_CUSTOM_PRINT_TYPE_7, SPF_CUSTOM_PRINT_TYPE_FMT_7) \
    REGISTER_CUSTOM_TYPE_8(_fn, SPF_CUSTOM_PRINT_TYPE_8, SPF_CUSTOM_PRINT_TYPE_FMT_8) \
    REGISTER_CUSTOM_TYPE_9(_fn, SPF_CUSTOM_PRINT_TYPE_9, SPF_CUSTOM_PRINT_TYPE_FMT_9) \
    REGISTER_CUSTOM_TYPE_10(_fn, SPF_CUSTOM_PRINT_TYPE_10, SPF_CUSTOM_PRINT_TYPE_FMT_10) \
    REGISTER_CUSTOM_TYPE_11(_fn, SPF_CUSTOM_PRINT_TYPE_11, SPF_CUSTOM_PRINT_TYPE_FMT_11) \
    REGISTER_CUSTOM_TYPE_12(_fn, SPF_CUSTOM_PRINT_TYPE_12, SPF_CUSTOM_PRINT_TYPE_FMT_12) \
    REGISTER_CUSTOM_TYPE_13(_fn, SPF_CUSTOM_PRINT_TYPE_13, SPF_CUSTOM_PRINT_TYPE_FMT_13) \
    REGISTER_CUSTOM_TYPE_14(_fn, SPF_CUSTOM_PRINT_TYPE_14, SPF_CUSTOM_PRINT_TYPE_FMT_14) \
    REGISTER_CUSTOM_TYPE_15(_fn, SPF_CUSTOM_PRINT_TYPE_15, SPF_CUSTOM_PRINT_TYPE_FMT_15) \
    REGISTER_CUSTOM_TYPE_16(_fn, SPF_CUSTOM_PRINT_TYPE_16, SPF_CUSTOM_PRINT_TYPE_FMT_16) \
    _fn(PTR,                 default,                         "", ...)

#define __ITER_TYPES_PROMOTED(_fn, ...)                                                                   \
    _fn(BOOL,                      int,                 "u",                                 ...)__COMMA \
    _fn(FLOAT,                     double,              "g",                                 ...)__COMMA \
    _fn(DOUBLE,                    double,              "g",                                 ...)__COMMA \
    _fn(CSTR,                      const char*,         "s",                                 ...)__COMMA \
    _fn(STR,                       char*,               "s",                                 ...)__COMMA \
    _fn(CHAR,                      int,                 "c",                                 ...)__COMMA \
    _fn(SCHAR,                     int,                 "d",                                 ...)__COMMA \
    _fn(UCHAR,                     int,                 "u",                                 ...)__COMMA \
    _fn(SINT16,                    int,                 "d",                                 ...)__COMMA \
    _fn(UINT16,                    int,                 "u",                                 ...)__COMMA \
    _fn(SINT32,                    signed int,          "d",                                 ...)__COMMA \
    _fn(UINT32,                    unsigned int,        "u",                                 ...)__COMMA \
    _fn(SLONG,                     signed long,         "ld",                                ...)__COMMA \
    _fn(ULONG,                     unsigned long,       "lu",                                ...)__COMMA \
    _fn(SLLONG,                    signed long long,    "lld",                               ...)__COMMA \
    _fn(ULLONG,                    unsigned long long,  "llu",                               ...)__COMMA \
    _fn(PTR,                       void*,               "p",                                 ...)

typedef enum
{
    #define __DECLARE_ENUM_VAL(up, lw, ...) __ARG_TYPE_##up,
    __ITER_TYPES(__DECLARE_ENUM_VAL)
    #undef __DECLARE_ENUM_VAL
} __ArgType;

#undef __COMMA
#define __COMMA ,

#define __TYPE_ENUM(_uc, _lc, ...) _lc: __ARG_TYPE_##_uc

#define __TYPE_TO_ENUM_TYPE(type) _Generic((type), \
    __ITER_TYPES(__TYPE_ENUM)                      \
),

#define print_stream(stream, fmt, ...)                                                                      \
    __print(                                                                                                \
        stream,                                                                                             \
        fmt,                                                                                                \
        __N_VA_ARGS(__VA_ARGS__)__VA_OPT__(,)                                                               \
        __FOREACH(__TYPE_TO_ENUM_TYPE, __EXPAND_ARGS(__VA_ARGS__), __VA_ARGS__) __VA_OPT__(0) __VA_OPT__(,) \
        __VA_ARGS__                                                                                         \
    )

#define print(fmt, ...)                                                                                     \
    __print(                                                                                                \
        stdout,                                                                                             \
        fmt,                                                                                                \
        __N_VA_ARGS(__VA_ARGS__)__VA_OPT__(,)                                                               \
        __FOREACH(__TYPE_TO_ENUM_TYPE, __EXPAND_ARGS(__VA_ARGS__), __VA_ARGS__) __VA_OPT__(0) __VA_OPT__(,) \
        __VA_ARGS__                                                                                         \
    )

#define println_stream(stream, fmt, ...)                                                                        \
    do {                                                                                                        \
        __print(                                                                                                \
            stream,                                                                                             \
            fmt,                                                                                                \
            __N_VA_ARGS(__VA_ARGS__)__VA_OPT__(,)                                                               \
            __FOREACH(__TYPE_TO_ENUM_TYPE, __EXPAND_ARGS(__VA_ARGS__), __VA_ARGS__) __VA_OPT__(0) __VA_OPT__(,) \
            __VA_ARGS__                                                                                         \
        );                                                                                                      \
        putc('\n', stdout);                                                                                     \
    } while (0)

#define println(fmt, ...)                                                                                       \
    do {                                                                                                        \
        __print(                                                                                                \
            stdout,                                                                                             \
            fmt,                                                                                                \
            __N_VA_ARGS(__VA_ARGS__)__VA_OPT__(,)                                                               \
            __FOREACH(__TYPE_TO_ENUM_TYPE, __EXPAND_ARGS(__VA_ARGS__), __VA_ARGS__) __VA_OPT__(0) __VA_OPT__(,) \
            __VA_ARGS__                                                                                         \
        );                                                                                                      \
        putc('\n', stdout);                                                                                     \
    } while (0)

#if defined(SPFLIB_NO_STRING)
    static size_t str_len(const char* str)
    {
        size_t c = 0;
        while (*str++) { c++; }
        return c;
    }
#endif

#if defined(COMPILER_MSVC)
#   include <malloc.h>    // _alloca
#endif

static void __print(FILE* stream, const char* fmt, size_t arg_count, ...)
{
    if (arg_count == 0) {
        for (size_t i = 0; i < str_len(fmt); i++) {
            putc(fmt[i], stream);
        }
        return;
    }

    va_list list;
    va_start(list, arg_count);

    size_t fmt_len = str_len(fmt);

#if defined(COMPILER_MSVC)
    __ArgType* arg_types = _alloca(sizeof(__ArgType) * arg_count);
#else
    __ArgType arg_types[arg_count];
#endif

    for (size_t i = 0; i < arg_count; i++) {
        arg_types[i] = (__ArgType)va_arg(list, int);
    }

    va_arg(list, int); // drain the useless 0

    size_t current_arg_type = 0;

    // if (fmt[c] == '{' &&
    //     ((c + 1) <= fmt_len) &&
    //     fmt[c + 1] != '\0')

    size_t c = 0;
    while (c < fmt_len) {
        if (fmt[c] == '%' &&
            ((c + 1) <= fmt_len) &&
            fmt[c + 1] != '%')
        {
            // char format[64];
            // format[0] = '%';

            // size_t n = c + 1;
            // size_t i = 1;

            // while (n <= fmt_len) {
            //     if (fmt[n] == '}') break;
            //     format[i] = fmt[n];
            //     n++;
            // }

            __ArgType type = arg_types[current_arg_type];
            // strncat(format + i, _fmt, sizeof(_fmt));

            #define __TYPE_CASE(_up, _lw, _fmt, ...) \
                case __ARG_TYPE_##_up: { \
                    fprintf(stream, "%"_fmt, va_arg(list, _lw)); \
                    break; \
                }

            #undef __COMMA
            #define __COMMA

            switch (type) {
                __ITER_TYPES_PROMOTED(__TYPE_CASE)
#if !defined(SPFLIB_NO_STRING)
                case __ARG_TYPE_STRING: {
                    string str = va_arg(list, string);

                    for (size_t i = 0; i < str.len; i++) {
                        putc(str.data[i], stream);
                    }

                    break;
                }
#endif
#if defined(SPFLIB_VEC2)
                case __ARG_TYPE_VEC2: {
                    Vec2 v = va_arg(list, Vec2);
                    fprintf(stream, "{ x: %.3f, y: %.3f }", v.x, v.y);
                    break;
                }
#endif

// custom types
#if defined(SPF_CUSTOM_PRINT_TYPE_0_DECLARED)
SPF_CUSTOM_CASE(0)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_1_DECLARED)
SPF_CUSTOM_CASE(1)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_2_DECLARED)
SPF_CUSTOM_CASE(2)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_3_DECLARED)
SPF_CUSTOM_CASE(3)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_4_DECLARED)
SPF_CUSTOM_CASE(4)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_5_DECLARED)
SPF_CUSTOM_CASE(5)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_6_DECLARED)
SPF_CUSTOM_CASE(6)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_7_DECLARED)
SPF_CUSTOM_CASE(7)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_8_DECLARED)
SPF_CUSTOM_CASE(8)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_9_DECLARED)
SPF_CUSTOM_CASE(9)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_10_DECLARED)
SPF_CUSTOM_CASE(10)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_11_DECLARED)
SPF_CUSTOM_CASE(11)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_12_DECLARED)
SPF_CUSTOM_CASE(12)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_13_DECLARED)
SPF_CUSTOM_CASE(13)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_14_DECLARED)
SPF_CUSTOM_CASE(14)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_15_DECLARED)
SPF_CUSTOM_CASE(15)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_16_DECLARED)
SPF_CUSTOM_CASE(16)
#endif
                default: break;
            }

            #undef __TYPE_CASE

            #undef __COMMA
            #define __COMMA ,

            current_arg_type++;
        }
        else {
            putc(fmt[c], stream);
        }

        c++;
    }

    va_end(list);
}

#else // STDC_VER >= 11
#error "Only C11 or higher is supported."
#endif // STDC_VER >= 11

#endif // !defined(SPFLIB_NO_IO)

// --- logger --- //

#if !defined(SPFLIB_NO_LOGGER)

#include <stdarg.h>   // va_list
#include <string.h>   // strncat
#include <stdio.h>    // fprintf, vfprintf

#if defined(SPFLIB_LOG_TIME)
#   include <time.h>
#endif

// #if !defined(SPFLIB_NO_IO)
//
// #endif

typedef enum
{
    __SPF_LEVEL_INFO,
    __SPF_LEVEL_WARNING,
    __SPF_LEVEL_ERROR
} __SPF_Level;

#define LOG(_lvl, _msg, ...) __spf_log(__SPF_LEVEL_##_lvl, _msg __VA_OPT__(, __VA_ARGS__))

// TODO: 
// - add suport for spf io printing
// - add file logging

static void __spf_log(__SPF_Level lvl, const char* msg, ...)
{
    va_list list;
    va_start(list, msg);

#if defined(SPFLIB_LOG_TIME)
    time_t t          = time(nullptr);
    struct tm tm      = *localtime(&t);

    static char log_name[32];

    snprintf(log_name, sizeof(log_name), "[%d-%d-%d %02d:%02d:%02d] ",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
            tm.tm_min, tm.tm_sec);
#endif

    switch (lvl) {
        case __SPF_LEVEL_INFO: {
#if defined(SPFLIB_LOG_TIME)
            strncat(log_name, "[INFO] ", sizeof(char) * 8);
            fprintf(stdout, "%s", log_name);
#else
            fprintf(stdout, "[INFO] ");
#endif
            vfprintf(stdout, msg, list);
            fprintf(stdout, "\n");
            break;
        }
        case __SPF_LEVEL_WARNING: {
#if defined(SPFLIB_LOG_TIME)
            strncat(log_name, "[WARNING] ", sizeof(char) * 11);
            fprintf(stdout, "%s", log_name);
#else
            fprintf(stdout, "[WARNING] ");
#endif
            vfprintf(stdout, msg, list);
            fprintf(stdout, "\n");
            break;
        }
        case __SPF_LEVEL_ERROR: {
#if defined(SPFLIB_LOG_TIME)
            strncat(log_name, "[ERROR] ", sizeof(char) * 9);
            fprintf(stderr, "%s", log_name);
#else
            fprintf(stderr, "[ERROR] ");
#endif
            vfprintf(stderr, msg, list);
            fprintf(stderr, "\n");
            break;
        }
        default: break;
    }

    va_end(list);
}

#endif // !defined(SPFLIB_NO_LOGGER)

#if defined(__GNUC__)
#   pragma GCC diagnostic pop
#elif defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(_MSC_VER)
#   pragma warning( pop )
#endif

// disable warning about using __VA_OPT__ prior to C23
#if defined(COMPILER_MSVC) && STDC_VER < 23
#   pragma warning( disable : 5110 )
#endif

#endif // SPFLIB_H

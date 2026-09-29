#ifndef SPFLIB_H
#define SPFLIB_H

#if defined(__cplusplus)
#   error "This library does not support C++ due to C11 exclusive features like _Generic."
#endif

#if __STDC_VERSION__ < 201112L
#   error "Version at least C11 is required to use this library."
#endif

#if defined(_MSC_VER) && !defined(__clang__) && _MSC_VER < 1944
#   error("MSVC version at least 19.44 is required.")
#elif defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 8
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
// - scratch buffer(linear allocator) for temporary allocations
// - is nan anf infinity checks
// - string to int, string to float etc
// - safe string operations like str_copy, str_cat etc.
// - reflection
// - window creation (RGFW) and opengl context
// - file io
// - matrix 3x3
// - good rand num gen
// - customizable lerp
// - profiling
// - platform specific actions like making directories, clipboard

#if defined(_WIN32)
#   define OS_WINDOWS
#   define WIN32_LEAN_AND_MEAN
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

#if STDC_VER < 23
#   define static_assert  _Static_assert
#   define alignas        _Alignas
#   define alignof        _Alignof
#   define thread_local   _Thread_local
#endif

#define atomic _Atomic

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

#define TO_RADIANS(_ang_deg)  ((_ang_deg) * SPF_PI / 180.)
#define TO_DEGREES(_ang_rad)  ((_ang_rad) * 180. / SPF_PI)

#define TO_KiB(_n)   ((_n) / 1024)
#define TO_MiB(_n)   TO_KiB((_n)) / 1024
#define TO_GiB(_n)   TO_MiB((_n)) / 1024
#define TO_TiB(_n)   TO_GiB((_n)) / 1024

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

// --- plaform --- //
#if defined(SPFLIB_PLATFORM)

#if defined(OS_LINUX)

#include <stdlib.h>  // getenv
#include <string.h>  // strcmp

typedef enum
{
    LWS_UNKNOWN = -1,
    LWS_X11,
    LWS_WAYLAND,
} LinuxWindowingSystem;

static LinuxWindowingSystem linux_windowing_system()
{
    const char* type = getenv("XDG_SESSION_TYPE");

    if      (strcmp(type, "x11")     == 0) return LWS_X11;
    else if (strcmp(type, "wayland") == 0) return LWS_WAYLAND;
    else
        return LWS_UNKNOWN;
}

#endif // defined(OS_LINUX)

#if defined(OS_WINDOWS)
#   include <windows.h>
// thanks windows very cool 👍
#   undef near
#   undef far 
#   include <Lmcons.h>      // UNLEN
#elif defined(OS_POSIX)
#   include <sys/types.h>   // uid_t
#   include <unistd.h>      // rmdir, geteuid
#   include <pwd.h>         // geteuid, getpwuid
#   include <stdlib.h>      // getenv
#endif

static const char* os_user_name()
{
#if defined(OS_WINDOWS)
    static char name[UNLEN + 1];
    DWORD sz = UNLEN + 1;
    GetUserNameA(name, &sz);
    return name;
#elif defined(OS_POSIX)
    const char* name = getenv("USER");
    if (name != nullptr) return name;

    struct passwd *pw;
    uid_t uid;

    uid = geteuid();
    pw = getpwuid(uid);

    if (pw != nullptr) return pw->pw_name;
    return "unknown";
#endif
}

#endif // defined(SPFLIB_PLATFORM)

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

#define str_compare(str1, str2) _Generic((str2), \
    string:      str_compare_str,                \
    const char*: str_compare_cstr,               \
    char*:       str_compare_cstr                \
) ((str1), (str2))

// TODO: add str_to_cstr

static size_t str_len(const char* str)
{
    size_t c = 0;
    while (*str++) { c++; }
    return c;
}

static void str_cpy(char* restrict dest, char* restrict src, size_t num)
{
    for (size_t i = 0; i < num; i++) {
        dest[i] = src[i];
    }
    dest[num] = '\0';
}

// static bool str_equal(char* str_a, char* str_b)
// {
//     while (*str_a++) {
//         if (*str_a != *str_b++) return false;
//     }
//     return true;
// }

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

static Vec2 vec2_make(float xy)
{
    return (Vec2){ .x = xy, .y = xy };
}

static Vec2 vec2_init(float x, float y)
{
    return (Vec2){ .x = x, .y = y };
}

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

static Vec2 vec2_addv(Vec2 a, Vec2 b)
{
    return (Vec2){ .x = a.x + b.x, .y = a.y + b.y };
}

static Vec2 vec2_adds(Vec2 v, float s)
{
    return (Vec2){ .x = v.x + s, .y = v.y + s };
}

static Vec2 vec2_subv(Vec2 a, Vec2 b)
{
    return (Vec2){ .x = a.x - b.x, .y = a.y - b.y };
}

static Vec2 vec2_subs(Vec2 v, float s)
{
    return (Vec2){ .x = v.x - s, .y = v.y - s };
}

static Vec2 vec2_mulv(Vec2 a, Vec2 b)
{
    return (Vec2){ .x = a.x * b.x, .y = a.y * b.y };
}

static Vec2 vec2_muls(Vec2 v, float s)
{
    return (Vec2){ .x = v.x * s, .y = v.y * s };
}

static Vec2 vec2_divv(Vec2 a, Vec2 b)
{
    return (Vec2){ .x = a.x / b.x, .y = a.y / b.y };
}

static Vec2 vec2_divs(Vec2 v, float s)
{
    return (Vec2){ .x = v.x / s, .y = v.y / s };
}

//

static float vec2_dot(Vec2 a, Vec2 b)
{
    return a.x * b.x + a.y * b.y;
}

static float vec2_cross(Vec2 a, Vec2 b)
{
    return a.x * b.y - a.y * b.x;
}

static float vec2_mag(Vec2 v)
{
    return sqrtf((v.x * v.x) + (v.y * v.y));
}

static Vec2 vec2_norm(Vec2 v)
{
    float len = vec2_mag(v);
    if (len <= 0.f) return (Vec2){0};

    return (Vec2){ .x = v.x * (1.f / len), .y = v.y * (1.f / len) };
}

static Vec2 vec2_lerp(Vec2 a, Vec2 b, float s)
{
    return (Vec2){
        .x = a.x + s * (b.x - a.x),
        .y = a.y + s * (b.y - a.y)
    };
}

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

#define __Vec3_Args(_1, _2, _3, x, ...) x
#define Vec3(...) __Vec3_Args(__VA_ARGS__, vec3_init(__VA_ARGS__), vec3_init(__VA_ARGS__, 0.f), vec3_make(__VA_ARGS__))

static Vec3 vec3_make(float xyz)
{
    return (Vec3){ .x = xyz, .y = xyz, .z = xyz };
}

static Vec3 vec3_init(float x, float y, float z)
{
    return (Vec3){ .x = x, .y = y, .z = z };
}

#define vec3_add(v, s) _Generic((s), \
    Vec3: vec3_addv, \
    float: vec3_adds \
) ((v), (s))
#define vec3_sub(v, s) _Generic((s), \
    Vec3: vec3_subv, \
    float: vec3_subs \
) ((v), (s))
#define vec3_scale(v, s) _Generic((s), \
    Vec3: vec3_mulv, \
    float: vec3_muls \
) ((v), (s))
#define vec3_div(v, s) _Generic((s), \
    Vec3: vec3_divv, \
    float: vec3_divs \
) ((v), (s))

static Vec3 vec3_addv(Vec3 a, Vec3 b)
{
    return (Vec3){
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z
    };
}

static Vec3 vec3_adds(Vec3 v, float s)
{
    return (Vec3){
        .x = v.x + s,
        .y = v.y + s,
        .z = v.z + s
    };
}

static Vec3 vec3_subv(Vec3 a, Vec3 b)
{
    return (Vec3){
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z
    };
}

static Vec3 vec3_subs(Vec3 v, float s)
{
    return (Vec3){
        .x = v.x - s,
        .y = v.y - s,
        .z = v.z - s
    };
}

static Vec3 vec3_mulv(Vec3 a, Vec3 b)
{
    return (Vec3){
        .x = a.x * b.x,
        .y = a.y * b.y,
        .z = a.z * b.z
    };
}

static Vec3 vec3_muls(Vec3 v, float s)
{
    return (Vec3){
        .x = v.x * s,
        .y = v.y * s,
        .z = v.z * s
    };
}

static Vec3 vec3_divv(Vec3 a, Vec3 b)
{
    return (Vec3){
        .x = a.x / b.x,
        .y = a.y / b.y,
        .z = a.z / b.z
    };
}

static Vec3 vec3_divs(Vec3 v, float s)
{
    return (Vec3){
        .x = v.x / s,
        .y = v.y / s,
        .z = v.z / s
    };
}

//

static float vec3_dot(Vec3 a, Vec3 b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

static Vec3 vec3_cross(Vec3 a, Vec3 b)
{
    return (Vec3){
        .x = a.y * b.z - a.z * b.y,
        .y = a.z * b.x - a.x * b.z,
        .z = a.x * b.y - a.y * b.x
    };
}

static float vec3_mag(Vec3 v)
{
    return sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

static Vec3 vec3_norm(Vec3 v)
{
    float len = vec3_mag(v);
    if (len == 0.f) return (Vec3){0};

    return (Vec3){
        .x = v.x * (1.f / len),
        .y = v.y * (1.f / len),
        .z = v.z * (1.f / len)
    };
}

static Vec3 vec3_lerp(Vec3 a, Vec3 b, float s)
{
    return (Vec3){
        .x = a.x + s * (b.x - a.x),
        .y = a.y + s * (b.y - a.y),
        .z = a.z + s * (b.z - a.z)
    };
}

#endif // defined(SPFLIB_VEC3)

// --- matrices --- //

#if defined(SPFLIB_MATRIX4)

#include <math.h>    // sqrtf, sin, cos

typedef struct
{
    union {
        struct {
            float m0, m4, m8,  m12;
            float m1, m5, m9,  m13;
            float m2, m6, m10, m14;
            float m3, m7, m11, m15;
        };
        float m[4][4];
    };
} Mat4;

#define mat4_scale(a, b) _Generic((b), \
    Mat4:  mat4_scalem, \
    float: mat4_scalef \
) ((a), (b))

#if defined(SPFLIB_VEC3)
    #define mat4_mul(a, b) _Generic((b), \
        Mat4:  mat4_mulm, \
        Vec3:  mat4_mulv3 \
    ) ((a), (b))
#else
    #define mat4_mul(a, b) _Generic((b), \
        Mat4:  mat4_mulm \
    ) ((a), (b))
#endif

#define __mat4_translate_args(_1, _2, _3, x, ...) x
#define mat4_translate(...) __mat4_translate_args(__VA_ARGS__,  \
                                mat4_translatev(__VA_ARGS__),\
                                mat4_translatev(__VA_ARGS__, 0.f), \
                                mat4_translatev3(__VA_ARGS__))

#define Mat4(_m) mat4_uniform((_m))
#define mat4_ptr(_m) &_m.m0

static Mat4 mat4_identity()
{
    return (Mat4){
        .m0 = 1.f, .m4 = 0.f, .m8  = 0.f, .m12 = 0.f,
        .m1 = 0.f, .m5 = 1.f, .m9  = 0.f, .m13 = 0.f,
        .m2 = 0.f, .m6 = 0.f, .m10 = 1.f, .m14 = 0.f,
        .m3 = 0.f, .m7 = 0.f, .m11 = 0.f, .m15 = 1.f
    };
}

static Mat4 mat4_zero()
{
    return (Mat4){0};
}

static Mat4 mat4_uniform(float v)
{
    return (Mat4){
        .m0 = v,   .m4 = 0.f, .m8  = 0.f, .m12 = 0.f,
        .m1 = 0.f, .m5 = v,   .m9  = 0.f, .m13 = 0.f,
        .m2 = 0.f, .m6 = 0.f, .m10 = v,   .m14 = 0.f,
        .m3 = 0.f, .m7 = 0.f, .m11 = 0.f, .m15 = 1.f
    };
}

static Mat4 mat4_add(Mat4 a, Mat4 b)
{
    Mat4 dest;

    dest.m0 =  a.m0  + b.m0; dest.m4 =  a.m4  + b.m4; dest.m8 =  a.m8  + b.m8; dest.m12 = a.m12 + b.m12;
    dest.m1 =  a.m1  + b.m1; dest.m5 =  a.m5  + b.m5; dest.m9 =  a.m9  + b.m9; dest.m13 = a.m13 + b.m13;
    dest.m2 =  a.m2  + b.m2; dest.m6 =  a.m6  + b.m6; dest.m10 = a.m10 + b.m10; dest.m14 = a.m14 + b.m14;
    dest.m3 =  a.m3  + b.m3; dest.m7 =  a.m7  + b.m7; dest.m11 = a.m11 + b.m11; dest.m15 = a.m15 + b.m15;

    return dest;
}

static Mat4 mat4_sub(Mat4 a, Mat4 b)
{
    Mat4 dest;

    dest.m0 =  a.m0  - b.m0; dest.m4 =  a.m4  - b.m4; dest.m8 =  a.m8  - b.m8; dest.m12 = a.m12 - b.m12;
    dest.m1 =  a.m1  - b.m1; dest.m5 =  a.m5  - b.m5; dest.m9 =  a.m9  - b.m9; dest.m13 = a.m13 - b.m13;
    dest.m2 =  a.m2  - b.m2; dest.m6 =  a.m6  - b.m6; dest.m10 = a.m10 - b.m10; dest.m14 = a.m14 - b.m14;
    dest.m3 =  a.m3  - b.m3; dest.m7 =  a.m7  - b.m7; dest.m11 = a.m11 - b.m11; dest.m15 = a.m15 - b.m15;

    return dest;
}

static Mat4 mat4_scalem(Mat4 a, Mat4 b)
{
    Mat4 dest;

    dest.m0 =  a.m0  * b.m0; dest.m4 =  a.m4  * b.m4; dest.m8 =  a.m8  * b.m8; dest.m12 = a.m12 * b.m12;
    dest.m1 =  a.m1  * b.m1; dest.m5 =  a.m5  * b.m5; dest.m9 =  a.m9  * b.m9; dest.m13 = a.m13 * b.m13;
    dest.m2 =  a.m2  * b.m2; dest.m6 =  a.m6  * b.m6; dest.m10 = a.m10 * b.m10; dest.m14 = a.m14 * b.m14;
    dest.m3 =  a.m3  * b.m3; dest.m7 =  a.m7  * b.m7; dest.m11 = a.m11 * b.m11; dest.m15 = a.m15 * b.m15;

    return dest;
}

static Mat4 mat4_scalef(Mat4 a, float s)
{
    Mat4 dest;

    dest.m0 =  a.m0  * s; dest.m4 =  a.m4  * s; dest.m8 =  a.m8  * s; dest.m12 = a.m12 * s;
    dest.m1 =  a.m1  * s; dest.m5 =  a.m5  * s; dest.m9 =  a.m9  * s; dest.m13 = a.m13 * s;
    dest.m2 =  a.m2  * s; dest.m6 =  a.m6  * s; dest.m10 = a.m10 * s; dest.m14 = a.m14 * s;
    dest.m3 =  a.m3  * s; dest.m7 =  a.m7  * s; dest.m11 = a.m11 * s; dest.m15 = a.m15 * s;

    return dest;
}

static Mat4 mat4_mulm(Mat4 a, Mat4 b)
{
    Mat4 dest;

    dest.m0  = a.m0 * b.m0 + a.m1 * b.m4 + a.m2 * b.m8 + a.m3 * b.m12;
    dest.m1  = a.m0 * b.m1 + a.m1 * b.m5 + a.m2 * b.m9 + a.m3 * b.m13;
    dest.m2  = a.m0 * b.m2 + a.m1 * b.m6 + a.m2 * b.m10 + a.m3 * b.m14;
    dest.m3  = a.m0 * b.m3 + a.m1 * b.m7 + a.m2 * b.m11 + a.m3 * b.m15;

    dest.m4  = a.m4 * b.m0 + a.m5 * b.m4 + a.m6 * b.m8 + a.m7 * b.m12;
    dest.m5  = a.m4 * b.m1 + a.m5 * b.m5 + a.m6 * b.m9 + a.m7 * b.m13;
    dest.m6  = a.m4 * b.m2 + a.m5 * b.m6 + a.m6 * b.m10 + a.m7 * b.m14;
    dest.m7  = a.m4 * b.m3 + a.m5 * b.m7 + a.m6 * b.m11 + a.m7 * b.m15;

    dest.m8  = a.m8 * b.m0 + a.m9 * b.m4 + a.m10 * b.m8 + a.m11 * b.m12;
    dest.m9  = a.m8 * b.m1 + a.m9 * b.m5 + a.m10 * b.m9 + a.m11 * b.m13;
    dest.m10 = a.m8 * b.m2 + a.m9 * b.m6 + a.m10 * b.m10 + a.m11 * b.m14;
    dest.m11 = a.m8 * b.m3 + a.m9 * b.m7 + a.m10 * b.m11 + a.m11 * b.m15;

    dest.m12 = a.m12 * b.m0 + a.m13 * b.m4 + a.m14 * b.m8 + a.m15 * b.m12;
    dest.m13 = a.m12 * b.m1 + a.m13 * b.m5 + a.m14 * b.m9 + a.m15 * b.m13;
    dest.m14 = a.m12 * b.m2 + a.m13 * b.m6 + a.m14 * b.m10 + a.m15 * b.m14;
    dest.m15 = a.m12 * b.m3 + a.m13 * b.m7 + a.m14 * b.m11 + a.m15 * b.m15;

    return dest;
}

#if defined(SPFLIB_VEC3)
    static Vec3 mat4_mulv3(Mat4 m, Vec3 v)
    {
        return (Vec3){
            .x = m.m0  * v.x + m.m4  * v.y + m.m8  * v.z + m.m12,
            .y = m.m1  * v.x + m.m5  * v.y + m.m9  * v.z + m.m13,
            .z = m.m2  * v.x + m.m6  * v.y + m.m10 * v.z + m.m14
        };
    }
#endif

static Mat4 mat4_inv(Mat4 m)
{
    float a00 = m.m0, a01 = m.m1, a02 = m.m2, a03 = m.m3;
    float a10 = m.m4, a11 = m.m5, a12 = m.m6, a13 = m.m7;
    float a20 = m.m8, a21 = m.m9, a22 = m.m10, a23 = m.m11;
    float a30 = m.m12, a31 = m.m13, a32 = m.m14, a33 = m.m15;

    float b00 = a00 * a11 - a01 * a10;
    float b01 = a00 * a12 - a02 * a10;
    float b02 = a00 * a13 - a03 * a10;
    float b03 = a01 * a12 - a02 * a11;
    float b04 = a01 * a13 - a03 * a11;
    float b05 = a02 * a13 - a03 * a12;
    float b06 = a20 * a31 - a21 * a30;
    float b07 = a20 * a32 - a22 * a30;
    float b08 = a20 * a33 - a23 * a30;
    float b09 = a21 * a32 - a22 * a31;
    float b10 = a21 * a33 - a23 * a31;
    float b11 = a22 * a33 - a23 * a32;

    float inv_det = 1.f / (b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06);

    Mat4 r;

    r.m0 = (a11 * b11 - a12 * b10 + a13 * b09) * inv_det;
    r.m1 = (-a01 * b11 + a02 * b10 - a03 * b09) * inv_det;
    r.m2 = (a31 * b05 - a32 * b04 + a33 * b03) * inv_det;
    r.m3 = (-a21 * b05 + a22 * b04 - a23 * b03) * inv_det;

    r.m4 = (-a10 * b11 + a12 * b08 - a13 * b07) * inv_det;
    r.m5 = (a00 * b11 - a02 * b08 + a03 * b07) * inv_det;
    r.m6 = (-a30 * b05 + a32 * b02 - a33 * b01) * inv_det;
    r.m7 = (a20 * b05 - a22 * b02 + a23 * b01) * inv_det;

    r.m8 = (a10 * b10 - a11 * b08 + a13 * b06) * inv_det;
    r.m9 = (-a00 * b10 + a01 * b08 - a03 * b06) * inv_det;
    r.m10 = (a30 * b04 - a31 * b02 + a33 * b00) * inv_det;
    r.m11 = (-a20 * b04 + a21 * b02 - a23 * b00) * inv_det;

    r.m12 = (-a10 * b09 + a11 * b07 - a12 * b06) * inv_det;
    r.m13 = (a00 * b09 - a01 * b07 + a02 * b06) * inv_det;
    r.m14 = (-a30 * b03 + a31 * b01 - a32 * b00) * inv_det;
    r.m15 = (a20 * b03 - a21 * b01 + a22 * b00) * inv_det;

    return r;
}

#if defined(SPFLIB_VEC3)
    static Mat4 mat4_rotate(float angle_rad, Vec3 axis)
    {
        Mat4 r;

        Vec3 n = vec3_norm(axis);

        float s = sinf(angle_rad);
        float c = cosf(angle_rad);
        float t = 1.f - c;

        r.m0 = n.x * n.x * t + c;
        r.m1 = n.y * n.x * t + n.z * s;
        r.m2 = n.z * n.x * t - n.y * s;
        r.m3 = 0.f;

        r.m4 = n.x * n.y * t - n.z * s;
        r.m5 = n.y * n.y * t + c;
        r.m6 = n.z * n.y * t + n.x * s;
        r.m7 = 0.f;

        r.m8 = n.x * n.z * t + n.y * s;
        r.m9 = n.y * n.z * t - n.x * s;
        r.m10 = n.z * n.z * t + c;
        r.m11 = 0.f;

        r.m12 = 0.f;
        r.m13 = 0.f;
        r.m14 = 0.f;
        r.m15 = 1.f;

        return r;
    }
#endif // if defined(SPFLIB_VEC3)

static Mat4 mat4_rotate_x(float angle_rad)
{
    Mat4 r  = Mat4(1.f);

    float c = cosf(angle_rad);
    float s = sinf(angle_rad);

    r.m5  =  c;
    r.m6  =  s;
    r.m9  = -s;
    r.m10 =  c;

    return r;
}

static Mat4 mat4_rotate_y(float angle_rad)
{
    Mat4 r  = Mat4(1.f);

    float c = cosf(angle_rad);
    float s = sinf(angle_rad);

    r.m0  =  c;
    r.m2  =  s;
    r.m8  = -s;
    r.m10 =  c;

    return r;
}

static Mat4 mat4_rotate_z(float angle_rad)
{
    Mat4 r  = Mat4(1.f);

    float c = cosf(angle_rad);
    float s = sinf(angle_rad);

    r.m0  =  c;
    r.m1  =  s;
    r.m4  = -s;
    r.m5  =  c;

    return r;
}

static Mat4 mat4_translatev(float x, float y, float z)
{
    return (Mat4){
        .m0 = 1.f, .m4 = 0.f, .m8  = 0.f, .m12 = x,
        .m1 = 0.f, .m5 = 1.f, .m9  = 0.f, .m13 = y,
        .m2 = 0.f, .m6 = 0.f, .m10 = 1.f, .m14 = z,
        .m3 = 0.f, .m7 = 0.f, .m11 = 0.f, .m15 = 1.f
    };
}

#if defined(SPFLIB_VEC3)
    static Mat4 mat4_translatev3(Vec3 v)
    {
        return mat4_translatev(v.x, v.y, v.z);
    }
#endif

static Mat4 mat4_frustum(double left, double right, double bottom, double top, double near, double far)
{
    Mat4 r;

    float rl = (float)(right - left);
    float tb = (float)(top   - bottom);
    float fn = (float)(far   - near);

    r.m0  = ((float)near * 2.f) / rl;
    r.m1  = 0.f;
    r.m2  = 0.f;
    r.m3  = 0.f;

    r.m4  = 0.f;
    r.m5  = ((float)near * 2.f) / tb;
    r.m6  = 0.f;
    r.m7  = 0.f;

    r.m8  =  ((float)right + (float)left)   / rl;
    r.m9  =  ((float)top   + (float)bottom) / tb;
    r.m10 = -((float)far   + (float)near)   / fn;
    r.m11 = -1.f;

    r.m12 = 0.f;
    r.m13 = 0.f;
    r.m14 = -((float)far * (float)near * 2.f) / fn;
    r.m15 = 0.f;

    return r;
}

static Mat4 mat4_perspective(double fov_y, double aspect, double near, double far)
{
    Mat4 r = {0};

    double top    = near * tan(fov_y * 0.5);
    double bottom = -top;
    double right  = top * aspect;
    double left   = -right;

    float rl = (float)(right - left);
    float tb = (float)(top   - bottom);
    float fn = (float)(far   - near);

    r.m0  =  ((float)near * 2.f) / rl;
    r.m5  =  ((float)near * 2.f) / tb;
    r.m8  =  ((float)right + (float)left)   / rl;
    r.m9  =  ((float)top   + (float)bottom) / tb;
    r.m10 = -((float)far   + (float)near)   / fn;
    r.m11 = -1.f;
    r.m14 = -((float)far * (float)near * 2.0f) / fn;

    return r;
}

static Mat4 mat4_ortho(double left, double right, double bottom, double top, double near, double far)
{
    Mat4 r;

    float rl = (float)(right - left);
    float tb = (float)(top   - bottom);
    float fn = (float)(far   - near);

    r.m0  = 2.f / rl;
    r.m1  = 0.f;
    r.m2  = 0.f;
    r.m3  = 0.f;

    r.m4  = 0.f;
    r.m5  = 2.f / tb;
    r.m6  = 0.f;
    r.m7  = 0.f;

    r.m8  = 0.f;
    r.m9  = 0.f;
    r.m10 = -2.f / fn;
    r.m11 = 0.f;

    r.m12 = -((float)left + (float)right)  / rl;
    r.m13 = -((float)top  + (float)bottom) / tb;
    r.m14 = -((float)far  + (float)near)   / fn;
    r.m15 = 1.f;

    return r;
}

#if defined(SPFLIB_VEC3)
    static Mat4 mat4_look_at(Vec3 eye, Vec3 target, Vec3 up)
    {
        Mat4 r;

        Vec3 vz = vec3_norm(vec3_sub(eye, target));
        Vec3 vx = vec3_norm(vec3_cross(up, vz));
        Vec3 vy = vec3_norm(vec3_cross(vz, vx));

        r.m0  = vx.x;
        r.m1  = vy.x;
        r.m2  = vz.x;
        r.m3  = 0.f;

        r.m4  = vx.y;
        r.m5  = vy.y;
        r.m6  = vz.y;
        r.m7  = 0.f;

        r.m8  = vx.z;
        r.m9  = vy.z;
        r.m10 = vz.z;
        r.m11 = 0.f;

        r.m12 = -vec3_dot(vx, eye);
        r.m13 = -vec3_dot(vy, eye);
        r.m14 = -vec3_dot(vz, eye);
        r.m15 = 1.f;

        return r;
    }
#endif

static Mat4 mat4_trans(Mat4 m)
{
    Mat4 r;

    r.m0 = m.m0;  r.m4 = m.m1;  r.m8 = m.m2;   r.m12 = m.m3;
    r.m1 = m.m4;  r.m5 = m.m5;  r.m9 = m.m6;   r.m13 = m.m7;
    r.m2 = m.m8;  r.m6 = m.m9;  r.m10 = m.m10; r.m14 = m.m11;
    r.m3 = m.m12; r.m7 = m.m13; r.m11 = m.m14; r.m15 = m.m15;

    return r;
}

static float mat4_det(Mat4 m)
{
    float m0  = m.m0, m1 = m.m1, m2 = m.m2, m3 = m.m3;
    float m4  = m.m4, m5 = m.m5, m6 = m.m6, m7 = m.m7;
    float m8  = m.m8, m9 = m.m9, m10 = m.m10, m11 = m.m11;
    float m12 = m.m12, m13 = m.m13, m14 = m.m14, m15 = m.m15;

    return (m0  * ((m5 * (m10 * m15 - m11 * m14) - m9 * (m6 * m15 - m7 * m14) + m13 * (m6 * m11 - m7 * m10))) -
            m4  * ((m1 * (m10 * m15 - m11 * m14) - m9 * (m2 * m15 - m3 * m14) + m13 * (m2 * m11 - m3 * m10))) +
            m8  * ((m1 * (m6 * m15 - m7 * m14)   - m5 * (m2 * m15 - m3 * m14) + m13 * (m2 * m7  - m3 * m6))) -
            m12 * ((m1 * (m6 * m11 - m7 * m10)   - m5 * (m2 * m11 - m3 * m10) + m9  * (m2 * m7  - m3 * m6))));
}

static float mat4_trace(Mat4 m)
{
    return m.m0 + m.m5 + m.m10 + m.m15;
}

#endif // defined(SPFLIB_MATRIX4)

// --- io --- //

#if !defined(SPFLIB_NO_IO)

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

// built in custom types
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

#if defined(SPFLIB_VEC3)
#   define SPF_VEC3_TYPE(_fn) _fn(VEC3, Vec3, "", ...)__COMMA
#else
#   define SPF_VEC3_TYPE(_fn)
#endif

// custom type slots
#if defined(SPF_CUSTOM_PRINT_TYPE_0)
#   define REGISTER_CUSTOM_TYPE_0(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define REGISTER_CUSTOM_TYPE_0(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_1)
#   define REGISTER_CUSTOM_TYPE_1(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define REGISTER_CUSTOM_TYPE_1(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_2)
#   define REGISTER_CUSTOM_TYPE_2(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define REGISTER_CUSTOM_TYPE_2(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_3)
#   define REGISTER_CUSTOM_TYPE_3(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define REGISTER_CUSTOM_TYPE_3(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_4)
#   define REGISTER_CUSTOM_TYPE_4(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define REGISTER_CUSTOM_TYPE_4(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_5)
#   define REGISTER_CUSTOM_TYPE_5(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define REGISTER_CUSTOM_TYPE_5(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_6)
#   define REGISTER_CUSTOM_TYPE_6(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define REGISTER_CUSTOM_TYPE_6(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_7)
#   define REGISTER_CUSTOM_TYPE_7(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define REGISTER_CUSTOM_TYPE_7(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_8)
#   define REGISTER_CUSTOM_TYPE_8(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define REGISTER_CUSTOM_TYPE_8(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_9)
#   define REGISTER_CUSTOM_TYPE_9(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define REGISTER_CUSTOM_TYPE_9(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_10)
#   define REGISTER_CUSTOM_TYPE_10(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define REGISTER_CUSTOM_TYPE_10(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_11)
#   define REGISTER_CUSTOM_TYPE_11(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define REGISTER_CUSTOM_TYPE_11(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_12)
#   define REGISTER_CUSTOM_TYPE_12(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define REGISTER_CUSTOM_TYPE_12(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_13)
#   define REGISTER_CUSTOM_TYPE_13(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define REGISTER_CUSTOM_TYPE_13(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_14)
#   define REGISTER_CUSTOM_TYPE_14(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define REGISTER_CUSTOM_TYPE_14(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_15)
#   define REGISTER_CUSTOM_TYPE_15(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define REGISTER_CUSTOM_TYPE_15(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_16)
#   define REGISTER_CUSTOM_TYPE_16(_fn, _type, _fn_fmt) _fn(_type, _type, "", ...)__COMMA
#else
#   define REGISTER_CUSTOM_TYPE_16(_fn, _type, _fn_fmt)
#endif

#if defined(SPF_CUSTOM_PRINT_TYPE_17)
#   error "No more custom print slots available."
#endif

#define CAT(a, b) CAT_(a, b)
#define CAT_(a, b) a ## b

#define __SPF_CUSTOM_CASE_ARG(_1, _2, x, ...) x
#define __SPF_CUSTOM_CASE_PRINT(_n)    fprintf(stream, SPF_CUSTOM_PRINT_TYPE_FMT_##_n);
#define __SPF_CUSTOM_CASE_SPRINT(_n)   if (buff_pos >= buff_sz - 1) goto end; int n = snprintf(buff + buff_pos, (buff_sz - buff_pos), SPF_CUSTOM_PRINT_TYPE_FMT_##_n); buff_pos += n;

#define SPF_CUSTOM_CASE(_n, ...) \
    case CAT(__ARG_TYPE_, SPF_CUSTOM_PRINT_TYPE_##_n): { \
        SPF_CUSTOM_PRINT_TYPE_##_n t = va_arg(list, SPF_CUSTOM_PRINT_TYPE_##_n); \
        __SPF_CUSTOM_CASE_ARG(__VA_ARGS__, __SPF_CUSTOM_CASE_PRINT(_n), __SPF_CUSTOM_CASE_SPRINT(_n)) \
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
    SPF_VEC3_TYPE(_fn) \
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
    _fn(FLOAT,                     double,              "f",                                 ...)__COMMA \
    _fn(DOUBLE,                    double,              "f",                                 ...)__COMMA \
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

#define sprint(_buff, _buff_sz, _fmt, ...)                                                                      \
    do {                                                                                                        \
        __sprint(                                                                                               \
            _buff,                                                                                              \
            _buff_sz,                                                                                           \
            _fmt,                                                                                               \
            __N_VA_ARGS(__VA_ARGS__)__VA_OPT__(,)                                                               \
            __FOREACH(__TYPE_TO_ENUM_TYPE, __EXPAND_ARGS(__VA_ARGS__), __VA_ARGS__) __VA_OPT__(0) __VA_OPT__(,) \
            __VA_ARGS__                                                                                         \
        );                                                                                                      \
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

    size_t c = 0;
    while (c < fmt_len) {
        // if (fmt[c] == '{' &&
        //     ((c + 1) <= fmt_len) &&
        //     fmt[c + 1] != '\0')
        if (fmt[c] == '%' &&
            ((c + 1) <= fmt_len) &&
            fmt[c + 1] != '%')
        {
            // char format[64];
            // format[0] = '%';

            // size_t n = c + 1;
            // size_t i = 1;

            // bool found_bracket = false;

            // while (n < 64) {
            //     if (fmt[n] == '}') { found_bracket = true; break; }
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
#if defined(SPFLIB_VEC3)
                case __ARG_TYPE_VEC3: {
                    Vec3 v = va_arg(list, Vec3);
                    fprintf(stream, "{ x: %.3f, y: %.3f, z: %.3f }", v.x, v.y, v.z);
                    break;
                }
#endif

// custom types
#if defined(SPF_CUSTOM_PRINT_TYPE_0)
SPF_CUSTOM_CASE(0, _, _)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_1)
SPF_CUSTOM_CASE(1, _, _)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_2)
SPF_CUSTOM_CASE(2, _, _)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_3)
SPF_CUSTOM_CASE(3, _, _)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_4)
SPF_CUSTOM_CASE(4, _, _)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_5)
SPF_CUSTOM_CASE(5, _, _)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_6)
SPF_CUSTOM_CASE(6, _, _)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_7)
SPF_CUSTOM_CASE(7, _, _)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_8)
SPF_CUSTOM_CASE(8, _, _)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_9)
SPF_CUSTOM_CASE(9, _, _)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_10)
SPF_CUSTOM_CASE(10, _, _)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_11)
SPF_CUSTOM_CASE(11, _, _)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_12)
SPF_CUSTOM_CASE(12, _, _)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_13)
SPF_CUSTOM_CASE(13, _, _)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_14)
SPF_CUSTOM_CASE(14, _, _)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_15)
SPF_CUSTOM_CASE(15, _, _)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_16)
SPF_CUSTOM_CASE(16, _, _)
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

static void __sprint(char* buff, size_t buff_sz, const char* fmt, size_t arg_count, ...)
{
    if (arg_count == 0) {
        size_t len = str_len(fmt);
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

    size_t buff_pos = 0;

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

    size_t c = 0;
    while (c < fmt_len) {
        if (fmt[c] == '%' &&
            ((c + 1) <= fmt_len) &&
            fmt[c + 1] != '%')
        {
            __ArgType type = arg_types[current_arg_type];

            #define __TYPE_CASE(_up, _lw, _fmt, ...) \
                case __ARG_TYPE_##_up: { \
                    if (buff_pos >= buff_sz - 1) goto end; \
                    int n = snprintf(buff + buff_pos, (buff_sz - buff_pos), "%"_fmt, va_arg(list, _lw)); \
                    buff_pos += n; \
                    break; \
                }

            #undef __COMMA
            #define __COMMA

            switch (type) {
                __ITER_TYPES_PROMOTED(__TYPE_CASE)
#if !defined(SPFLIB_NO_STRING)
                case __ARG_TYPE_STRING: {
                    if (buff_pos >= buff_sz - 1) goto end;
                    string str = va_arg(list, string);

                    if ((buff_pos + str.len) < buff_sz - 1) {
                        strncpy(buff + buff_pos, str.data, str.len);
                        buff_pos += str.len;
                    }
                    else {
                        strncpy(buff + buff_pos, str.data, (buff_sz - buff_pos));
                        buff_pos += (buff_sz - buff_pos);
                    }

                    break;
                }
#endif
#if defined(SPFLIB_VEC2)
                case __ARG_TYPE_VEC2: {
                    if (buff_pos >= buff_sz - 1) goto end;
                    Vec2 v = va_arg(list, Vec2);

                    int n = snprintf(buff + buff_pos, (buff_sz - buff_pos), "{ x: %.3f, y: %.3f }", v.x, v.y);
                    buff_pos += n;

                    break;
                }
#endif
#if defined(SPFLIB_VEC3)
                case __ARG_TYPE_VEC3: {
                    if (buff_pos >= buff_sz - 1) goto end;
                    Vec3 v = va_arg(list, Vec3);

                    int n = snprintf(buff + buff_pos, (buff_sz - buff_pos), "{ x: %.3f, y: %.3f, z: %.3f }", v.x, v.y, v.z);
                    buff_pos += n;

                    break;
                }
#endif

// custom types
#if defined(SPF_CUSTOM_PRINT_TYPE_0)
SPF_CUSTOM_CASE(0)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_1)
SPF_CUSTOM_CASE(1)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_2)
SPF_CUSTOM_CASE(2)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_3)
SPF_CUSTOM_CASE(3)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_4)
SPF_CUSTOM_CASE(4)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_5)
SPF_CUSTOM_CASE(5)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_6)
SPF_CUSTOM_CASE(6)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_7)
SPF_CUSTOM_CASE(7)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_8)
SPF_CUSTOM_CASE(8)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_9)
SPF_CUSTOM_CASE(9)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_10)
SPF_CUSTOM_CASE(10)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_11)
SPF_CUSTOM_CASE(11)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_12)
SPF_CUSTOM_CASE(12)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_13)
SPF_CUSTOM_CASE(13)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_14)
SPF_CUSTOM_CASE(14)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_15)
SPF_CUSTOM_CASE(15)
#endif
#if defined(SPF_CUSTOM_PRINT_TYPE_16)
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
            if (buff_pos >= buff_sz - 1) goto end;
            buff[buff_pos++] = fmt[c];
        }

        c++;
    }

end:
    va_end(list);

    if (buff_pos < buff_sz)
        buff[buff_pos] = '\0';
    else
        buff[buff_sz - 1] = '\0';
}

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

// --- file management --- //

#if defined(SPFLIB_FILEIO)

#if defined(OS_WINDOWS)
#   include <windows.h>
#   include <Lmcons.h>       // UNLEN
#elif defined(OS_POSIX)
#   include <sys/types.h>    // uid_t
#   include <sys/stat.h>     // stat
#   include <sys/statvfs.h>  // statvfs
#   include <unistd.h>       // rmdir, geteuid
#   include <stdlib.h>       // getenv
#   include <pwd.h>          // geteuid, getpwuid
#endif

static bool make_dir(const char* path)
{
#if defined(OS_WINDOWS)
    return CreateDirectoryA(path, nullptr);
#elif defined(OS_POSIX)
    struct stat st;

    if (stat(path, &st) == -1) {
        int r = mkdir(path, 0755);
        if (r == 0) return true;
    }

    return false;
#endif
}

// TODO: allow deletion of non empty directories
static bool delete_dir(const char* path)
{
#if defined(OS_WINDOWS)
    return RemoveDirectoryA(path);
#elif defined(OS_POSIX)
    int r = rmdir(path);
    if (r == 0) return true;
    return false;
#endif
}

static const char* temp_dir()
{
#if defined(OS_WINDOWS)
    static char path[256];
    GetTempPathA(256, path);
    return path;
#elif defined(OS_POSIX)

    // TOOD: potentally find a better way of doing this
    const char* dir = getenv("TMPDIR");
    if (dir != nullptr) return dir;
    return "/tmp";
#endif
}

static size_t disk_free_space()
{
#if defined(OS_WINDOWS)
    ULARGE_INTEGER free_bytes;

    bool ok = GetDiskFreeSpaceExA(
        nullptr,    /* lpDirectoryName              */
        nullptr,    /* lpFreeBytesAvailableToCaller */
        nullptr,    /* lpTotalNumberOfBytes         */
        &free_bytes /* lpTotalNumberOfFreeBytes     */
    );

    if (!ok) return 0;
    return free_bytes.QuadPart;
#elif defined(OS_POSIX)
    struct statvfs st;

    int r = statvfs(".", &st);

    if (r == 0) return st.f_bfree * st.f_frsize;
    return 0;
#endif
}

#endif // defined(SPFLIB_FILEIO)

// --- memory --- //

#if defined(SPFLIB_MEMORY)

// rpmalloc start
/* rpmalloc.c  -  Memory allocator  -  2016-2020 Mattias Jansson
 *
 * SPDX-FileCopyrightText: 2016-2020 Mattias Jansson
 * SPDX-License-Identifier: Unlicense OR MIT
 *
 * This library provides a cross-platform lock free thread caching malloc
 * implementation in C11. The latest source code is always available at
 *
 * https://github.com/mjansson/rpmalloc
 *
 * This library is put in the public domain; you can redistribute it and/or
 * modify it without any restrictions. Or, if you choose, you can use it under
 * the MIT license.
 *
 */

//! rpmalloc version. RPMALLOC_VERSION is a human-readable string and may carry a pre-release
//  suffix such as "-rc1". RPMALLOC_VERSION_NUMBER is a monotonic integer for comparisons,
//  computed as major*10000 + minor*100 + patch (pre-release suffixes are not encoded).
#define RPMALLOC_VERSION "2.0.1"
#define RPMALLOC_VERSION_MAJOR 2
#define RPMALLOC_VERSION_MINOR 0
#define RPMALLOC_VERSION_PATCH 1
#define RPMALLOC_VERSION_NUMBER \
	(RPMALLOC_VERSION_MAJOR * 10000 + RPMALLOC_VERSION_MINOR * 100 + RPMALLOC_VERSION_PATCH)

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RPMALLOC_CACHE_LINE_SIZE 64
#if defined(__clang__) || defined(__GNUC__)
#define RPMALLOC_EXPORT __attribute__((visibility("default")))
#define RPMALLOC_RESTRICT __restrict
#define RPMALLOC_ALLOCATOR
#define RPMALLOC_CACHE_ALIGNED __attribute__((aligned(RPMALLOC_CACHE_LINE_SIZE)))
#if (defined(__clang_major__) && (__clang_major__ < 4)) || (!defined(__clang_major__) && defined(__GNUC__))
#define RPMALLOC_ATTRIB_MALLOC
#define RPMALLOC_ATTRIB_ALLOC_SIZE(size)
#define RPMALLOC_ATTRIB_ALLOC_SIZE2(count, size)
#else
#define RPMALLOC_ATTRIB_MALLOC __attribute__((__malloc__))
#define RPMALLOC_ATTRIB_ALLOC_SIZE(size) __attribute__((alloc_size(size)))
#define RPMALLOC_ATTRIB_ALLOC_SIZE2(count, size) __attribute__((alloc_size(count, size)))
#endif
#define RPMALLOC_CDECL
#elif defined(_MSC_VER)
#define RPMALLOC_EXPORT
#define RPMALLOC_RESTRICT __declspec(restrict)
#define RPMALLOC_ALLOCATOR __declspec(allocator) __declspec(restrict)
#define RPMALLOC_CACHE_ALIGNED __declspec(align(RPMALLOC_CACHE_LINE_SIZE))
#define RPMALLOC_ATTRIB_MALLOC
#define RPMALLOC_ATTRIB_ALLOC_SIZE(size)
#define RPMALLOC_ATTRIB_ALLOC_SIZE2(count, size)
#define RPMALLOC_CDECL __cdecl
#else
#define RPMALLOC_EXPORT
#define RPMALLOC_ALLOCATOR
#define RPMALLOC_ATTRIB_MALLOC
#define RPMALLOC_ATTRIB_ALLOC_SIZE(size)
#define RPMALLOC_ATTRIB_ALLOC_SIZE2(count, size)
#define RPMALLOC_CDECL
#endif

#define RPMALLOC_MAX_ALIGNMENT (256 * 1024)

//! Define RPMALLOC_FIRST_CLASS_HEAPS to non-zero to enable heap based API (rpmalloc_heap_* functions).
#ifndef RPMALLOC_FIRST_CLASS_HEAPS
#define RPMALLOC_FIRST_CLASS_HEAPS 0
#endif

//! Define RPMALLOC_HEAP_STATISTICS to non-zero to enable first class heap statistics gathering.
#ifndef RPMALLOC_HEAP_STATISTICS
#define RPMALLOC_HEAP_STATISTICS 0
#endif

//! Flag to rpaligned_realloc to not preserve content in reallocation
#define RPMALLOC_NO_PRESERVE 1
//! Flag to rpaligned_realloc to fail and return null pointer if grow cannot be done in-place,
//  in which case the original pointer is still valid (just like a call to realloc which failes to allocate
//  a new block).
#define RPMALLOC_GROW_OR_FAIL 2

typedef struct rpmalloc_global_statistics_t {
	//! Current amount of virtual memory mapped, all of which might not have been committed (only if
	//! ENABLE_STATISTICS=1)
	size_t mapped;
	//! Peak amount of virtual memory mapped, all of which might not have been committed (only if ENABLE_STATISTICS=1)
	size_t mapped_peak;
	//! Running counter of total amount of memory committed (only if ENABLE_STATISTICS=1)
	size_t committed;
	//! Running counter of total amount of memory decommitted (only if ENABLE_STATISTICS=1)
	size_t decommitted;
	//! Current amount of virtual memory active and committed (only if ENABLE_STATISTICS=1)
	size_t active;
	//! Peak amount of virtual memory active and committed (only if ENABLE_STATISTICS=1)
	size_t active_peak;
	//! Current amount of memory allocated in huge block allocations, i.e blocks above the
	//! largest size class (only if ENABLE_STATISTICS=1)
	size_t huge_alloc;
	//! Peak amount of memory allocated in huge block allocations, i.e blocks above the
	//! largest size class (only if ENABLE_STATISTICS=1)
	size_t huge_alloc_peak;
	//! Current heap count (only if ENABLE_STATISTICS=1)
	size_t heap_count;
} rpmalloc_global_statistics_t;

typedef struct rpmalloc_thread_statistics_t {
	//! Current number of bytes available in thread size class caches (only if ENABLE_STATISTICS=1)
	size_t sizecache;
	//! Current number of bytes available in thread page caches (only if ENABLE_STATISTICS=1)
	size_t spancache;
	//! Per page type span statistics, indexed by page type (small, medium-small, medium-large,
	//! large, huge)
	struct {
		//! Currently mapped number of spans of this page type
		size_t current;
		//! Number of raw memory map calls for this page type resulting in actual OS mmap calls
		//! (only if ENABLE_STATISTICS=1)
		size_t map_calls;
	} span_use[5];
	//! Per size class statistics (only if ENABLE_STATISTICS=1)
	struct {
		//! Current number of allocations
		size_t alloc_current;
		//! Peak number of allocations
		size_t alloc_peak;
		//! Total number of allocations
		size_t alloc_total;
		//! Total number of frees
		size_t free_total;
	} size_use[128];
} rpmalloc_thread_statistics_t;

typedef struct rpmalloc_interface_t {
	//! Map memory pages for the given number of bytes. The returned address MUST be aligned to the given alignment,
	//! which will always be either 0 or the span size. The function can store an alignment offset in the offset
	//! variable in case it performs alignment and the returned pointer is offset from the actual start of the memory
	//! region due to this alignment. This alignment offset will be passed to the memory unmap function. The mapped size
	//! can be stored in the mapped_size variable, which will also be passed to the memory unmap function as the release
	//! parameter once the entire mapped region is ready to be released. If you set a memory_map function, you must also
	//! set a memory_unmap function or else the default implementation will be used for both. This function must be
	//! thread safe, it can be called by multiple threads simultaneously.
	void* (*memory_map)(size_t size, size_t alignment, size_t* offset, size_t* mapped_size);
	//! Commit a range of memory pages. Return non-zero if the operation failed and the address range could not be committed.
	int (*memory_commit)(void* address, size_t size);
	//! Decommit a range of memory pages. Return non-zero if the operation failed and the address range could not be decommitted.
	int (*memory_decommit)(void* address, size_t size);
	//! Unmap the memory pages starting at address and spanning the given number of bytes. If you set a memory_unmap
	//! function, you must also set a memory_map function or else the default implementation will be used for both. This
	//! function must be thread safe, it can be called by multiple threads simultaneously.
	void (*memory_unmap)(void* address, size_t offset, size_t mapped_size);
	//! Called when a call to map memory pages fails (out of memory). If this callback is not set or returns zero the
	//! library will return a null pointer in the allocation call. If this callback returns non-zero the map call will
	//! be retried. The argument passed is the number of bytes that was requested in the map call. Only used if the
	//! default system memory map function is used (memory_map callback is not set).
	int (*map_fail_callback)(size_t size);
	//! Called when an assert fails, if asserts are enabled. Will use the standard assert() if this is not set.
	void (*error_callback)(const char* message);
} rpmalloc_interface_t;

typedef struct rpmalloc_config_t {
	//! Size of memory pages. The page size MUST be a power of two. All memory mapping
	//  requests to memory_map will be made with size set to a multiple of the page size.
	//  Set to 0 to use the OS default page size.
	size_t page_size;
	//! Enable use of large/huge pages. If this flag is set to non-zero and page size is
	//  zero, the allocator will try to enable huge pages and auto detect the configuration.
	//  If this is set to non-zero and page_size is also non-zero, the allocator will
	//  assume huge pages have been configured and enabled prior to initializing the
	//  allocator.
	//  For Windows, see https://docs.microsoft.com/en-us/windows/desktop/memory/large-page-support
	//  For Linux, see https://www.kernel.org/doc/Documentation/vm/hugetlbpage.txt
	int enable_huge_pages;
	//! Enable use of transparent huge pages, advising the kernel to back large memory
	//  mappings with huge pages without requiring a preallocated huge page pool. Unlike
	//  enable_huge_pages this does not affect page size or accounting, and unused memory
	//  ranges can still be decommitted. Ignored if enable_huge_pages is in effect or if
	//  the platform has no transparent huge page support (currently implemented for
	//  Linux and Android only). After initialization the config value reflects if
	//  transparent huge pages are actually used.
	int enable_thp;
	//! Disable decommitting unused pages when allocator determines the memory pressure
	//  is low and there is enough active pages cached. If set to 1, keep all pages committed.
	int disable_decommit;
	//! Allocated pages names for systems supporting it to be able to distinguish among anonymous regions.
	const char* page_name;
	//! Allocated huge pages names for systems supporting it to be able to distinguish among anonymous regions.
	const char* huge_page_name;
	//! Unmap all memory on finalize if set to 1. Normally you can let the OS unmap all pages
	//  when process exits, but if using rpmalloc in a dynamic library you might want to unmap
	//  all pages when the dynamic library unloads to avoid process memory leaks and bloat.
	//  NOTE: this is ignored when rpmalloc is built with ENABLE_OVERRIDE, because the standard
	//  library override makes rpmalloc the backing store for the C runtime's own allocations
	//  (for example per-thread TLS). Returning all mappings to the OS while the process keeps
	//  running would unmap memory the runtime still uses. Only honored without the override.
	int unmap_on_finalize;
#if defined(__linux__) || defined(__ANDROID__)
	///! Allows to disable the Transparent Huge Page feature on Linux on a process basis,
	///  rather than enabling/disabling system-wise (done via /sys/kernel/mm/transparent_hugepage/enabled).
	///  It can possibly improve performance and reduced allocation overhead in some contexts, albeit
	///  THP is usually enabled by default.
	int disable_thp;
#endif
} rpmalloc_config_t;

struct rpmalloc_heap_statistics_t {
	// Number of bytes allocated
	size_t allocated_size;
	// Number of bytes committed
	size_t committed_size;
	// Number of bytes mapped
	size_t mapped_size;
};



#include <errno.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdatomic.h>

#if !defined(__has_builtin)
#define __has_builtin(b) 0
#endif

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wunused-macros"
#pragma clang diagnostic ignored "-Wunused-function"
#if __has_warning("-Wreserved-identifier")
#pragma clang diagnostic ignored "-Wreserved-identifier"
#endif
#if __has_warning("-Wstatic-in-inline")
#pragma clang diagnostic ignored "-Wstatic-in-inline"
#endif
#if __has_warning("-Wunsafe-buffer-usage")
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif
#if __has_warning("-Wimplicit-void-ptr-cast")
#pragma clang diagnostic ignored "-Wimplicit-void-ptr-cast"
#endif
#if __has_warning("-Wallocator-wrappers")
#pragma clang diagnostic ignored "-Wallocator-wrappers"
#endif
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wunused-macros"
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
#define PLATFORM_WINDOWS 1
#define PLATFORM_POSIX 0
#else
#define PLATFORM_WINDOWS 0
#define PLATFORM_POSIX 1
#endif

#if defined(_MSC_VER)
#define NOINLINE __declspec(noinline)
#else
#define NOINLINE __attribute__((noinline))
#endif

#if PLATFORM_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <fibersapi.h>
static DWORD fls_key;
//! VirtualAlloc2 (Windows 10+), resolved dynamically; null on older systems
typedef PVOID(WINAPI* virtualalloc2_fn)(HANDLE, PVOID, SIZE_T, ULONG, ULONG, MEM_EXTENDED_PARAMETER*, ULONG);
static virtualalloc2_fn os_virtualalloc2;
#endif
#if PLATFORM_POSIX
#include <sys/mman.h>
#include <sched.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
static pthread_key_t pthread_key;
#ifdef __FreeBSD__
#include <sys/sysctl.h>
#define MAP_HUGETLB MAP_ALIGNED_SUPER
#ifndef PROT_MAX
#define PROT_MAX(f) 0
#endif
#else
#define PROT_MAX(f) 0
#endif
#ifdef __sun
extern int
madvise(caddr_t, size_t, int);
#endif
#ifndef MAP_UNINITIALIZED
#define MAP_UNINITIALIZED 0
#endif
#endif

#if defined(__linux__) || defined(__ANDROID__)
#include <sys/prctl.h>
#if !defined(PR_SET_VMA)
#define PR_SET_VMA 0x53564d41
#define PR_SET_VMA_ANON_NAME 0
#endif
#endif
#if defined(__APPLE__)
#include <TargetConditionals.h>
#if !TARGET_OS_IPHONE && !TARGET_OS_SIMULATOR
#include <mach/mach_vm.h>
#include <mach/vm_statistics.h>
#endif
#include <pthread.h>
#endif
#if defined(__HAIKU__) || defined(__TINYC__)
#include <pthread.h>
#endif

#include <limits.h>
#if (INTPTR_MAX > INT32_MAX)
#define ARCH_64BIT 1
#define ARCH_32BIT 0
#else
#define ARCH_64BIT 0
#define ARCH_32BIT 1
#endif

#define pointer_offset(ptr, ofs) (void*)((char*)(ptr) + (ptrdiff_t)(ofs))
#define pointer_diff(first, second) (ptrdiff_t)((const char*)(first) - (const char*)(second))

////////////
///
/// Build time configurable limits
///
//////

#ifndef ENABLE_VALIDATE_ARGS
//! Enable validation of args to public entry points
#define ENABLE_VALIDATE_ARGS 0
#endif
#ifndef ENABLE_ASSERTS
//! Enable asserts
#define ENABLE_ASSERTS 0
#endif
#ifndef ENABLE_UNMAP
//! Enable unmapping memory pages
#define ENABLE_UNMAP 1
#endif
#ifndef ENABLE_DECOMMIT
//! Enable decommitting memory pages
#define ENABLE_DECOMMIT 1
#endif
#ifndef ENABLE_DYNAMIC_LINK
//! Enable building as dynamic library
#define ENABLE_DYNAMIC_LINK 0
#endif
#ifndef ENABLE_OVERRIDE
//! Enable standard library malloc/free/new/delete overrides. When enabled, rpmalloc becomes the
//  backing store for the entire process, including the C runtime's own allocations (for example
//  per-thread TLS allocated by the loader). Two finalize-time features are therefore adjusted:
//  the unmap_on_finalize config option is ignored (see rpmalloc_initialize), since returning all
//  mappings to the OS while the process keeps running would unmap memory the runtime still holds;
//  and ENABLE_LEAK_DETECTION defaults off, since the runtime's still-live allocations are
//  indistinguishable from application leaks at finalize.
#define ENABLE_OVERRIDE 1
#endif
#ifndef ENABLE_STATISTICS
//! Enable statistics
#define ENABLE_STATISTICS 0
#endif
#ifndef ENABLE_LEAK_DETECTION
//! Enable detection of allocations still outstanding at rpmalloc_finalize. Requires ENABLE_STATISTICS
//  for the allocation counters, and follows it by default - except under ENABLE_OVERRIDE, where it
//  defaults off: the override makes rpmalloc the backing store for the C runtime's own allocations
//  (for example per-thread TLS allocated by the loader), which are still live at finalize and
//  indistinguishable from application leaks, so the check would always report false positives.
#if ENABLE_OVERRIDE
#define ENABLE_LEAK_DETECTION 0
#else
#define ENABLE_LEAK_DETECTION ENABLE_STATISTICS
#endif
#endif
#if ENABLE_LEAK_DETECTION && !ENABLE_STATISTICS
#error ENABLE_LEAK_DETECTION requires ENABLE_STATISTICS
#endif

////////////
///
/// Built in size configurations
///
//////

#define PAGE_HEADER_SIZE 128
#define SPAN_HEADER_SIZE PAGE_HEADER_SIZE

//! Alignment of each heap control structure within a shared mapping. Keeps adjacent
//  heaps on separate cache lines so cross-thread writes to one heap (thread_free) do
//  not falsely share with a neighbour heap owned by another thread. Sized for a 128 byte
//  cache line (Apple Silicon, x86 destructive interference) to cover 64 byte lines too.
#define HEAP_ALIGNMENT 128

#define SMALL_GRANULARITY 16

#define SMALL_BLOCK_SIZE_LIMIT (4 * 1024)
#define MEDIUM_SMALL_BLOCK_SIZE_LIMIT (32 * 1024)
#define MEDIUM_LARGE_BLOCK_SIZE_LIMIT (256 * 1024)
#define LARGE_BLOCK_SIZE_LIMIT (2 * 1024 * 1024)

#define SMALL_SIZE_CLASS_COUNT 73
#define MEDIUM_SMALL_SIZE_CLASS_COUNT 12
#define MEDIUM_LARGE_SIZE_CLASS_COUNT 12
#define LARGE_SIZE_CLASS_COUNT 12
#define SIZE_CLASS_COUNT \
	(SMALL_SIZE_CLASS_COUNT + MEDIUM_SMALL_SIZE_CLASS_COUNT + MEDIUM_LARGE_SIZE_CLASS_COUNT + LARGE_SIZE_CLASS_COUNT)

#define SMALL_PAGE_SIZE_SHIFT 16
#define SMALL_PAGE_SIZE (1 << SMALL_PAGE_SIZE_SHIFT)
#define SMALL_PAGE_MASK (~((uintptr_t)SMALL_PAGE_SIZE - 1))
#define MEDIUM_SMALL_PAGE_SIZE_SHIFT 20
#define MEDIUM_SMALL_PAGE_SIZE (1 << MEDIUM_SMALL_PAGE_SIZE_SHIFT)
#define MEDIUM_SMALL_PAGE_MASK (~((uintptr_t)MEDIUM_SMALL_PAGE_SIZE - 1))
#define MEDIUM_LARGE_PAGE_SIZE_SHIFT 22
#define MEDIUM_LARGE_PAGE_SIZE (1 << MEDIUM_LARGE_PAGE_SIZE_SHIFT)
#define MEDIUM_LARGE_PAGE_MASK (~((uintptr_t)MEDIUM_LARGE_PAGE_SIZE - 1))
#define LARGE_PAGE_SIZE_SHIFT 24
#define LARGE_PAGE_SIZE (1 << LARGE_PAGE_SIZE_SHIFT)
#define LARGE_PAGE_MASK (~((uintptr_t)LARGE_PAGE_SIZE - 1))

#define SPAN_SIZE (256 * 1024 * 1024)
#define SPAN_MASK (~((uintptr_t)(SPAN_SIZE - 1)))

#if ENABLE_VALIDATE_ARGS
//! Maximum allocation size to avoid integer overflow
#undef  MAX_ALLOC_SIZE
#define MAX_ALLOC_SIZE (((size_t)-1) - SPAN_SIZE)
#endif

////////////
///
/// Utility macros
///
//////

#if ENABLE_ASSERTS
#undef NDEBUG
#if defined(_MSC_VER) && !defined(_DEBUG)
#define _DEBUG
#endif
#include <assert.h>
#define RPMALLOC_TOSTRING_M(x) #x
#define RPMALLOC_TOSTRING(x) RPMALLOC_TOSTRING_M(x)
#define rpmalloc_assert(truth, message) \
	do {                                \
		if (!(truth)) {                 \
			assert((truth) && message); \
		}                               \
	} while (0)
#else
#define rpmalloc_assert(truth, message) \
	do {                                \
	} while (0)
#endif

#if defined(_MSC_VER)
#define rpmalloc_assume(cond) __assume(cond)
#elif defined(__clang__) && __has_builtin(__builtin_assume)
#define rpmalloc_assume(cond) __builtin_assume(cond)
#elif defined(__GNUC__)
#define rpmalloc_assume(cond)           \
	do {                                \
		if (!__builtin_expect(cond, 0)) \
			__builtin_unreachable();    \
	} while (0)
#else
#define rpmalloc_assume(cond) 0
#endif

////////////
///
/// Statistics
///
//////

#if ENABLE_STATISTICS

typedef struct rpmalloc_statistics_t {
	atomic_size_t page_mapped;
	atomic_size_t page_mapped_peak;
	atomic_size_t page_commit;
	atomic_size_t page_decommit;
	atomic_size_t page_active;
	atomic_size_t page_active_peak;
	atomic_size_t huge_alloc;
	atomic_size_t huge_alloc_peak;
	atomic_size_t heap_count;
} rpmalloc_statistics_t;

static rpmalloc_statistics_t global_statistics;

//! Subtract from a statistics counter, clamping at zero. Counters are in units of the
//! configured page size and heaps with committed pages are retained across finalize and
//! initialize cycles where the page size can change, so strict accounting is not possible.
static void
statistics_sub_saturating(atomic_size_t* counter, size_t value) {
	size_t current = atomic_load_explicit(counter, memory_order_relaxed);
	size_t next;
	do {
		next = (current > value) ? (current - value) : 0;
	} while (
	    !atomic_compare_exchange_weak_explicit(counter, &current, next, memory_order_relaxed, memory_order_relaxed));
}

//! Per size class allocation counters, updated only by the owning thread (cross-thread frees are
//  deferred and accounted when the owner processes them), so plain integers suffice
typedef struct heap_size_use_t {
	int32_t alloc_current;
	int32_t alloc_peak;
	int32_t alloc_total;
	int32_t free_total;
} heap_size_use_t;

//! A successful allocation of the given size class on the owning heap
#define _rpmalloc_stat_inc_alloc(heap, class_idx)                       \
	do {                                                                \
		heap_size_use_t* _su = (heap)->size_use + (class_idx);          \
		if (++_su->alloc_current > _su->alloc_peak)                     \
			_su->alloc_peak = _su->alloc_current;                       \
		++_su->alloc_total;                                             \
	} while (0)
//! Account for a number of freed blocks of the given size class on the owning heap
#define _rpmalloc_stat_add_free(heap, class_idx, count)                 \
	do {                                                                \
		heap_size_use_t* _su = (heap)->size_use + (class_idx);          \
		_su->alloc_current -= (int32_t)(count);                         \
		_su->free_total += (int32_t)(count);                            \
	} while (0)
//! A raw memory map call for a span of the given page type on the owning heap
#define _rpmalloc_stat_inc_span_map(heap, page_type) \
	do {                                             \
		++(heap)->span_map_calls[page_type];         \
	} while (0)

#else

#define _rpmalloc_stat_inc_alloc(heap, class_idx) \
	do {                                          \
	} while (0)
#define _rpmalloc_stat_add_free(heap, class_idx, count) \
	do {                                                \
	} while (0)
#define _rpmalloc_stat_inc_span_map(heap, page_type) \
	do {                                             \
	} while (0)

#endif

////////////
///
/// Low level abstractions
///
//////

static inline size_t
rpmalloc_clz(uintptr_t x) {
#if ARCH_64BIT
#if defined(_MSC_VER) && !defined(__clang__)
#if defined(_M_ARM64)
	return (size_t)_CountLeadingZeros64(x);  // __lzcnt64 is x86-only
#else
	return (size_t)__lzcnt64(x);
#endif
#else
	return (size_t)__builtin_clzll(x);
#endif
#else
#if defined(_MSC_VER) && !defined(__clang__)
#if defined(_M_ARM64) || defined(_M_ARM)
	return (size_t)_CountLeadingZeros((unsigned long)x);  // __lzcnt32 is x86-only
#else
	return (size_t)__lzcnt32(x);
#endif
#else
	return (size_t)__builtin_clzl(x);
#endif
#endif
}

//! Yield the rest of this thread's scheduling quantum so another runnable thread (e.g. a lock
//! holder that has been preempted) can run. This is a real scheduler yield, unlike the per-arch
//! CPU pause/yield hint used by wait_spin below.
static inline void
thread_yield(void) {
#if PLATFORM_WINDOWS
	SwitchToThread();
#else
	sched_yield();
#endif
}

#ifndef WAIT_SPIN_YIELD_THRESHOLD
//! Number of cheap CPU-pause spins before wait_spin escalates to a real OS scheduler yield.
#define WAIT_SPIN_YIELD_THRESHOLD 100
#endif

static inline void
wait_spin(uint32_t* spin_count) {
	if (++(*spin_count) >= WAIT_SPIN_YIELD_THRESHOLD) {
		thread_yield();
		return;
	}
#if defined(_MSC_VER)
#if defined(_M_ARM64)
	__yield();
#else
	_mm_pause();
#endif
#elif (defined(__x86_64__) || defined(__i386__)) && !defined(_M_ARM64EC)
	__asm__ volatile("pause" ::: "memory");
#elif defined(__aarch64__) || (defined(__arm__) && __ARM_ARCH >= 7) || defined(_M_ARM64EC)
	__asm__ volatile("yield" ::: "memory");
#elif defined(__powerpc__) || defined(__powerpc64__)
	// No idea if ever been compiled in such archs but ... as precaution
	__asm__ volatile("or 27,27,27");
#elif defined(__sparc__)
	__asm__ volatile("rd %ccr, %g0 \n\trd %ccr, %g0 \n\trd %ccr, %g0");
#else
	struct timespec ts = {0};
	nanosleep(&ts, 0);
#endif
}

#if defined(__GNUC__) || defined(__clang__)

#define EXPECTED(x) __builtin_expect((x), 1)
#define UNEXPECTED(x) __builtin_expect((x), 0)

#else

#define EXPECTED(x) x
#define UNEXPECTED(x) x

#endif

#if defined(__GNUC__) || defined(__clang__)
#ifdef __has_builtin
#if __has_builtin(__builtin_memcpy_inline)
#define memcpy_const(x, y, s) __builtin_memcpy_inline(x, y, s)
#else
#define memcpy_const(x, y, s)                                                                                   \
	do {                                                                                                        \
		_Static_assert(__builtin_choose_expr(__builtin_constant_p(s), 1, 0), "len must be a constant integer"); \
		memcpy(x, y, s);                                                                                        \
	} while (0)
#endif

#if __has_builtin(__builtin_memset_inline)
#define memset_const(x, y, s) __builtin_memset_inline(x, y, s)
#else
#define memset_const(x, y, s)                                                                                   \
	do {                                                                                                        \
		_Static_assert(__builtin_choose_expr(__builtin_constant_p(s), 1, 0), "len must be a constant integer"); \
		memset(x, y, s);                                                                                        \
	} while (0)
#endif
#endif
#endif

#ifndef memcpy_const
#define memcpy_const(x, y, s) memcpy(x, y, s)
#define memset_const(x, y, s) memset(x, y, s)
#endif

////////////
///
/// Data types
///
//////

//! A memory heap, per thread
typedef struct heap_t heap_t;
//! Span of memory pages
typedef struct span_t span_t;
//! Memory page
typedef struct page_t page_t;
//! Memory block
typedef struct block_t block_t;
//! Size class for a memory block
typedef struct size_class_t size_class_t;

//! Memory page type
typedef enum page_type_t {
	PAGE_SMALL,         // 64KiB
	PAGE_MEDIUM_SMALL,  // 1MiB
	PAGE_MEDIUM_LARGE,  // 4MiB
	PAGE_LARGE,         // 16MiB
	PAGE_HUGE
} page_type_t;

//! Block size class
struct size_class_t {
	//! Size of blocks in this class
	uint32_t block_size;
	//! Number of blocks in each chunk
	uint32_t block_count;
};

//! A memory block
struct block_t {
	//! Next block in list
	block_t* next;
};

//! A page contains blocks of a given size
struct page_t {
	//! Size class of blocks
	uint32_t size_class;
	//! Block size
	uint32_t block_size;
	//! Block count
	uint32_t block_count;
	//! Block initialized count
	uint32_t block_initialized;
	//! Block used count
	uint32_t block_used;
	//! Page type
	page_type_t page_type;
	//! Flag set if part of heap full list
	uint32_t is_full : 1;
	//! Flag set if part of heap free list
	uint32_t is_free : 1;
	//! Flag set if blocks are zero initialied
	uint32_t is_zero : 1;
	//! Flag set if memory pages have been decommitted
	uint32_t is_decommitted : 1;
	//! Flag set if containing aligned blocks.
	//  Note: has_aligned_block and is_full are read on the cross-thread free path while the
	//  owner thread writes this packed flag word. The owner is the sole writer and the word is
	//  naturally aligned (hardware-atomic access), and the read flags are monotonic/tolerant,
	//  so the read/write race is benign. ThreadSanitizer still reports it; see
	//  test/tsan-suppressions.txt.
	uint32_t has_aligned_block : 1;
	//! Fast combination flag for either huge, fully allocated or has aligned blocks
	uint32_t generic_free : 1;
	//! Set if the span maps reserve-only (pages commit on demand, may be decommitted),
	//  captured at map time so it survives a config change across a finalize/initialize
	uint32_t commit_on_demand : 1;
	//! log2 of the committed prefix (the page size at the span's map time)
	uint32_t commit_prefix_shift : 6;
	//! Local free list count
	uint32_t local_free_count;
	//! Local free list
	block_t* local_free;
	//! Owning heap
	heap_t* heap;
	//! Next page in list
	page_t* next;
	//! Previous page in list
	page_t* prev;
	//! Multithreaded free list, block index is in low 32 bit, list count is high 32 bit
	atomic_ullong thread_free;
};

//! A span contains pages of a given type
struct span_t {
	//! Page header
	page_t page;
	//! Owning heap (do not deduplicate into page.heap - that field is the
	//! owner of page 0 as a memory page and changes if the page is adopted
	//! by another heap, while the span owner must remain stable)
	heap_t* heap;
	//! Page address mask
	uintptr_t page_address_mask;
	//! Number of pages initialized
	uint32_t page_initialized;
	//! Number of pages in use
	uint32_t page_count;
	//! Number of bytes per page
	uint32_t page_size;
	//! Page type
	page_type_t page_type;
	//! Offset to start of mapped memory region
	uint32_t offset;
	//! Mapped size
	uint64_t mapped_size;
	//! Next span in list
	span_t* next;
};

// Control structure for a heap, either a thread heap or a first class heap if enabled
struct heap_t {
	//! Owning thread ID
	uintptr_t owner_thread;
	//! Heap local free list for small size classes
	block_t* local_free[SIZE_CLASS_COUNT];
	//! Available non-full pages for each size class
	page_t* page_available[SIZE_CLASS_COUNT];
	//! Free pages for each page type
	page_t* page_free[4];
	//! Free but still committed page count for each page tyoe
	uint32_t page_free_commit_count[4];
	//! Multithreaded free list
	atomic_uintptr_t thread_free[4];
	//! Available partially initialized spans for each page type
	span_t* span_partial[4];
	//! Spans in full use for each page type
	span_t* span_used[5];
	//! Next heap in queue
	heap_t* next;
	//! Previous heap in queue
	heap_t* prev;
	//! Heap ID
	uint32_t id;
	//! Finalization state flag
	uint32_t finalize;
	//! Memory map region offset
	uint32_t offset;
	//! Memory map size
	size_t mapped_size;
#if RPMALLOC_HEAP_STATISTICS
	struct rpmalloc_heap_statistics_t stats;
#endif
#if ENABLE_STATISTICS
	//! Per size class allocation counters (current span_use count is computed live on demand)
	heap_size_use_t size_use[SIZE_CLASS_COUNT];
	//! Per page type raw span map call counters
	uint32_t span_map_calls[5];
#endif
};

_Static_assert(sizeof(page_t) <= PAGE_HEADER_SIZE, "Invalid page header size");
_Static_assert(sizeof(span_t) <= SPAN_HEADER_SIZE, "Invalid span header size");
_Static_assert(sizeof(heap_t) <= 4096, "Invalid heap size");

////////////
///
/// Global data
///
//////

//! Fallback heap
static RPMALLOC_CACHE_ALIGNED heap_t global_heap_fallback;
//! Default heap
static heap_t* global_heap_default = &global_heap_fallback;
//! Released heaps available for reuse. These have been used and may hold caches, so they are
//  only handed to ordinary thread heaps, never to first-class heaps (which require pristine state)
static heap_t* global_heap_queue;
//! Pristine, never-used heaps carved from a shared block but not yet handed out. Safe for any
//  request including first-class, which needs a heap with no prior allocations
static heap_t* global_heap_pristine;
//! In use heaps
static heap_t* global_heap_used;
//! Set while a thread is mapping and carving a new shared heap block, so concurrent thread
//  starts wait for the carved heaps to reach a pool instead of each mapping their own block.
//  A user memory interface callback must not allocate through rpmalloc on the creating thread:
//  a nested heap creation would wait on this flag set by the same thread and deadlock
static int global_heap_creating;
//! Lock for heap queue
static atomic_uintptr_t global_heap_lock;
//! Heap ID counter
static atomic_uint global_heap_id = 1;
//! Global initialization state. rpmalloc_initialize can be called concurrently from multiple
//! threads (the first allocation on any thread may trigger it), so it is driven as an atomic
//! state machine: exactly one thread performs the global setup while the others wait for it to
//! complete before allocating - they must never observe, or allocate against, a partially
//! written global_config. Release/acquire ordering on the DONE transition publishes all of the
//! global_config writes to threads that observe completion.
#define RPMALLOC_INIT_UNINIT 0
#define RPMALLOC_INIT_RUNNING 1
#define RPMALLOC_INIT_DONE 2
static atomic_int global_rpmalloc_init_state;
//! Memory interface
static rpmalloc_interface_t* global_memory_interface;
//! Default memory interface
static rpmalloc_interface_t global_memory_interface_default;
//! Current configuration
static rpmalloc_config_t global_config = {0};
//! Main thread ID
static uintptr_t global_main_thread_id;

//! Size classes
#define SCLASS(n) \
	{ (n * SMALL_GRANULARITY), (SMALL_PAGE_SIZE - PAGE_HEADER_SIZE) / (n * SMALL_GRANULARITY) }
#define MSCLASS(n) \
	{ (n * SMALL_GRANULARITY), (MEDIUM_SMALL_PAGE_SIZE - PAGE_HEADER_SIZE) / (n * SMALL_GRANULARITY) }
#define MLCLASS(n) \
	{ (n * SMALL_GRANULARITY), (MEDIUM_LARGE_PAGE_SIZE - PAGE_HEADER_SIZE) / (n * SMALL_GRANULARITY) }
#define LCLASS(n) \
	{ (n * SMALL_GRANULARITY), (LARGE_PAGE_SIZE - PAGE_HEADER_SIZE) / (n * SMALL_GRANULARITY) }
static const size_class_t global_size_class[SIZE_CLASS_COUNT] = {
    SCLASS(1),      SCLASS(1),      SCLASS(2),      SCLASS(3),      SCLASS(4),      SCLASS(5),      SCLASS(6),
    SCLASS(7),      SCLASS(8),      SCLASS(9),      SCLASS(10),     SCLASS(11),     SCLASS(12),     SCLASS(13),
    SCLASS(14),     SCLASS(15),     SCLASS(16),     SCLASS(17),     SCLASS(18),     SCLASS(19),     SCLASS(20),
    SCLASS(21),     SCLASS(22),     SCLASS(23),     SCLASS(24),     SCLASS(25),     SCLASS(26),     SCLASS(27),
    SCLASS(28),     SCLASS(29),     SCLASS(30),     SCLASS(31),     SCLASS(32),     SCLASS(33),     SCLASS(34),
    SCLASS(35),     SCLASS(36),     SCLASS(37),     SCLASS(38),     SCLASS(39),     SCLASS(40),     SCLASS(41),
    SCLASS(42),     SCLASS(43),     SCLASS(44),     SCLASS(45),     SCLASS(46),     SCLASS(47),     SCLASS(48),
    SCLASS(49),     SCLASS(50),     SCLASS(51),     SCLASS(52),     SCLASS(53),     SCLASS(54),     SCLASS(55),
    SCLASS(56),     SCLASS(57),     SCLASS(58),     SCLASS(59),     SCLASS(60),     SCLASS(61),     SCLASS(62),
    SCLASS(63),     SCLASS(64),     SCLASS(80),     SCLASS(96),     SCLASS(112),    SCLASS(128),    SCLASS(160),
    SCLASS(192),    SCLASS(224),    SCLASS(256),    MSCLASS(320),   MSCLASS(384),   MSCLASS(448),   MSCLASS(512),
    MSCLASS(640),   MSCLASS(768),   MSCLASS(896),   MSCLASS(1024),  MSCLASS(1280),  MSCLASS(1536),  MSCLASS(1792),
    MSCLASS(2048),  MLCLASS(2560),  MLCLASS(3072),  MLCLASS(3584),  MLCLASS(4096),  MLCLASS(5120),  MLCLASS(6144),
    MLCLASS(7168),  MLCLASS(8192),  MLCLASS(10240), MLCLASS(12288), MLCLASS(14336), MLCLASS(16384), LCLASS(20480),
    LCLASS(24576),  LCLASS(28672),  LCLASS(32768),  LCLASS(40960),  LCLASS(49152),  LCLASS(57344),  LCLASS(65536),
    LCLASS(81920),  LCLASS(98304),  LCLASS(114688), LCLASS(131072)};

//! Threshold number of pages for when free pages are decommitted
static uint32_t global_page_free_overflow[5] = {16, 8, 4, 2, 0};

//! Number of pages to retain when free page threshold overflows
static uint32_t global_page_free_retain[5] = {4, 2, 1, 1, 0};

//! OS huge page support
static int os_huge_pages;
//! OS transparent huge page support (advise mappings to be huge page backed without
//! requiring a preallocated huge page pool, currently Linux/Android only)
static int os_thp;
//! Shift for the huge page size (log2 of huge page size when huge pages are enabled)
static size_t os_huge_page_shift;
//! OS memory map granularity
static size_t os_map_granularity;
//! OS memory page size
static size_t os_page_size;

////////////
///
/// Thread local heap and ID
///
//////

//! Current thread heap
#if defined(_MSC_VER) && !defined(__clang__)
#define TLS_MODEL
#define _Thread_local __declspec(thread)
#elif defined(__ANDROID__)
#if __ANDROID_API__ >= 29 && \
    ((defined(__clang__) && (__clang_major__ >= 17)) || (defined(__NDK_MAJOR__) && (__NDK_MAJOR__ >= 26)))
#define TLS_MODEL __attribute__((tls_model("local-dynamic")))
#else
#define TLS_MODEL
#endif
#else
#define TLS_MODEL __attribute__((tls_model("initial-exec")))
// #define TLS_MODEL
#endif
static _Thread_local heap_t* global_thread_heap TLS_MODEL = &global_heap_fallback;

static heap_t*
heap_allocate(int first_class);

static void
heap_page_free_decommit(heap_t* heap, uint32_t page_type, uint32_t page_retain_count);

//! Fast thread ID
static inline uintptr_t
get_thread_id(void) {
#if defined(_WIN32)
	return (uintptr_t)((void*)NtCurrentTeb());
#elif !defined(__APPLE__) && !defined(__CYGWIN__) &&                                                \
    ((defined(__clang__) && (__clang_major__ >= 7)) || ((defined(__GNUC__) && (__GNUC__ >= 5)))) && \
    (defined(__aarch64__) || defined(__x86_64__) || defined(__loongarch__))  // Unsure of other archs, needs testing
	void* thp = __builtin_thread_pointer();
	return (uintptr_t)thp;
#else
	uintptr_t tid;
#if defined(__i386__)
	__asm__("movl %%gs:0, %0" : "=r"(tid) : :);
#elif defined(__x86_64__)
#if defined(__MACH__)
	__asm__("movq %%gs:0, %0" : "=r"(tid) : :);
#else
	__asm__("movq %%fs:0, %0" : "=r"(tid) : :);
#endif
#elif defined(__arm__)
	__asm__ volatile("mrc p15, 0, %0, c13, c0, 3" : "=r"(tid));
#elif defined(__aarch64__)
#if defined(__MACH__)
	// tpidr_el0 likely unused, always return 0 on iOS
	__asm__ volatile("mrs %0, tpidrro_el0" : "=r"(tid));
#else
	__asm__ volatile("mrs %0, tpidr_el0" : "=r"(tid));
#endif
#else
	tid = (uintptr_t)&global_thread_heap;
#endif
	return tid;
#endif
}

//! Set the current thread heap
static void
set_thread_heap(heap_t* heap) {
	global_thread_heap = heap;
	if (heap && (heap->id != 0)) {
		rpmalloc_assert(heap->id != 0, "Default heap being used");
		heap->owner_thread = get_thread_id();
	}
#if PLATFORM_WINDOWS
	FlsSetValue(fls_key, heap);
#else
	pthread_setspecific(pthread_key, heap);
#endif
}

static heap_t*
get_thread_heap_allocate(void) {
	heap_t* heap = heap_allocate(0);
	set_thread_heap(heap);
	return heap;
}

//! Get the current thread heap
static inline heap_t*
get_thread_heap(void) {
	return global_thread_heap;
}

//! Get the size class from given size in bytes for tiny blocks (below 16 times the minimum granularity)
static inline uint32_t
get_size_class_tiny(size_t size) {
	return (((uint32_t)size + (SMALL_GRANULARITY - 1)) / SMALL_GRANULARITY);
}

//! Get the size class from given size in bytes
static inline uint32_t
get_size_class(size_t size) {
	uintptr_t minblock_count = (size + (SMALL_GRANULARITY - 1)) / SMALL_GRANULARITY;
	// For sizes up to 64 times the minimum granularity (i.e 1024 bytes) the size class is equal to number of such
	// blocks
	if (size <= (SMALL_GRANULARITY * 64)) {
		rpmalloc_assert(global_size_class[minblock_count].block_size >= size, "Size class misconfiguration");
		return (uint32_t)(minblock_count ? minblock_count : 1);
	}
	--minblock_count;
	// Calculate position of most significant bit, since minblock_count now guaranteed to be > 64 this position is
	// guaranteed to be >= 6
#if ARCH_64BIT
	const uint32_t most_significant_bit = (uint32_t)(63 - (int)rpmalloc_clz(minblock_count));
#else
	const uint32_t most_significant_bit = (uint32_t)(31 - (int)rpmalloc_clz(minblock_count));
#endif
	// Class sizes are of the bit format [..]000xxx000[..] where we already have the position of the most significant
	// bit, now calculate the subclass from the remaining two bits
	const uint32_t subclass_bits = (minblock_count >> (most_significant_bit - 2)) & 0x03;
	const uint32_t class_idx = (uint32_t)((most_significant_bit << 2) + subclass_bits) + 41;
	rpmalloc_assert((class_idx >= SIZE_CLASS_COUNT) || (global_size_class[class_idx].block_size >= size),
	                "Size class misconfiguration");
	rpmalloc_assert((class_idx >= SIZE_CLASS_COUNT) || (global_size_class[class_idx - 1].block_size < size),
	                "Size class misconfiguration");
	return class_idx;
}

static inline page_type_t
get_page_type(uint32_t size_class) {
	if (size_class < SMALL_SIZE_CLASS_COUNT)
		return PAGE_SMALL;
	else if (size_class < (SMALL_SIZE_CLASS_COUNT + MEDIUM_SMALL_SIZE_CLASS_COUNT))
		return PAGE_MEDIUM_SMALL;
	else if (size_class < (SMALL_SIZE_CLASS_COUNT + MEDIUM_SMALL_SIZE_CLASS_COUNT + MEDIUM_LARGE_SIZE_CLASS_COUNT))
		return PAGE_MEDIUM_LARGE;
	else if (size_class < SIZE_CLASS_COUNT)
		return PAGE_LARGE;
	return PAGE_HUGE;
}

static inline size_t
get_page_aligned_size(size_t size) {
	size_t unalign = size % global_config.page_size;
	if (unalign)
		size += global_config.page_size - unalign;
	return size;
}

////////////
///
/// OS entry points
///
//////

static void
os_set_page_name(void* address, size_t size) {
#if defined(__linux__) || defined(__ANDROID__)
	const char* name = os_huge_pages ? global_config.huge_page_name : global_config.page_name;
	if ((address == MAP_FAILED) || !name)
		return;
	// If the kernel does not support CONFIG_ANON_VMA_NAME or if the call fails
	// (e.g. invalid name) it is a no-op basically.
	(void)prctl(PR_SET_VMA, PR_SET_VMA_ANON_NAME, (uintptr_t)address, size, (uintptr_t)name);
#else
	(void)sizeof(size);
	(void)sizeof(address);
#endif
}

static void*
os_mmap(size_t size, size_t alignment, size_t* offset, size_t* mapped_size) {
	size_t map_size = size + alignment;
#if PLATFORM_WINDOWS
	// Ok to MEM_COMMIT - according to MSDN, "actual physical pages are not allocated unless/until the virtual addresses
	// are actually accessed". But if we enable decommit it's better to not immediately commit and instead commit per
	// page to avoid saturating the OS commit limit
#if ENABLE_DECOMMIT
	DWORD do_commit = 0;
	if (global_config.disable_decommit)
	    do_commit = MEM_COMMIT;
#else
	DWORD do_commit = MEM_COMMIT;
#endif
	const DWORD base_flags = MEM_RESERVE | do_commit;
	const DWORD large_flag = (os_huge_pages ? MEM_LARGE_PAGES : 0);
	void* ptr = 0;
	if (alignment && os_virtualalloc2) {
		// Map an aligned region directly so no alignment padding is retained per mapping
		MEM_ADDRESS_REQUIREMENTS reqs;
		memset(&reqs, 0, sizeof(reqs));
		reqs.Alignment = alignment;
		MEM_EXTENDED_PARAMETER param;
		memset(&param, 0, sizeof(param));
		param.Type = MemExtendedParameterAddressRequirements;
		param.Pointer = &reqs;
		map_size = size;
		ptr = os_virtualalloc2(0, 0, map_size, large_flag | base_flags, PAGE_READWRITE, &param, 1);
		if (!ptr && large_flag)
			ptr = os_virtualalloc2(0, 0, map_size, base_flags, PAGE_READWRITE, &param, 1);
	} else {
		// Pre Windows 10: over-allocate and offset for alignment below
		ptr = VirtualAlloc(0, map_size, large_flag | base_flags, PAGE_READWRITE);
		if (!ptr && large_flag)
			ptr = VirtualAlloc(0, map_size, base_flags, PAGE_READWRITE);
	}
#else
	int flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_UNINITIALIZED;
#if defined(__APPLE__) && !TARGET_OS_IPHONE && !TARGET_OS_SIMULATOR
	int fd = (int)VM_MAKE_TAG(240U);
	if (os_huge_pages)
		fd |= VM_FLAGS_SUPERPAGE_SIZE_2MB;
	void* ptr = mmap(0, map_size, PROT_READ | PROT_WRITE, flags, fd, 0);
	if (((ptr == MAP_FAILED) || !ptr) && os_huge_pages) {
		// Superpage allocations can fail due to physical memory fragmentation,
		// fall back to regular pages
		ptr = mmap(0, map_size, PROT_READ | PROT_WRITE, flags, (int)VM_MAKE_TAG(240U), 0);
	}
#elif defined(MAP_HUGETLB)
	int huge_flags = 0;
	if (os_huge_pages) {
		huge_flags = MAP_HUGETLB;
#if defined(MAP_HUGE_SHIFT)
		// Explicitly request the detected huge page size, the system default huge
		// page size can be a different (unusable) size like 1GiB
		if (os_huge_page_shift)
			huge_flags |= (int)(os_huge_page_shift << MAP_HUGE_SHIFT);
#endif
	}
	void* ptr =
	    mmap(0, map_size, PROT_READ | PROT_WRITE | PROT_MAX(PROT_READ | PROT_WRITE), huge_flags | flags, -1, 0);
#if defined(MADV_HUGEPAGE)
	// In some configurations, huge pages allocations might fail thus
	// we fallback to normal allocations and promote the region as transparent huge page
	if ((ptr == MAP_FAILED || !ptr) && os_huge_pages) {
		ptr = mmap(0, map_size, PROT_READ | PROT_WRITE, flags, -1, 0);
		if (ptr && (ptr != MAP_FAILED)) {
			// Best-effort promotion to transparent huge pages; ignore failures (for
			// example when THP is disabled system-wide, where madvise returns EINVAL).
			// The mapping is still valid, just backed by normal pages.
			(void)madvise(ptr, map_size, MADV_HUGEPAGE);
		}
	} else if (os_thp && ptr && (ptr != MAP_FAILED)) {
		// Transparent huge page mode, advise the kernel to back this mapping with
		// huge pages without requiring a preallocated huge page pool
		(void)madvise(ptr, map_size, MADV_HUGEPAGE);
	}
#endif
	os_set_page_name(ptr, map_size);
#elif defined(MAP_ALIGNED)
	const size_t align = (sizeof(size_t) * 8) - (size_t)(__builtin_clzl(size - 1));
	void* ptr = mmap(0, map_size, PROT_READ | PROT_WRITE, (os_huge_pages ? MAP_ALIGNED(align) : 0) | flags, -1, 0);
#elif defined(MAP_ALIGN)
	caddr_t base = (os_huge_pages ? (caddr_t)(4 << 20) : 0);
	void* ptr = mmap(base, map_size, PROT_READ | PROT_WRITE, (os_huge_pages ? MAP_ALIGN : 0) | flags, -1, 0);
#else
	void* ptr = mmap(0, map_size, PROT_READ | PROT_WRITE, flags, -1, 0);
#endif
	if (ptr == MAP_FAILED)
		ptr = 0;
#endif
	if (!ptr) {
		if (global_memory_interface->map_fail_callback) {
			if (global_memory_interface->map_fail_callback(map_size))
				return os_mmap(size, alignment, offset, mapped_size);
		} else {
			rpmalloc_assert(ptr != 0, "Failed to map more virtual memory");
		}
		return 0;
	}
	if (alignment) {
		size_t padding = ((uintptr_t)ptr & (uintptr_t)(alignment - 1));
		if (padding)
			padding = alignment - padding;
		rpmalloc_assert(padding <= alignment, "Internal failure in padding");
		rpmalloc_assert(!(padding % 8), "Internal failure in padding");
#if !PLATFORM_WINDOWS
		// Unmap the alignment padding head and tail to avoid holding on to unusable
		// address space (and huge page reservations when using huge pages). Both the
		// mmap result and the alignment are multiples of the mapping page size, so
		// head and tail stay page aligned (huge page aligned for huge page mappings)
		if (padding)
			munmap(ptr, padding);
		ptr = pointer_offset(ptr, padding);
		if (alignment - padding)
			munmap(pointer_offset(ptr, size), alignment - padding);
		*offset = 0;
		map_size = size;
#else
		ptr = pointer_offset(ptr, padding);
		*offset = padding;
#endif
	}
	*mapped_size = map_size;
#if ENABLE_STATISTICS
	size_t page_count = map_size / global_config.page_size;
	size_t page_mapped_current =
	    atomic_fetch_add_explicit(&global_statistics.page_mapped, page_count, memory_order_relaxed) + page_count;
	size_t page_mapped_peak = atomic_load_explicit(&global_statistics.page_mapped_peak, memory_order_relaxed);
	while (page_mapped_current > page_mapped_peak) {
		if (atomic_compare_exchange_weak_explicit(&global_statistics.page_mapped_peak, &page_mapped_peak,
		                                          page_mapped_current, memory_order_relaxed, memory_order_relaxed))
			break;
	}
#endif
	return ptr;
}

//! Whether newly mapped regions are reserve-only (pages committed on demand). Mirrors the
//  commit choice os_mmap makes; the single definition spans/heaps capture at map time.
static inline int
os_commit_on_demand(void) {
#if ENABLE_DECOMMIT
	return global_config.disable_decommit ? 0 : 1;
#else
	return 0;
#endif
}

static int
os_mcommit(void* address, size_t size) {
#if ENABLE_DECOMMIT
	// Gated by callers on the per-span commit_on_demand flag; spans committed in full at
	// map time (incl. huge/large pages, which cannot be partially committed) never reach here
#if PLATFORM_WINDOWS
	if (!VirtualAlloc(address, size, MEM_COMMIT, PAGE_READWRITE)) {
		if (global_memory_interface->map_fail_callback && global_memory_interface->map_fail_callback(size))
			return os_mcommit(address, size);
		rpmalloc_assert(0, "Failed to commit virtual memory block");
		return 1;
	}
#else
	/*
	if (mprotect(address, size, PROT_READ | PROT_WRITE)) {
		rpmalloc_assert(0, "Failed to commit virtual memory block");
	}
	*/
#endif
#if ENABLE_STATISTICS
	size_t page_count = size / global_config.page_size;
	atomic_fetch_add_explicit(&global_statistics.page_commit, page_count, memory_order_relaxed);
	size_t page_active_current =
	    atomic_fetch_add_explicit(&global_statistics.page_active, page_count, memory_order_relaxed) + page_count;
	size_t page_active_peak = atomic_load_explicit(&global_statistics.page_active_peak, memory_order_relaxed);
	while (page_active_current > page_active_peak) {
		if (atomic_compare_exchange_weak_explicit(&global_statistics.page_active_peak, &page_active_peak,
		                                          page_active_current, memory_order_relaxed, memory_order_relaxed))
			break;
	}
#endif
#endif
	(void)sizeof(address);
	(void)sizeof(size);
	return 0;
}

static int
os_mdecommit(void* address, size_t size) {
#if ENABLE_DECOMMIT
	// Unlike os_mcommit, honor the live flag so the Linux fallback below can stop all
	// further decommits by setting disable_decommit mid-run
	if (global_config.disable_decommit)
		return 1;
#if PLATFORM_WINDOWS
	if (!VirtualFree(address, size, MEM_DECOMMIT)) {
		rpmalloc_assert(0, "Failed to decommit virtual memory block");
		return 1;
	}
#else
	/*
	if (mprotect(address, size, PROT_NONE)) {
		rpmalloc_assert(0, "Failed to decommit virtual memory block");
	}
	*/
#if defined(MADV_DONTNEED)
	if (madvise(address, size, MADV_DONTNEED)) {
#elif defined(MADV_FREE_REUSABLE)
	int ret;
	while ((ret = madvise(address, size, MADV_FREE_REUSABLE)) == -1 && (errno == EAGAIN))
		errno = 0;
	if ((ret == -1) && (errno != 0)) {
#elif defined(MADV_PAGEOUT)
	if (madvise(address, size, MADV_PAGEOUT)) {
#elif defined(MADV_FREE)
	if (madvise(address, size, MADV_FREE)) {
#else
	if (posix_madvise(address, size, POSIX_MADV_DONTNEED)) {
#endif
#if defined(__linux__)
		if (os_huge_pages) {
			// MADV_DONTNEED on huge page mappings requires Linux 5.18 or later, keep
			// pages committed and stop further decommit attempts
			global_config.disable_decommit = 1;
			return 1;
		}
#endif
		rpmalloc_assert(0, "Failed to decommit virtual memory block");
		return 1;
	}
#endif
#if ENABLE_STATISTICS
	size_t page_count = size / global_config.page_size;
	atomic_fetch_add_explicit(&global_statistics.page_decommit, page_count, memory_order_relaxed);
	statistics_sub_saturating(&global_statistics.page_active, page_count);
#endif
#else
	(void)sizeof(address);
	(void)sizeof(size);
#endif
	return 0;
}

static void
os_munmap(void* address, size_t offset, size_t mapped_size) {
	(void)sizeof(mapped_size);
	address = pointer_offset(address, -(int32_t)offset);
#if ENABLE_UNMAP
#if PLATFORM_WINDOWS
	if (!VirtualFree(address, 0, MEM_RELEASE)) {
		rpmalloc_assert(0, "Failed to unmap virtual memory block");
	}
#else
	if (munmap(address, mapped_size))
		rpmalloc_assert(0, "Failed to unmap virtual memory block");
#endif
#if ENABLE_STATISTICS
	size_t page_count = mapped_size / global_config.page_size;
	statistics_sub_saturating(&global_statistics.page_mapped, page_count);
	statistics_sub_saturating(&global_statistics.page_active, page_count);
#endif
#endif
}

////////////
///
/// Page interface
///
//////

static inline span_t*
page_get_span(page_t* page) {
	return (span_t*)((uintptr_t)page & SPAN_MASK);
}

static inline size_t
page_get_size(page_t* page) {
	if (page->page_type == PAGE_SMALL)
		return SMALL_PAGE_SIZE;
	else if (page->page_type == PAGE_MEDIUM_SMALL)
		return MEDIUM_SMALL_PAGE_SIZE;
	else if (page->page_type == PAGE_MEDIUM_LARGE)
		return MEDIUM_LARGE_PAGE_SIZE;
	else if (page->page_type == PAGE_LARGE)
		return LARGE_PAGE_SIZE;
	else
		return page_get_span(page)->page_size;
}

static inline int
page_is_thread_heap(page_t* page) {
#if RPMALLOC_FIRST_CLASS_HEAPS
	return (!page->heap->owner_thread || (page->heap->owner_thread == get_thread_id()));
#else
	return (page->heap->owner_thread == get_thread_id());
#endif
}

static inline block_t*
page_block_start(page_t* page) {
	return pointer_offset(page, PAGE_HEADER_SIZE);
}

static inline block_t*
page_block(page_t* page, uint32_t block_index) {
	return pointer_offset(page, PAGE_HEADER_SIZE + (page->block_size * block_index));
}

static inline uint32_t
page_block_index(page_t* page, block_t* block) {
	block_t* block_first = page_block_start(page);
	return (uint32_t)pointer_diff(block, block_first) / page->block_size;
}

static inline uint32_t
page_block_from_thread_free_list(page_t* page, uint64_t token, block_t** block) {
	uint32_t block_index = (uint32_t)(token & 0xFFFFFFFFULL);
	uint32_t list_count = (uint32_t)((token >> 32ULL) & 0xFFFFFFFFULL);
	*block = list_count ? page_block(page, block_index) : 0;
	return list_count;
}

static inline uint64_t
page_block_to_thread_free_list(page_t* page, uint32_t block_index, uint32_t list_count) {
	(void)sizeof(page);
	return ((uint64_t)list_count << 32ULL) | (uint64_t)block_index;
}

static inline block_t*
page_block_realign(page_t* page, block_t* block) {
	void* blocks_start = page_block_start(page);
	uint32_t block_offset = (uint32_t)pointer_diff(block, blocks_start);
	return pointer_offset(block, -(int32_t)(block_offset % page->block_size));
}

static block_t*
page_get_local_free_block(page_t* page) {
	block_t* block = page->local_free;
	page->local_free = block->next;
	--page->local_free_count;
	++page->block_used;
	return block;
}

static inline void
page_decommit_memory_pages(page_t* page) {
	if (page->is_decommitted)
		return;
	// Only reserve-only spans support decommit
	if (!page->commit_on_demand)
		return;
	// Keep the prefix (holding the page header) committed and decommit the rest. The prefix
	// is the span's fixed map-time page size, so commit and decommit always use the same one.
	size_t commit_prefix = (size_t)1 << page->commit_prefix_shift;
	size_t page_size = page_get_size(page);
	if (page_size <= commit_prefix)
		return;
	void* extra_page = pointer_offset(page, commit_prefix);
	size_t extra_page_size = page_size - commit_prefix;
	if (global_memory_interface->memory_decommit(extra_page, extra_page_size) != 0)
		return;
#if RPMALLOC_HEAP_STATISTICS && ENABLE_DECOMMIT
	if (page->heap)
		page->heap->stats.committed_size -= extra_page_size;
#endif
	page->is_decommitted = 1;
}

static inline int
page_commit_memory_pages(page_t* page) {
	if (!page->is_decommitted)
		return 0;
	size_t commit_prefix = (size_t)1 << page->commit_prefix_shift;
	void* extra_page = pointer_offset(page, commit_prefix);
	size_t extra_page_size = page_get_size(page) - commit_prefix;
	if (global_memory_interface->memory_commit(extra_page, extra_page_size) != 0)
		return 1;
	page->is_decommitted = 0;
#if ENABLE_DECOMMIT
#if RPMALLOC_HEAP_STATISTICS
	if (page->heap)
		page->heap->stats.committed_size += extra_page_size;
#endif
#if !defined(__APPLE__)
	// When page is recommitted, the blocks in the second memory page and forward
	// will be zeroed out by OS - take advantage in zalloc/calloc calls and make sure
	// blocks in first page is zeroed out
	void* first_page = pointer_offset(page, PAGE_HEADER_SIZE);
	memset(first_page, 0, commit_prefix - PAGE_HEADER_SIZE);
	page->is_zero = 1;
#endif
#endif
	return 0;
}

static void
page_available_to_free(page_t* page) {
	rpmalloc_assert(page->is_full == 0, "Page full flag internal failure");
	rpmalloc_assert(page->is_decommitted == 0, "Page decommitted flag internal failure");
	heap_t* heap = page->heap;
	if (heap->page_available[page->size_class] == page) {
		heap->page_available[page->size_class] = page->next;
	} else {
		page->prev->next = page->next;
		if (page->next)
			page->next->prev = page->prev;
	}
	page->is_free = 1;
	page->is_zero = 0;
	page->next = heap->page_free[page->page_type];
	heap->page_free[page->page_type] = page;
	if (++heap->page_free_commit_count[page->page_type] >= global_page_free_overflow[page->page_type])
		heap_page_free_decommit(heap, page->page_type, global_page_free_retain[page->page_type]);
}

static void
page_full_to_available(page_t* page) {
	rpmalloc_assert(page->is_full == 1, "Page full flag internal failure");
	rpmalloc_assert(page->is_decommitted == 0, "Page decommitted flag internal failure");
	heap_t* heap = page->heap;
	page->next = heap->page_available[page->size_class];
	if (page->next)
		page->next->prev = page;
	heap->page_available[page->size_class] = page;
	page->is_full = 0;
	if (page->has_aligned_block == 0)
		page->generic_free = 0;
}

static void
page_full_to_free_on_new_heap(page_t* page, heap_t* heap) {
	rpmalloc_assert(heap->id, "Page full to free on default heap");
	rpmalloc_assert(page->is_full == 1, "Page full flag internal failure");
	rpmalloc_assert(page->is_decommitted == 0, "Page decommitted flag internal failure");
	page->is_full = 0;
	page->is_free = 1;
	page->heap = heap;
	atomic_store_explicit(&page->thread_free, 0, memory_order_release);
	page->next = heap->page_free[page->page_type];
	heap->page_free[page->page_type] = page;
	if (++heap->page_free_commit_count[page->page_type] >= global_page_free_overflow[page->page_type])
		heap_page_free_decommit(heap, page->page_type, global_page_free_retain[page->page_type]);
}

static void
page_available_to_full(page_t* page) {
	heap_t* heap = page->heap;
	if (heap->page_available[page->size_class] == page) {
		heap->page_available[page->size_class] = page->next;
	} else {
		page->prev->next = page->next;
		if (page->next)
			page->next->prev = page->prev;
	}
	page->is_full = 1;
	page->is_zero = 0;
	page->generic_free = 1;
}

static inline void
page_put_local_free_block(page_t* page, block_t* block) {
	block->next = page->local_free;
	page->local_free = block;
	++page->local_free_count;
	_rpmalloc_stat_add_free(page->heap, page->size_class, 1);
	if (UNEXPECTED(--page->block_used == 0)) {
		page_available_to_free(page);
	} else if (UNEXPECTED(page->is_full != 0)) {
		page_full_to_available(page);
	}
}

static NOINLINE void
page_adopt_thread_free_block_list(page_t* page) {
	if (page->local_free)
		return;
	unsigned long long thread_free = atomic_load_explicit(&page->thread_free, memory_order_relaxed);
	if (thread_free != 0) {
		// Other threads can only replace with another valid list head, this will never change to 0 in other threads
		uint32_t spin = 0;
		while (!atomic_compare_exchange_weak_explicit(&page->thread_free, &thread_free, 0, memory_order_acquire,
		                                              memory_order_relaxed))
			wait_spin(&spin);
		page->local_free_count = page_block_from_thread_free_list(page, thread_free, &page->local_free);
		rpmalloc_assert(page->local_free_count <= page->block_used, "Page thread free list count internal failure");
		page->block_used -= page->local_free_count;
		_rpmalloc_stat_add_free(page->heap, page->size_class, page->local_free_count);
	}
}

static NOINLINE void
page_put_thread_free_block(page_t* page, block_t* block) {
	atomic_thread_fence(memory_order_acquire);
	if (page->is_full) {
		// Page is full, put the block in the heap thread free list instead, otherwise
		// the heap will not pick up the free blocks until a thread local free happens
		heap_t* heap = page->heap;
		uintptr_t prev_head = atomic_load_explicit(&heap->thread_free[page->page_type], memory_order_relaxed);
		block->next = (void*)prev_head;
		uint32_t spin = 0;
		while (!atomic_compare_exchange_weak_explicit(&heap->thread_free[page->page_type], &prev_head, (uintptr_t)block,
		                                              memory_order_release, memory_order_relaxed)) {
			block->next = (void*)prev_head;
			wait_spin(&spin);
		}
	} else {
		unsigned long long prev_thread_free = atomic_load_explicit(&page->thread_free, memory_order_relaxed);
		uint32_t block_index = page_block_index(page, block);
		rpmalloc_assert(page_block(page, block_index) == block, "Block pointer is not aligned to start of block");
		uint32_t list_size = page_block_from_thread_free_list(page, prev_thread_free, &block->next) + 1;
		uint64_t thread_free = page_block_to_thread_free_list(page, block_index, list_size);
		uint32_t spin = 0;
		while (!atomic_compare_exchange_weak_explicit(&page->thread_free, &prev_thread_free, thread_free,
		                                              memory_order_release, memory_order_relaxed)) {
			list_size = page_block_from_thread_free_list(page, prev_thread_free, &block->next) + 1;
			thread_free = page_block_to_thread_free_list(page, block_index, list_size);
			wait_spin(&spin);
		}
	}
}

static void
page_push_local_free_to_heap(page_t* page) {
	// Push the page free list as the fast track list of free blocks for heap
	page->heap->local_free[page->size_class] = page->local_free;
	page->block_used += page->local_free_count;
	page->local_free = 0;
	page->local_free_count = 0;
}

static NOINLINE void*
page_initialize_blocks(page_t* page) {
	rpmalloc_assert(page->block_initialized < page->block_count, "Block initialization internal failure");
	block_t* block = page_block(page, page->block_initialized);
	++page->block_initialized;
	++page->block_used;

	if ((page->page_type == PAGE_SMALL) && (page->block_size < (global_config.page_size >> 1))) {
		// Link up until next memory page in free list
		void* memory_page_start = (void*)((uintptr_t)block & ~(uintptr_t)(global_config.page_size - 1));
		void* memory_page_next = pointer_offset(memory_page_start, global_config.page_size);
		block_t* free_block = pointer_offset(block, page->block_size);
		block_t* first_block = free_block;
		block_t* last_block = free_block;
		uint32_t list_count = 0;
		uint32_t max_list_count = page->block_count - page->block_initialized;
		while (((void*)free_block < memory_page_next) && (list_count < max_list_count)) {
			last_block = free_block;
			free_block->next = pointer_offset(free_block, page->block_size);
			free_block = free_block->next;
			++list_count;
		}
		if (list_count) {
			last_block->next = 0;
			page->local_free = first_block;
			page->block_initialized += list_count;
			page->local_free_count = list_count;
		}
	}

	return block;
}

static inline RPMALLOC_ALLOCATOR void*
page_allocate_block(page_t* page, unsigned int zero) {
	unsigned int is_zero = 0;
	block_t* block = (page->local_free != 0) ? page_get_local_free_block(page) : 0;
	if (UNEXPECTED(block == 0)) {
		if (atomic_load_explicit(&page->thread_free, memory_order_acquire) != 0) {
			page_adopt_thread_free_block_list(page);
			block = (page->local_free != 0) ? page_get_local_free_block(page) : 0;
		}
		if (block == 0) {
			block = page_initialize_blocks(page);
			is_zero = page->is_zero;
		}
	}

	rpmalloc_assert(page->block_used <= page->block_count, "Page block use counter out of sync");
	if (page->local_free && !page->heap->local_free[page->size_class])
		page_push_local_free_to_heap(page);

	// The page might be full when free list has been pushed to heap local free list,
	// check if there is a thread free list to adopt
	if (page->block_used == page->block_count) {
		page_adopt_thread_free_block_list(page);
		if (page->block_used == page->block_count) {
			// Page is now fully utilized
			rpmalloc_assert(!page->is_full, "Page block use counter out of sync with full flag");
			page_available_to_full(page);
		}
	}

	if (zero) {
		if (!is_zero)
			memset(block, 0, page->block_size);
		else
			*(uintptr_t*)block = 0;
	}

	return block;
}

////////////
///
/// Span interface
///
//////

static inline int
span_is_thread_heap(span_t* span) {
#if RPMALLOC_FIRST_CLASS_HEAPS
	return (!span->heap->owner_thread || (span->heap->owner_thread == get_thread_id()));
#else
	return (span->heap->owner_thread == get_thread_id());
#endif
}

static inline page_t*
span_get_page_from_block(span_t* span, void* block) {
	return (page_t*)((uintptr_t)block & span->page_address_mask);
}

//! Find or allocate a page from the given span
static inline page_t*
span_allocate_page(span_t* span) {
	// Allocate path, initialize a new chunk of memory for a page in the given span
	rpmalloc_assert(span->page_initialized < span->page_count, "Page initialization internal failure");
	heap_t* heap = span->heap;
	page_t* page = pointer_offset(span, span->page_size * span->page_initialized);

#if ENABLE_DECOMMIT
	// Reserve-only spans commit later pages on demand; fully committed spans are already
	// committed from the map and must not be committed per page
	if (span->page_initialized && span->page.commit_on_demand) {
		if (global_memory_interface->memory_commit(page, span->page_size) != 0)
			return 0;
#if RPMALLOC_HEAP_STATISTICS
		heap->stats.committed_size += span->page_size;
#endif
	}
#endif
	++span->page_initialized;

	page->page_type = span->page_type;
	page->commit_on_demand = span->page.commit_on_demand;
	page->commit_prefix_shift = span->page.commit_prefix_shift;
	page->is_zero = 1;
	page->heap = heap;
	rpmalloc_assert(page_is_thread_heap(page), "Page owner thread mismatch");

	if (span->page_initialized == span->page_count) {
		// Span fully utilized
		rpmalloc_assert(span == heap->span_partial[span->page_type], "Span partial tracking out of sync");
		heap->span_partial[span->page_type] = 0;

		span->next = heap->span_used[span->page_type];
		heap->span_used[span->page_type] = span;
	}

	return page;
}

//! Cache recently freed huge block mappings for reuse instead of paying a
//! munmap + mmap + page fault round trip per huge allocation cycle. Entries
//! are bounded by a committed byte budget and unmapped after an idle epoch.
//! A cached mapping is reused when its size fits the request within the size
//! class spacing (25% overshoot).
#ifndef HUGE_CACHE_SLOT_COUNT
#define HUGE_CACHE_SLOT_COUNT 32
#endif
#ifndef HUGE_CACHE_COMMITTED_LIMIT
#define HUGE_CACHE_COMMITTED_LIMIT (128 * 1024 * 1024)
#endif
//! Largest single mapping the cache will hold - keeps one rare giant from
//! monopolizing the committed budget and starving caching of the smaller,
//! more frequently cycled sizes
#ifndef HUGE_CACHE_ENTRY_LIMIT
#define HUGE_CACHE_ENTRY_LIMIT (HUGE_CACHE_COMMITTED_LIMIT / 4)
#endif
#ifndef HUGE_CACHE_EPOCH_MS
#define HUGE_CACHE_EPOCH_MS 1000
#endif

#if HUGE_CACHE_SLOT_COUNT

//! Coarse monotonic time in milliseconds (no syscall on Linux/vDSO)
static inline uint64_t
monotonic_time_ms(void) {
#if PLATFORM_WINDOWS
	return (uint64_t)GetTickCount64();
#else
	struct timespec ts;
#if defined(CLOCK_MONOTONIC_COARSE)
	clock_gettime(CLOCK_MONOTONIC_COARSE, &ts);
#else
	clock_gettime(CLOCK_MONOTONIC, &ts);
#endif
	return ((uint64_t)ts.tv_sec * 1000ULL) + ((uint64_t)ts.tv_nsec / 1000000ULL);
#endif
}

typedef struct huge_cache_t {
	//! Cache lock
	atomic_uintptr_t lock;
	//! Committed bytes currently cached
	size_t committed;
	//! Cached mappings with their free timestamp
	struct {
		span_t* span;
		uint64_t free_ms;
	} slot[HUGE_CACHE_SLOT_COUNT];
} huge_cache_t;

static huge_cache_t global_huge_cache;

static inline void
huge_cache_lock_acquire(void) {
	uintptr_t lock = 0;
	uint32_t spin = 0;
	while (!atomic_compare_exchange_weak_explicit(&global_huge_cache.lock, &lock, 1, memory_order_acquire,
	                                              memory_order_relaxed)) {
		lock = 0;
		wait_spin(&spin);
	}
}

static inline void
huge_cache_lock_release(void) {
	atomic_store_explicit(&global_huge_cache.lock, 0, memory_order_release);
}

//! Unmap cached mappings that have been idle for an epoch (syscalls are made
//! outside the cache lock)
static void
huge_cache_purge_aged(uint64_t now) {
	span_t* purge[HUGE_CACHE_SLOT_COUNT];
	uint32_t purge_count = 0;
	huge_cache_lock_acquire();
	for (uint32_t islot = 0; islot < HUGE_CACHE_SLOT_COUNT; ++islot) {
		span_t* span = global_huge_cache.slot[islot].span;
		if (span && ((now - global_huge_cache.slot[islot].free_ms) >= HUGE_CACHE_EPOCH_MS)) {
			global_huge_cache.committed -= (size_t)span->page_count * span->page_size;
			global_huge_cache.slot[islot].span = 0;
			purge[purge_count++] = span;
		}
	}
	huge_cache_lock_release();
	for (uint32_t ispan = 0; ispan < purge_count; ++ispan)
		global_memory_interface->memory_unmap(purge[ispan], purge[ispan]->offset, purge[ispan]->mapped_size);
}

//! Unmap all cached huge mappings, the cache must not outlive allocator finalization
static void
huge_cache_flush(void) {
	span_t* purge[HUGE_CACHE_SLOT_COUNT];
	uint32_t purge_count = 0;
	huge_cache_lock_acquire();
	for (uint32_t islot = 0; islot < HUGE_CACHE_SLOT_COUNT; ++islot) {
		span_t* span = global_huge_cache.slot[islot].span;
		if (span) {
			global_huge_cache.committed -= (size_t)span->page_count * span->page_size;
			global_huge_cache.slot[islot].span = 0;
			purge[purge_count++] = span;
		}
	}
	huge_cache_lock_release();
	for (uint32_t ispan = 0; ispan < purge_count; ++ispan)
		global_memory_interface->memory_unmap(purge[ispan], purge[ispan]->offset, purge[ispan]->mapped_size);
}

//! Try to cache a freed huge mapping, returns nonzero if cached
static int
huge_cache_push(span_t* span) {
	size_t committed_size = (size_t)span->page_count * span->page_size;
	if (committed_size > HUGE_CACHE_ENTRY_LIMIT)
		return 0;
	uint64_t now = monotonic_time_ms();
	huge_cache_purge_aged(now);
	int cached = 0;
	huge_cache_lock_acquire();
	if (global_huge_cache.committed + committed_size <= HUGE_CACHE_COMMITTED_LIMIT) {
		for (uint32_t islot = 0; islot < HUGE_CACHE_SLOT_COUNT; ++islot) {
			if (!global_huge_cache.slot[islot].span) {
				global_huge_cache.slot[islot].span = span;
				global_huge_cache.slot[islot].free_ms = now;
				global_huge_cache.committed += committed_size;
				cached = 1;
				break;
			}
		}
	}
	huge_cache_lock_release();
	return cached;
}

//! Try to pop a cached huge mapping fitting the requested size (within the
//! size class spacing), returns 0 on miss
static span_t*
huge_cache_pop(size_t alloc_size) {
	// Racy emptiness peek is fine - a missed entry only costs a fresh mapping
	int empty = 1;
	for (uint32_t islot = 0; islot < HUGE_CACHE_SLOT_COUNT; ++islot) {
		if (global_huge_cache.slot[islot].span) {
			empty = 0;
			break;
		}
	}
	if (empty)
		return 0;
	size_t size_limit = alloc_size + (alloc_size >> 2);
	span_t* best = 0;
	size_t best_size = 0;
	uint32_t best_slot = 0;
	huge_cache_lock_acquire();
	for (uint32_t islot = 0; islot < HUGE_CACHE_SLOT_COUNT; ++islot) {
		span_t* span = global_huge_cache.slot[islot].span;
		if (!span)
			continue;
		size_t committed_size = (size_t)span->page_count * span->page_size;
		if ((committed_size >= alloc_size) && (committed_size <= size_limit) &&
		    (!best || (committed_size < best_size))) {
			best = span;
			best_size = committed_size;
			best_slot = islot;
		}
	}
	if (best) {
		global_huge_cache.slot[best_slot].span = 0;
		global_huge_cache.committed -= best_size;
	}
	huge_cache_lock_release();
	return best;
}

#endif

static NOINLINE void
span_deallocate_block(span_t* span, page_t* page, void* block) {
	if (UNEXPECTED(page->page_type == PAGE_HUGE)) {
#if ENABLE_STATISTICS
		statistics_sub_saturating(&global_statistics.huge_alloc, (size_t)span->page_count * span->page_size);
#endif
#if RPMALLOC_HEAP_STATISTICS
		if (span->heap) {
			span->heap->stats.mapped_size -= span->mapped_size;
#if ENABLE_DECOMMIT
			span->heap->stats.committed_size -= span->page_count * span->page_size;
#else
			span->heap->stats.committed_size -= span->mapped_size;
#endif
		}
#endif
#if HUGE_CACHE_SLOT_COUNT
		if (huge_cache_push(span))
			return;
#endif
		global_memory_interface->memory_unmap(span, span->offset, span->mapped_size);
		return;
	}

	if (page->has_aligned_block) {
		block = page_block_realign(page, block);
	}

	int is_thread_local = page_is_thread_heap(page);
	if (EXPECTED(is_thread_local != 0)) {
		page_put_local_free_block(page, block);
	} else {
		// Multithreaded deallocation, push to deferred deallocation list.
		page_put_thread_free_block(page, block);
	}
}

////////////
///
/// Block interface
///
//////

static inline span_t*
block_get_span(block_t* block) {
	return (span_t*)((uintptr_t)block & SPAN_MASK);
}

static inline void
block_deallocate(block_t* block) {
	span_t* span = (span_t*)((uintptr_t)block & SPAN_MASK);
	page_t* page = span_get_page_from_block(span, block);
	const int is_thread_local = page_is_thread_heap(page);

#if RPMALLOC_HEAP_STATISTICS
	heap_t* heap = span->heap;
	if (heap) {
		if (span->page_type <= PAGE_LARGE)
			heap->stats.allocated_size -= page->block_size;
		else
			heap->stats.allocated_size -= ((size_t)span->page_size * (size_t)span->page_count);
	}
#endif

	// Optimized path for thread local free with non-huge block in page
	// that has no aligned blocks
	if (EXPECTED(is_thread_local != 0)) {
		if (EXPECTED(page->generic_free == 0)) {
			// Page is not huge, not full and has no aligned block - fast path
			block->next = page->local_free;
			page->local_free = block;
			++page->local_free_count;
			_rpmalloc_stat_add_free(page->heap, page->size_class, 1);
			if (UNEXPECTED(--page->block_used == 0))
				page_available_to_free(page);
		} else {
			span_deallocate_block(span, page, block);
		}
	} else {
		span_deallocate_block(span, page, block);
	}
}

static inline size_t
block_usable_size(block_t* block) {
	span_t* span = (span_t*)((uintptr_t)block & SPAN_MASK);
	if (EXPECTED(span->page_type <= PAGE_LARGE)) {
		page_t* page = span_get_page_from_block(span, block);
		void* blocks_start = pointer_offset(page, PAGE_HEADER_SIZE);
		return page->block_size - ((size_t)pointer_diff(block, blocks_start) % page->block_size);
	} else {
		return ((size_t)span->page_size * (size_t)span->page_count) - (size_t)pointer_diff(block, span);
	}
}

////////////
///
/// Heap interface
///
//////

static inline void
heap_lock_acquire(void) {
	uintptr_t lock = 0;
	uintptr_t this_lock = get_thread_id();
	uint32_t spin = 0;
	while (!atomic_compare_exchange_strong(&global_heap_lock, &lock, this_lock)) {
		lock = 0;
		wait_spin(&spin);
	}
}

static inline void
heap_lock_release(void) {
	rpmalloc_assert((uintptr_t)atomic_load_explicit(&global_heap_lock, memory_order_relaxed) == get_thread_id(),
	                "Bad heap lock");
	atomic_store_explicit(&global_heap_lock, 0, memory_order_release);
}

static inline heap_t*
heap_initialize(void* block) {
	heap_t* heap = block;
	memset_const(heap, 0, sizeof(heap_t));
	heap->id = 1 + atomic_fetch_add_explicit(&global_heap_id, 1, memory_order_relaxed);
	return heap;
}

// A heap control structure is much smaller than the mapping page size (and far smaller than a
// huge page), so a single page-aligned mapping is carved into as many heaps as fit, the first
// returned to the caller and the extras queued for reuse. This avoids mapping (and, under huge
// pages, committing) a whole page per heap. Only the first heap records the mapping offset and
// size; it owns the shared mapping and is the one heap that unmaps it at finalize.
//
// Block creation is serialized through global_heap_creating: when no heap is available, one thread
// maps and carves a block while concurrent thread starts wait for the carved heaps to be published,
// so a startup burst maps a single block instead of one block per thread. Ordinary requests reuse a
// released heap first, then a pristine carved one. First-class requests need a heap with no prior
// allocations, so they take only pristine heaps (a carved extra, or a freshly mapped master), never
// a released one, but still wait so they do not map concurrently with another creation.

static inline void
heap_release(heap_t* heap) {
#if ENABLE_STATISTICS
	statistics_sub_saturating(&global_statistics.heap_count, 1);
#endif
	heap_lock_acquire();
	if (heap->prev)
		heap->prev->next = heap->next;
	if (heap->next)
		heap->next->prev = heap->prev;
	if (global_heap_used == heap)
		global_heap_used = heap->next;
	heap->next = global_heap_queue;
	global_heap_queue = heap;
	heap_lock_release();
}

static void
rpmalloc_thread_finalize(void) {
	heap_t* heap = get_thread_heap();
	if (heap != global_heap_default) {
		heap_release(heap);
		set_thread_heap(global_heap_default);
	}
}

static void
rpmalloc_thread_destructor(void* value) {
	// If this is called on main thread assume it means rpmalloc_finalize
	// has not been called and shutdown is forced (through _exit) or unclean
	if (get_thread_id() == global_main_thread_id)
		return;
	if (value)
		rpmalloc_thread_finalize();
}

static void
rpmalloc_thread_initialize(void) {
	if (get_thread_heap() == global_heap_default)
		get_thread_heap_allocate();
}

static int
rpmalloc_initialize(rpmalloc_interface_t* memory_interface) {
	for (;;) {
		int state = atomic_load_explicit(&global_rpmalloc_init_state, memory_order_acquire);
		if (state == RPMALLOC_INIT_DONE) {
			rpmalloc_thread_initialize();
			return 0;
		}
		if (state == RPMALLOC_INIT_RUNNING) {
			while (atomic_load_explicit(&global_rpmalloc_init_state, memory_order_acquire) ==
			       RPMALLOC_INIT_RUNNING)
				thread_yield();
			continue;
		}
		int expected = RPMALLOC_INIT_UNINIT;
		if (atomic_compare_exchange_strong_explicit(&global_rpmalloc_init_state, &expected,
		                                            RPMALLOC_INIT_RUNNING, memory_order_acq_rel,
		                                            memory_order_acquire))
			break;
	}

	// Remember whether the caller explicitly requested huge pages, the detection below
	// overwrites global_config.enable_huge_pages with what is actually available.
	const int huge_pages_requested = global_config.enable_huge_pages;

	global_memory_interface = memory_interface ? memory_interface : &global_memory_interface_default;
	if (!global_memory_interface->memory_map || !global_memory_interface->memory_unmap) {
		global_memory_interface->memory_map = os_mmap;
		global_memory_interface->memory_commit = os_mcommit;
		global_memory_interface->memory_decommit = os_mdecommit;
		global_memory_interface->memory_unmap = os_munmap;
	}

#if ENABLE_OVERRIDE
	// With the standard library override, rpmalloc backs the C runtime's own allocations (for
	// example per-thread TLS allocated by the loader). Unmapping everything at finalize while the
	// process keeps running would pull that memory out from under the runtime, so the option
	// cannot be honored here; clear it (also reflected back through the effective config).
	global_config.unmap_on_finalize = 0;
#endif

#if PLATFORM_WINDOWS
	SYSTEM_INFO system_info;
	memset(&system_info, 0, sizeof(system_info));
	GetSystemInfo(&system_info);
	os_map_granularity = system_info.dwAllocationGranularity;
	os_virtualalloc2 = 0;
	{
		HMODULE kernelbase = GetModuleHandleW(L"kernelbase.dll");
		if (kernelbase)
			os_virtualalloc2 = (virtualalloc2_fn)(void (*)(void))GetProcAddress(kernelbase, "VirtualAlloc2");
	}
#else
	os_map_granularity = (size_t)sysconf(_SC_PAGESIZE);
#endif

#if PLATFORM_WINDOWS
	os_page_size = system_info.dwPageSize;
#else
	os_page_size = os_map_granularity;
#endif
	os_huge_pages = 0;
	os_thp = 0;
	os_huge_page_shift = 0;

	rpmalloc_assert(!(global_config.page_size & (global_config.page_size - 1)),
	                "Configured page size must be a power of two");

	// Establish a valid (normal) page size up front so the allocator never observes a
	// zero page size during the rest of initialization. Huge page detection below may
	// raise it to the huge page size once availability has been confirmed.
	if (!memory_interface || (global_config.page_size < os_page_size))
		global_config.page_size = os_page_size;

	if (global_config.enable_huge_pages) {
#if PLATFORM_WINDOWS
		HANDLE token = 0;
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP | WINAPI_PARTITION_SYSTEM)
		size_t large_page_minimum = GetLargePageMinimum();
#else
		size_t large_page_minimum = 2 * 1024 * 1024;
#endif
		if (large_page_minimum)
			OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token);
		if (token) {
			LUID luid;
			if (LookupPrivilegeValue(0, SE_LOCK_MEMORY_NAME, &luid)) {
				TOKEN_PRIVILEGES token_privileges;
				memset(&token_privileges, 0, sizeof(token_privileges));
				token_privileges.PrivilegeCount = 1;
				token_privileges.Privileges[0].Luid = luid;
				token_privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
				if (AdjustTokenPrivileges(token, FALSE, &token_privileges, 0, 0, 0)) {
					if (GetLastError() == ERROR_SUCCESS)
						os_huge_pages = 1;
				}
			}
			CloseHandle(token);
		}
		// Disable huge pages if the system large page size is not a power of two that fits the builtin geometry
		if (os_huge_pages && ((large_page_minimum & (large_page_minimum - 1)) || (large_page_minimum > LARGE_PAGE_SIZE)))
			os_huge_pages = 0;
		if (os_huge_pages) {
			if (large_page_minimum > os_page_size)
				os_page_size = large_page_minimum;
			if (large_page_minimum > os_map_granularity)
				os_map_granularity = large_page_minimum;
		}
#elif defined(__linux__)
		size_t huge_page_size = 0;
		char meminfo[4096];
		if (os_read_system_file("/proc/meminfo", meminfo, sizeof(meminfo))) {
			const char* line = strstr(meminfo, "Hugepagesize:");
			if (line)
				huge_page_size = (size_t)strtol(line + 13, 0, 10) * 1024;
		}
		// Sanity check the detected default huge page size, the builtin page geometry
		// cannot use huge pages larger than the largest builtin page size (e.g. 1GiB
		// default huge pages). Since the huge page size is passed explicitly to mmap,
		// a non-default 2MiB huge page pool can still be used in that case.
		if (huge_page_size & (huge_page_size - 1))
			huge_page_size = 0;
		if (huge_page_size > LARGE_PAGE_SIZE)
			huge_page_size = 2 * 1024 * 1024;
#if defined(MAP_HUGETLB)
		if (huge_page_size) {
			// Hugepagesize in /proc/meminfo only reports the configured huge page size, not
			// whether any huge pages are actually available (the static pool can be empty,
			// HugePages_Total == 0). Probe by mapping a single huge page with the same flags
			// used for real mappings and only enable huge pages if the request can actually
			// be satisfied (static pool or overcommit). This also covers the case where the
			// page size is reported but the kernel rejects that specific MAP_HUGE_* size.
			size_t huge_shift = 0;
			while (((size_t)1 << huge_shift) < huge_page_size)
				++huge_shift;
			int probe_flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB;
#if defined(MAP_HUGE_SHIFT)
			probe_flags |= (int)(huge_shift << MAP_HUGE_SHIFT);
#endif
			void* probe = mmap(0, huge_page_size, PROT_READ | PROT_WRITE, probe_flags, -1, 0);
			if ((probe != MAP_FAILED) && probe) {
				munmap(probe, huge_page_size);
				os_huge_pages = 1;
				os_huge_page_shift = huge_shift;
				os_page_size = huge_page_size;
				os_map_granularity = huge_page_size;
			}
		}
#endif
#elif defined(__FreeBSD__)
		int rc;
		size_t sz = sizeof(rc);

		if (sysctlbyname("vm.pmap.pg_ps_enabled", &rc, &sz, NULL, 0) == 0 && rc == 1) {
			os_huge_pages = 1;
			os_page_size = 2 * 1024 * 1024;
			os_map_granularity = os_page_size;
		}
#elif defined(__APPLE__)
#if defined(__x86_64__) && !TARGET_OS_IPHONE && !TARGET_OS_SIMULATOR
		// Superpages are only supported on x86-64, Apple silicon has no huge page support
		os_huge_pages = 1;
		os_page_size = 2 * 1024 * 1024;
		os_map_granularity = os_page_size;
#endif
#elif defined(__NetBSD__)
		os_huge_pages = 1;
		os_page_size = 2 * 1024 * 1024;
		os_map_granularity = os_page_size;
#endif
	}

	if (huge_pages_requested && !os_huge_pages) {
		// Explicit huge pages were requested but are not available on this system. Fail
		// initialization rather than silently backing the heap with normal pages behind a
		// huge page size. Leave the allocator uninitialized (and the requested flag cleared)
		// so the caller can detect this and re-initialize without huge pages if desired.
		global_config.enable_huge_pages = 0;
		atomic_store_explicit(&global_rpmalloc_init_state, RPMALLOC_INIT_UNINIT, memory_order_release);
		return -1;
	}

	global_config.enable_huge_pages = os_huge_pages;

	if (os_huge_pages) {
		while (((size_t)1 << os_huge_page_shift) < os_page_size)
			++os_huge_page_shift;
	}

	if (!memory_interface || (global_config.page_size < os_page_size))
		global_config.page_size = os_page_size;

#if PLATFORM_WINDOWS
	// Large pages on Windows must be committed when mapped and can never be decommitted
	if (global_config.enable_huge_pages)
		global_config.disable_decommit = 1;
#endif

#if defined(__linux__) || defined(__ANDROID__)
	// Transparent huge pages, advise mappings to be huge page backed without requiring
	// a preallocated huge page pool and without affecting page size or decommit
	if (global_config.enable_thp && !os_huge_pages && !global_config.disable_thp) {
		// When THP is disabled system-wide (mode "never") madvise(MADV_HUGEPAGE) is a
		// no-op, so report it as disabled rather than claiming THP is in use.
		char thp_state[64];
		int thp_never = 0;
		if (os_read_system_file("/sys/kernel/mm/transparent_hugepage/enabled", thp_state, sizeof(thp_state)))
			thp_never = (strstr(thp_state, "[never]") != 0);
		if (!thp_never)
			os_thp = 1;
	}
	if (global_config.disable_thp)
		(void)prctl(PR_SET_THP_DISABLE, 1, 0, 0, 0);
#endif
	global_config.enable_thp = os_thp;

	// Decommit of free pages only applies to page types larger than the configured
	// page size, disable the free page overflow pass for page types where it can
	// never apply (always the case when huge pages or a large page size is in use)
	const uint32_t page_free_overflow_default[4] = {16, 8, 4, 2};
	const uint32_t builtin_page_size[4] = {SMALL_PAGE_SIZE, MEDIUM_SMALL_PAGE_SIZE, MEDIUM_LARGE_PAGE_SIZE,
	                                       LARGE_PAGE_SIZE};
	for (unsigned int itype = 0; itype < 4; ++itype) {
		int page_can_decommit =
		    !global_config.disable_decommit && ((size_t)builtin_page_size[itype] > global_config.page_size);
		global_page_free_overflow[itype] = page_can_decommit ? page_free_overflow_default[itype] : 0xFFFFFFFFU;
	}

#ifdef _WIN32
	fls_key = FlsAlloc(&rpmalloc_thread_destructor);
#else
	pthread_key_create(&pthread_key, rpmalloc_thread_destructor);
#endif

	global_main_thread_id = get_thread_id();

	atomic_store_explicit(&global_rpmalloc_init_state, RPMALLOC_INIT_DONE, memory_order_release);

	rpmalloc_thread_initialize();

	return 0;
}

static heap_t*
heap_allocate_new(int first_class) {
	if (!global_config.page_size)
		rpmalloc_initialize(0);

	heap_lock_acquire();
	uint32_t spin = 0;
	while (1) {
		if (!first_class && global_heap_queue) {
			heap_t* heap = global_heap_queue;
			global_heap_queue = heap->next;
			heap_lock_release();
			return heap;
		}
		if (global_heap_pristine) {
			heap_t* heap = global_heap_pristine;
			global_heap_pristine = heap->next;
			heap_lock_release();
			return heap;
		}
		if (!global_heap_creating)
			break;
		heap_lock_release();
		wait_spin(&spin);
		heap_lock_acquire();
	}
	global_heap_creating = 1;
	heap_lock_release();

	size_t aligned_heap_size = HEAP_ALIGNMENT * ((sizeof(heap_t) + (HEAP_ALIGNMENT - 1)) / HEAP_ALIGNMENT);
	size_t block_size = get_page_aligned_size(aligned_heap_size);
	size_t offset = 0;
	size_t mapped_size = 0;
	block_t* block = global_memory_interface->memory_map(block_size, 0, &offset, &mapped_size);
#if ENABLE_DECOMMIT
	if (block && os_commit_on_demand() && (global_memory_interface->memory_commit(block, block_size) != 0)) {
		global_memory_interface->memory_unmap(block, offset, mapped_size);
		block = 0;
	}
#endif
	if (!block) {
		heap_lock_acquire();
		global_heap_creating = 0;
		heap_lock_release();
		return 0;
	}

	heap_t* heap = heap_initialize((void*)block);
	heap->offset = (uint32_t)offset;
	heap->mapped_size = mapped_size;

	// Carve the remaining heaps into a local chain, then splice it onto the pristine pool in one
	// step so the lock is not held across the per-heap initialization. heap_initialize zeroes each
	// structure, leaving mapped_size == 0 so the extras are not treated as mapping owners. They are
	// pristine until handed out, so first-class requests may reuse them.
	size_t heap_count = mapped_size / aligned_heap_size;
	heap_t* chain_head = 0;
	heap_t* chain_tail = 0;
	heap_t* extra_heap = (heap_t*)pointer_offset(heap, aligned_heap_size);
	for (size_t iheap = 1; iheap < heap_count; ++iheap) {
		heap_initialize((void*)extra_heap);
		if (!chain_tail)
			chain_tail = extra_heap;
		extra_heap->next = chain_head;
		chain_head = extra_heap;
		extra_heap = (heap_t*)pointer_offset(extra_heap, aligned_heap_size);
	}

	heap_lock_acquire();
	if (chain_tail) {
		chain_tail->next = global_heap_pristine;
		global_heap_pristine = chain_head;
	}
	global_heap_creating = 0;
	heap_lock_release();
	return heap;
}

// Unmaps the shared block owned by this heap. INVARIANT: heap blocks are only ever unmapped here,
// at global finalize, never while the allocator is running. Several heaps share a block, and a heap
// may be borrowed by an unrelated owner's thread or by a first-class heap that outlives the thread
// whose request mapped the block (see heap_allocate_new). Unmapping a block mid-run would leave
// those borrowed heaps pointing at freed memory, so any future block reclamation must guarantee no
// heap carved from the block is still live.
static void
heap_unmap(heap_t* heap) {
	global_memory_interface->memory_unmap(heap, heap->offset, heap->mapped_size);
}

static heap_t*
heap_allocate(int first_class) {
	heap_t* heap = heap_allocate_new(first_class);
	if (heap) {
		uintptr_t current_thread_id = get_thread_id();
		heap_lock_acquire();
		heap->next = global_heap_used;
		heap->prev = 0;
		if (global_heap_used)
			global_heap_used->prev = heap;
		global_heap_used = heap;
		heap_lock_release();
		heap->owner_thread = current_thread_id;
#if ENABLE_STATISTICS
		// Counters belong to the heap, not the thread, so alloc_current reflects the blocks live in
		// the heap's pages regardless of which thread owns it.
		atomic_fetch_add_explicit(&global_statistics.heap_count, 1, memory_order_relaxed);
#endif
	}
	return heap;
}


static void
heap_page_free_decommit(heap_t* heap, uint32_t page_type, uint32_t page_retain_count) {
	page_t* page = heap->page_free[page_type];
	while (page && page_retain_count) {
		page = page->next;
		--page_retain_count;
	}
	while (page && (page->is_decommitted == 0)) {
		page_decommit_memory_pages(page);
		// Decommit can be refused (decommit disabled, page type not larger than the
		// configured page size, or kernel rejecting decommit of huge page mappings),
		// in which case remaining pages cannot be decommitted either
		if (page->is_decommitted == 0)
			break;
		--heap->page_free_commit_count[page_type];
		page = page->next;
	}
}

static inline int
heap_make_free_page_available(heap_t* heap, uint32_t size_class, page_t* page) {
	page->size_class = size_class;
	page->block_size = global_size_class[size_class].block_size;
	page->block_count = global_size_class[size_class].block_count;
	page->block_used = 0;
	page->block_initialized = 0;
	page->local_free = 0;
	page->local_free_count = 0;
	page->is_full = 0;
	page->is_free = 0;
	page->has_aligned_block = 0;
	page->generic_free = 0;
	page->heap = heap;
	page_t* head = heap->page_available[size_class];
	page->next = head;
	page->prev = 0;
	atomic_store_explicit(&page->thread_free, 0, memory_order_release);
	if (head)
		head->prev = page;
	heap->page_available[size_class] = page;
	if (page->is_decommitted != 0)
		return page_commit_memory_pages(page);
	return 0;
}

//! Find or allocate a span for the given page type with the given size class
static inline span_t*
heap_get_span(heap_t* heap, page_type_t page_type) {
	// Fast path, available span for given page type
	if (EXPECTED(heap->span_partial[page_type] != 0))
		return heap->span_partial[page_type];

	// Fallback path, map more memory
	size_t offset = 0;
	size_t mapped_size = 0;
	span_t* span = global_memory_interface->memory_map(SPAN_SIZE, SPAN_SIZE, &offset, &mapped_size);
#if RPMALLOC_HEAP_STATISTICS
	heap->stats.mapped_size += mapped_size;
#endif
	if (EXPECTED(span != 0)) {
		rpmalloc_assert(!((uintptr_t)span & (SPAN_SIZE - 1)),
		                "memory_map returned a block not aligned to the span size");
		_rpmalloc_stat_inc_span_map(heap, page_type);
		uint32_t page_count = 0;
		uint32_t page_size = 0;
		uintptr_t page_address_mask = 0;
		if (page_type == PAGE_SMALL) {
			page_count = SPAN_SIZE / SMALL_PAGE_SIZE;
			page_size = SMALL_PAGE_SIZE;
			page_address_mask = SMALL_PAGE_MASK;
		} else if (page_type == PAGE_MEDIUM_SMALL) {
			page_count = SPAN_SIZE / MEDIUM_SMALL_PAGE_SIZE;
			page_size = MEDIUM_SMALL_PAGE_SIZE;
			page_address_mask = MEDIUM_SMALL_PAGE_MASK;
		} else if (page_type == PAGE_MEDIUM_LARGE) {
			page_count = SPAN_SIZE / MEDIUM_LARGE_PAGE_SIZE;
			page_size = MEDIUM_LARGE_PAGE_SIZE;
			page_address_mask = MEDIUM_LARGE_PAGE_MASK;
		} else {
			page_count = SPAN_SIZE / LARGE_PAGE_SIZE;
			page_size = LARGE_PAGE_SIZE;
			page_address_mask = LARGE_PAGE_MASK;
		}
		// Commit the first page for reserve-only spans (fully committed spans commit nothing
		// here); record the commit model on the span only after its header is committed
		int commit_on_demand = os_commit_on_demand();
#if ENABLE_DECOMMIT
		if (commit_on_demand) {
			if (global_memory_interface->memory_commit(span, page_size) != 0) {
				global_memory_interface->memory_unmap(span, offset, mapped_size);
				return 0;
			}
		}
#endif
		span->page.commit_on_demand = (uint32_t)commit_on_demand;
		uint32_t commit_prefix_shift = 0;
		while (((size_t)1 << commit_prefix_shift) < global_config.page_size)
			++commit_prefix_shift;
		span->page.commit_prefix_shift = commit_prefix_shift;
#if RPMALLOC_HEAP_STATISTICS
#if ENABLE_DECOMMIT
		heap->stats.committed_size += page_size;
#else
		heap->stats.committed_size += mapped_size;
#endif
#endif
		span->heap = heap;
		span->page_type = page_type;
		span->page_count = page_count;
		span->page_size = page_size;
		span->page_address_mask = page_address_mask;
		span->offset = (uint32_t)offset;
		span->mapped_size = mapped_size;

		heap->span_partial[page_type] = span;
	}

	return span;
}

static page_t*
heap_get_page(heap_t* heap, uint32_t size_class);

static void
block_deallocate(block_t* block);

static page_t*
heap_get_page_generic(heap_t* heap, uint32_t size_class) {
	page_type_t page_type = get_page_type(size_class);

	// Check if there is a free page from multithreaded deallocations
	uintptr_t block_mt = atomic_load_explicit(&heap->thread_free[page_type], memory_order_relaxed);
	if (UNEXPECTED(block_mt != 0)) {
		uint32_t spin = 0;
		while (!atomic_compare_exchange_weak_explicit(&heap->thread_free[page_type], &block_mt, 0, memory_order_acquire,
		                                              memory_order_relaxed)) {
			wait_spin(&spin);
		}
		block_t* block = (void*)block_mt;
		while (block) {
			block_t* next_block = block->next;
			block_deallocate(block);
			block = next_block;
		}
		// Retry after processing deferred thread frees
		return heap_get_page(heap, size_class);
	}

	// Check if there is a free page
	page_t* page = heap->page_free[page_type];
	if (EXPECTED(page != 0)) {
		heap->page_free[page_type] = page->next;
		if (page->is_decommitted == 0) {
			rpmalloc_assert(heap->page_free_commit_count[page_type] > 0, "Free committed page count out of sync");
			--heap->page_free_commit_count[page_type];
		}
		if (heap_make_free_page_available(heap, size_class, page) != 0)
			return 0;
		return page;
	}
	rpmalloc_assert(heap->page_free_commit_count[page_type] == 0, "Free committed page count out of sync");

	if (heap->id == 0) {
		// Thread has not yet initialized, assign heap and try again
		rpmalloc_initialize(0);
		return heap_get_page(get_thread_heap(), size_class);
	}

	// Fallback path, find or allocate span for given size class
	// If thread was not initialized, the heap for the new span
	// will be different from the local heap variable in this scope
	// (which is the default heap) - so use span page heap instead
	span_t* span = heap_get_span(heap, page_type);
	if (EXPECTED(span != 0)) {
		page = span_allocate_page(span);
		if (heap_make_free_page_available(page->heap, size_class, page) != 0)
			return 0;
	}

	return page;
}

//! Find or allocate a page for the given size class
static page_t*
heap_get_page(heap_t* heap, uint32_t size_class) {
	// Fast path, available page for given size class
	page_t* page = heap->page_available[size_class];
	if (EXPECTED(page != 0))
		return page;
	return heap_get_page_generic(heap, size_class);
}

//! Pop a block from the heap local free list
static inline RPMALLOC_ALLOCATOR void*
heap_pop_local_free(heap_t* heap, uint32_t size_class) {
	block_t** free_list = heap->local_free + size_class;
	block_t* block = *free_list;
	if (EXPECTED(block != 0))
		*free_list = block->next;
	return block;
}

//! Generic allocation path from heap pages, spans or new mapping
static NOINLINE RPMALLOC_ALLOCATOR void*
heap_allocate_block_small_to_large(heap_t* heap, uint32_t size_class, unsigned int zero) {
	page_t* page = heap_get_page(heap, size_class);
	if (EXPECTED(page != 0)) {
		// Count on the page owner, which may differ from heap when an uninitialized thread is routed
		// to its real heap, so the allocation and its later free are accounted on the same heap
		_rpmalloc_stat_inc_alloc(page->heap, size_class);
		return page_allocate_block(page, zero);
	}
	return 0;
}

//! Generic allocation path from heap pages, spans or new mapping
static NOINLINE RPMALLOC_ALLOCATOR void*
heap_allocate_block_huge(heap_t* heap, size_t size, unsigned int zero) {
	if (heap->id == 0) {
		// Thread has not yet initialized, assign heap and try again
		rpmalloc_initialize(0);
		heap = get_thread_heap();
	}
	size_t alloc_size = get_page_aligned_size(size + SPAN_HEADER_SIZE);
	size_t offset = 0;
	size_t mapped_size = 0;
	void* block = 0;
	int from_cache = 0;
#if HUGE_CACHE_SLOT_COUNT
	// Reuse a recently freed huge mapping of fitting size if available (the
	// cached span keeps its own offset/mapped_size/page_count fields)
	block = huge_cache_pop(alloc_size);
	if (block)
		from_cache = 1;
#endif
	if (!block)
		block = global_memory_interface->memory_map(alloc_size, SPAN_SIZE, &offset, &mapped_size);
	if (block) {
		span_t* span = block;
		rpmalloc_assert(!((uintptr_t)span & (SPAN_SIZE - 1)),
		                "memory_map returned a block not aligned to the span size");
		if (!from_cache)
			_rpmalloc_stat_inc_span_map(heap, PAGE_HUGE);
#if ENABLE_DECOMMIT
		if (!from_cache && os_commit_on_demand() &&
		    (global_memory_interface->memory_commit(span, alloc_size) != 0)) {
			global_memory_interface->memory_unmap(block, offset, mapped_size);
			return 0;
		}
#endif
#if RPMALLOC_HEAP_STATISTICS
		heap->stats.mapped_size += mapped_size;
#if ENABLE_DECOMMIT
		heap->stats.committed_size += alloc_size;
#else
		heap->stats.committed_size += mapped_size;
#endif
#endif
		span->heap = heap;
		span->page_type = PAGE_HUGE;
		span->page_address_mask = LARGE_PAGE_MASK;
		// Huge blocks never use the per-page commit path; set explicitly so a reused cached
		// mapping carries no stale value
		span->page.commit_on_demand = 0;
		if (!from_cache) {
			span->page_size = (uint32_t)global_config.page_size;
			span->page_count = (uint32_t)(alloc_size / global_config.page_size);
			span->offset = (uint32_t)offset;
			span->mapped_size = mapped_size;
		}
		span->page.heap = heap;
		span->page.is_full = 1;
		span->page.generic_free = 1;
		span->page.page_type = PAGE_HUGE;
#if ENABLE_STATISTICS
		size_t huge_alloc_size = (size_t)span->page_count * span->page_size;
		size_t huge_alloc_current =
		    atomic_fetch_add_explicit(&global_statistics.huge_alloc, huge_alloc_size, memory_order_relaxed) +
		    huge_alloc_size;
		size_t huge_alloc_peak = atomic_load_explicit(&global_statistics.huge_alloc_peak, memory_order_relaxed);
		while (huge_alloc_current > huge_alloc_peak) {
			if (atomic_compare_exchange_weak_explicit(&global_statistics.huge_alloc_peak, &huge_alloc_peak,
			                                          huge_alloc_current, memory_order_relaxed, memory_order_relaxed))
				break;
		}
#endif
		// Keep track of span if first class heap
		if (!heap->owner_thread) {
			span->next = heap->span_used[PAGE_HUGE];
			heap->span_used[PAGE_HUGE] = span;
		}
		void* ptr = pointer_offset(block, SPAN_HEADER_SIZE);
		if (zero)
			memset(ptr, 0, size);
#if RPMALLOC_HEAP_STATISTICS
		heap->stats.allocated_size += size;
#endif
		return ptr;
	}
	return 0;
}

static RPMALLOC_ALLOCATOR NOINLINE void*
heap_allocate_block_generic(heap_t* heap, size_t size, unsigned int zero) {
	uint32_t size_class = get_size_class(size);
	if (EXPECTED(size_class < SIZE_CLASS_COUNT)) {
#if RPMALLOC_HEAP_STATISTICS
		heap->stats.allocated_size += global_size_class[size_class].block_size;
#endif

		block_t* block = heap_pop_local_free(heap, size_class);
		if (EXPECTED(block != 0)) {
			// Fast track with small block available in heap level local free list; a hit means this
			// heap owns the block
			_rpmalloc_stat_inc_alloc(heap, size_class);
			if (zero)
				memset(block, 0, global_size_class[size_class].block_size);
			return block;
		}

		return heap_allocate_block_small_to_large(heap, size_class, zero);
	}

	return heap_allocate_block_huge(heap, size, zero);
}

//! Find or allocate a block of the given size
static inline RPMALLOC_ALLOCATOR void*
heap_allocate_block(heap_t* heap, size_t size, unsigned int zero) {
	if (size <= (SMALL_GRANULARITY * 64)) {
		uint32_t size_class = get_size_class_tiny(size);
		block_t* block = heap_pop_local_free(heap, size_class);
		if (EXPECTED(block != 0)) {
			// Fast track with small block available in heap level local free list
			if (zero)
				memset(block, 0, global_size_class[size_class].block_size);
#if RPMALLOC_HEAP_STATISTICS
			heap->stats.allocated_size += global_size_class[size_class].block_size;
#endif
			_rpmalloc_stat_inc_alloc(heap, size_class);
			return block;
		}
#if RPMALLOC_HEAP_STATISTICS
		heap->stats.allocated_size += global_size_class[size_class].block_size;
#endif
		// The size class is already known - refill directly, skipping the
		// generic path size class recomputation and dead local free list pop
		// (small_to_large counts the allocation on the owning heap)
		return heap_allocate_block_small_to_large(heap, size_class, zero);
	}
	return heap_allocate_block_generic(heap, size, zero);
}

static RPMALLOC_ALLOCATOR void*
heap_allocate_block_aligned(heap_t* heap, size_t alignment, size_t size, unsigned int zero) {
	if (alignment <= SMALL_GRANULARITY)
		return heap_allocate_block(heap, size, zero);

#if ENABLE_VALIDATE_ARGS
	if ((size + alignment) < size) {
		errno = EINVAL;
		return 0;
	}
	if (alignment & (alignment - 1)) {
		errno = EINVAL;
		return 0;
	}
#endif
	if (alignment >= RPMALLOC_MAX_ALIGNMENT) {
		errno = EINVAL;
		return 0;
	}

	size_t align_mask = alignment - 1;
	block_t* block = heap_allocate_block(heap, size + alignment, zero);
	if ((uintptr_t)block & align_mask) {
		block = (void*)(((uintptr_t)block & ~(uintptr_t)align_mask) + alignment);
		// Mark as having aligned blocks
		span_t* span = block_get_span(block);
		page_t* page = span_get_page_from_block(span, block);
		page->has_aligned_block = 1;
		page->generic_free = 1;
	}
	return block;
}

static void*
heap_reallocate_block(heap_t* heap, void* block, size_t size, size_t old_size, unsigned int flags) {
	if (block) {
		// Grab the span using guaranteed span alignment
		span_t* span = block_get_span(block);
		if (EXPECTED(span->page_type <= PAGE_LARGE)) {
			// Normal sized block
			page_t* page = span_get_page_from_block(span, block);
			void* blocks_start = pointer_offset(page, PAGE_HEADER_SIZE);
			uint32_t block_offset = (uint32_t)pointer_diff(block, blocks_start);
			uint32_t block_idx = block_offset / page->block_size;
			void* block_origin = pointer_offset(blocks_start, (size_t)block_idx * page->block_size);
			if (!old_size)
				old_size = (size_t)((ptrdiff_t)page->block_size - pointer_diff(block, block_origin));
			if ((size_t)page->block_size >= size) {
				// Still fits in block, never mind trying to save memory, but preserve data if alignment changed
				if ((block != block_origin) && !(flags & RPMALLOC_NO_PRESERVE))
					memmove(block_origin, block, old_size);
				return block_origin;
			}
		} else {
			// Huge block
			void* block_start = pointer_offset(span, SPAN_HEADER_SIZE);
			if (!old_size)
				old_size = ((size_t)span->page_size * (size_t)span->page_count) - (size_t)pointer_diff(block, span);
			if ((size < old_size) && (size >= (old_size / 2)) && (size > LARGE_BLOCK_SIZE_LIMIT)) {
				// Still fits in block, still huge and saves less than half the memory,
				// never mind trying to save memory, but preserve data if alignment changed
				if ((block_start != block) && !(flags & RPMALLOC_NO_PRESERVE))
					memmove(block_start, block, old_size);
				return block_start;
			}
		}
	} else {
		old_size = 0;
	}

	if (!!(flags & RPMALLOC_GROW_OR_FAIL))
		return 0;

	// Size is greater than block size or saves enough memory to resize, need to allocate a new block
	// and deallocate the old. Avoid hysteresis by overallocating if increase is small (below 37%)
	size_t lower_bound = old_size + (old_size >> 2) + (old_size >> 3);
	size_t new_size = (size > lower_bound) ? size : ((size > old_size) ? lower_bound : size);
	void* old_block = block;
	block = heap_allocate_block(heap, new_size, 0);
	if (block && old_block) {
		if (!(flags & RPMALLOC_NO_PRESERVE))
			memcpy(block, old_block, old_size < new_size ? old_size : new_size);
		block_deallocate(old_block);
	}

	return block;
}

static void*
heap_reallocate_block_aligned(heap_t* heap, void* block, size_t alignment, size_t size, size_t old_size,
                              unsigned int flags) {
	if (alignment <= SMALL_GRANULARITY)
		return heap_reallocate_block(heap, block, size, old_size, flags);

	int no_alloc = !!(flags & RPMALLOC_GROW_OR_FAIL);
	size_t usable_size = (block ? block_usable_size(block) : 0);
	if ((usable_size >= size) && !((uintptr_t)block & (alignment - 1))) {
		if (no_alloc || (size >= (usable_size / 2)))
			return block;
	}
	// Aligned alloc marks span as having aligned blocks
	void* old_block = block;
	block = (!no_alloc ? heap_allocate_block_aligned(heap, alignment, size, 0) : 0);
	if (EXPECTED(block != 0)) {
		if (!(flags & RPMALLOC_NO_PRESERVE) && old_block) {
			if (!old_size)
				old_size = usable_size;
			memcpy(block, old_block, old_size < size ? old_size : size);
		}
		if (EXPECTED(old_block != 0))
			block_deallocate(old_block);
	}
	return block;
}

static void
heap_free_all(heap_t* heap) {
	for (int itype = 0; itype < 4; ++itype) {
		span_t* span = heap->span_partial[itype];
		while (span) {
			span_t* span_next = span->next;
			global_memory_interface->memory_unmap(span, span->offset, span->mapped_size);
			span = span_next;
		}
		heap->span_partial[itype] = 0;
		heap->page_free[itype] = 0;
		heap->page_free_commit_count[itype] = 0;
		atomic_store_explicit(&heap->thread_free[itype], 0, memory_order_release);
	}
	for (int itype = 0; itype < 5; ++itype) {
		span_t* span = heap->span_used[itype];
		while (span) {
			span_t* span_next = span->next;
#if ENABLE_STATISTICS
			if (itype == PAGE_HUGE)
				statistics_sub_saturating(&global_statistics.huge_alloc,
				                          (size_t)span->page_count * span->page_size);
#endif
			global_memory_interface->memory_unmap(span, span->offset, span->mapped_size);
			span = span_next;
		}
		heap->span_used[itype] = 0;
	}
	memset(heap->local_free, 0, sizeof(heap->local_free));
	memset(heap->page_available, 0, sizeof(heap->page_available));

#if ENABLE_STATISTICS
	// Every block in this heap has been released, so reverse the per size class allocation counters
	for (uint32_t iclass = 0; iclass < SIZE_CLASS_COUNT; ++iclass) {
		heap->size_use[iclass].free_total += heap->size_use[iclass].alloc_current;
		heap->size_use[iclass].alloc_current = 0;
	}
#endif
}

////////////
///
/// Extern interface
///
//////

static int
rpmalloc_is_thread_initialized(void) {
	return (get_thread_heap() != global_heap_default) ? 1 : 0;
}

static inline RPMALLOC_ALLOCATOR void*
rpmalloc(size_t size) {
#if ENABLE_VALIDATE_ARGS
	if (size >= MAX_ALLOC_SIZE) {
		errno = EINVAL;
		return 0;
	}
#endif
	heap_t* heap = get_thread_heap();
	return heap_allocate_block(heap, size, 0);
}

static inline RPMALLOC_ALLOCATOR void*
rpzalloc(size_t size) {
#if ENABLE_VALIDATE_ARGS
	if (size >= MAX_ALLOC_SIZE) {
		errno = EINVAL;
		return 0;
	}
#endif
	heap_t* heap = get_thread_heap();
	return heap_allocate_block(heap, size, 1);
}

static inline void
rpfree(void* ptr) {
	if (UNEXPECTED(ptr == 0))
		return;
	block_deallocate(ptr);
}

static inline RPMALLOC_ALLOCATOR void*
rpcalloc(size_t num, size_t size) {
	size_t total;
#if ENABLE_VALIDATE_ARGS
#if PLATFORM_WINDOWS
	int err = SizeTMult(num, size, &total);
	if ((err != S_OK) || (total >= MAX_ALLOC_SIZE)) {
		errno = EINVAL;
		return 0;
	}
#else
	int err = __builtin_umull_overflow(num, size, &total);
	if (err || (total >= MAX_ALLOC_SIZE)) {
		errno = EINVAL;
		return 0;
	}
#endif
#else
	total = num * size;
#endif
	heap_t* heap = get_thread_heap();
	return heap_allocate_block(heap, total, 1);
}

static inline RPMALLOC_ALLOCATOR void*
rprealloc(void* ptr, size_t size) {
#if ENABLE_VALIDATE_ARGS
	if (size >= MAX_ALLOC_SIZE) {
		errno = EINVAL;
		return ptr;
	}
#endif
	heap_t* heap = get_thread_heap();
	return heap_reallocate_block(heap, ptr, size, 0, 0);
}

static RPMALLOC_ALLOCATOR void*
rpaligned_realloc(void* ptr, size_t alignment, size_t size, size_t oldsize, unsigned int flags) {
#if ENABLE_VALIDATE_ARGS
	if ((size + alignment < size) || (alignment > SMALL_PAGE_SIZE)) {
		errno = EINVAL;
		return 0;
	}
#endif
	heap_t* heap = get_thread_heap();
	return heap_reallocate_block_aligned(heap, ptr, alignment, size, oldsize, flags);
}

static RPMALLOC_ALLOCATOR void*
rpaligned_alloc(size_t alignment, size_t size) {
	heap_t* heap = get_thread_heap();
	return heap_allocate_block_aligned(heap, alignment, size, 0);
}

static RPMALLOC_ALLOCATOR void*
rpaligned_zalloc(size_t alignment, size_t size) {
	heap_t* heap = get_thread_heap();
	return heap_allocate_block_aligned(heap, alignment, size, 1);
}

static inline RPMALLOC_ALLOCATOR void*
rpaligned_calloc(size_t alignment, size_t num, size_t size) {
	size_t total;
#if ENABLE_VALIDATE_ARGS
#if PLATFORM_WINDOWS
	int err = SizeTMult(num, size, &total);
	if ((err != S_OK) || (total >= MAX_ALLOC_SIZE)) {
		errno = EINVAL;
		return 0;
	}
#else
	int err = __builtin_umull_overflow(num, size, &total);
	if (err || (total >= MAX_ALLOC_SIZE)) {
		errno = EINVAL;
		return 0;
	}
#endif
#else
	total = num * size;
#endif
	heap_t* heap = get_thread_heap();
	return heap_allocate_block_aligned(heap, alignment, total, 1);
}

static inline RPMALLOC_ALLOCATOR void*
rpmemalign(size_t alignment, size_t size) {
	heap_t* heap = get_thread_heap();
	return heap_allocate_block_aligned(heap, alignment, size, 0);
}

static inline int
rpposix_memalign(void** memptr, size_t alignment, size_t size) {
	heap_t* heap = get_thread_heap();
	if (memptr)
		*memptr = heap_allocate_block_aligned(heap, alignment, size, 0);
	else
		return EINVAL;
	return *memptr ? 0 : ENOMEM;
}

static inline size_t
rpmalloc_usable_size(void* ptr) {
	return (ptr ? block_usable_size(ptr) : 0);
}

static void
rpmalloc_linker_reference(void) {
}

////////////
///
/// Initialization and finalization
///
//////


#if defined(__linux__) || defined(__ANDROID__)
//! Read a small (pseudo) file such as /proc/meminfo into a caller provided stack buffer
//! using raw syscalls. This deliberately avoids stdio (fopen/fgets), which allocates an
//! internal buffer through malloc - during rpmalloc_initialize that allocation would
//! re-enter the allocator (when the malloc override is enabled) before the page size is
//! established. Returns the number of bytes read (buffer is always null terminated), or 0.
static size_t
os_read_system_file(const char* path, char* buffer, size_t capacity) {
	if (capacity < 2)
		return 0;
	int fd = open(path, O_RDONLY | O_CLOEXEC);
	if (fd < 0)
		return 0;
	size_t total = 0;
	while ((total + 1) < capacity) {
		ssize_t got = read(fd, buffer + total, (capacity - 1) - total);
		if (got <= 0)
			break;
		total += (size_t)got;
	}
	close(fd);
	buffer[total] = 0;
	return total;
}
#endif



static const rpmalloc_config_t*
rpmalloc_config(void) {
	return &global_config;
}

#if ENABLE_LEAK_DETECTION
//! Number of size class blocks still allocated across every heap. A non-zero value once all threads
//! have finalized means the application has not freed all its allocations (leak or double free).
static long long
statistics_outstanding_blocks(void) {
	heap_t* lists[3] = {global_heap_used, global_heap_queue, global_heap_pristine};
	long long outstanding = 0;
	for (int ilist = 0; ilist < 3; ++ilist) {
		for (heap_t* heap = lists[ilist]; heap; heap = heap->next) {
			for (uint32_t iclass = 0; iclass < SIZE_CLASS_COUNT; ++iclass)
				outstanding += heap->size_use[iclass].alloc_current;
		}
	}
	return outstanding;
}

//! Number of cross-thread freed blocks parked in a span's per page deferred lists, counting every
//! initialized page (a full page is not reachable from page_available but can still hold deferred
//! frees, since adoption is skipped while a page has a local free list). Page count is in the token
//! high bits.
static long long
span_deferred_block_count(span_t* span) {
	long long deferred = 0;
	for (uint32_t ipage = 0; ipage < span->page_initialized; ++ipage) {
		page_t* page = (page_t*)pointer_offset(span, (size_t)span->page_size * ipage);
		uint64_t token = atomic_load_explicit(&page->thread_free, memory_order_relaxed);
		deferred += (long long)(uint32_t)(token >> 32ULL);
	}
	return deferred;
}

//! Number of blocks freed cross-thread but not yet applied to the owning heap's alloc_current,
//! waiting in the deferred free lists. These are accounted in alloc_current until processed, so
//! subtracting them from the outstanding count yields the blocks the application has truly leaked.
static long long
statistics_pending_deferred_blocks(void) {
	heap_t* lists[3] = {global_heap_used, global_heap_queue, global_heap_pristine};
	long long deferred = 0;
	for (int ilist = 0; ilist < 3; ++ilist) {
		for (heap_t* heap = lists[ilist]; heap; heap = heap->next) {
			// Page types small through large carry the per heap full page deferred list and the per
			// page deferred lists of every span. Huge blocks are never deferred per block.
			for (uint32_t itype = 0; itype < 4; ++itype) {
				block_t* block = (block_t*)atomic_load_explicit(&heap->thread_free[itype], memory_order_relaxed);
				while (block) {
					++deferred;
					block = block->next;
				}
				if (heap->span_partial[itype])
					deferred += span_deferred_block_count(heap->span_partial[itype]);
				for (span_t* span = heap->span_used[itype]; span; span = span->next)
					deferred += span_deferred_block_count(span);
			}
		}
	}
	return deferred;
}
#endif

static void
rpmalloc_finalize(void) {
	rpmalloc_thread_finalize();

#if HUGE_CACHE_SLOT_COUNT
	// Release cached huge mappings, the cache must not hold memory across a finalize
	// (and potential re-initialize with a different page size)
	huge_cache_flush();
#endif

#if ENABLE_LEAK_DETECTION && ENABLE_ASSERTS
	// Leak detection: every thread has finalized, so blocks still allocated and not merely waiting
	// in a deferred cross-thread free list are blocks the application never freed (or freed twice).
	rpmalloc_assert(statistics_outstanding_blocks() <= statistics_pending_deferred_blocks(),
	                "Memory leak detected");
	rpmalloc_assert(atomic_load_explicit(&global_statistics.huge_alloc, memory_order_relaxed) == 0,
	                "Memory leak detected");
#endif


	if (global_config.unmap_on_finalize) {
		// Heaps can share a mapping (heap_allocate_new); only the owner carries mapped_size != 0.
		// Resources are freed while every heap is still mapped, then owners are unmapped in a
		// separate pass so a heap is never dereferenced after its shared mapping is gone.
		heap_t* owners = 0;
		heap_t* heap = global_heap_pristine;
		global_heap_pristine = 0;
		while (heap) {
			heap_t* heap_next = heap->next;
			heap_free_all(heap);
			if (heap->mapped_size) {
				heap->next = owners;
				owners = heap;
			}
			heap = heap_next;
		}
		heap = global_heap_queue;
		global_heap_queue = 0;
		while (heap) {
			heap_t* heap_next = heap->next;
			heap_free_all(heap);
			if (heap->mapped_size) {
				heap->next = owners;
				owners = heap;
			}
			heap = heap_next;
		}
		heap = global_heap_used;
		global_heap_used = 0;
		while (heap) {
			heap_t* heap_next = heap->next;
			heap_free_all(heap);
			if (heap->mapped_size) {
				heap->next = owners;
				owners = heap;
			}
			heap = heap_next;
		}
		while (owners) {
			heap_t* owner_next = owners->next;
			heap_unmap(owners);
			owners = owner_next;
		}
#if ENABLE_STATISTICS
		memset(&global_statistics, 0, sizeof(global_statistics));
#endif
	}

#ifdef _WIN32
	FlsFree(fls_key);
	fls_key = 0;
#else
	pthread_key_delete(pthread_key);
	pthread_key = 0;
#endif

	global_heap_creating = 0;
	global_main_thread_id = 0;
	atomic_store_explicit(&global_rpmalloc_init_state, RPMALLOC_INIT_UNINIT, memory_order_release);
}



static void
rpmalloc_thread_collect(void) {
}

static void
rpmalloc_thread_statistics(rpmalloc_thread_statistics_t* stats) {
	memset(stats, 0, sizeof(rpmalloc_thread_statistics_t));
#if ENABLE_STATISTICS
	heap_t* heap = get_thread_heap();
	if (!heap || (heap->id == 0))
		return;

	// Size class cache: blocks available without mapping a new page. A page's block_used counts the
	// blocks held on the heap free list, so those are counted from the heap list and each page adds
	// its remaining blocks (page free list plus the not yet initialized tail).
	for (uint32_t iclass = 0; iclass < SIZE_CLASS_COUNT; ++iclass) {
		size_t block_size = global_size_class[iclass].block_size;
		size_t free_count = 0;
		for (block_t* block = heap->local_free[iclass]; block; block = block->next)
			++free_count;
		for (page_t* page = heap->page_available[iclass]; page; page = page->next)
			free_count += (size_t)(page->block_count - page->block_used);
		stats->sizecache += free_count * block_size;

		stats->size_use[iclass].alloc_current = (size_t)heap->size_use[iclass].alloc_current;
		stats->size_use[iclass].alloc_peak = (size_t)heap->size_use[iclass].alloc_peak;
		stats->size_use[iclass].alloc_total = (size_t)heap->size_use[iclass].alloc_total;
		stats->size_use[iclass].free_total = (size_t)heap->size_use[iclass].free_total;
	}

	// Span cache: free but still committed pages retained per page type
	static const size_t page_type_size[4] = {SMALL_PAGE_SIZE, MEDIUM_SMALL_PAGE_SIZE, MEDIUM_LARGE_PAGE_SIZE,
	                                          LARGE_PAGE_SIZE};
	for (uint32_t itype = 0; itype < 4; ++itype)
		stats->spancache += (size_t)heap->page_free_commit_count[itype] * page_type_size[itype];

	// Per page type span use: current spans in use computed live (partial plus full), map calls counted
	for (uint32_t itype = 0; itype < 5; ++itype) {
		size_t current = 0;
		if ((itype < 4) && heap->span_partial[itype])
			++current;
		for (span_t* span = heap->span_used[itype]; span; span = span->next)
			++current;
		stats->span_use[itype].current = current;
		stats->span_use[itype].map_calls = (size_t)heap->span_map_calls[itype];
	}
#endif
}

static void
rpmalloc_dump_statistics(void* file) {
#if ENABLE_STATISTICS
	fprintf(file, "Mapped pages:        %llu\n",
	        (unsigned long long)atomic_load_explicit(&global_statistics.page_mapped, memory_order_relaxed));
	fprintf(file, "Mapped pages (peak): %llu\n",
	        (unsigned long long)atomic_load_explicit(&global_statistics.page_mapped_peak, memory_order_relaxed));
	fprintf(file, "Active pages:        %llu\n",
	        (unsigned long long)atomic_load_explicit(&global_statistics.page_active, memory_order_relaxed));
	fprintf(file, "Active pages (peak): %llu\n",
	        (unsigned long long)atomic_load_explicit(&global_statistics.page_active_peak, memory_order_relaxed));
	fprintf(file, "Pages committed:     %llu\n",
	        (unsigned long long)atomic_load_explicit(&global_statistics.page_commit, memory_order_relaxed));
	fprintf(file, "Pages decommitted:   %llu\n",
	        (unsigned long long)atomic_load_explicit(&global_statistics.page_decommit, memory_order_relaxed));
	fprintf(file, "Huge bytes:          %llu\n",
	        (unsigned long long)atomic_load_explicit(&global_statistics.huge_alloc, memory_order_relaxed));
	fprintf(file, "Huge bytes (peak):   %llu\n",
	        (unsigned long long)atomic_load_explicit(&global_statistics.huge_alloc_peak, memory_order_relaxed));
	fprintf(file, "Heaps in use:        %llu\n",
	        (unsigned long long)atomic_load_explicit(&global_statistics.heap_count, memory_order_relaxed));
#if ENABLE_LEAK_DETECTION
	fprintf(file, "Outstanding blocks:  %lld\n",
	        statistics_outstanding_blocks() - statistics_pending_deferred_blocks());
#endif

	// Aggregate per size class and per page type counters across all in-use heaps. This walks live
	// heaps without stopping the world, so the values are a best-effort snapshot.
	heap_size_use_t size_use[SIZE_CLASS_COUNT];
	memset(size_use, 0, sizeof(size_use));
	uint64_t span_map_calls[5];
	memset(span_map_calls, 0, sizeof(span_map_calls));
	heap_lock_acquire();
	for (heap_t* heap = global_heap_used; heap; heap = heap->next) {
		for (uint32_t iclass = 0; iclass < SIZE_CLASS_COUNT; ++iclass) {
			size_use[iclass].alloc_current += heap->size_use[iclass].alloc_current;
			size_use[iclass].alloc_peak += heap->size_use[iclass].alloc_peak;
			size_use[iclass].alloc_total += heap->size_use[iclass].alloc_total;
			size_use[iclass].free_total += heap->size_use[iclass].free_total;
		}
		for (uint32_t itype = 0; itype < 5; ++itype)
			span_map_calls[itype] += heap->span_map_calls[itype];
	}
	heap_lock_release();

	fprintf(file, "SizeClass  CurAlloc PeakAlloc  TotAlloc   TotFree BlockSize\n");
	for (uint32_t iclass = 0; iclass < SIZE_CLASS_COUNT; ++iclass) {
		if (!size_use[iclass].alloc_total)
			continue;
		fprintf(file, "%9u %9d %9d %9d %9d %9u\n", iclass, size_use[iclass].alloc_current,
		        size_use[iclass].alloc_peak, size_use[iclass].alloc_total, size_use[iclass].free_total,
		        global_size_class[iclass].block_size);
	}
	fprintf(file, "PageType  MapCalls\n");
	for (uint32_t itype = 0; itype < 5; ++itype) {
		if (!span_map_calls[itype])
			continue;
		fprintf(file, "%8u %9llu\n", itype, (unsigned long long)span_map_calls[itype]);
	}
#else
	(void)sizeof(file);
#endif
}

static void
rpmalloc_global_statistics(rpmalloc_global_statistics_t* stats) {
#if ENABLE_STATISTICS
    stats->mapped = global_config.page_size * atomic_load_explicit(&global_statistics.page_mapped, memory_order_relaxed);
    stats->mapped_peak = global_config.page_size * atomic_load_explicit(&global_statistics.page_mapped_peak, memory_order_relaxed);
    stats->committed = global_config.page_size * atomic_load_explicit(&global_statistics.page_commit, memory_order_relaxed);
    stats->decommitted = global_config.page_size * atomic_load_explicit(&global_statistics.page_decommit, memory_order_relaxed);
    stats->active = global_config.page_size * atomic_load_explicit(&global_statistics.page_active, memory_order_relaxed);
    stats->active_peak = global_config.page_size * atomic_load_explicit(&global_statistics.page_active_peak, memory_order_relaxed);
    stats->huge_alloc = atomic_load_explicit(&global_statistics.huge_alloc, memory_order_relaxed);
    stats->huge_alloc_peak = atomic_load_explicit(&global_statistics.huge_alloc_peak, memory_order_relaxed);
    stats->heap_count = atomic_load_explicit(&global_statistics.heap_count, memory_order_relaxed);
#else
    memset(stats, 0, sizeof(rpmalloc_global_statistics_t));
#endif
}

#if RPMALLOC_FIRST_CLASS_HEAPS

static rpmalloc_heap_t*
rpmalloc_heap_acquire(void) {
	// Must be a pristine heap, or else memory blocks could already be allocated from the heap
	// which would (wrongly) be released when heap is cleared with rpmalloc_heap_free_all().
	// heap_allocate(1) returns either a never-used heap carved from a shared block (whether that
	// block was mapped for an ordinary heap or a prior first-class heap) or a freshly mapped one.
	heap_t* heap = heap_allocate(1);
	rpmalloc_assume(heap != 0);
	heap->owner_thread = 0;
	return heap;
}

static void
rpmalloc_heap_release(rpmalloc_heap_t* heap) {
	if (heap)
		heap_release(heap);
}

static RPMALLOC_ALLOCATOR void*
rpmalloc_heap_alloc(rpmalloc_heap_t* heap, size_t size) {
#if ENABLE_VALIDATE_ARGS
	if (size >= MAX_ALLOC_SIZE) {
		errno = EINVAL;
		return 0;
	}
#endif
	return heap_allocate_block(heap, size, 0);
}

static RPMALLOC_ALLOCATOR void*
rpmalloc_heap_aligned_alloc(rpmalloc_heap_t* heap, size_t alignment, size_t size) {
#if ENABLE_VALIDATE_ARGS
	if (size >= MAX_ALLOC_SIZE) {
		errno = EINVAL;
		return 0;
	}
#endif
	return heap_allocate_block_aligned(heap, alignment, size, 0);
}

static RPMALLOC_ALLOCATOR void*
rpmalloc_heap_aligned_zalloc(rpmalloc_heap_t* heap, size_t alignment, size_t size) {
#if ENABLE_VALIDATE_ARGS
	if (size >= MAX_ALLOC_SIZE) {
		errno = EINVAL;
		return 0;
	}
#endif
	return heap_allocate_block_aligned(heap, alignment, size, 1);
}

static RPMALLOC_ALLOCATOR void*
rpmalloc_heap_calloc(rpmalloc_heap_t* heap, size_t num, size_t size) {
	size_t total;
#if ENABLE_VALIDATE_ARGS
#if PLATFORM_WINDOWS
	int err = SizeTMult(num, size, &total);
	if ((err != S_OK) || (total >= MAX_ALLOC_SIZE)) {
		errno = EINVAL;
		return 0;
	}
#else
	int err = __builtin_umull_overflow(num, size, &total);
	if (err || (total >= MAX_ALLOC_SIZE)) {
		errno = EINVAL;
		return 0;
	}
#endif
#else
	total = num * size;
#endif
	return heap_allocate_block(heap, total, 1);
}

static inline RPMALLOC_ALLOCATOR void*
rpmalloc_heap_aligned_calloc(rpmalloc_heap_t* heap, size_t alignment, size_t num, size_t size) {
	size_t total;
#if ENABLE_VALIDATE_ARGS
#if PLATFORM_WINDOWS
	int err = SizeTMult(num, size, &total);
	if ((err != S_OK) || (total >= MAX_ALLOC_SIZE)) {
		errno = EINVAL;
		return 0;
	}
#else
	int err = __builtin_umull_overflow(num, size, &total);
	if (err || (total >= MAX_ALLOC_SIZE)) {
		errno = EINVAL;
		return 0;
	}
#endif
#else
	total = num * size;
#endif
	return heap_allocate_block_aligned(heap, alignment, total, 1);
}

static RPMALLOC_ALLOCATOR void*
rpmalloc_heap_realloc(rpmalloc_heap_t* heap, void* ptr, size_t size, unsigned int flags) {
#if ENABLE_VALIDATE_ARGS
	if (size >= MAX_ALLOC_SIZE) {
		errno = EINVAL;
		return ptr;
	}
#endif
	return heap_reallocate_block(heap, ptr, size, 0, flags);
}

static RPMALLOC_ALLOCATOR void*
rpmalloc_heap_aligned_realloc(rpmalloc_heap_t* heap, void* ptr, size_t alignment, size_t size, unsigned int flags) {
#if ENABLE_VALIDATE_ARGS
	if ((size + alignment < size) || (alignment > SMALL_PAGE_SIZE)) {
		errno = EINVAL;
		return 0;
	}
#endif
	return heap_reallocate_block_aligned(heap, ptr, alignment, size, 0, flags);
}

static void
rpmalloc_heap_free(rpmalloc_heap_t* heap, void* ptr) {
	(void)sizeof(heap);
	block_deallocate(ptr);
}

//! Free all memory allocated by the heap
static void
rpmalloc_heap_free_all(rpmalloc_heap_t* heap) {
	heap_free_all(heap);
}

static struct rpmalloc_heap_statistics_t
rpmalloc_heap_statistics(rpmalloc_heap_t* heap) {
#if RPMALLOC_HEAP_STATISTICS
	if (heap) {
		return heap->stats;
	}
#endif
	(void)sizeof(heap);
	struct rpmalloc_heap_statistics_t stats = {0};
	return stats;
}

static inline void
rpmalloc_heap_thread_set_current(rpmalloc_heap_t* heap) {
	heap_t* prev_heap = get_thread_heap();
	if (prev_heap != heap) {
		set_thread_heap(heap);
		if (prev_heap)
			heap_release(prev_heap);
	}
}

static rpmalloc_heap_t*
rpmalloc_get_heap_for_ptr(void* ptr) {
	span_t* span = (span_t*)((uintptr_t)ptr & SPAN_MASK);
	if (span)
		return span_get_page_from_block(span, ptr)->heap;
	return 0;
}

#endif


#ifdef __cplusplus
}
#endif
// rpmalloc end

#if defined(BUILD_DEBUG) 
#   include <stdlib.h>   // malloc, calloc, realloc, free
#endif

// NOTE: 
// we still use regular malloc, free etc in debug mode
// to make ASAN and potentially other memory debugging tools still work correctly

static void* mem_alloc(size_t sz)
{
#if defined(BUILD_DEBUG) 
    return malloc(sz);
#else
    return rpmalloc(sz);
#endif
}

static void* mem_calloc(size_t num, size_t sz)
{
#if defined(BUILD_DEBUG) 
    return calloc(num, sz);
#else
    return rpcalloc(num, sz);
#endif
}

static void* mem_realloc(void* ptr, size_t num)
{
#if defined(BUILD_DEBUG) 
    return realloc(ptr, num);
#else
    return rprealloc(ptr, num);
#endif
}

static void mem_free(void* ptr)
{
#if defined(BUILD_DEBUG) 
    return free(ptr);
#else
    return rpfree(ptr);
#endif
}

#endif // defined(SPFLIB_MEMORY)

// --- allocators --- //

#if defined(SPFLIB_ALLOCATORS)

#if !defined(SPFLIB_MEMORY)
#   include <stdlib.h>    // malloc, free
#endif

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
#if defined(SPFLIB_MEMORY)
        .data      = malloc(capacity),
#else
        .data      = mem_alloc(capacity),
#endif
        .capacity  = capacity,
        .pos_bytes = 0
    };
}

static void* bump_alloc(BumpAllocator* ac, size_t size)
{
    if (ac->data == nullptr) return nullptr;
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

#if defined(SPFLIB_MEMORY)
    mem_free(ac->data);
#else
    free(ac->data);
#endif
    ac->data      = nullptr;
}

#endif // defined(SPFLIB_ALLOCATORS)

// --- color --- //

#if defined(SPFLIB_COLOR)

typedef struct
{
   u8 r, g, b, a; 
} Color4;

#define COLOR4_TO_GL(_c) _c.r / 255.f, _c.g / 255.f, _c.b / 255.f, _c.a / 255.f

#define __Color4_Make(...) (Color4){ __VA_ARGS__ }
#define __Color4_Args(_1, _2, _3, _4, x, ...) x
#define Color4(...) __Color4_Args(__VA_ARGS__, \
                            __Color4_Make(__VA_ARGS__), \
                            __Color4_Make(__VA_ARGS__, 255), \
                            __Color4_Make(__VA_ARGS__, 255, 255), \
                            __Color4_Make(__VA_ARGS__, __VA_ARGS__, __VA_ARGS__, 255))

static Color4 color_from_hex(u32 hex)
{
    Color4 c = {0};

    c.r = (hex >> 24) & 0xFF;
    c.g = (hex >> 16) & 0xFF;
    c.b = (hex >> 8)  & 0xFF;
    c.a = (hex >> 0)  & 0xFF;

    return c;
}

static u32 color_to_hex(Color4 c)
{
    return (u32)((u32)c.r << 24) | ((u32)c.g << 16) | ((u32)c.b << 8) | ((u32)c.a << 0);
}

#endif // defined(SPFLIB_COLOR)

// --- RGFW (windowing system) --- //

#if defined(SPFLIB_FRAMEWORK)

// RGFW start
// TODO:
// RGFW end

#endif // defined(SPFLIB_FRAMEWORK)

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

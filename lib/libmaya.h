/* libmaya (started on 29-Jul-26)
 *
 * General-purpose library for many use-cases.
 *
 * The usage case differs depending on if you are using this library as a
 * single-header library or as a shared-object file. To use it as single-header:
 *
 *     #define MAYA_IMPLEMENTATION
 *     #define MAYA_MODULE_(module or "all")
 *     #include "libmaya.h"
 *
 * To use it as a shared object:
 *
 *     #include "libmaya.h"
 *
 * If you are using it as shared-object file, you do not want to define
 * MAYA_IMPLEMENTATION. It duplicates the code.
 *
 */

#ifndef MAYA_H
#define MAYA_H

// Library Version
// I will certainly forget to update this at some point.
#define MAYA_VERSION "0.2.0"

/*----------------------------------------------------------------------------*/

// Get C Version

#define MAYA_C99 199901L
#define MAYA_C11 201112L
#define MAYA_C17 201710L
#define MAYA_C23 202311L

// Require C99+
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < MAYA_C99
    #error "LIBMAYA: Requires C99 or newer."
#endif

#define MAYA_C_VERSION __STDC_VERSION__

/*----------------------------------------------------------------------------*/

// Get Platform Information

// Sources:
// https://stackoverflow.com/questions/4605842/how-to-identify-platform-compiler-from-preprocessor-macros

// The order matters here, since Linux systems tend to also expose unix-globals,
// Linux needs to be checked first. This applies for both FreeBSD and OpenBSD
// as well.

#if defined(__linux__)
    #define MAYA_OS "LINUX"
    #define MAYA_OS_LINUX 1

#elif defined(__FreeBSD__)
    #define MAYA_OS "FREEBSD"
    #define MAYA_OS_FREEBSD 1

#elif defined(__OpenBSD__)
    #define MAYA_OS "OPENBSD"
    #define MAYA_OS_OPENBSD 1

#elif defined(__APPLE__) || defined(__MACH__)
    #define MAYA_OS "MACOS"
    #define MAYA_OS_MACOS 1

#elif defined(unix) || defined(__unix__) || defined(__unix)
    #define MAYA_OS "UNIX"
    #define MAYA_OS_UNIX 1

#elif defined(_WIN64)
    #define MAYA_OS "WIN64"
    #define MAYA_OS_WIN 1
    #define MAYA_OS_WIN64 1

#elif defined(_WIN32) // _WIN32 is also for _WIN64 so it's after it.
    #define MAYA_OS "WIN32"
    #define MAYA_OS_WIN 1
    #define MAYA_OS_WIN32 1

#else
    #define MAYA_OS "UNKNOWN"
    #define MAYA_OS_UNKNOWN 1
#endif

/*----------------------------------------------------------------------------*/

// Compiler Agnostic Macros

#if defined(__clang__)
    #define MAYA_COMPILER "CLANG"
    #define MAYA_COMPILER_CLANG 1

#elif defined(__GNUC__)
    #define MAYA_COMPILER "GCC"
    #define MAYA_COMPILER_GCC 1

#elif defined(_MSC_VER)
    #define MAYA_COMPILER "MSVC"
    #define MAYA_COMPILER_MSVC 1

#else
    #define MAYA_COMPILER "UNKNOWN"
    #define MAYA_COMPILER_UNKNOWN 1
#endif

/*----------------------------------------------------------------------------*/

/**
 * MAYA_IMPLEMENTATION
 *
 * This is different than stb_* styled "IMPLEMENTATION". However it will not
 * change because I believe my thought process is still valid.
 *
 * Each MODULE is a part of the "IMPLEMENTATION" of this library. I use defines
 * in to replace functions to be simpler. I could very well just use functions
 * but I deem defines to be simpler.
 *
 * As stated before, this should NOT be defined when linking against the shared
 * library. The shared library is compiled with all the modules.
 */
#ifdef MAYA_IMPLEMENTATION

/*----------------------------------------------------------------------------*/

#if defined(MAYA_MODULE_LOGS) || defined(MAYA_MODULES_ALL)

#include <stdio.h>

// Colors the text if ANSI module is also defined.
#ifdef MAYA_MODULE_ANSI

#define TODO(msg) do {\
    printf("[%sTODO%s] (%s - %s:%d) %s\n",\
    COLOR_BRIGHT_CYAN, COLOR_RESET, __FILE__, __func__, __LINE__, (msg));\
} while(0)

#define FTODO(msg) do {\
    fprintf(stderr, "[%sTODO%s] (%s - %s:%d) %s\n",\
    COLOR_BRIGHT_CYAN, COLOR_RESET, __FILE__, __func__, __LINE__, (msg));\
} while(0)

#define INFO(msg) do {\
    printf("[%sINFO%s] (%s - %s:%d) %s\n",\
    COLOR_BRIGHT_BLUE, COLOR_RESET, __FILE__, __func__, __LINE__, (msg));\
} while(0)

#define WARN(msg) do {\
    printf("[%sWARN%s] (%s - %s:%d) %s\n",\
    COLOR_BRIGHT_YELLOW, COLOR_RESET, __FILE__, __func__, __LINE__, (msg));\
} while(0)

#define ERROR(msg) do {\
    fprintf(stderr, "[%sERR %s] (%s - %s:%d) %s\n",\
    COLOR_BRIGHT_RED, COLOR_RESET, __FILE__, __func__, __LINE__, (msg));\
    abort();\
} while(0)

#define ERRORC(msg, code) do {\
    fprintf(stderr, "[%sERRC%s] (%s - %s:%d) %d: %s\n",\
    COLOR_BRIGHT_RED, COLOR_RESET, __FILE__, __func__, __LINE__, (code),\
    (msg)); abort();\
} while(0)

#ifdef MAYA_DEBUG
#define DEBUG(msg) do {\
    printf("[%sDEBU%s] (%s - %s:%d) %s\n",\
    COLOR_BRIGHT_MAGENTA, COLOR_RESET, __FILE__, __func__, __LINE__, (msg));\
} while(0)
#endif

#else

#define TODO(msg) do {\
    printf("[TODO] (%s - %s:%d) %s\n",\
    __FILE__, __func__, __LINE__, (msg));\
} while(0)

#define FTODO(msg) do {\
    fprintf(stderr, "[TODO] (%s - %s:%d) %s\n", \
    __FILE__, __func__, __LINE__, (msg));\
} while(0)

#define INFO(msg) do {\
    printf("[INFO] (%s - %s:%d) %s\n",\
    __FILE__, __func__, __LINE__, (msg));\
} while(0)

#define WARN(msg) do {\
    printf("[WARN] (%s - %s:%d) %s\n",\
    __FILE__, __func__, __LINE__, (msg));\
} while(0)

#define ERROR(msg) do {\
    fprintf(stderr, "[ERR ] (%s - %s:%d) %s\n",\
    __FILE__, __func__, __LINE__, (msg));\
} while(0)

#define ERRORC(msg, code) do {\
    fprintf(stderr, "[ERRC] (%s - %s:%d) %d: %s\n",\
    __FILE__, __func__, __LINE__, (code), (msg));\
} while(0)

#ifdef MAYA_DEBUG
    #define DEBUG(msg) do {\
        printf("[DEBU] (%s - %s:%d) %s\n",\
        __FILE__, __func__, __LINE__, (msg));\
    } while(0)
#endif

#endif
#endif // MAYA_MODULE_LOGS

/*----------------------------------------------------------------------------*/

#if defined(MAYA_MODULE_ANSI) || defined(MAYA_MODULES_ALL)

// Sources:
// https://en.wikipedia.org/wiki/ANSI_escape_code

// These do not work for every termianl. Find if they are compatible for
// your terminal/use-case.
//
// Terminals that typically support all of these are: Kitty, iTerm2, mintty,
// Windows Terminal, and Konsole. Anecdotally I use Wezterm on Windows and it
// works great too. The wikipedia page has some description of supported
// terminals for each escape code.

// ANSI Color Escape Codes
#define COLOR_RESET "\033[39;49m"
#define COLOR_RED "\033[31m"
#define COLOR_BRIGHT_RED "\033[91m"
#define COLOR_GREEN "\033[32m"
#define COLOR_BRIGHT_GREEN "\033[92m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BRIGHT_YELLOW "\033[93m"
#define COLOR_BLUE "\033[34m"
#define COLOR_BRIGHT_BLUE "\033[94m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_BRIGHT_MAGENTA "\033[95m"
#define COLOR_CYAN "\033[36m"
#define COLOR_BRIGHT_CYAN "\033[96m"
#define COLOR_WHITE "\033[37m"
#define COLOR_BRIGHT_WHITE "\033[97m"
#define COLOR_BLACK "\033[30m"
#define COLOR_GREY "\033[90m"

// Text Styling
// TODO: Add the reverse cases for each. (Not blinking, Not underlined, etc..)
#define ANSI_RESET "\033[0m"
#define ANSI_BOLD "\033[1m"
#define ANSI_DIM "\033[2m"
#define ANSI_ITALIC "\033[3m"
#define ANSI_UNDERLINE "\033[4m"
#define ANSI_INVERT "\033[7m"

// Terminal Control Sequences

/**
 * Clears a line in the terminal.
 *
 * @param n    0 - clear cursor to end
 *             1 - clear cursor to beginning
 *             2 - clear entire line
 */
#define ANSI_LINE_CLEAR(n) printf("\033[%dK", (n))

/**
 * Moves cursor N line(s) up.
 *
 * @param n    number of lines to move up
 */
#define ANSI_LINE_UP(n) printf("\033[%dF", (n))

/**
 * Moves cursor N line(s) down.
 *
 * @param n    number of lines to move down
 */
#define ANSI_LINE_DOWN(n) printf("\033[%dE", (n))

/**
 * Places the cursor on column N.
 *
 * @param n    column number
 */
#define ANSI_CURSOR_COLUMN(n) printf("\033[%dG", (n))

/**
 * Moves the cursor to position _r_, _c_.
 *
 * @param r    row
 * @param c    column
 */
#define ANSI_CURSOR_POS(r, c) printf("\033[%d;%dH", (r), (c))

/**
 * Hides the cursor.
 */
#define ANSI_CURSOR_HIDE() printf("\033[?25l")

/**
 * Shows the cursor.
 */
#define ANSI_CURSOR_SHOW() printf("\033[?25h")

/**
 * Saves current cursor position.
 * Used with ANSI_CURSOR_RESTORE() to restore position.
 */
#define ANSI_CURSOR_SAVE() printf("\033[s")

/**
 * Restores the current cursor position.
 * Used with ANSI_CURSOR_SAVE() to save the position.
 */
#define ANSI_CURSOR_RESTORE() printf("\033[u")

/**
 * Scrolls the terminal N lines up.
 *
 * @param n    number of lines to scroll
 */
#define ANSI_SCROLL_UP(n) printf("\033[%dS", (n))

/**
 * Scrolls the terminal N lines down.
 *
 * @param n    number of lines to scroll
 */
#define ANSI_SCROLL_DOWN(n) printf("\033[%dT", (n))

#endif // MAYA_MODULES_ANSI

/*----------------------------------------------------------------------------*/

#if defined(MAYA_MODULE_TYPES) || defined(MAYA_MODULES_ALL)

#include <stdint.h>
#include <stddef.h>

typedef float     f32;
typedef double    f64;

typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;

typedef int8_t    i8;
typedef int16_t   i16;
typedef int32_t   i32;
typedef int64_t   i64;

typedef uintmax_t umax;
typedef intmax_t  imax;

#endif // MAYA_MODULE_TYPES

/*----------------------------------------------------------------------------*/

#if defined(MAYA_MODULE_BITS) || defined(MAYA_MODULES_ALL)

// TODO/NOTE: This probably should be rewritten to work with different sized
//            types. I.e BIT_SET should work against any sized `reg`. This most
//            likely means it will be moved over to functions instead of
//            definitions. Save it for another release.

#include <stdint.h>

/**
 * Sets the bit at index `bit` in `reg`.
 *
 * @param reg    register to alter
 * @param bit    index of the bit to set (set to 1)
 */
#define BIT_SET(reg, bit)    ((reg) |= (1U << (bit)))

/**
 * Clears the bit at index `bit` in `reg`.
 *
 * @param reg    register to alter
 * @param bit    index of the bit to clear (set to 0)
 */
#define BIT_CLEAR(reg, bit)  ((reg) &= ~(1U << (bit)))

/**
 * Toggles the bit at index `bit` in `reg`.
 *
 * @param reg    register to alter
 * @param bit    index of the bit to toggle
 */
#define BIT_TOGGLE(reg, bit) ((reg) ^= (1U << (bit)))

/**
 * Checks the value at index `bit` of `reg`.
 *
 * @param reg    register to check
 * @param bit    index of the bit to check
 *
 * @return `0` if the bit is clear, otherwise `1`.
 */
#define BIT_CHECK(reg, bit)  (((reg) >> (bit)) & 1U)

#endif // MAYA_MODULE_BITS

/*----------------------------------------------------------------------------*/

#endif // MAYA_IMPLEMENTATION
#endif // MAYA_H
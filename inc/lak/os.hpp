#ifndef LAK_OS_HPP
#define LAK_OS_HPP

// https://sourceforge.net/p/predef/wiki/OperatingSystems/
// https://web.archive.org/web/20200422000946/https://sourceforge.net/p/predef/wiki/OperatingSystems/

#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) ||               \
  defined(__WIN64__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
#  define LAK_OS_WINDOWS
#  if !defined(LAK_USE_XLIB) && !defined(LAK_USE_XCB)
#    define LAK_USE_WINAPI
#  endif
#endif

#if defined(__APPLE__) || defined(__MACH__)
#  define LAK_OS_APPLE
#endif

#if defined(__ANDROID__)
#  define LAK_OS_ANDROID
#endif

#if defined(__gnu_linux__)
#  define LAK_OS_GNU_LINUX
#endif

#if defined(__linux__)
// *Anything* running the Linux kernel.
#  define LAK_OS_LINUX
#  if !defined(LAK_USE_XLIB) && !defined(LAK_USE_XCB) &&                      \
    !defined(LAK_USE_WINAPI)
#    define LAK_USE_XLIB
#  endif
#endif

#endif
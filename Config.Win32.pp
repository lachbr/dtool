//
// Config.Win32.pp
//
// This file defines some custom config variables for the Windows
// platform, using MS VC++.  It makes some initial guesses about
// compiler features, etc.
//

// What additional flags should we pass to interrogate?
#define SYSTEM_IGATE_FLAGS -longlong __int64 -D_X86_ -DWIN32_VC -D"_declspec(param)=" -D_near  -D_far -D__near  -D__far

// Is the platform big-endian (like an SGI workstation) or
// little-endian (like a PC)?  Define this to the empty string to
// indicate little-endian, or nonempty to indicate big-endian.
#define WORDS_BIGENDIAN

// Does the C++ compiler support namespaces?
#define HAVE_NAMESPACE 1

// Does the C++ compiler support ios::binary?
#define HAVE_IOS_BINARY 1

// Will the compiler avoid inserting extra bytes in structs between a
// base struct and its derived structs?  It is safe to define this
// true if you don't know, but if you know that you can get away with
// this you may gain a tiny performance gain by defining this false.
// If you define this false incorrectly, you will get lots of
// assertion failures on execution.
#define SIMPLE_STRUCT_POINTERS 1

// Do we have a gettimeofday() function?
#define HAVE_GETTIMEOFDAY

// Does gettimeofday() take only one parameter?
#define GETTIMEOFDAY_ONE_PARAM

// Do we have getopt() and/or getopt_long_only() built into the
// system?
#define HAVE_GETOPT
#define HAVE_GETOPT_LONG_ONLY

// Can we determine the terminal width by making an ioctl(TIOCGWINSZ) call?
#define IOCTL_TERMINAL_WIDTH

// Do the compiler or the system headers define a "streamsize" typedef?
#define HAVE_STREAMSIZE 1

// Can we safely call getenv() at static init time?
#define STATIC_INIT_GETENV 1

// Can we read the file /proc/self/environ to determine our
// environment variables at static init time?
#define HAVE_PROC_SELF_ENVIRON

// Do we have a global pair of argc/argv variables that we can read at
// static init time?  Should we prototype them?  What are they called?
#define HAVE_GLOBAL_ARGV 1
#define PROTOTYPE_GLOBAL_ARGV
#define GLOBAL_ARGV __argv
#define GLOBAL_ARGC __argc

// Can we read the file /proc/self/cmdline to determine our
// command-line arguments at static init time?
#define HAVE_PROC_SELF_CMDLINE

// Should we include <iostream> or <iostream.h>?  Define HAVE_IOSTREAM
// to nonempty if we should use <iostream>, or empty if we should use
// <iostream.h>.
#define HAVE_IOSTREAM 1

// Do we have a true stringstream class defined in <sstream>?
#define HAVE_SSTREAM 1

// Do we have <new>?
#define HAVE_NEW 1

// Do we have <io.h>?
#define HAVE_IO_H 1

// Do we have <malloc.h>?
#define HAVE_MALLOC_H 1

// Do we have <alloca.h>?
#define HAVE_ALLOCA_H

// Do we have <minmax.h>?
#define HAVE_MINMAX_H 1

// Do we have <sys/types.h>?
#define HAVE_SYS_TYPES_H 1

// Do we have <unistd.h>?
#define HAVE_UNISTD_H

// Do we have <utime.h>?
#define HAVE_UTIME_H

// Do we have <dirent.h>?
#define HAVE_DIRENT_H

// Do we have <sys/soundcard.h> (and presumably a Linux-style audio
// interface)?
#define HAVE_SYS_SOUNDCARD_H

// Do we have RTTI (and <typeinfo>)?
#define HAVE_RTTI 1

// Must global operator new and delete functions throw exceptions?
#define GLOBAL_OPERATOR_NEW_EXCEPTIONS

// What is the syntax of the STL allocator declaration?  See
// LocalSetup.pp for allowable values.
//#define STL_ALLOCATOR VC6
#define STL_ALLOCATOR MODERN

// can Intel C++ build this directory successfully (if not, change CC to msvc)
#define NOT_INTEL_BUILDABLE false

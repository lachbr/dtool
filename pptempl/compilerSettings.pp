
#if $[eq $[USE_COMPILER], MSVC]
  #define COMPILER cl
  #define LINKER link
  #define LIBBER lib
  #define COMMONFLAGS /Gi-
  #define OPTFLAGS /O2 /Ob1 /G6
  #define OPT1FLAGS /GZ 
  #defer DEBUGFLAGS /MDd /Zi $[BROWSEINFO_FLAG] /Fd"$[osfilename $[target:%.obj=%.pdb]]"
  #define RELEASEFLAGS /MD
  
// in case we have mixed intel/msvc build
  #define EXTRA_LIBPATH /ia32/lib
  #define EXTRA_INCPATH /ia32/include    
#elif $[eq $[USE_COMPILER], BOUNDS]
  #define COMPILER nmcl
  #define LINKER nmlink
  #define LIBBER lib
  #define COMMONFLAGS
  #define OPTFLAGS /O2 /Ogity /G6
  #define OPT1FLAGS /GZ   
  #defer DEBUGFLAGS /MDd /Zi $[BROWSEINFO_FLAG] /Fd"$[osfilename $[target:%.obj=%.pdb]]"
  #define RELEASEFLAGS /MD
  #define EXTRA_LIBPATH
  #define EXTRA_INCPATH
#elif $[eq $[USE_COMPILER], INTEL]
  #define COMPILER icl
  #define LINKER xilink
  #define LIBBER xilib
  #define COMMONFLAGS /Gi- /Qwd985
//  #define OPTFLAGS /O3 /G6 /Qvc6 /Qipo /QaxW /Qvec_report1 
  #define OPTFLAGS /O3 /G6 /Qvc6 /Qip
  // Oy- needed for MS debugger
  #define DEBUGFLAGS /MDd /Zi /Qinline_debug_info /Oy-
  #define OPT1FLAGS /GZ /Od
  #define RELEASEFLAGS /MD
  // We assume the Intel compiler installation dir is mounted as /ia32.
  #define EXTRA_LIBPATH /ia32/lib
  #define EXTRA_INCPATH /ia32/include  
#else
  #error Invalid value specified for USE_COMPILER.
#endif

#if $[CHECK_SYNTAX_ONLY]
#define END_CFLAGS $[END_CFLAGS] /Zs 
#endif 
  
#if $[GEN_ASSEMBLY]
#define END_CFLAGS $[END_CFLAGS] /FAs
#endif 

#if $[PREPROCESSOR_OUTPUT]
#define END_CFLAGS $[END_CFLAGS] /E 
#endif 


#!/bin/csh -f

setenv OS `uname`

setenv LD_LIBRARY_PATH "."

# Setup the initial path
if ( $OS == "Linux" ) then
  set path = ( /var/local/bin ~/bin . /usr/sbin /sbin /usr/bin /bin \
             /usr/bin/X11 /usr/etc /usr/local/bin )
else if ( $OS == "IRIX64" ) then
  set path = ( /var/local/bin ~/bin /usr/local/prman/bin \
             . /usr/sbin /usr/bsd /sbin /usr/bin /bin /usr/bin/X11 /usr/etc  \
             /usr/demos/bin /usr/local/bin )
  setenv LD_LIBRARY_PATH ".:/usr/local/lib"
else if (($OS == "CYGWIN_NT-5.1") || ($OS == "CYGWIN_NT-5.0") || ( $OS == "CYGWIN_NT-4.0" ) || ( $OS == "WINNT" )) then
  set path = ( /usr/local/bin /contrib/bin . /usr/lib /bin $path )
  if ( $?LIB ) then
    setenv LIB "$LIB;"`cygpath -w /usr/lib`
  else 
    setenv LIB `cygpath -w /usr/lib`
  endif
else if (( $OS == "CYGWIN_98-4.10" ) || ( $OS == "WIN95" )) then
  set path = ( /bin /usr/local/bin /contrib/bin /msvc98/Bin \
        /mscommon/MSDev98/Bin /mscommon/Tools . /usr/lib $path )
  setenv LIB `cygpath -w /msvc98/mfc/lib`\;`cygpath -w /msvc98/lib`\;`cygpath -w /usr/lib`
  setenv INCLUDE `cygpath -w /msvc98/Include`
else
  set path = ( /var/local/bin ~/bin /usr/local/prman/bin \
             . /usr/sbin /usr/bsd /sbin /usr/bin /bin /usr/bin/X11 /usr/etc  \
             /usr/demos/bin /usr/local/bin )
endif

# Setup the initial manpath
if ( $OS == "Linux" ) then
setenv MANPATH "/usr/local/man:/usr/man/preformat:/usr/man:/usr/X11R6/man"
else if ( $OS == "IRIX64" ) then
setenv MANPATH "/usr/share/catman:/usr/catman:/usr/local/share/catman:/usr/local/share/man:/usr/local/man"
else if (( $OS == "CYGWIN_NT-5.1") || ( $OS == "CYGWIN_NT-5.0") || ( $OS == "CYGWIN_NT-4.0" ) || ( $OS == "CYGWIN_98-4.10" ) || ( $OS == "WIN95" )) then
setenv MANPATH "/usr/man:/contrib/man"
else
setenv MANPATH "/usr/share/catman:/usr/catman:/usr/local/share/catman:/usr/local/share/man:/usr/local/man"
endif

setenv CT_INCLUDE_PATH "."
set cdpath = ( . )
setenv CDPATH "."
setenv DC_PATH "."
setenv SSPATH "."
setenv STKPATH "."

if ( ! $?HAVE_ATRIA ) then
   if ( -e /usr/atria ) then
      /usr/atria/bin/cleartool mount -all >& /dev/null
      if ( $status == 0 ) setenv HAVE_ATRIA "yes"
   endif
endif

if ( ! $?CTDEFAULT_FLAV ) setenv CTDEFAULT_FLAV "default"
if ( ! $?CTEMACS_OPTS ) setenv CTEMACS_OPTS ""

if ( -e /usr/atria/bin ) set path = ( /usr/atria/bin $path )
rehash

if ( ! $?PENV ) then
   if ( $OS == "Linux" ) then
      setenv PENV "Linux"
   else if ( $OS == "IRIX64" ) then
      setenv PENV "SGI"
   else if (( $OS == "CYGWIN_NT-5.1") || ( $OS == "CYGWIN_NT-5.0") || ( $OS == "CYGWIN_NT-4.0" ) || ( $OS == "CYGWIN_98-4.10" ) || ( $OS == "WIN95" )) then
      setenv PENV "WIN32"
   else
      setenv PENV "SGI"
   endif
endif

if ( ! $?DTOOL ) setenv DTOOL /beta/player/bootstrap/dtool
if ( $#argv == 0 ) then
   source `$DTOOL/bin/ctattach.drv dtool default`
else
   source `$DTOOL/bin/ctattach.drv dtool $argv[1]`
endif

setenv CTEMACS_FOREHIGHLIGHT white
setenv CTEMACS_BACKHIGHLIGHT blue

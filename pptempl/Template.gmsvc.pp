//
// Template.gmsvc.pp
//
// This file defines the set of output files that will be generated to
// support a makefile build system invoking Microsoft's Visual C++
// command-line compiler, similar to Template.msvc.pp, but using
// Cygwin's GNU make instead of Microsoft's nmake.
//

// Before this file is processed, the following files are read and
// processed (in order):

// The Package.pp file in the root of the current source hierarchy
//   (e.g. $PANDA/Package.pp)
// $DTOOL/Package.pp
// $DTOOL/Config.pp
// $DTOOL/Config.Platform.pp
// The user's PPREMAKE_CONFIG file.
// $DTOOL/pptempl/System.pp
// All of the Sources.pp files in the current source hierarchy
// $DTOOL/pptempl/Global.pp
// $DTOOL/pptempl/Global.gmsvc.pp
// $DTOOL/pptempl/Depends.pp, once for each Sources.pp file
// Template.gmsvc.pp (this file), once for each Sources.pp file

#if $[ne $[DTOOL],]
#define dtool_ver_dir_cyg $[DTOOL]/src/dtoolbase
#define dtool_ver_dir $[osfilename $[dtool_ver_dir_cyg]]
#endif

//////////////////////////////////////////////////////////////////////
#if $[or $[eq $[DIR_TYPE], src],$[eq $[DIR_TYPE], metalib]]
//////////////////////////////////////////////////////////////////////
// For a source directory, build a single Makefile with rules to build
// each target.

#if $[build_directory]
  // This is the real set of lib_targets we'll be building.  On Windows,
  // we don't build the shared libraries which are included on metalibs.
  #define real_lib_targets
  #define deferred_objs
  #forscopes lib_target
    #if $[eq $[module $[TARGET],$[TARGET]],]
      // This library is not on a metalib, so we can build it.
      #set real_lib_targets $[real_lib_targets] $[TARGET]
    #else
      // This library is on a metalib, so we can't build it, but we
      // should build all the obj's that go into it.
      #set deferred_objs $[deferred_objs] \
        $[patsubst %,$[%_obj],$[compile_sources]]
    #endif
  #end lib_target
  
  // We need to know the various targets we'll be building.
  // $[lib_targets] will be the list of dynamic libraries,
  // $[static_lib_targets] the list of static libraries, and
  // $[bin_targets] the list of binaries.  $[test_bin_targets] is the
  // list of binaries that are to be built only when specifically asked
  // for.
  #define lib_targets $[patsubst %,$[st_dir]/lib%$[dllext].$[dlllib],$[active_target(metalib_target noinst_lib_target)] $[real_lib_targets]]
  #define static_lib_targets $[active_target(static_lib_target ss_lib_target):%=$[st_dir]/lib%$[dllext].lib]
  #define bin_targets \
      $[active_target(bin_target noinst_bin_target):%=$[st_dir]/%.exe] \
      $[active_target(sed_bin_target):%=$[st_dir]/%]
  #define test_bin_targets $[active_target(test_bin_target):%=$[st_dir]/%.exe]
  
  // And these variables will define the various things we need to
  // install.
  #define install_lib $[active_target(metalib_target static_lib_target ss_lib_target)] $[real_lib_targets]
  #define install_bin $[active_target(bin_target)]
  #define install_scripts $[sort $[INSTALL_SCRIPTS(metalib_target lib_target static_lib_target ss_lib_target bin_target)] $[INSTALL_SCRIPTS]]
  #define install_headers $[sort $[INSTALL_HEADERS(metalib_target lib_target static_lib_target ss_lib_target bin_target)] $[INSTALL_HEADERS]]
  #define install_parser_inc $[sort $[INSTALL_PARSER_INC]]
  #define install_data $[sort $[INSTALL_DATA(metalib_target lib_target static_lib_target ss_lib_target bin_target)] $[INSTALL_DATA]]
  #define install_config $[sort $[INSTALL_CONFIG(metalib_target lib_target static_lib_target ss_lib_target bin_target)] $[INSTALL_CONFIG]]
  #define install_igatedb $[sort $[get_igatedb(metalib_target lib_target)]]

  // Since we don't make a distinction in Windows between building for
  // static or dynamic targets, let's eliminate so_sources and just
  // use st_sources for simplicity.
  #define st_sources $[compile_sources(metalib_target lib_target noinst_lib_target static_lib_target ss_lib_target bin_target noinst_bin_target test_bin_target)]
  #define yxx_st_sources $[yxx_sources(metalib_target lib_target noinst_lib_target static_lib_target ss_lib_target bin_target noinst_bin_target test_bin_target)]
  #define lxx_st_sources $[lxx_sources(metalib_target lib_target noinst_lib_target static_lib_target ss_lib_target bin_target noinst_bin_target test_bin_target)]
  #define dep_sources_1 $[get_sources(metalib_target lib_target noinst_lib_target static_lib_target ss_lib_target bin_target noinst_bin_target test_bin_target)]
  
  // These are the source files that our dependency cache file will
  // depend on.  If it's an empty list, we won't bother writing rules to
  // freshen the cache file.
  #define dep_sources $[sort $[filter %.c %.cxx %.yxx %.lxx %.h %.I %.T,$[dep_sources_1]]]

#endif  // $[build_directory]


// We define $[complete_local_libs] as the full set of libraries (from
// within this tree) that we must link a particular target with.  It
// is the transitive closure of our dependent libs: the libraries we
// depend on, plus the libraries *those* libraries depend on, and so on.
#defer complete_local_libs $[unique $[closure all_libs,$[active_libs]]]
#defer actual_local_libs $[get_metalibs $[TARGET],$[complete_local_libs]]

// And $[complete_ipath] is the list of directories (from within this
// tree) we should add to our -I list.  It's basically just one for
// each directory named in the $[complete_local_libs], above, plus
// whatever else the user might have explicitly named in
// $[LOCAL_INCS].
#defer complete_ipath $[all_libs $[RELDIR],$[complete_local_libs]] $[RELDIR($[LOCAL_INCS:%=%/])]

// $[target_ipath] is the proper ipath to put on the command line,
// from the context of a particular target.

#defer target_ipath $[TOPDIR] $[sort $[complete_ipath]] $[other_trees:%=%\include] $[get_ipath]

// These are the complete set of extra flags the compiler requires.
#defer cflags $[get_cflags] $[CFLAGS] $[CFLAGS_OPT$[OPTIMIZE]] 
#defer c++flags $[get_cflags] $[C++FLAGS] $[CFLAGS_OPT$[OPTIMIZE]] 

// $[complete_lpath] is rather like $[complete_ipath]: the list of
// directories (from within this tree) we should add to our -L list.
#defer complete_lpath $[static_libs $[RELDIR:%=%/$[st_dir]],$[actual_local_libs]] $[dynamic_libs $[RELDIR:%=%/$[st_dir]],$[actual_local_libs]]

// $[lpath] is like $[target_ipath]: it's the list of directories we
// should add to our -L list, from the context of a particular target.
#defer lpath $[sort $[complete_lpath]] $[other_trees:%=%/lib] $[get_lpath]

// And $[libs] is the set of libraries we will link with.
#defer libs $[unique $[actual_local_libs:%=%$[dllext]] $[patsubst %:c,,%:m %,%$[dllext],$[OTHER_LIBS]] $[get_libs]]

// This is the set of files we might copy into *.prebuilt, if we have
// bison and flex (or copy from *.prebuilt if we don't have them).
#define bison_prebuilt $[patsubst %.yxx,%.cxx %.h,$[yxx_st_sources]] $[patsubst %.lxx,%.cxx,$[lxx_st_sources]]

// Rather than making a rule to generate each install directory later,
// we create the directories now.  This reduces problems from
// multiprocess builds.
#mkdir $[sort \
    $[if $[install_lib],$[install_lib_dir]] \
    $[if $[install_bin] $[install_scripts],$[install_bin_dir]] \
    $[if $[install_headers],$[install_headers_dir]] \
    $[if $[install_parser_inc],$[install_parser_inc_dir]] \
    $[if $[install_data],$[install_data_dir]] \
    $[if $[install_config],$[install_config_dir]] \
    $[if $[install_igatedb],$[install_igatedb_dir]] \
    ]

// Similarly, we need to ensure that $[st_dir] exists.  Trying to make
// the makefiles do this automatically just causes problems with
// multiprocess builds.
#mkdir $[st_dir]

// Pre-compiled headers are one way to speed the compilation of many
// C++ source files that include similar headers, but it turns out a
// more effective (and more portable) way is simply to compile all the
// similar source files in one pass.

// We do this by generating a *_composite.cxx file that has an
// #include line for each of several actual source files, and then we
// compile the composite file instead of the original files.
#foreach composite_file $[composite_list]
#output $[composite_file] notouch
#format collapse
/* Generated automatically by $[PPREMAKE] $[PPREMAKE_VERSION] from $[SOURCEFILE]. */
/* ################################# DO NOT EDIT ########################### */

#foreach file $[$[composite_file]_sources]
##include "$[file]"
#end file

#end $[composite_file]
#end composite_file

// Okay, we're ready.  Start outputting the Makefile now.
#output Makefile
#format makefile
#### Generated automatically by $[PPREMAKE] $[PPREMAKE_VERSION] from $[SOURCEFILE].
################################# DO NOT EDIT ###########################

// If we are using GNU make, this will automatically enable the
// multiprocessor build mode according to the value in
// NUMBER_OF_PROCESSORS, which should be set by NT.  Maybe this isn't
// a good idea to do all the time, but you can always disable it by
// explicitly unsetting NUMBER_OF_PROCESSORS, or by setting it to 1.
#if $[NUMBER_OF_PROCESSORS]
MAKEFLAGS := -j$[NUMBER_OF_PROCESSORS]
#endif

// The 'all' rule makes all the stuff in the directory except for the
// test_bin_targets.  It doesn't do any installation, however.
#define all_targets \
    Makefile \
    $[if $[dep_sources],$[DEPENDENCY_CACHE_FILENAME]] \
    $[sort $[lib_targets] $[static_lib_targets] $[bin_targets]] \
    $[deferred_objs]
all : $[all_targets]

// The 'test' rule makes all the test_bin_targets.
test : $[test_bin_targets]

clean :
#if $[st_sources]
$[TAB] rm -f $[st_dir]/*
#endif
$[TAB] rm -f *.pyc *.pyo // Also scrub out old generated Python code.
                         
// 'cleanall' is intended to undo all the effects of running ppremake
// and building.  It removes everything except the Makefile.
cleanall : clean
#if $[st_sources]
$[TAB] rm -rf $[st_dir]
#endif
#if $[yxx_st_sources] $[lxx_st_sources]
$[TAB] rm -f $[patsubst %.yxx,%.cxx %.h,$[yxx_st_sources]] $[patsubst %.lxx,%.cxx,$[lxx_st_sources]]
#endif
#if $[ne $[DEPENDENCY_CACHE_FILENAME],]
$[TAB] rm -f $[DEPENDENCY_CACHE_FILENAME]
#endif
#if $[composite_list]
$[TAB] rm -f $[composite_list]
#endif

clean-igate :
#forscopes metalib_target lib_target ss_lib_target
  #define igatedb $[get_igatedb]
  #define igateoutput $[get_igateoutput]
  #define igatemscan $[get_igatemscan]
  #define igatemout $[get_igatemout]
  #if $[igatedb]
$[TAB] rm -f $[igatedb]
  #endif
  #if $[igateoutput]
$[TAB] rm -f $[igateoutput] $[$[igateoutput]_obj]
  #endif
  #if $[igatemout]
$[TAB] rm -f $[igatemout] $[$[igatemout]_obj]
  #endif
#end metalib_target lib_target ss_lib_target

// Now, 'install' and 'uninstall'.  These simply copy files into the
// install directory (or remove them).  The 'install' rule also makes
// the directories if necessary.
#define installed_files \
     $[INSTALL_SCRIPTS:%=$[install_bin_dir]/%] \
     $[INSTALL_HEADERS:%=$[install_headers_dir]/%] \
     $[INSTALL_PARSER_INC:%=$[install_parser_inc_dir]/%] \
     $[INSTALL_DATA:%=$[install_data_dir]/%] \
     $[INSTALL_CONFIG:%=$[install_config_dir]/%]

#define installed_igate_files \
     $[get_igatedb(metalib_target lib_target ss_lib_target):$[st_dir]/%=$[install_igatedb_dir]/%]

#define install_targets \
     $[active_target(metalib_target lib_target static_lib_target ss_lib_target):%=install-lib%] \
     $[active_target(bin_target sed_bin_target):%=install-%] \
     $[installed_files]

install : all $[install_targets]

install-igate : $[sort $[installed_igate_files]]

uninstall : $[active_target(metalib_target lib_target static_lib_target ss_lib_target):%=uninstall-lib%] $[active_target(bin_target):%=uninstall-%]
#if $[installed_files]
$[TAB] rm -f $[sort $[installed_files]]
#endif

uninstall-igate :
#if $[installed_igate_files]
$[TAB] rm -f $[sort $[installed_igate_files]]
#endif

#if $[HAVE_BISON]
prebuild-bison : $[patsubst %,%.prebuilt,$[bison_prebuilt]]
clean-prebuild-bison : 
#if $[bison_prebuilt]
$[TAB] rm -f $[sort $[patsubst %,%.prebuilt,$[bison_prebuilt]]]
#endif
#endif

// Now it's time to start generating the rules to make our actual
// targets.

igate : $[get_igatedb(metalib_target lib_target ss_lib_target)]


/////////////////////////////////////////////////////////////////////
// First, the dynamic libraries.  Each lib_target and metalib_target
// is a dynamic library.
/////////////////////////////////////////////////////////////////////

#forscopes metalib_target lib_target

// In Windows, we don't actually build all the libraries.  In
// particular, we don't build any libraries that are listed on a
// metalib.  Is this one such library?
#define build_it $[eq $[module $[TARGET],$[TARGET]],]

// We might need to define a BUILDING_ symbol for win32.  We use the
// BUILDING_DLL variable name, defined typically in the metalib, for
// this; but in some cases, where the library isn't part of a metalib,
// we define BUILDING_DLL directly for the target.
#define building_var $[or $[BUILDING_DLL],$[module $[BUILDING_DLL],$[TARGET]]]

// $[igatescan] is the set of C++ headers and source files that we
// need to scan for interrogate.  $[igateoutput] is the name of the
// generated .cxx file that interrogate will produce (and which we
// should compile into the library).  $[igatedb] is the name of the
// generated .in file that interrogate will produce (and which should
// be installed into the /etc directory).
#define igatescan $[get_igatescan]
#define igateoutput $[get_igateoutput]
#define igatedb $[get_igatedb]

// If this is a metalib, it may have a number of components that
// include interrogated interfaces.  If so, we need to generate a
// 'module' file within this library.  This is mainly necessary for
// Python; it contains a table of all of the interrogated functions,
// so we can load the library as a Python module and have access to
// the interrogated functions.

// $[igatemscan] is the set of .in files generated by all of our
// component libraries.  If it is nonempty, then we do need to
// generate a module, and $[igatemout] is the name of the .cxx file
// that interrogate will produce to make this module.
#define igatemscan $[get_igatemscan]
#define igatemout $[get_igatemout]

#if $[build_it]
  // Now output the rule to actually link the library from all of its
  // various .obj files.

  #define sources \
   $[patsubst %,$[%_obj],$[compile_sources]] \
   $[components $[patsubst %,$[RELDIR]/$[%_obj],$[compile_sources]],$[active_component_libs]]

  #define varname $[subst -,_,lib$[TARGET]_so]
$[varname] = $[sources]
  #define target $[st_dir]/lib$[TARGET]$[dllext].$[dlllib]
  #define sources $($[varname])
  #define flags   $[get_cflags] $[C++FLAGS] $[CFLAGS_OPT$[OPTIMIZE]] $[CFLAGS_SHARED] $[building_var:%=/D%]
  #define mybasename $[basename $[notdir $[target]]]  
  #define tmpdirname_cyg $[install_lib_dir]/$[mybasename]
  #define tmpdirname_win $[osfilename $[tmpdirname_cyg]]
  
// not parallel (requires gmake 3.79) because of link.exe conflicts in TMP dir (see audiotraits dir)
#if $[GENERATE_BUILDDATE]
.NOTPARALLEL $[target] : $[sources] $[dtool_ver_dir_cyg]/version.rc $[dtool_ver_dir_cyg]/$[DLLBASEADDRFILENAME]
// first generate builddate for rc compiler using compiler preprocessor
$[TAB]  mkdir -p $[tmpdirname_cyg]  // this dir-creation-stuff is leftover from trying to resolve parallel link difficulties
 #define VER_RESOURCE "$[tmpdirname_win]\$[mybasename].res"
$[TAB]  cl /nologo /EP "$[dtool_ver_dir]\verdate.cpp"  > "$[tmpdirname_win]\verdate.h"
$[TAB]  rc /n /I"$[tmpdirname_win]" $[DECYGWINED_INC_PATHLIST_ARGS] /fo$[VER_RESOURCE] $[filter /D%, $[flags]]  "$[dtool_ver_dir]\version.rc"
  #if $[filter %.cxx %.yxx %.lxx,$[get_sources]]
$[TAB] $[SHARED_LIB_C++] $[VER_RESOURCE]
  #else  
$[TAB] $[SHARED_LIB_C] $[VER_RESOURCE]
  #endif
#else
.NOTPARALLEL $[target] : $[sources] $[dtool_ver_dir_cyg]/$[DLLBASEADDRFILENAME]
  #if $[filter %.cxx %.yxx %.lxx,$[get_sources]]
$[TAB] $[SHARED_LIB_C++]
  #else  
$[TAB] $[SHARED_LIB_C]
  #endif
#endif

#if $[build_dlls]
$[st_dir]/lib$[TARGET]$[dllext].lib : $[st_dir]/lib$[TARGET]$[dllext].dll
#endif
#if $[build_pdbs]
$[st_dir]/lib$[TARGET]$[dllext].pdb : $[st_dir]/lib$[TARGET]$[dllext].dll
#endif

#endif

// Here are the rules to install and uninstall the library and
// everything that goes along with it.
#define installed_files \
    $[if $[build_it], \
      $[if $[build_dlls],$[install_lib_dir]/lib$[TARGET]$[dllext].dll] \
      $[install_lib_dir]/lib$[TARGET]$[dllext].lib \
      $[if $[and $[build_dlls],$[build_pdbs]],$[install_lib_dir]/lib$[TARGET]$[dllext].pdb] \
    ] \
    $[INSTALL_SCRIPTS:%=$[install_bin_dir]/%] \
    $[INSTALL_HEADERS:%=$[install_headers_dir]/%] \
    $[INSTALL_DATA:%=$[install_data_dir]/%] \
    $[INSTALL_CONFIG:%=$[install_config_dir]/%] \
    $[igatedb:$[st_dir]/%=$[install_igatedb_dir]/%]

install-lib$[TARGET] : $[installed_files]

uninstall-lib$[TARGET] :
#if $[installed_files]
$[TAB] rm -f $[sort $[installed_files]]
#endif

#if $[build_dlls]
$[install_lib_dir]/lib$[TARGET]$[dllext].dll : $[st_dir]/lib$[TARGET]$[dllext].dll
#define local lib$[TARGET]$[dllext].dll
#define dest $[install_lib_dir]
$[TAB] cp -f $[st_dir]/$[local] $[dest]
#endif

$[install_lib_dir]/lib$[TARGET]$[dllext].lib : $[st_dir]/lib$[TARGET]$[dllext].lib
#define local lib$[TARGET]$[dllext].lib
#define dest $[install_lib_dir]
$[TAB] cp -f $[st_dir]/$[local] $[dest]

#if $[and $[build_dlls],$[build_pdbs]]
$[install_lib_dir]/lib$[TARGET]$[dllext].pdb : $[st_dir]/lib$[TARGET]$[dllext].pdb
#define local lib$[TARGET]$[dllext].pdb
#define dest $[install_lib_dir]
$[TAB] cp -f $[st_dir]/$[local] $[dest]
#endif

#if $[igatescan]
// Now, some additional rules to generate and compile the interrogate
// data, if needed.

// The library name is based on this library.
#define igatelib lib$[TARGET]
// The module name comes from the metalib that includes this library.
#define igatemod $[module $[TARGET],$[TARGET]]
#if $[eq $[igatemod],]
  // Unless no metalib includes this library.
  #define igatemod $[TARGET]
#endif

$[igatedb:$[st_dir]/%=$[install_igatedb_dir]/%] : $[igatedb]
#define local $[igatedb]
#define dest $[install_igatedb_dir]
$[TAB] cp -f $[local] $[dest]

// We have to split this out as a separate rule to properly support
// parallel make.
$[igatedb] : $[igateoutput]

lib$[TARGET]_igatescan = $[igatescan]
$[igateoutput] : $[sort $[patsubst %.h,%.h,%.I,%.I,%.T,%.T,%,,$[dependencies $[igatescan]] $[igatescan:%=./%]]]
$[TAB] $[INTERROGATE] -od $[igatedb] -oc $[igateoutput] $[interrogate_options] -module "$[igatemod]" -library "$[igatelib]" $(lib$[TARGET]_igatescan)

#endif  // igatescan


#if $[igatemout]
// And finally, some additional rules to build the interrogate module
// file into the library, if this is a metalib that includes
// interrogated components.

#define igatelib lib$[TARGET]
#define igatemod $[TARGET]

lib$[TARGET]_igatemscan = $[igatemscan]
#define target $[igatemout]
#define sources $(lib$[TARGET]_igatemscan)

$[target] : $[sources]
$[TAB] $[INTERROGATE_MODULE] -oc $[target] -module "$[igatemod]" -library "$[igatelib]" $[interrogate_module_options] $[sources]

#endif  // igatemout

#end metalib_target lib_target




/////////////////////////////////////////////////////////////////////
// Now, the noninstalled dynamic libraries.  These are presumably used
// only within this directory, or at the most within this tree, and
// also presumably will never include interrogate data.  That, plus
// the fact that we don't need to generate install rules, makes it a
// lot simpler.
/////////////////////////////////////////////////////////////////////

#forscopes noinst_lib_target
#define varname $[subst -,_,lib$[TARGET]_so]
$[varname] = $[patsubst %,$[%_obj],$[compile_sources]]
#define target $[st_dir]/lib$[TARGET]$[dllext].$[dlllib]
#define sources $($[varname])
$[target] : $[sources]
#if $[filter %.cxx %.yxx %.lxx,$[get_sources]]
$[TAB] $[SHARED_LIB_C++]
#else
$[TAB] $[SHARED_LIB_C]
#endif

#if $[build_dlls]
$[st_dir]/lib$[TARGET]$[dllext].lib : $[st_dir]/lib$[TARGET]$[dllext].dll
#endif
#if $[build_pdbs]
$[st_dir]/lib$[TARGET]$[dllext].pdb : $[st_dir]/lib$[TARGET]$[dllext].dll
#endif

#end noinst_lib_target



/////////////////////////////////////////////////////////////////////
// Now the static libraries.  Again, we assume there's no interrogate
// interfaces going on in here, and there's no question of this being
// a metalib, making the rules relatively simple.
/////////////////////////////////////////////////////////////////////

#forscopes static_lib_target ss_lib_target
#define varname $[subst -,_,lib$[TARGET]_a]
$[varname] = $[patsubst %,$[%_obj],$[compile_sources]]
#define target $[st_dir]/lib$[TARGET]$[dllext].lib
#define sources $($[varname])
$[target] : $[sources]
#if $[filter %.cxx %.yxx %.lxx,$[get_sources]]
$[TAB] $[STATIC_LIB_C++]
#else
$[TAB] $[STATIC_LIB_C]
#endif

#define installed_files \
    $[install_lib_dir]/lib$[TARGET]$[dllext].lib \
    $[INSTALL_SCRIPTS:%=$[install_bin_dir]/%] \
    $[INSTALL_HEADERS:%=$[install_headers_dir]/%] \
    $[INSTALL_DATA:%=$[install_data_dir]/%] \
    $[INSTALL_CONFIG:%=$[install_config_dir]/%]

install-lib$[TARGET] : $[installed_files]

uninstall-lib$[TARGET] :
#if $[installed_files]
$[TAB] rm -f $[sort $[installed_files]]
#endif

$[install_lib_dir]/lib$[TARGET]$[dllext].lib : $[st_dir]/lib$[TARGET]$[dllext].lib
#define local lib$[TARGET]$[dllext].lib
#define dest $[install_lib_dir]
$[TAB] cp -f $[st_dir]/$[local] $[dest]

#end static_lib_target ss_lib_target



/////////////////////////////////////////////////////////////////////
// The sed_bin_targets are a special bunch.  These are scripts that
// are to be preprocessed with sed before being installed, for
// instance to insert a path or something in an appropriate place.
/////////////////////////////////////////////////////////////////////

#forscopes sed_bin_target
$[TARGET] : $[st_dir]/$[TARGET]

#define target $[st_dir]/$[TARGET]
#define source $[SOURCE]
#define script $[COMMAND]
$[target] : $[source]
$[TAB] $[SED]
$[TAB] chmod +x $[target]

#define installed_files \
    $[install_bin_dir]/$[TARGET]

install-$[TARGET] : $[installed_files]

uninstall-$[TARGET] :
#if $[installed_files]
$[TAB] rm -f $[sort $[installed_files]]
#endif

#define local $[TARGET]
#define dest $[install_bin_dir]
$[install_bin_dir]/$[TARGET] : $[st_dir]/$[TARGET]
$[TAB] cp -f $[st_dir]/$[local] $[dest]

#end sed_bin_target


/////////////////////////////////////////////////////////////////////
// And now, the bin_targets.  These are normal C++ executables.  No
// interrogate, metalibs, or any such nonsense here.
/////////////////////////////////////////////////////////////////////

#forscopes bin_target
$[TARGET] : $[st_dir]/$[TARGET].exe

#define varname $[subst -,_,bin_$[TARGET]]
$[varname] = $[patsubst %,$[%_obj],$[compile_sources]]
#define target $[st_dir]/$[TARGET].exe
#define sources $($[varname])
#define ld $[get_ld]
$[target] : $[sources]
#if $[ld]
  // If there's a custom linker defined for the target, we have to use it.
$[TAB] $[ld] -o $[target] $[sources] $[lpath:%=-L%] $[libs:%=-l%]
#else
  // Otherwise, we can use the normal linker.
  #if $[filter %.cxx %.yxx %.lxx,$[get_sources]]
$[TAB] $[LINK_BIN_C++]
  #else
$[TAB] $[LINK_BIN_C]
  #endif
#endif

#if $[build_pdbs]
$[st_dir]/$[TARGET].pdb : $[st_dir]/$[TARGET].exe
#endif

#define installed_files \
    $[install_bin_dir]/$[TARGET].exe \
    $[if $[build_pdbs],$[install_bin_dir]/$[TARGET].pdb] \
    $[INSTALL_SCRIPTS:%=$[install_bin_dir]/%] \
    $[INSTALL_HEADERS:%=$[install_headers_dir]/%] \
    $[INSTALL_DATA:%=$[install_data_dir]/%] \
    $[INSTALL_CONFIG:%=$[install_config_dir]/%]

install-$[TARGET] : $[installed_files]

uninstall-$[TARGET] :
#if $[installed_files]
$[TAB] rm -f $[sort $[installed_files]]
#endif

$[install_bin_dir]/$[TARGET].exe : $[st_dir]/$[TARGET].exe
#define local $[TARGET].exe
#define dest $[install_bin_dir]
$[TAB] cp -f $[st_dir]/$[local] $[dest]

#if $[build_pdbs]
$[install_bin_dir]/$[TARGET].pdb : $[st_dir]/$[TARGET].pdb
#define local $[TARGET].pdb
#define dest $[install_bin_dir]
$[TAB] cp -f $[st_dir]/$[local] $[dest]
#endif

#end bin_target



/////////////////////////////////////////////////////////////////////
// The noinst_bin_targets and the test_bin_targets share the property
// of being built (when requested), but having no install rules.
/////////////////////////////////////////////////////////////////////

#forscopes noinst_bin_target test_bin_target
$[TARGET] : $[st_dir]/$[TARGET].exe

#define varname $[subst -,_,bin_$[TARGET]]
$[varname] = $[patsubst %,$[%_obj],$[compile_sources]]
#define target $[st_dir]/$[TARGET].exe
#define sources $($[varname])
$[target] : $[sources]
#if $[filter %.cxx %.yxx %.lxx,$[get_sources]]
$[TAB] $[LINK_BIN_C++]
#else
$[TAB] $[LINK_BIN_C]
#endif

#end noinst_bin_target test_bin_target



/////////////////////////////////////////////////////////////////////
// Rules to run bison and/or flex as needed.
/////////////////////////////////////////////////////////////////////

// Rules to generate a C++ file from a Bison input file.
#foreach file $[sort $[yxx_st_sources]]
#define target $[patsubst %.yxx,%.cxx,$[file]]
#define target_header $[patsubst %.yxx,%.h,$[file]]
#define target_prebuilt $[target].prebuilt
#define target_header_prebuilt $[target_header].prebuilt
#if $[HAVE_BISON]
$[target] : $[file]
$[TAB] $[BISON] $[YFLAGS] -y $[if $[YACC_PREFIX],-d --name-prefix=$[YACC_PREFIX]] $[file]
$[TAB] mv y.tab.c $[target]
$[TAB] mv y.tab.h $[target_header]
$[target_header] : $[target]
$[target_prebuilt] : $[target]
$[TAB] cp $[target] $[target_prebuilt]
$[target_header_prebuilt] : $[target_header]
$[TAB] cp $[target_header] $[target_header_prebuilt]
#else // HAVE_BISON
$[target] : $[target_prebuilt]
$[TAB] cp $[target_prebuilt] $[target]
$[target_header] : $[target_header_prebuilt]
$[TAB] cp $[target_header_prebuilt] $[target_header]
#endif // HAVE_BISON

#end file

// Rules to generate a C++ file from a Flex input file.
#foreach file $[sort $[lxx_st_sources]]
#define target $[patsubst %.lxx,%.cxx,$[file]]
#define target_prebuilt $[target].prebuilt
#if $[HAVE_BISON]
#define source $[file]
$[target] : $[file]
$[TAB] $[FLEX] $[LFLAGS] $[if $[YACC_PREFIX],-P$[YACC_PREFIX]] -olex.yy.c $[file]
#define source lex.yy.c
#define script /#include <unistd.h>/d
$[TAB] $[SED]
$[TAB] rm lex.yy.c
$[target_prebuilt] : $[target]
$[TAB] cp $[target] $[target_prebuilt]
#else // HAVE_BISON
$[target] : $[target_prebuilt]
$[TAB] cp $[target_prebuilt] $[target]
#endif // HAVE_BISON

#end file


/////////////////////////////////////////////////////////////////////
// Finally, we put in the rules to compile each source file into a .obj
// file.
/////////////////////////////////////////////////////////////////////

#forscopes metalib_target lib_target noinst_lib_target static_lib_target ss_lib_target bin_target noinst_bin_target test_bin_target

// Rules to compile ordinary C files.
#foreach file $[sort $[c_sources]]
#define target $[$[file]_obj]
#define source $[file]
#define ipath $[target_ipath]
#define flags $[cflags] $[building_var:%=/D%]
#if $[ne $[file], $[notdir $file]]
  // If the source file is not in the current directory, tack on "."
  // to front of the ipath.
  #set ipath . $[ipath]
#endif

$[target] : $[source] $[get_depends $[source]]
$[TAB] $[COMPILE_C]

#end file

// Rules to compile C++ files.

#foreach file $[sort $[cxx_sources]]
#define target $[$[file]_obj]
#define source $[file]
#define ipath $[target_ipath]
#define flags $[c++flags] $[building_var:%=/D%]
#if $[ne $[file], $[notdir $file]]
  // If the source file is not in the current directory, tack on "."
  // to front of the ipath.
  #set ipath . $[ipath]
#endif

// Yacc must run before some files can be compiled, so all files
// depend on yacc having run.
$[target] : $[source] $[get_depends $[source]] $[yxx_sources:%.yxx=%.h]
$[TAB] $[COMPILE_C++]

#end file

#end metalib_target lib_target noinst_lib_target static_lib_target ss_lib_target bin_target noinst_bin_target test_bin_target

// And now the rules to install the auxiliary files, like headers and
// data files.
#foreach file $[install_scripts]
$[install_bin_dir]/$[file] : $[file]
#define local $[file]
#define dest $[install_bin_dir]
$[TAB] cp -f $[local] $[dest]
#end file

#foreach file $[install_headers]
$[install_headers_dir]/$[file] : $[file]
#define local $[file]
#define dest $[install_headers_dir]
$[TAB] cp -f $[local] $[dest]
#end file

#foreach file $[install_parser_inc]
$[install_parser_inc_dir]/$[file] : $[file]
#define local $[file]
#define dest $[install_parser_inc_dir]
$[TAB] cp -f $[local] $[dest]
#end file

#foreach file $[install_data]
$[install_data_dir]/$[file] : $[file]
#define local $[file]
#define dest $[install_data_dir]
$[TAB] cp -f $[local] $[dest]
#end file

#foreach file $[install_config]
$[install_config_dir]/$[file] : $[file]
#define local $[file]
#define dest $[install_config_dir]
$[TAB] cp -f $[local] $[dest]
#end file

// Finally, all the special targets.  These are commands that just need
// to be invoked; we don't pretend to know what they are.
#forscopes special_target
$[TARGET] :
$[TAB] $[COMMAND]

#end special_target


// Finally, the rules to freshen the Makefile itself.
Makefile : $[SOURCE_FILENAME]
$[TAB] ppremake

#if $[and $[DEPENDENCY_CACHE_FILENAME],$[dep_sources]]
$[DEPENDENCY_CACHE_FILENAME] : $[dep_sources]
$[TAB] @ppremake -D $[DEPENDENCY_CACHE_FILENAME]
#endif

#end Makefile

//////////////////////////////////////////////////////////////////////
#elif $[eq $[DIR_TYPE], group]
//////////////////////////////////////////////////////////////////////

// This is a group directory: a directory above a collection of source
// directories, e.g. $DTOOL/src.  We don't need to output anything in
// this directory.



//////////////////////////////////////////////////////////////////////
#elif $[eq $[DIR_TYPE], toplevel]
//////////////////////////////////////////////////////////////////////

// This is the toplevel directory, e.g. $DTOOL.  Here we build the
// root makefile and also synthesize the dtool_config.h (or whichever
// file) we need.

#map subdirs
// Iterate through all of our known source files.  Each src and
// metalib type file gets its corresponding Makefile listed
// here.  However, we test for $[DIR_TYPE] of toplevel, because the
// source directories typically don't define their own DIR_TYPE
// variable, and they end up inheriting this one dynamically.
#forscopes */
#if $[or $[eq $[DIR_TYPE], src],$[eq $[DIR_TYPE], metalib],$[and $[eq $[DIR_TYPE], toplevel],$[ne $[DIRNAME],top]]]
#if $[build_directory]
  #addmap subdirs $[DIRNAME]
#endif
#endif
#end */

#output Makefile
#format makefile
#### Generated automatically by $[PPREMAKE] $[PPREMAKE_VERSION] from $[SOURCEFILE].
################################# DO NOT EDIT ###########################

all : $[subdirs]
test : $[subdirs:%=test-%]
igate : $[subdirs:%=igate-%]
clean : $[subdirs:%=clean-%]
clean-igate : $[subdirs:%=clean-igate-%]
cleanall : $[subdirs:%=cleanall-%]
install : $[if $[CONFIG_HEADER],$[install_headers_dir] $[install_headers_dir]/$[CONFIG_HEADER]] $[subdirs:%=install-%]
install-igate : $[subdirs:%=install-igate-%]
uninstall : $[subdirs:%=uninstall-%]
#if $[CONFIG_HEADER]
$[TAB] rm -f $[install_headers_dir]/$[CONFIG_HEADER]
#endif
uninstall-igate : $[subdirs:%=uninstall-igate-%]

#if $[HAVE_BISON]
prebuild-bison : $[subdirs:%=prebuild-bison-%]
clean-prebuild-bison : $[subdirs:%=clean-prebuild-bison-%]
#endif

#formap dirname subdirs
#define depends 
$[dirname] : $[dirnames $[if $[build_directory],$[DIRNAME]],$[DEPEND_DIRS]]
$[TAB] cd ./$[PATH] && $(MAKE) all
#end dirname

#formap dirname subdirs
test-$[dirname] :
$[TAB] cd ./$[PATH] && $(MAKE) test
#end dirname

#formap dirname subdirs
igate-$[dirname] :
$[TAB]cd ./$[PATH] && $(MAKE) igate
#end dirname

#formap dirname subdirs
clean-$[dirname] :
$[TAB] cd ./$[PATH] && $(MAKE) clean
#end dirname

#formap dirname subdirs
clean-igate-$[dirname] :
$[TAB] cd ./$[PATH] && $(MAKE) clean-igate
#end dirname

#formap dirname subdirs
cleanall-$[dirname] : $[patsubst %,cleanall-%,$[dirnames $[if $[build_directory],$[DIRNAME]],$[DEPEND_DIRS]]]
$[TAB] cd ./$[PATH] && $(MAKE) cleanall
#end dirname

#formap dirname subdirs
install-$[dirname] : $[patsubst %,install-%,$[dirnames $[if $[build_directory],$[DIRNAME]],$[DEPEND_DIRS]]]
$[TAB] cd ./$[PATH] && $(MAKE) install
#end dirname

#formap dirname subdirs
install-igate-$[dirname] :
$[TAB] cd ./$[PATH] && $(MAKE) install-igate
#end dirname

#formap dirname subdirs
uninstall-$[dirname] :
$[TAB] cd ./$[PATH] && $(MAKE) uninstall
#end dirname

#formap dirname subdirs
uninstall-igate-$[dirname] :
$[TAB] cd ./$[PATH] && $(MAKE) uninstall-igate
#end dirname

#if $[HAVE_BISON]
#formap dirname subdirs
prebuild-bison-$[dirname] :
$[TAB]cd ./$[PATH] && $(MAKE) prebuild-bison
clean-prebuild-bison-$[dirname] :
$[TAB]cd ./$[PATH] && $(MAKE) clean-prebuild-bison
#end dirname
#endif

#if $[ne $[CONFIG_HEADER],]
$[install_headers_dir] :
$[TAB] @test -d $[install_headers_dir] || echo mkdir -p $[install_headers_dir]
$[TAB] @test -d $[install_headers_dir] || mkdir -p $[install_headers_dir]

$[install_headers_dir]/$[CONFIG_HEADER] : $[CONFIG_HEADER]
#define local $[CONFIG_HEADER]
#define dest $[install_headers_dir]
$[TAB] cp -f $[local] $[dest]
#endif

#end Makefile

// If there is a file called LocalSetup.pp in the package's top
// directory, then invoke that.  It might contain some further setup
// instructions.
#sinclude $[TOPDIRPREFIX]LocalSetup.gmsvc.pp
#sinclude $[TOPDIRPREFIX]LocalSetup.pp


//////////////////////////////////////////////////////////////////////
#elif $[or $[eq $[DIR_TYPE], models],$[eq $[DIR_TYPE], models_toplevel],$[eq $[DIR_TYPE], models_group]]
//////////////////////////////////////////////////////////////////////

#include $[THISDIRPREFIX]Template.models.pp

//////////////////////////////////////////////////////////////////////
#endif // DIR_TYPE

//
// Depends.pp
//
// This file is loaded and run after each Sources.pp is read.  It
// defines the inter-directory dependencies, which is useful for
// determining build order.
//

#if $[or $[eq $[DIR_TYPE], src], $[eq $[DIR_TYPE], metalib]]
#if $[eq $[DEPEND_DIRS],]
  #define DEPENDABLE_HEADERS

  #forscopes metalib_target static_lib_target lib_target noinst_lib_target bin_target noinst_bin_target
    // We can optimize quite a bit by evaluating now several of the key
    // deferred variables defined in Globals.pp.  This way they won't need
    // to get repeatedly reevaluated as each directory examines each
    // other.
    #define build_directory $[build_directory]
    #define build_target $[build_target]
    #define active_local_libs $[active_local_libs]
    #define active_component_libs $[active_component_libs]
    #define active_libs $[active_libs]
    #define get_sources $[get_sources]

    // Report a warning for nonexisting dependencies.
    #define nonexisting $[unmapped all_libs,$[LOCAL_LIBS]]
    #if $[ne $[nonexisting],]
Warning: Lib(s) $[nonexisting], referenced in $[DIRNAME]/$[TARGET], not found.
    #endif

    #set DEPENDABLE_HEADERS $[DEPENDABLE_HEADERS] $[filter %.h %.I,$[SOURCES]]
  #end metalib_target static_lib_target lib_target noinst_lib_target bin_target noinst_bin_target

  // Allow the user to define additional EXTRA_DEPENDS targets in each
  // Sources.pp.
  #define DEPEND_DIRS \
    $[sort $[EXTRA_DEPENDS] $[all_libs $[DIRNAME],$[get_depend_libs]]]
  #set DEPENDABLE_HEADERS $[sort $[DEPENDABLE_HEADERS]]
#endif

#endif // DIR_TYPE

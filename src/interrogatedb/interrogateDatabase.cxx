// Filename: interrogateDatabase.cxx
// Created by:  drose (01Aug00)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) 2001 - 2004, Disney Enterprises, Inc.  All rights reserved
//
// All use of this software is subject to the terms of the Panda 3d
// Software license.  You should have received a copy of this license
// along with this source code; you will also find a current copy of
// the license at http://etc.cmu.edu/panda3d/docs/license/ .
//
// To contact the maintainers of this program write to
// panda3d-general@lists.sourceforge.net .
//
////////////////////////////////////////////////////////////////////

#include "interrogateDatabase.h"
#include "config_interrogatedb.h"
#include "indexRemapper.h"
#include "interrogate_datafile.h"

InterrogateDatabase *InterrogateDatabase::_global_ptr = NULL;
int InterrogateDatabase::_file_major_version = 0;
int InterrogateDatabase::_file_minor_version = 0;
int InterrogateDatabase::_current_major_version = 2;
int InterrogateDatabase::_current_minor_version = 2;

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::Constructor
//       Access: Private
//  Description:
////////////////////////////////////////////////////////////////////
InterrogateDatabase::
InterrogateDatabase() {
  _next_index = 1;
  _lookups_fresh = 0;
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::get_ptr
//       Access: Private
//  Description: Returns the global pointer to the one
//               InterrogateDatabase.
////////////////////////////////////////////////////////////////////
InterrogateDatabase *InterrogateDatabase::
get_ptr() {
  if (_global_ptr == (InterrogateDatabase *)NULL) {
    if (interrogatedb_cat->is_debug()) {
      interrogatedb_cat->debug()
        << "Creating interrogate database\n";
    }
    _global_ptr = new InterrogateDatabase;
  }
  return _global_ptr;
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::request_module
//       Access: Public
//  Description: Requests that the interrogate data for the given
//               module be made available.  The function pointers will
//               be made available immediately, while the database
//               file will be read later, the next time someone asks
//               for interrogate data that requires it.
////////////////////////////////////////////////////////////////////
void InterrogateDatabase::
request_module(InterrogateModuleDef *def) {
  if (interrogatedb_cat->is_debug()) {
    if (def->library_name == (const char *)NULL) {
      interrogatedb_cat->debug()
        << "Got interrogate data for anonymous module\n";
    } else {
      interrogatedb_cat->debug()
        << "Got interrogate data for module " << def->library_name << "\n";
    }
  }

  int num_indices = def->next_index - def->first_index;
  if (num_indices > 0) {
    // If the module def has any definitions--any index numbers
    // used--assign it to its own unique range of index numbers.
    def->first_index = _next_index;
    _next_index += num_indices;
    def->next_index = _next_index;

    // Assign a reference to the module def by index number.  When we
    // need to look up a function by its index number, we'll be able
    // to use this.
    _modules.push_back(def);
  }

  if (def->num_unique_names > 0 && def->library_name != (const char *)NULL) {
    // Define a lookup by hash for this module, mainly so we can look
    // up functions by their unique names.
    _modules_by_hash[def->library_hash_name] = def;
  }

  if (def->database_filename != (const char *)NULL) {
    _requests.push_back(def);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::get_num_global_types
//       Access: Public
//  Description: Returns the total number of "global" types known to
//               the interrogate database.  These are types defined at
//               the global level that should be considered for
//               exporting, but not the incidental types (like
//               pointers, etc.) that must be defined to support
//               these.
////////////////////////////////////////////////////////////////////
int InterrogateDatabase::
get_num_global_types() {
  check_latest();
  return _global_types.size();
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::get_global_type
//       Access: Public
//  Description: Returns the index of the nth global type known to the
//               interrogate database.
////////////////////////////////////////////////////////////////////
TypeIndex InterrogateDatabase::
get_global_type(int n) {
  check_latest();
  if (n >= 0 && n < (int)_global_types.size()) {
    return _global_types[n];
  }
  return 0;
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::get_num_all_types
//       Access: Public
//  Description: Returns the total number of types known to the
//               interrogate database.  This includes all known types,
//               global as well as incidental.  See also
//               get_num_global_types().
////////////////////////////////////////////////////////////////////
int InterrogateDatabase::
get_num_all_types() {
  check_latest();
  return _all_types.size();
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::get_all_type
//       Access: Public
//  Description: Returns the index of the nth type known to the
//               interrogate database.
////////////////////////////////////////////////////////////////////
TypeIndex InterrogateDatabase::
get_all_type(int n) {
  check_latest();
  if (n >= 0 && n < (int)_all_types.size()) {
    return _all_types[n];
  }
  return 0;
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::get_num_global_functions
//       Access: Public
//  Description: Returns the total number of global functions known to
//               the interrogate database.  These are functions
//               defined at the global level, e.g. non-member
//               functions.
////////////////////////////////////////////////////////////////////
int InterrogateDatabase::
get_num_global_functions() {
  check_latest();
  return _global_functions.size();
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::get_global_function
//       Access: Public
//  Description: Returns the index of the nth global function known to
//               the interrogate database.
////////////////////////////////////////////////////////////////////
FunctionIndex InterrogateDatabase::
get_global_function(int n) {
  check_latest();
  if (n >= 0 && n < (int)_global_functions.size()) {
    return _global_functions[n];
  }
  return 0;
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::get_num_all_functions
//       Access: Public
//  Description: Returns the total number of functions known to the
//               interrogate database.  This includes all known
//               functions, global, method, or synthesized.  See also
//               get_num_global_functions().
////////////////////////////////////////////////////////////////////
int InterrogateDatabase::
get_num_all_functions() {
  check_latest();
  return _all_functions.size();
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::get_all_function
//       Access: Public
//  Description: Returns the index of the nth function known to the
//               interrogate database.
////////////////////////////////////////////////////////////////////
FunctionIndex InterrogateDatabase::
get_all_function(int n) {
  check_latest();
  if (n >= 0 && n < (int)_all_functions.size()) {
    return _all_functions[n];
  }
  return 0;
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::get_num_global_manifests
//       Access: Public
//  Description: Returns the total number of global manifest constants
//               known to the interrogate database.
////////////////////////////////////////////////////////////////////
int InterrogateDatabase::
get_num_global_manifests() {
  check_latest();
  return _global_manifests.size();
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::get_global_manifest
//       Access: Public
//  Description: Returns the index of the nth global manifest constant
//               known to the interrogate database.
////////////////////////////////////////////////////////////////////
ManifestIndex InterrogateDatabase::
get_global_manifest(int n) {
  check_latest();
  if (n >= 0 && n < (int)_global_manifests.size()) {
    return _global_manifests[n];
  }
  return 0;
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::get_num_global_elements
//       Access: Public
//  Description: Returns the total number of global data elements
//               known to the interrogate database.
////////////////////////////////////////////////////////////////////
int InterrogateDatabase::
get_num_global_elements() {
  check_latest();
  return _global_elements.size();
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::get_global_element
//       Access: Public
//  Description: Returns the index of the nth global data element
//               known to the interrogate database.
////////////////////////////////////////////////////////////////////
ElementIndex InterrogateDatabase::
get_global_element(int n) {
  check_latest();
  if (n >= 0 && n < (int)_global_elements.size()) {
    return _global_elements[n];
  }
  return 0;
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::get_type
//       Access: Public
//  Description: Returns the type associated with the given TypeIndex,
//               if there is one.
////////////////////////////////////////////////////////////////////
const InterrogateType &InterrogateDatabase::
get_type(TypeIndex type) {
  static InterrogateType bogus_type;

  check_latest();
  TypeMap::const_iterator ti;
  ti = _type_map.find(type);
  if (ti == _type_map.end()) {
    return bogus_type;
  }
  return (*ti).second;
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::get_function
//       Access: Public
//  Description: Returns the function associated with the given
//               FunctionIndex, if there is one.
////////////////////////////////////////////////////////////////////
const InterrogateFunction &InterrogateDatabase::
get_function(FunctionIndex function) {
  static InterrogateFunction bogus_function;

  check_latest();
  FunctionMap::const_iterator fi;
  fi = _function_map.find(function);
  if (fi == _function_map.end()) {
    return bogus_function;
  }
  return *(*fi).second;
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::get_wrapper
//       Access: Public
//  Description: Returns the function wrapper associated with the
//               given FunctionWrapperIndex, if there is one.
////////////////////////////////////////////////////////////////////
const InterrogateFunctionWrapper &InterrogateDatabase::
get_wrapper(FunctionWrapperIndex wrapper) {
  static InterrogateFunctionWrapper bogus_wrapper;

  check_latest();
  FunctionWrapperMap::const_iterator wi;
  wi = _wrapper_map.find(wrapper);
  if (wi == _wrapper_map.end()) {
    return bogus_wrapper;
  }
  return (*wi).second;
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::get_manifest
//       Access: Public
//  Description: Returns the manifest constant associated with the
//               given ManifestIndex, if there is one.
////////////////////////////////////////////////////////////////////
const InterrogateManifest &InterrogateDatabase::
get_manifest(ManifestIndex manifest) {
  static InterrogateManifest bogus_manifest;

  check_latest();
  ManifestMap::const_iterator mi;
  mi = _manifest_map.find(manifest);
  if (mi == _manifest_map.end()) {
    return bogus_manifest;
  }
  return (*mi).second;
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::get_element
//       Access: Public
//  Description: Returns the data element associated with the
//               given ElementIndex, if there is one.
////////////////////////////////////////////////////////////////////
const InterrogateElement &InterrogateDatabase::
get_element(ElementIndex element) {
  static InterrogateElement bogus_element;

  check_latest();
  ElementMap::const_iterator ei;
  ei = _element_map.find(element);
  if (ei == _element_map.end()) {
    return bogus_element;
  }
  return (*ei).second;
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::remove_type
//       Access: Public
//  Description: Erases the type from the database.
////////////////////////////////////////////////////////////////////
void InterrogateDatabase::
remove_type(TypeIndex type) {
  _type_map.erase(type);
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::get_fptr
//       Access: Public
//  Description: Returns the function pointer associated with the
//               given function wrapper, if it has a pointer
//               available.  Returns NULL if the pointer is not
//               available.
////////////////////////////////////////////////////////////////////
void *InterrogateDatabase::
get_fptr(FunctionWrapperIndex wrapper) {
  InterrogateModuleDef *def;
  int module_index;
  if (find_module(wrapper, def, module_index)) {
    if (module_index >= 0 && module_index < def->num_fptrs) {
      return def->fptrs[module_index];
    }
  }
  return (void *)NULL;
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::get_wrapper_by_unique_name
//       Access: Public
//  Description: Looks up the function wrapper corresponding to the
//               given unique name, if available.  Returns the
//               corresponding wrapper index, or 0 if no such
//               wrapper is found.
////////////////////////////////////////////////////////////////////
FunctionWrapperIndex InterrogateDatabase::
get_wrapper_by_unique_name(const string &unique_name) {
  // First, split the unique_name into a library_hash_name and a
  // wrapper_hash_name.

  // The first four characters are always the library_name.
  string library_hash_name = unique_name.substr(0, 4);
  string wrapper_hash_name = unique_name.substr(4);

  // Is the library_name defined?
  ModulesByHash::const_iterator mi;
  mi = _modules_by_hash.find(library_hash_name);
  if (mi == _modules_by_hash.end()) {
    return 0;
  }

  InterrogateModuleDef *def = (*mi).second;
  int index_offset =
    binary_search_wrapper_hash(def->unique_names,
                               def->unique_names + def->num_unique_names,
                               wrapper_hash_name);
  if (index_offset >= 0) {
    return def->first_index + index_offset;
  }

  return 0;
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::get_file_major_version
//       Access: Public
//  Description: Returns the major version number of the interrogate
//               database file currently being read.
////////////////////////////////////////////////////////////////////
int InterrogateDatabase::
get_file_major_version() {
  return _file_major_version;
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::get_file_minor_version
//       Access: Public
//  Description: Returns the minor version number of the interrogate
//               database file currently being read.
////////////////////////////////////////////////////////////////////
int InterrogateDatabase::
get_file_minor_version() {
  return _file_minor_version;
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::get_current_major_version
//       Access: Public
//  Description: Returns the major version number currently expected
//               in interrogate database files generated by this code
//               base.
////////////////////////////////////////////////////////////////////
int InterrogateDatabase::
get_current_major_version() {
  return _current_major_version;
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::get_current_minor_version
//       Access: Public
//  Description: Returns the minor version number currently expected
//               in interrogate database files generated by this code
//               base.
////////////////////////////////////////////////////////////////////
int InterrogateDatabase::
get_current_minor_version() {
  return _current_minor_version;
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::get_next_index
//       Access: Public
//  Description: Returns a new index number suitable for the next
//               thing, that will not be shared with any other index
//               numbers.
////////////////////////////////////////////////////////////////////
int InterrogateDatabase::
get_next_index() {
  return _next_index++;
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::add_type
//       Access: Public
//  Description: Adds the indicated type to the database at the given
//               index number.
////////////////////////////////////////////////////////////////////
void InterrogateDatabase::
add_type(TypeIndex index, const InterrogateType &type) {
  assert(index != 0);
  bool inserted =
    _type_map.insert(TypeMap::value_type(index, type)).second;

  if (!inserted) {
    // If there was already a type at that index, maybe it was a
    // forward reference.  If its _fully_defined bit isn't set, then
    // it's ok.
    InterrogateType &old_type = _type_map[index];
    assert(!old_type.is_fully_defined());

    // It was a forward reference.  Merge it with the new one.
    old_type.merge_with(type);
  }

  if (type.is_global()) {
    _global_types.push_back(index);
  }
  _all_types.push_back(index);
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::add_function
//       Access: Public
//  Description: Adds the indicated function to the database at
//               the given index number.
////////////////////////////////////////////////////////////////////
void InterrogateDatabase::
add_function(FunctionIndex index, InterrogateFunction *function) {
  bool inserted =
    _function_map.insert(FunctionMap::value_type(index, function)).second;
  assert(inserted);

  if (function->is_global()) {
    _global_functions.push_back(index);
  }
  _all_functions.push_back(index);
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::add_wrapper
//       Access: Public
//  Description: Adds the indicated function wrapper to the database at
//               the given index number.
////////////////////////////////////////////////////////////////////
void InterrogateDatabase::
add_wrapper(FunctionWrapperIndex index,
            const InterrogateFunctionWrapper &wrapper) {
  bool inserted =
    _wrapper_map.insert(FunctionWrapperMap::value_type(index, wrapper)).second;
  assert(inserted);
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::add_manifest
//       Access: Public
//  Description: Adds the indicated manifest constant to the database
//               at the given index number.
////////////////////////////////////////////////////////////////////
void InterrogateDatabase::
add_manifest(ManifestIndex index, const InterrogateManifest &manifest) {
  bool inserted =
    _manifest_map.insert(ManifestMap::value_type(index, manifest)).second;
  assert(inserted);

  _global_manifests.push_back(index);
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::add_element
//       Access: Public
//  Description: Adds the indicated data element to the database
//               at the given index number.
////////////////////////////////////////////////////////////////////
void InterrogateDatabase::
add_element(ElementIndex index, const InterrogateElement &element) {
  bool inserted =
    _element_map.insert(ElementMap::value_type(index, element)).second;
  assert(inserted);

  if (element.is_global()) {
    _global_elements.push_back(index);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::update_type
//       Access: Public
//  Description: Returns a non-const reference to the indicated type,
//               allowing the user to update it.
////////////////////////////////////////////////////////////////////
InterrogateType &InterrogateDatabase::
update_type(TypeIndex type) {
  assert(type != 0);
  check_latest();
  return _type_map[type];
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::update_function
//       Access: Public
//  Description: Returns a non-const reference to the indicated
//               function, allowing the user to update it.
////////////////////////////////////////////////////////////////////
InterrogateFunction &InterrogateDatabase::
update_function(FunctionIndex function) {
  check_latest();
  return *_function_map[function];
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::update_wrapper
//       Access: Public
//  Description: Returns a non-const reference to the indicated
//               function wrapper, allowing the user to update it.
////////////////////////////////////////////////////////////////////
InterrogateFunctionWrapper &InterrogateDatabase::
update_wrapper(FunctionWrapperIndex wrapper) {
  check_latest();
  return _wrapper_map[wrapper];
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::update_manifest
//       Access: Public
//  Description: Returns a non-const reference to the indicated
//               manifest constant, allowing the user to update it.
////////////////////////////////////////////////////////////////////
InterrogateManifest &InterrogateDatabase::
update_manifest(ManifestIndex manifest) {
  check_latest();
  return _manifest_map[manifest];
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::update_element
//       Access: Public
//  Description: Returns a non-const reference to the indicated
//               data element, allowing the user to update it.
////////////////////////////////////////////////////////////////////
InterrogateElement &InterrogateDatabase::
update_element(ElementIndex element) {
  check_latest();
  return _element_map[element];
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::remap_indices
//       Access: Public
//  Description: Resequences all of the various index numbers so that
//               all of the functions start at first_index and
//               increment consecutively from there, and then all of
//               the types follow.  Returns the next available index
//               number.
////////////////////////////////////////////////////////////////////
int InterrogateDatabase::
remap_indices(int first_index) {
  IndexRemapper remap;
  return remap_indices(first_index, remap);
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::remap_indices
//       Access: Public
//  Description: This flavor of remap_indices() accepts a map that
//               should be empty on initial call, and will be filled
//               with the mapping of old index number to new index
//               number.  This allows the caller to update its own
//               data structures to match the new index numbers.
////////////////////////////////////////////////////////////////////
int InterrogateDatabase::
remap_indices(int first_index, IndexRemapper &remap) {
  remap.clear();

  // First, build up the complete map.

  // Get the wrapper indices first.  This is important, because the
  // InterrogateBuilder wants these to be first, and consecutive.
  FunctionWrapperMap new_wrapper_map;
  FunctionWrapperMap::iterator wi;
  for (wi = _wrapper_map.begin(); wi != _wrapper_map.end(); ++wi) {
    remap.add_mapping((*wi).first, first_index);
    new_wrapper_map[first_index] = (*wi).second;
    first_index++;
  }

  // Everything else can follow; it doesn't matter so much.
  FunctionMap new_function_map;
  FunctionMap::iterator fi;
  for (fi = _function_map.begin(); fi != _function_map.end(); ++fi) {
    remap.add_mapping((*fi).first, first_index);
    new_function_map[first_index] = (*fi).second;
    first_index++;
  }

  TypeMap new_type_map;
  TypeMap::iterator ti;
  for (ti = _type_map.begin(); ti != _type_map.end(); ++ti) {
    assert((*ti).first != 0);
    remap.add_mapping((*ti).first, first_index);
    new_type_map[first_index] = (*ti).second;
    first_index++;
  }

  ManifestMap new_manifest_map;
  ManifestMap::iterator mi;
  for (mi = _manifest_map.begin(); mi != _manifest_map.end(); ++mi) {
    remap.add_mapping((*mi).first, first_index);
    new_manifest_map[first_index] = (*mi).second;
    first_index++;
  }

  ElementMap new_element_map;
  ElementMap::iterator ei;
  for (ei = _element_map.begin(); ei != _element_map.end(); ++ei) {
    remap.add_mapping((*ei).first, first_index);
    new_element_map[first_index] = (*ei).second;
    first_index++;
  }

  _next_index = first_index;

  _wrapper_map.swap(new_wrapper_map);
  _function_map.swap(new_function_map);
  _type_map.swap(new_type_map);
  _manifest_map.swap(new_manifest_map);
  _element_map.swap(new_element_map);

  // Then, go back through and update all of the internal references.
  for (wi = _wrapper_map.begin(); wi != _wrapper_map.end(); ++wi) {
    (*wi).second.remap_indices(remap);
  }
  for (fi = _function_map.begin(); fi != _function_map.end(); ++fi) {
    (*fi).second->remap_indices(remap);
  }
  for (ti = _type_map.begin(); ti != _type_map.end(); ++ti) {
    (*ti).second.remap_indices(remap);
  }
  for (mi = _manifest_map.begin(); mi != _manifest_map.end(); ++mi) {
    (*mi).second.remap_indices(remap);
  }
  for (ei = _element_map.begin(); ei != _element_map.end(); ++ei) {
    (*ei).second.remap_indices(remap);
  }
  GlobalFunctions::iterator gfi;
  for (gfi = _global_functions.begin(); gfi != _global_functions.end(); ++gfi) {
    (*gfi) = remap.map_from(*gfi);
  }
  for (gfi = _all_functions.begin(); gfi != _all_functions.end(); ++gfi) {
    (*gfi) = remap.map_from(*gfi);
  }
  GlobalTypes::iterator gti;
  for (gti = _global_types.begin(); gti != _global_types.end(); ++gti) {
    (*gti) = remap.map_from(*gti);
  }
  for (gti = _all_types.begin(); gti != _all_types.end(); ++gti) {
    (*gti) = remap.map_from(*gti);
  }
  GlobalManifests::iterator gmi;
  for (gmi = _global_manifests.begin(); gmi != _global_manifests.end(); ++gmi) {
    (*gmi) = remap.map_from(*gmi);
  }
  GlobalElements::iterator gei;
  for (gei = _global_elements.begin(); gei != _global_elements.end(); ++gei) {
    (*gei) = remap.map_from(*gei);
  }

  return _next_index;
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::write
//       Access: Public
//  Description: Writes the database to the indicated stream for later
//               reading.
////////////////////////////////////////////////////////////////////
void InterrogateDatabase::
write(ostream &out, InterrogateModuleDef *def) const {
  // Write out the file header.
  out << def->file_identifier << "\n"
      << _current_major_version << " " << _current_minor_version << "\n";

  // Write out the module definition.
  idf_output_string(out, def->library_name);
  idf_output_string(out, def->library_hash_name);
  idf_output_string(out, def->module_name);
  out << "\n";

  // Now write out the components.

  out << _function_map.size() << "\n";
  FunctionMap::const_iterator fi;
  for (fi = _function_map.begin(); fi != _function_map.end(); ++fi) {
    out << (*fi).first << " " << *(*fi).second << "\n";
  }

  out << _wrapper_map.size() << "\n";
  FunctionWrapperMap::const_iterator wi;
  for (wi = _wrapper_map.begin(); wi != _wrapper_map.end(); ++wi) {
    out << (*wi).first << " " << (*wi).second << "\n";
  }

  out << _type_map.size() << "\n";
  TypeMap::const_iterator ti;
  for (ti = _type_map.begin(); ti != _type_map.end(); ++ti) {
    out << (*ti).first << " " << (*ti).second << "\n";
  }

  out << _manifest_map.size() << "\n";
  ManifestMap::const_iterator mi;
  for (mi = _manifest_map.begin(); mi != _manifest_map.end(); ++mi) {
    out << (*mi).first << " " << (*mi).second << "\n";
  }

  out << _element_map.size() << "\n";
  ElementMap::const_iterator ei;
  for (ei = _element_map.begin(); ei != _element_map.end(); ++ei) {
    out << (*ei).first << " " << (*ei).second << "\n";
  }
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::read
//       Access: Public
//  Description: Reads a database from the indicated stream,
//               associated with the indicated module definition and
//               merges it with any existing data in the database,
//               according to the expected index numbers specified in
//               the module def.  The header information has already
//               been read.
//
//               Returns true if the file is read successfully, false
//               if there is an error.
////////////////////////////////////////////////////////////////////
bool InterrogateDatabase::
read(istream &in, InterrogateModuleDef *def) {
  InterrogateDatabase temp;
  if (!temp.read_new(in, def)) {
    return false;
  }

  if (def->first_index == 0 && def->next_index == 0) {
    _next_index = temp.remap_indices(_next_index);

  } else {
    int next = temp.remap_indices(def->first_index);
    if (next != def->next_index) {
      interrogatedb_cat->error()
        << "Module database file " << def->database_filename
        << " is out of date.\n";
      return false;
    }
  }

  merge_from(temp);
  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::load_latest
//       Access: Private
//  Description: Reads in the latest interrogate data.
////////////////////////////////////////////////////////////////////
void InterrogateDatabase::
load_latest() {
  const DSearchPath &searchpath = get_interrogatedb_path();

  Requests copy_requests;
  copy_requests.swap(_requests);

  Requests::const_iterator ri;
  for (ri = copy_requests.begin(); ri != copy_requests.end(); ++ri) {
    InterrogateModuleDef *def = (*ri);

    if (def->database_filename != (char *)NULL) {
      Filename filename = def->database_filename;
      Filename pathname = filename;
      if (!pathname.empty() && pathname[0] != '/') {
        pathname = searchpath.find_file(pathname);
      }

      if (pathname.empty()) {
        interrogatedb_cat->error()
          << "Unable to find " << filename << " on " << searchpath << "\n";
      } else {

        ifstream input;
        pathname.set_text();
        if (!pathname.open_read(input)) {
          interrogatedb_cat->error() << "Unable to read " << pathname << ".\n";
        } else {
          int file_identifier;
          input >> file_identifier
                >> _file_major_version >> _file_minor_version;

          if (def->file_identifier != 0 &&
              file_identifier != def->file_identifier) {
            interrogatedb_cat->warning()
              << "Interrogate data in " << pathname
              << " is out of sync with the compiled-in data.\n";
          }

          if (_file_major_version != _current_major_version ||
              _file_minor_version > _current_minor_version) {
            interrogatedb_cat->error()
              << "Cannot read interrogate data in " << pathname
              << "; database is version " << _file_major_version << "."
              << _file_minor_version << " while we are expecting "
              << _current_major_version << "." << _current_minor_version
              << ".\n";
          } else {
            if (interrogatedb_cat->is_debug()) {
              interrogatedb_cat->debug()
                << "Reading " << filename << "\n";
            }
            if (!read(input, def)) {
              interrogatedb_cat->error()
                << "Error reading " << pathname << ".\n";
            }
          }
        }
      }
    }
  }

  _requests.clear();
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::read_new
//       Access: Private
//  Description: Reads from the indicated stream (the header
//               information has already been read) into the
//               newly-created database.  It is an error if the
//               database already has some data in it.
////////////////////////////////////////////////////////////////////
bool InterrogateDatabase::
read_new(istream &in, InterrogateModuleDef *def) {
  // We've already read the header.  Read the module definition.
  idf_input_string(in, def->library_name);
  idf_input_string(in, def->library_hash_name);
  idf_input_string(in, def->module_name);

  // Now read all of the components.

  { // Functions.
    int num_functions;
    in >> num_functions;
    if (in.fail()) {
      return false;
    }

    while (num_functions > 0) {
      FunctionIndex index;
      InterrogateFunction *function = new InterrogateFunction(def);
      in >> index >> *function;
      if (in.fail()) {
        delete function;
        return false;
      }

      add_function(index, function);
      num_functions--;
    }
  }

  { // Wrappers.
    int num_wrappers;
    in >> num_wrappers;
    if (in.fail()) {
      return false;
    }

    while (num_wrappers > 0) {
      FunctionWrapperIndex index;
      InterrogateFunctionWrapper wrapper(def);
      in >> index >> wrapper;
      if (in.fail()) {
        return false;
      }

      add_wrapper(index, wrapper);
      num_wrappers--;
    }
  }

  { // Types.
    int num_types;
    in >> num_types;
    if (in.fail()) {
      return false;
    }

    while (num_types > 0) {
      TypeIndex index;
      InterrogateType type(def);
      in >> index >> type;
      if (in.fail()) {
        return false;
      }

      add_type(index, type);
      num_types--;
    }
  }

  { // Manifests.
    int num_manifests;
    in >> num_manifests;
    if (in.fail()) {
      return false;
    }

    while (num_manifests > 0) {
      ManifestIndex index;
      InterrogateManifest manifest(def);
      in >> index >> manifest;
      if (in.fail()) {
        return false;
      }

      add_manifest(index, manifest);
      num_manifests--;
    }
  }

  { // Elements.
    int num_elements;
    in >> num_elements;
    if (in.fail()) {
      return false;
    }

    while (num_elements > 0) {
      ElementIndex index;
      InterrogateElement element(def);
      in >> index >> element;
      if (in.fail()) {
        return false;
      }

      add_element(index, element);
      num_elements--;
    }
  }

  return true;
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::merge_from
//       Access: Private
//  Description: Copies all the data from the indicated database into
//               this one.  It is an error if any index numbers are
//               shared between the two databases.
////////////////////////////////////////////////////////////////////
void InterrogateDatabase::
merge_from(const InterrogateDatabase &other) {
  // We want to collapse shared types together.
  IndexRemapper remap;

  // First, we need to build a set of types by name, so we know what
  // types we already have.
  map<string, TypeIndex> types_by_name;

  TypeMap::const_iterator ti;
  for (ti = _type_map.begin(); ti != _type_map.end(); ++ti) {
    const InterrogateType &type = (*ti).second;
    if (type.has_true_name()) {
      types_by_name[type.get_true_name()] = (*ti).first;
    }
  }

  // Now go through the other set of types and determine the mapping
  // into this set.
  for (ti = other._type_map.begin(); ti != other._type_map.end(); ++ti) {
    TypeIndex other_type_index = (*ti).first;
    const InterrogateType &other_type = (*ti).second;

    if (other_type.has_name()) {
      map<string, TypeIndex>::iterator ni;
      ni = types_by_name.find(other_type.get_true_name());
      if (ni != types_by_name.end()) {
        // Here's a type that we seem to have in common!  We'll have
        // to merge them.
        TypeIndex this_type_index = (*ni).second;
        remap.add_mapping(other_type_index, this_type_index);
      }
    }
  }

  // Now that we know the full type-to-type mapping, we can copy the
  // new types, one at a time.
  for (ti = other._type_map.begin(); ti != other._type_map.end(); ++ti) {
    TypeIndex other_type_index = (*ti).first;
    const InterrogateType &other_type = (*ti).second;

    if (!remap.in_map(other_type_index)) {
      // Here's a new type.
      add_type(other_type_index, other_type);
      update_type(other_type_index).remap_indices(remap);

    } else {
      // Here's a type to merge.
      TypeIndex this_type_index = remap.map_from(other_type_index);

      InterrogateType &this_type = update_type(this_type_index);
      if (!this_type.is_global() && other_type.is_global()) {
        // If the type is about to become global, we need to add it
        // to our global_types list.
        _global_types.push_back(this_type_index);
      }

      InterrogateType merge_type = other_type;
      merge_type.remap_indices(remap);
      this_type.merge_with(merge_type);
    }
  }

  // And copy all of the functions, wrappers, manifests, and elements.
  FunctionMap::const_iterator fi;
  for (fi = other._function_map.begin();
       fi != other._function_map.end();
       ++fi) {
    FunctionIndex other_function_index = (*fi).first;
    InterrogateFunction *other_function = (*fi).second;
    add_function(other_function_index, other_function);
    update_function(other_function_index).remap_indices(remap);
  }

  FunctionWrapperMap::const_iterator wi;
  for (wi = other._wrapper_map.begin();
       wi != other._wrapper_map.end();
       ++wi) {
    FunctionWrapperIndex other_wrapper_index = (*wi).first;
    const InterrogateFunctionWrapper &other_wrapper = (*wi).second;
    add_wrapper(other_wrapper_index, other_wrapper);
    update_wrapper(other_wrapper_index).remap_indices(remap);
  }

  ManifestMap::const_iterator mi;
  for (mi = other._manifest_map.begin();
       mi != other._manifest_map.end();
       ++mi) {
    ManifestIndex other_manifest_index = (*mi).first;
    const InterrogateManifest &other_manifest = (*mi).second;
    add_manifest(other_manifest_index, other_manifest);
    update_manifest(other_manifest_index).remap_indices(remap);
  }

  ElementMap::const_iterator ei;
  for (ei = other._element_map.begin();
       ei != other._element_map.end();
       ++ei) {
    ElementIndex other_element_index = (*ei).first;
    const InterrogateElement &other_element = (*ei).second;
    add_element(other_element_index, other_element);
    update_element(other_element_index).remap_indices(remap);
  }

  _lookups_fresh = 0;
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::find_module
//       Access: Private
//  Description: Looks up the wrapper definition in the set of module
//               defs that are loaded in at runtime and represent the
//               part of the interrogate database that's compiled in.
//
//               If the wrapper definition is not found, returns
//               false.  If it is found, returns true and sets def and
//               module_index to the particular module and the index
//               within the module where the wrapper is defined.
////////////////////////////////////////////////////////////////////
bool InterrogateDatabase::
find_module(FunctionWrapperIndex wrapper, InterrogateModuleDef *&def,
            int &module_index) {
  if (_modules.empty()) {
    return false;
  }

  int mi = binary_search_module(0, _modules.size(), wrapper);
  assert(mi >= 0 && mi < (int)_modules.size());
  def = _modules[mi];
  module_index = wrapper - def->first_index;

  return (wrapper < def->next_index);
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::binary_search_module
//       Access: Private
//  Description: Searches for the function module that includes the
//               given function index by binary search.
////////////////////////////////////////////////////////////////////
int InterrogateDatabase::
binary_search_module(int begin, int end, FunctionIndex function) {
  int mid = begin + (end - begin) / 2;
  if (mid == begin) {
    return mid;
  }

  int index = _modules[mid]->first_index;
  if (index <= function) {
    return binary_search_module(mid, end, function);

  } else {
    return binary_search_module(begin, mid, function);
  }
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::binary_search_wrapper_hash
//       Access: Private
//  Description: Searches for the particular function wrapper's hash
//               name within a given module.  Returns the index number
//               local to the module, or -1 if it is not found.
////////////////////////////////////////////////////////////////////
int InterrogateDatabase::
binary_search_wrapper_hash(InterrogateUniqueNameDef *begin,
                           InterrogateUniqueNameDef *end,
                           const string &wrapper_hash_name) {
  if (end <= begin) {
    return -1;
  }

  InterrogateUniqueNameDef *mid = begin + (end - begin) / 2;
  string name = mid->name;
  if (name < wrapper_hash_name) {
    return binary_search_wrapper_hash(mid, end, wrapper_hash_name);

  } else if (wrapper_hash_name < name) {
    return binary_search_wrapper_hash(begin, mid, wrapper_hash_name);

  } else {
    return mid->index_offset;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::freshen_types_by_name
//       Access: Private
//  Description: Builds up the lookup of types by name.
////////////////////////////////////////////////////////////////////
void InterrogateDatabase::
freshen_types_by_name() {
  _types_by_name.clear();
  TypeMap::const_iterator ti;
  for (ti = _type_map.begin(); ti != _type_map.end(); ++ti) {
    _types_by_name[(*ti).second.get_name()] = (*ti).first;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::freshen_types_by_scoped_name
//       Access: Private
//  Description: Builds up the lookup of types by scoped name.
////////////////////////////////////////////////////////////////////
void InterrogateDatabase::
freshen_types_by_scoped_name() {
  _types_by_scoped_name.clear();
  TypeMap::const_iterator ti;
  for (ti = _type_map.begin(); ti != _type_map.end(); ++ti) {
    _types_by_scoped_name[(*ti).second.get_scoped_name()] = (*ti).first;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::freshen_types_by_true_name
//       Access: Private
//  Description: Builds up the lookup of types by true name.
////////////////////////////////////////////////////////////////////
void InterrogateDatabase::
freshen_types_by_true_name() {
  _types_by_true_name.clear();
  TypeMap::const_iterator ti;
  for (ti = _type_map.begin(); ti != _type_map.end(); ++ti) {
    _types_by_true_name[(*ti).second.get_true_name()] = (*ti).first;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::freshen_manifests_by_name
//       Access: Private
//  Description: Builds up the lookup of manifests by name.
////////////////////////////////////////////////////////////////////
void InterrogateDatabase::
freshen_manifests_by_name() {
  _manifests_by_name.clear();
  ManifestMap::const_iterator ti;
  for (ti = _manifest_map.begin(); ti != _manifest_map.end(); ++ti) {
    _manifests_by_name[(*ti).second.get_name()] = (*ti).first;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::freshen_elements_by_name
//       Access: Private
//  Description: Builds up the lookup of elements by name.
////////////////////////////////////////////////////////////////////
void InterrogateDatabase::
freshen_elements_by_name() {
  _elements_by_name.clear();
  ElementMap::const_iterator ti;
  for (ti = _element_map.begin(); ti != _element_map.end(); ++ti) {
    _elements_by_name[(*ti).second.get_name()] = (*ti).first;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::freshen_elements_by_scoped_name
//       Access: Private
//  Description: Builds up the lookup of elements by scoped name.
////////////////////////////////////////////////////////////////////
void InterrogateDatabase::
freshen_elements_by_scoped_name() {
  _elements_by_scoped_name.clear();
  ElementMap::const_iterator ti;
  for (ti = _element_map.begin(); ti != _element_map.end(); ++ti) {
    _elements_by_scoped_name[(*ti).second.get_scoped_name()] = (*ti).first;
  }
}

////////////////////////////////////////////////////////////////////
//     Function: InterrogateDatabase::lookup
//       Access: Private
//  Description: Looks up a type, manifest, or element in the
//               indicated lookup table by name.  This is an internal
//               support function.
////////////////////////////////////////////////////////////////////
int InterrogateDatabase::
lookup(const string &name, Lookup &lookup, LookupType type,
       void (InterrogateDatabase::*freshen)()) {
  if ((_lookups_fresh & (int)type) == 0) {
    // The lookup table isn't fresh; we need to freshen it.
    (this->*freshen)();
    _lookups_fresh |= (int)type;
  }

  Lookup::const_iterator li;
  li = lookup.find(name);
  if (li != lookup.end()) {
    return (*li).second;
  }
  return 0;
}

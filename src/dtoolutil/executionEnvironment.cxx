// Filename: executionEnvironment.cxx
// Created by:  drose (15May00)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) 2001, Disney Enterprises, Inc.  All rights reserved
//
// All use of this software is subject to the terms of the Panda 3d
// Software license.  You should have received a copy of this license
// along with this source code; you will also find a current copy of
// the license at http://www.panda3d.org/license.txt .
//
// To contact the maintainers of this program write to
// panda3d@yahoogroups.com .
//
////////////////////////////////////////////////////////////////////

#include "executionEnvironment.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>  // for perror

#ifdef WIN32_VC
// Windows requires this for getcwd().
#include <direct.h>
#define getcwd _getcwd
#endif


// We define the symbol PREREAD_ENVIRONMENT if we cannot rely on
// getenv() to read environment variables at static init time.  In
// this case, we must read all of the environment variables directly
// and cache them locally.

#ifndef STATIC_INIT_GETENV
#define PREREAD_ENVIRONMENT
#endif


// We define the symbol HAVE_GLOBAL_ARGV if we have global variables
// named GLOBAL_ARGC/GLOBAL_ARGV that we can read at static init time
// to determine our command-line arguments.

#if defined(HAVE_GLOBAL_ARGV) && defined(PROTOTYPE_GLOBAL_ARGV)
extern char **GLOBAL_ARGV;
extern int GLOBAL_ARGC;
#endif

// Linux with GNU libc does have global argv/argc variables, but we
// can't safely access them at stat init time--at least, not in libc5.
// (It does seem to work with glibc2, however.)

ExecutionEnvironment *ExecutionEnvironment::_global_ptr = NULL;


////////////////////////////////////////////////////////////////////
//     Function: ExecutionEnvironment::Constructor
//       Access: Private
//  Description: You shouldn't need to construct one of these; there's
//               only one and it constructs itself.
////////////////////////////////////////////////////////////////////
ExecutionEnvironment::
ExecutionEnvironment() {
  read_environment_variables();
  read_args();
}

////////////////////////////////////////////////////////////////////
//     Function: ExecutionEnviroment::expand_string
//       Access: Public, Static
//  Description: Reads the string, looking for environment variable
//               names marked by a $.  Expands all such variable
//               names.  A repeated dollar sign ($$) is mapped to a
//               single dollar sign.
//
//               Returns the expanded string.
////////////////////////////////////////////////////////////////////
string ExecutionEnvironment::
expand_string(const string &str) {
  string result;

  size_t last = 0;
  size_t dollar = str.find('$');
  while (dollar != string::npos && dollar + 1 < str.length()) {
    size_t start = dollar + 1;

    if (str[start] == '$') {
      // A double dollar sign maps to a single dollar sign.
      result += str.substr(last, start - last);
      last = start + 1;

    } else {
      string varname;
      size_t end = start;

      if (str[start] == '{') {
        // Curly braces delimit the variable name explicitly.
        end = str.find('}', start + 1);
        if (end != string::npos) {
          varname = str.substr(start + 1, end - (start + 1));
          end++;
        }
      }

      if (end == start) {
        // Scan for the end of the variable name.
        while (end < str.length() && (isalnum(str[end]) || str[end] == '_')) {
          end++;
        }
        varname = str.substr(start, end - start);
      }

      string subst =
      result += str.substr(last, dollar - last);
      result += get_environment_variable(varname);
      last = end;
    }

    dollar = str.find('$', last);
  }

  result += str.substr(last);

  return result;
}

////////////////////////////////////////////////////////////////////
//     Function: ExecutionEnviroment::get_cwd
//       Access: Public, Static
//  Description: Returns the name of the current working directory.
////////////////////////////////////////////////////////////////////
Filename ExecutionEnvironment::
get_cwd() {
  // getcwd() requires us to allocate a dynamic buffer and grow it on
  // demand.
  static size_t bufsize = 1024;
  static char *buffer = NULL;

  if (buffer == (char *)NULL) {
    buffer = new char[bufsize];
  }

  while (getcwd(buffer, bufsize) == (char *)NULL) {
    if (errno != ERANGE) {
      perror("getcwd");
      return string();
    }
    delete[] buffer;
    bufsize = bufsize * 2;
    buffer = new char[bufsize];
    assert(buffer != (char *)NULL);
  }

  return Filename::from_os_specific(buffer);
}

////////////////////////////////////////////////////////////////////
//     Function: ExecutionEnvironment::ns_has_environment_variable
//       Access: Private
//  Description: Returns true if the indicated environment variable
//               is defined.  The nonstatic implementation.
////////////////////////////////////////////////////////////////////
bool ExecutionEnvironment::
ns_has_environment_variable(const string &var) const {
#ifdef PREREAD_ENVIRONMENT
  return _variables.count(var) != 0;
#else
  return getenv(var.c_str()) != (char *)NULL;
#endif
}

////////////////////////////////////////////////////////////////////
//     Function: ExecutionEnvironment::ns_get_environment_variable
//       Access: Private
//  Description: Returns the definition of the indicated environment
//               variable, or the empty string if the variable is
//               undefined.  The nonstatic implementation.
////////////////////////////////////////////////////////////////////
string ExecutionEnvironment::
ns_get_environment_variable(const string &var) const {
#ifdef PREREAD_ENVIRONMENT
  EnvironmentVariables::const_iterator evi;
  evi = _variables.find(var);
  if (evi != _variables.end()) {
    return (*evi).second;
  }
  return string();
#else
  const char *def = getenv(var.c_str());
  if (def != (char *)NULL) {
    return def;
  }
  return string();
#endif
}

////////////////////////////////////////////////////////////////////
//     Function: ExecutionEnvironment::ns_get_num_args
//       Access: Private
//  Description: Returns the number of command-line arguments
//               available, not counting arg 0, the binary name.  The
//               nonstatic implementation.
////////////////////////////////////////////////////////////////////
int ExecutionEnvironment::
ns_get_num_args() const {
  return _args.size();
}

////////////////////////////////////////////////////////////////////
//     Function: ExecutionEnvironment::ns_get_arg
//       Access: Private
//  Description: Returns the nth command-line argument.  The index n
//               must be in the range [0 .. get_num_args()).  The
//               first parameter, n == 0, is the first actual
//               parameter, not the binary name.  The nonstatic
//               implementation.
////////////////////////////////////////////////////////////////////
string ExecutionEnvironment::
ns_get_arg(int n) const {
  assert(n >= 0 && n < ns_get_num_args());
  return _args[n];
}

////////////////////////////////////////////////////////////////////
//     Function: ExecutionEnvironment::ns_get_binary_name
//       Access: Private
//  Description: Returns the name of the binary executable that
//               started this program, if it can be determined.  The
//               nonstatic implementation.
////////////////////////////////////////////////////////////////////
string ExecutionEnvironment::
ns_get_binary_name() const {
  if (_binary_name.empty()) {
    return "unknown";
  }
  return _binary_name;
}

////////////////////////////////////////////////////////////////////
//     Function: ExecutionEnvironment::get_ptr
//       Access: Private, Static
//  Description: Returns a static pointer that may be used to access
//               the global ExecutionEnvironment object.
////////////////////////////////////////////////////////////////////
ExecutionEnvironment *ExecutionEnvironment::
get_ptr() {
  if (_global_ptr == (ExecutionEnvironment *)NULL) {
    _global_ptr = new ExecutionEnvironment;
  }
  return _global_ptr;
}


////////////////////////////////////////////////////////////////////
//     Function: ExecutionEnvironment::read_environment_variables
//       Access: Private
//  Description: Fills up the internal table of existing environment
//               variables, if we are in PREREAD_ENVIRONMENT mode.
//               Otherwise, does nothing.
////////////////////////////////////////////////////////////////////
void ExecutionEnvironment::
read_environment_variables() {
#ifdef PREREAD_ENVIRONMENT
#if defined(HAVE_PROC_SELF_ENVIRON)
  // In Linux, and possibly in other systems, we might not be able to
  // use getenv() at static init time.  However, we may be lucky and
  // have a file called /proc/self/environ that may be read to
  // determine all of our environment variables.

  ifstream proc("/proc/self/environ");
  if (proc.fail()) {
    cerr << "Cannot read /proc/self/environ; environment variables unavailable.\n";
    return;
  }

  int ch = proc.get();
  while (!proc.eof() && !proc.fail()) {
    string variable;
    string value;

    while (!proc.eof() && !proc.fail() && ch != '=' && ch != '\0') {
      variable += (char)ch;
      ch = proc.get();
    }

    if (ch == '=') {
      ch = proc.get();
      while (!proc.eof() && !proc.fail() && ch != '\0') {
        value += (char)ch;
        ch = proc.get();
      }
    }

    if (!variable.empty()) {
      _variables[variable] = value;
    }
    ch = proc.get();
  }
#else
  cerr << "Warning: environment variables unavailable to dconfig.\n";
#endif
#endif // PREREAD_ENVIRONMENT
}

////////////////////////////////////////////////////////////////////
//     Function: ExecutionEnvironment::read_args
//       Access: Private
//  Description: Reads all the command-line arguments and the name of
//               the binary file, if possible.
////////////////////////////////////////////////////////////////////
void ExecutionEnvironment::
read_args() {
#if defined(HAVE_GLOBAL_ARGV)
  int argc = GLOBAL_ARGC;
  if (argc > 0) {
    _binary_name = GLOBAL_ARGV[0];
  }

  for (int i = 1; i < argc; i++) {
    _args.push_back(GLOBAL_ARGV[i]);
  }

#elif defined(HAVE_PROC_SELF_CMDLINE)
  // In Linux, and possibly in other systems as well, we might not be
  // able to use the global ARGC/ARGV variables at static init time.
  // However, we may be lucky and have a file called
  // /proc/self/cmdline that may be read to determine all of our
  // command-line arguments.

  ifstream proc("/proc/self/cmdline");
  if (proc.fail()) {
    cerr << "Cannot read /proc/self/cmdline; command-line arguments unavailable to config.\n";
    return;
  }

  int ch = proc.get();
  int index = 0;
  while (!proc.eof() && !proc.fail()) {
    string arg;

    while (!proc.eof() && !proc.fail() && ch != '\0') {
      arg += (char)ch;
      ch = proc.get();
    }

    if (index == 0) {
      _binary_name = arg;
    } else {
      _args.push_back(arg);
    }
    index++;

    ch = proc.get();
  }
#else
  cerr << "Warning: command line parameters unavailable to dconfig.\n";
#endif
}

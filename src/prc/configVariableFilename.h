// Filename: configVariableFilename.h
// Created by:  drose (22Nov04)
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

#ifndef CONFIGVARIABLEFILENAME_H
#define CONFIGVARIABLEFILENAME_H

#include "dtoolbase.h"
#include "configVariable.h"
#include "filename.h"
////////////////////////////////////////////////////////////////////
//       Class : ConfigVariableFilename
// Description : This is a convenience class to specialize
//               ConfigVariable as a Filename type.  It is almost the
//               same thing as ConfigVariableString, except it handles
//               an implicit Filename::expand_from() operation so that
//               the user may put OS-specific filenames, or filenames
//               based on environment variables, in the prc file.
////////////////////////////////////////////////////////////////////
class EXPCL_DTOOLCONFIG ConfigVariableFilename : public ConfigVariable {
PUBLISHED:
  INLINE ConfigVariableFilename(const string &name);
  INLINE ConfigVariableFilename(const string &name, const Filename &default_value,
                                const string &description = string(), int flags = 0);

  INLINE void operator = (const Filename &value);
  INLINE operator const Filename &() const;

  // These methods help the ConfigVariableFilename act like a Filename
  // object.
  INLINE const char *c_str() const;
  INLINE bool empty() const;
  INLINE size_t length() const;
  INLINE char operator [] (int n) const;

  INLINE string get_fullpath() const;
  INLINE string get_dirname() const;
  INLINE string get_basename() const;
  INLINE string get_fullpath_wo_extension() const;
  INLINE string get_basename_wo_extension() const;
  INLINE string get_extension() const;

  // Comparison operators are handy.
  INLINE bool operator == (const Filename &other) const;
  INLINE bool operator != (const Filename &other) const;
  INLINE bool operator < (const Filename &other) const;

  INLINE void set_value(const Filename &value);
  INLINE const Filename &get_value() const;
  INLINE Filename get_default_value() const;

  INLINE Filename get_word(int n) const;
  INLINE void set_word(int n, const Filename &value);

private:
  void reload_cache();

private:
  PN_int32 _local_modified;
  Filename _cache;
};

#include "configVariableFilename.I"

#endif

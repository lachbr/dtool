// Filename: cppParser.h
// Created by:  drose (19Oct99)
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

#ifndef CPPPARSER_H
#define CPPPARSER_H

#include <dtoolbase.h>

#include "cppScope.h"
#include "cppPreprocessor.h"

#include <set>

///////////////////////////////////////////////////////////////////
//       Class : CPPParser
// Description :
////////////////////////////////////////////////////////////////////
class CPPParser : public CPPScope, public CPPPreprocessor {
public:
  CPPParser();

  virtual bool is_fully_specified() const;

  bool parse_file(const string &filename);

  CPPExpression *parse_expr(const string &expr);
  CPPType *parse_type(const string &type);
};

// Normally, this variable should be left true, especially while
// parsing.  However, after parsing has finished, and you want to
// output the results of parsing in a way that can be successfully
// compiled by VC++, you may need to set this variable to false.  It
// controls the way typenames are written.  When true, class names are
// written 'class X', which is the way the parser expects things to
// come, and which compiles successfully under every compiler except
// VC++.  When false, class names are written simply 'X', which is the
// only way they'll compile under VC++.
extern bool cppparser_output_class_keyword;

#endif



// Filename: parameterRemapCharStarToString.cxx
// Created by:  drose (09Aug00)
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

#include "parameterRemapCharStarToString.h"

////////////////////////////////////////////////////////////////////
//     Function: ParameterRemapCharStarToString::Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
ParameterRemapCharStarToString::
ParameterRemapCharStarToString(CPPType *orig_type) :
  ParameterRemapToString(orig_type)
{
}

// Filename: atomicAdjustNsprImpl.h
// Created by:  drose (09Aug02)
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

#ifndef ATOMICADJUSTNSPRIMPL_H
#define ATOMICADJUSTNSPRIMPL_H

#include "dtoolbase.h"
#include "selectThreadImpl.h"

#ifdef THREAD_NSPR_IMPL

#include "numeric_types.h"

#include <pratom.h>

////////////////////////////////////////////////////////////////////
//       Class : AtomicAdjustNsprImpl
// Description : Uses NSPR to implement atomic adjustments.
////////////////////////////////////////////////////////////////////
class EXPCL_DTOOL AtomicAdjustNsprImpl {
public:
  INLINE static void inc(PN_int32 &var);
  INLINE static bool dec(PN_int32 &var);
  INLINE static PN_int32 set(PN_int32 &var, PN_int32 new_value);
  INLINE static PN_int32 get(const PN_int32 &var);
};

#include "atomicAdjustNsprImpl.I"

#endif  // THREAD_NSPR_IMPL

#endif
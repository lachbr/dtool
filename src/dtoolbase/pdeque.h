// Filename: pdeque.h
// Created by:  drose (05Jun01)
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

#ifndef PDEQUE_H
#define PDEQUE_H

#include "dtoolbase.h"
#include "pallocator.h"
#include <deque>

#ifndef USE_STL_ALLOCATOR
// If we're not using custom allocators, just use the standard class
// definition.
#define pdeque deque 

#else

////////////////////////////////////////////////////////////////////
//       Class : pdeque
// Description : This is our own Panda specialization on the default
//               STL deque.  Its main purpose is to call the hooks
//               for MemoryUsage to properly track STL-allocated
//               memory.
////////////////////////////////////////////////////////////////////
template<class Type>
class pdeque : public deque<Type, pallocator_array<Type> > {
public:
  typedef TYPENAME deque<Type, pallocator_array<Type> >::size_type size_type;
  pdeque() : deque<Type, pallocator_array<Type> >() { }
  pdeque(const pdeque<Type> &copy) : deque<Type, pallocator_array<Type> >(copy) { }
  pdeque(size_type n) : deque<Type, pallocator_array<Type> >(n) { }
  pdeque(size_type n, const Type &value) : deque<Type, pallocator_array<Type> >(n, value) { }
};

#endif  // USE_STL_ALLOCATOR
#endif

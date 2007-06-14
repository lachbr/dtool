// Filename: deletedChain.h
// Created by:  drose (01Apr06)
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

#ifndef DELETEDCHAIN_H
#define DELETEDCHAIN_H

#include "dtoolbase.h"
#include "neverFreeMemory.h"
#include "mutexImpl.h"
#include "atomicAdjust.h"
#include "numeric_types.h"
#include "register_type.h"
#include "typeHandle.h"
#include <assert.h>

#ifdef HAVE_ATOMIC_COMPARE_AND_EXCHANGE_PTR
// Actually, there appears to be a (maybe fatal) flaw in our
// implementation of DeletedChain via the atomic exchange operation.
// Specifically, a pointer may be removed from the head of the chain,
// then the same pointer reinserted in the chain, while another thread
// is waiting; and that thread will not detect the change.  For now,
// then, let's not use this implementation, and fall back to the mutex.
//#define DELETED_CHAIN_USE_ATOMIC_EXCHANGE
#endif

#ifndef NDEBUG
// In development mode, we define USE_DELETEDCHAINFLAG, which
// triggers the piggyback of an additional word of data on every
// allocated block, so we can ensure that an object is not
// double-deleted and that the deleted chain remains intact.
#define USE_DELETEDCHAINFLAG 1
#endif // NDEBUG

#ifdef USE_DELETEDCHAINFLAG
enum DeletedChainFlag {
  DCF_deleted = 0xfeedba0f,
  DCF_alive = 0x12487654,
};
#endif

////////////////////////////////////////////////////////////////////
//       Class : DeletedChain
// Description : This template class can be used to provide faster
//               allocation/deallocation for many Panda objects.  It
//               works by maintaining a linked list of deleted objects
//               that are all of the same type; when a new object is
//               allocated that matches that type, the same space is
//               just reused.
//
//               This is particularly important to do in the case of a
//               multithreaded pipeline, so we minimize contention
//               between the parallel threads.  If we didn't do this,
//               the threads might compete overmuch on the system
//               mutex protecting the malloc() call.  This way, there
//               can be a different mutex for each type of object; or on
//               some systems (e.g. i386), no mutex at all.
//
//               Of course, this trick of maintaining the deleted
//               object chain won't work in the presence of
//               polymorphism, where you might have many classes that
//               derive from a base class, and all of them have a
//               different size--unless you instantiate a DeletedChain
//               for *every* kind of derived class.  The
//               ALLOC_DELETED_CHAIN macro, below, is designed to make
//               this easy.
////////////////////////////////////////////////////////////////////
template<class Type>
class DeletedChain {
public:
  INLINE Type *allocate(size_t size, TypeHandle type_handle);
  INLINE void deallocate(Type *ptr, TypeHandle type_handle);

  INLINE bool validate(const Type *ptr);

  static INLINE ReferenceCount *make_ref_ptr(void *ptr);
  static INLINE ReferenceCount *make_ref_ptr(ReferenceCount *ptr);

private:
  class ObjectNode {
  public:
#ifdef USE_DELETEDCHAINFLAG
    // In development mode, we piggyback this extra data.  This is
    // maintained out-of-band from the actual pointer returned, so we
    // can safely use this flag to indicate the difference between
    // allocated and freed pointers.
    TVOLATILE PN_int32 _flag;
#endif

    // This pointer sits in the same space referenced by the actual
    // pointer returned (unlike _flag, above).  It's only used when
    // the object is deleted, so there's no harm in sharing space with
    // the undeleted object.
    TVOLATILE ObjectNode * TVOLATILE _next;
  };

  static INLINE Type *node_to_type(TVOLATILE ObjectNode *node);
  static INLINE ObjectNode *type_to_node(Type *ptr);

  TVOLATILE ObjectNode * TVOLATILE _deleted_chain;
  
#ifndef DELETED_CHAIN_USE_ATOMIC_EXCHANGE
  // If we don't have atomic compare-and-exchange, we need to use a
  // Mutex to protect the above linked list.
  INLINE void init_lock();
  void do_init_lock(MutexImpl *&lock);
  MutexImpl *_lock;
#endif
};

////////////////////////////////////////////////////////////////////
//       Class : StaticDeletedChain
// Description : This template class is used to conveniently
//               declare a single instance of the DeletedChain
//               template object, above, for a particular type.
//
//               It relies on the fact that the compiler and linker
//               should unify all references to this static pointer
//               for a given type, as per the C++ spec. However, this
//               sometimes fails; and if the compiler fails to do
//               this, it mostly won't be a big deal; it just means
//               there will be multiple unrelated chains of deleted
//               objects for a particular type.  This is only a
//               problem if the code structure causes objects to be
//               allocated from one chain and freed to another, which
//               can lead to leaks.
////////////////////////////////////////////////////////////////////
template<class Type>
class StaticDeletedChain {
public:
  INLINE static Type *allocate(size_t size, TypeHandle type_handle);
  INLINE static void deallocate(Type *ptr, TypeHandle type_handle);

  INLINE static bool validate(const Type *ptr);

  static DeletedChain<Type> _chain;
};

// Place this macro within a class definition to define appropriate
// operator new and delete methods that take advantage of
// DeletedChain.
#define ALLOC_DELETED_CHAIN(Type)                            \
  inline void *operator new(size_t size) {                   \
    return (void *)StaticDeletedChain< Type >::allocate(size, get_type_handle(Type)); \
  }                                                          \
  inline void *operator new(size_t size, void *ptr) {        \
    return ptr;                                              \
  }                                                          \
  inline void operator delete(void *ptr) {                   \
    StaticDeletedChain< Type >::deallocate((Type *)ptr, get_type_handle(Type)); \
  }                                                          \
  inline void operator delete(void *, void *) {              \
  }                                                          \
  inline static bool validate_ptr(const void *ptr) {         \
    return StaticDeletedChain< Type >::validate((const Type *)ptr); \
  }

// Use this variant of the above macro in cases in which the compiler
// fails to unify the static template pointers properly, to prevent
// leaks.
#define ALLOC_DELETED_CHAIN_DECL(Type)                       \
  inline void *operator new(size_t size) {                   \
    return (void *)_deleted_chain.allocate(size, get_type_handle(Type)); \
  }                                                          \
  inline void *operator new(size_t size, void *ptr) {        \
    return ptr;                                              \
  }                                                          \
  inline void operator delete(void *ptr) {                   \
    _deleted_chain.deallocate((Type *)ptr, get_type_handle(Type)); \
  }                                                          \
  inline void operator delete(void *, void *) {              \
  }                                                          \
  inline static bool validate_ptr(const void *ptr) {         \
    return _deleted_chain.validate((const Type *)ptr);       \
  }                                                          \
  static DeletedChain< Type > _deleted_chain;

// When you use ALLOC_DELETED_CHAIN_DECL in a class body, you must
// also put this line in the .cxx file defining that class body.
#define ALLOC_DELETED_CHAIN_DEF(Type)                        \
  DeletedChain< Type > Type::_deleted_chain;

#include "deletedChain.T"

#endif


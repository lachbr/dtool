// Filename: cppPointerType.h
// Created by:  drose (19Oct99)
//
////////////////////////////////////////////////////////////////////
// Copyright (C) 1992,93,94,95,96,97  Walt Disney Imagineering, Inc.
//
// These  coded  instructions,  statements,  data   structures   and
// computer  programs contain unpublished proprietary information of
// Walt Disney Imagineering and are protected by  Federal  copyright
// law.  They may  not be  disclosed to third  parties  or copied or
// duplicated in any form, in whole or in part,  without  the  prior
// written consent of Walt Disney Imagineering Inc.
////////////////////////////////////////////////////////////////////

#ifndef CPPPOINTERTYPE_H
#define CPPPOINTERTYPE_H

#include <dtoolbase.h>

#include "cppType.h"

///////////////////////////////////////////////////////////////////
// 	 Class : CPPPointerType
// Description :
////////////////////////////////////////////////////////////////////
class CPPPointerType : public CPPType {
public:
  CPPPointerType(CPPType *pointing_at);

  CPPType *_pointing_at;

  virtual bool is_fully_specified() const;
  virtual CPPDeclaration *substitute_decl(SubstDecl &subst,
					  CPPScope *current_scope,
					  CPPScope *global_scope);

  virtual CPPType *resolve_type(CPPScope *current_scope,
				CPPScope *global_scope);

  virtual bool is_tbd() const;
  virtual bool is_equivalent(const CPPType &other) const;

  virtual void output(ostream &out, int indent_level, CPPScope *scope,
		      bool complete) const;
  virtual void output_instance(ostream &out, int indent_level,
			       CPPScope *scope,
			       bool complete, const string &prename,
			       const string &name) const;

  virtual SubType get_subtype() const;

  virtual CPPPointerType *as_pointer_type();

protected:
  virtual bool is_equal(const CPPDeclaration *other) const;
  virtual bool is_less(const CPPDeclaration *other) const;
};

#endif

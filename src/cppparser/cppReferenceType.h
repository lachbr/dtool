// Filename: cppReferenceType.h
// Created by:  drose (19Oct99)
//
////////////////////////////////////////////////////////////////////

#ifndef CPPREFERENCETYPE_H
#define CPPREFERENCETYPE_H

#include <dtoolbase.h>

#include "cppType.h"

///////////////////////////////////////////////////////////////////
//       Class : CPPReferenceType
// Description :
////////////////////////////////////////////////////////////////////
class CPPReferenceType : public CPPType {
public:
  CPPReferenceType(CPPType *pointing_at);

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

  virtual CPPReferenceType *as_reference_type();

protected:
  virtual bool is_equal(const CPPDeclaration *other) const;
  virtual bool is_less(const CPPDeclaration *other) const;
};

#endif

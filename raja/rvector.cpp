// Copyright (c) 2010, Lawrence Livermore National Security, LLC. Produced at
// the Lawrence Livermore National Laboratory. LLNL-CODE-443211. All Rights
// reserved. See file COPYRIGHT for details.
//
// This file is part of the MFEM library. For more information and source code
// availability see http://mfem.org.
//
// MFEM is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License (as published by the Free
// Software Foundation) version 2.1 dated February 1999.
#include "raja.hpp"

namespace mfem {

// ***************************************************************************
static double* rmalloc(const size_t sz) {
  return (double*)::malloc(sz*sizeof(double));
}

// ***************************************************************************
void RajaVector::SetSize(const size_t sz, const void* ptr) {
  size = sz;
  if (!data) { data = rmalloc(size); }
  if (ptr) { ::memcpy(data,ptr,bytes()); }
}

// ***************************************************************************
RajaVector::RajaVector(const size_t sz):size(sz),data(rmalloc(sz)) {}

RajaVector::RajaVector(const RajaVector& v):
  size(0),data(NULL) { SetSize(v.Size(), v); }

RajaVector::RajaVector(const RajaVectorRef& ref):
  size(ref.v.size),data(ref.v.data) { }

RajaVector::RajaVector(const Vector& v):
  size(0),data(NULL) { SetSize(v.Size(), v.GetData()); }

RajaVector::RajaVector(RajaArray<double>& v):
  size(0),data(NULL) { SetSize(v.size(),v.ptr()); }

// ***************************************************************************
RajaVector::operator Vector() { return Vector(data,size); }
RajaVector::operator Vector() const { return Vector(data,size); }

// ***************************************************************************
void RajaVector::Print(const char *title, std::ostream& out, int width) const {
  printf("\n\033[32m[%s]\033",title);
  for (size_t i=0; i<size; i+=1) {
    printf("\n\t\033[32m[%ld]\033[m %.16e",i,data[i]);
  }
  //Vector(data,size).Print(out, width);
}

// ***************************************************************************
RajaVectorRef RajaVector::GetRange(const size_t offset,
                                   const size_t entries) const {
  RajaVectorRef ret;
  RajaVector& v = ret.v;
  v.data = (double*) ((unsigned char*)data + (offset*sizeof(double)));
  v.size = entries;
  return ret;
}

// ***************************************************************************
RajaVector& RajaVector::operator=(const RajaVector& v) {
  SetSize(v.Size(),v.data);
  return *this;
}

// ***************************************************************************
RajaVector& RajaVector::operator=(double value) {
  vector_op_eq(size, value, data);
  return *this;
}

// ***************************************************************************
double RajaVector::operator*(const RajaVector& v) const {
  return vector_dot(size, data, v.data);
}

// *****************************************************************************
RajaVector& RajaVector::operator-=(const RajaVector& v) {
  vector_vec_sub(size, data, v.data);
  return *this;
}

// ***************************************************************************
RajaVector& RajaVector::operator+=(const RajaVector& v) {
  vector_vec_add(size, data, v);
  return *this;
}

// ***************************************************************************
RajaVector& RajaVector::operator*=(const double d) {
  for (size_t i=0; i<size; i+=1)
    data[i]*=d;
  return *this;
}

// ***************************************************************************
RajaVector& RajaVector::Add(const double a, const RajaVector& Va) {
  vector_axpy(Size(),a, data, Va);
  return *this;
}


// ***************************************************************************
void RajaVector::Neg() {
  vector_neg(Size(),ptr());
}

// *****************************************************************************
void RajaVector::SetSubVector(const void* dofs,
                              const double value,
                              const int entries) {
  vector_set_subvector_const(entries, value,ptr(),(const int*)dofs);
}


// ***************************************************************************
double RajaVector::Min() const {
  return vector_min(Size(),(double*)data);
}

// ***************************************************************************
// from mfem::TCGSolver<mfem::RajaVector>::Mult in linalg/solvers.hpp:224
void add(const RajaVector& v1, const double alpha,
         const RajaVector& v2, RajaVector& out) {
  vector_xpay(out.Size(),alpha,out.ptr(),v1.ptr(),v2.ptr());
}

// *****************************************************************************
void add(const double alpha,
         const RajaVector& v1,
         const double beta,
         const RajaVector& v2,
         RajaVector& out) {assert(false);}

// ***************************************************************************
void subtract(const RajaVector& v1,
              const RajaVector& v2,
              RajaVector& out) {
  vector_xsy(out.Size(),out.ptr(),v1.ptr(),v2.ptr());
}

} // mfem

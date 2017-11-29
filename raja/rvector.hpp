// Copyright (c) 2017, Lawrence Livermore National Security, LLC. Produced at
// the Lawrence Livermore National Laboratory. LLNL-CODE-734707. All Rights
// reserved. See files LICENSE and NOTICE for details.
//
// This file is part of CEED, a collection of benchmarks, miniapps, software
// libraries and APIs for efficient high-order finite element and spectral
// element discretizations for exascale applications. For more information and
// source code availability see http://github.com/ceed.
//
// The CEED research is supported by the Exascale Computing Project 17-SC-20-SC,
// a collaborative effort of two U.S. Department of Energy organizations (Office
// of Science and the National Nuclear Security Administration) responsible for
// the planning and preparation of a capable exascale ecosystem, including
// software, applications, hardware, advanced system engineering and early
// testbed platforms, in support of the nation's exascale computing imperative.
#ifndef MFEM_RAJA_VECTOR
#define MFEM_RAJA_VECTOR

namespace mfem {

struct RajaVectorRef;

class RajaVector {
 private:
  size_t size;
  double* data;
 public:
  RajaVector(): size(0),data(NULL) {}
  RajaVector(const RajaVector&);
  RajaVector(const RajaVectorRef&);
  RajaVector(const size_t);
  RajaVector(const Vector& v);
  RajaVector(RajaArray<double>& v);
  operator Vector();
  operator Vector() const;
  inline double* ptr() const { return data;}
  inline operator double* () { return data; }
  inline operator const double* () const { return data; }
  void Print(const char* = "", std::ostream& = std::cout, int = 8) const;
  void SetSize(const size_t,const void* =NULL);
  inline size_t Size() const { return size; }
  inline size_t bytes() const { return size*sizeof(double); }
  double operator* (const RajaVector& v) const;
  RajaVector& operator = (const RajaVector& v);
  RajaVector& operator = (double value);
  RajaVector& operator -= (const RajaVector& v);
  RajaVector& operator += (const RajaVector& v);
  RajaVector& operator*=(const double d);
  RajaVector& Add(const double a, const RajaVector& Va);
  void Neg();
  RajaVectorRef GetRange(const size_t, const size_t) const;
  void SetSubVector(const void*, const double, const int);
  double Min() const;
  inline virtual ~RajaVector() {}
};

struct RajaVectorRef { RajaVector v; };

// ***************************************************************************
void add(const RajaVector&,const double,const RajaVector&,RajaVector&);
void add(const RajaVector&,const RajaVector&,RajaVector&);
void add(const double,const RajaVector&,const double,const RajaVector&,
         RajaVector&);
void subtract(const RajaVector&,const RajaVector&,RajaVector&);

}

#endif // MFEM_RAJA_VECTOR

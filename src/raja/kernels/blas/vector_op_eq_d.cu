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
#include "../raja.hpp"

// *****************************************************************************
static __global__ void d_vector_op_eq0(const int N,
                                       const double c0,
                                       double* __restrict v0){
  const int i = blockDim.x * blockIdx.x + threadIdx.x;
  if (i < N) v0[i] = c0;
}

// *****************************************************************************
extern "C" __global__ void d_vector_op_eq(const int N,
                                          const double c0,
                                          double* __restrict v0){
  const size_t blockSize = 128;
  const size_t gridSize = (N+blockSize-1)/blockSize;
  const int i = blockDim.x * blockIdx.x + threadIdx.x;
  if (i < N) v0[i] = c0;
}

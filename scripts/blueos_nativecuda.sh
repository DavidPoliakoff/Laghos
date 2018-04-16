#!/bin/bash

##
## Copyright (c) 2017, Lawrence Livermore National Security, LLC.
##
## Produced at the Lawrence Livermore National Laboratory.
##
## LLNL-CODE-738930
##
## All rights reserved.
## 
## This file is part of the RAJA Performance Suite.
##
## For details about use and distribution, please read raja-perfsuite/LICENSE.
##

rm -rf build_native_cuda >/dev/null
mkdir build_native_cuda && cd build_native_cuda

module load cmake/3.9.2

LAGHOS_DIR=$(git rev-parse --show-toplevel)

cmake \
  -DCMAKE_BUILD_TYPE=Debug \
  -C ${LAGHOS_DIR}/host-configs/blueos/nvcc_gcc_4_9_3.cmake \
  -DMFEM_USE_MPI=On \
  -DENABLE_UMPIRE=On \
  -DENABLE_OPENMP=Off \
  -DWITH_GOTCHA=Off \
  -DENABLE_CALIPER=On \
  -DWITH_SAMPLER=Off \
  -DEXTRA_LIBS="mpiprofilesupport mpi_ibm" \
  -DEXTRA_PREFIXES="/usr/tce/packages/spectrum-mpi/ibm/spectrum-mpi-2017.04.03" \
  -DWITH_NVPROF=On \
  -DWITH_CALLPATH=Off \
  -DCUDA_SEPARABLE_COMPILATION=On \
  -DENABLE_CUDA=On \
  -DENABLE_CUB=On \
  -DCMAKE_CUDA_COMPILER=/usr/tce/packages/cuda/cuda-9.0.176/bin/nvcc \
  -DCMAKE_CUDA_FLAGS="-std=c++11 -expt-extended-lambda" \
  -DCUDA_TOOLKIT_ROOT_DIR=/usr/tce/packages/cuda/cuda-9.0.176 \
  -DENABLE_ALL_WARNINGS=Off \
  -DCMAKE_INSTALL_PREFIX=../install_native_cuda \
  -DCUB_DIR=$HOME/workspace/cub \
  "$@" \
  ${LAGHOS_DIR}

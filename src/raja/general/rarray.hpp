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
#ifndef LAGHOS_RAJA_ARRAY
#define LAGHOS_RAJA_ARRAY


#include "umpire/config.hpp"
#include "umpire/ResourceManager.hpp"
#include "umpire/Allocator.hpp"
#include "umpire/util/Exception.hpp"
#include "umpire/op/MemoryOperationRegistry.hpp"
#include "umpire/op/MemoryOperation.hpp"
#include "umpire/strategy/AllocationStrategy.hpp"
#include "umpire/util/Platform.hpp"

namespace mfem {

template <class T, bool xyz = true> class RajaArray;

// Partial Specializations for xyz==TRUE *************************************
template <class T> class RajaArray<T,true> : public rmalloc<T>{
 private:
  T* data = NULL;
  size_t sz,d[4];
 public:
  RajaArray():data(NULL),sz(0),d{0,0,0,0} {}
  RajaArray(const size_t x) {allocate(x);}
  RajaArray(const size_t x,const size_t y) {allocate(x,y);}
  RajaArray(const RajaArray<T,true> &r) {assert(false);}
  RajaArray& operator=(Array<T> &a){
#if defined(__NVCC__)
    if (rconfig::Get().Cuda()) {
      try { 
        umpire::ResourceManager& rm = umpire::ResourceManager::getInstance();
        auto &op_registry = umpire::op::MemoryOperationRegistry::getInstance();
        auto host_strat = rm.getAllocator("HOST").getAllocationStrategy();
        auto device_strat = rm.getAllocator("DEVICE").getAllocationStrategy();
        auto op = op_registry.find("COPY",host_strat,device_strat);
        op->transform((void*)a.GetData(), data, nullptr, nullptr,a.Size()*sizeof(T));
      }
      catch(...) {
        printf("RajaArray operator=  host or device pointer not mapped with umpire\n");
        checkCudaErrors(cuMemcpyHtoD((CUdeviceptr)data,a.GetData(),a.Size()*sizeof(T)));
      }  
    }  
    else std::memcpy(data,a.GetData(),a.Size()*sizeof(T));
#else
    std::memcpy(data,a.GetData(),a.Size()*sizeof(T));
#endif
    return *this;
  }
  ~RajaArray(){rdbg("\033[32m[~i"); rmalloc<T>::operator delete(data);}
  inline size_t* dim() { return &d[0]; }
  inline T* ptr() { return data; }
  inline const T* GetData() const { return data; }
  inline const T* ptr() const { return data; }
  inline operator T* () { return data; }
  inline operator const T* () const { return data; }
  double operator* (const RajaArray& a) const { return vector_dot(sz, data, a.data); }
  inline size_t size() const { return sz; }
  inline size_t Size() const { return sz; }
  inline size_t bytes() const { return size()*sizeof(T); }
  void allocate(const size_t X, const size_t Y =1,
                const size_t Z =1, const size_t D =1,
                const bool transposed = false) {
    d[0]=X; d[1]=Y; d[2]=Z; d[3]=D;
    sz=d[0]*d[1]*d[2]*d[3];
    rdbg("\033[32m[i");
    data=(T*) rmalloc<T>::operator new(sz);
  }
  inline T& operator[](const size_t x) { return data[x]; }
  inline T& operator()(const size_t x, const size_t y) {
    return data[x + d[0]*y];
  }
  inline T& operator()(const size_t x, const size_t y, const size_t z) {
    return data[x + d[0]*(y + d[1]*z)];
  }
  void Print(std::ostream& out= std::cout, int width = 8) const {
#ifdef __NVCC__
    T *h_data;
    if (rconfig::Get().Cuda()){
      umpire::ResourceManager& rm = umpire::ResourceManager::getInstance();
      umpire::Allocator host_allocator = rm.getAllocator("HOST");
      auto &op_registry = umpire::op::MemoryOperationRegistry::getInstance();
      auto host_strat = rm.getAllocator("HOST").getAllocationStrategy();
      auto device_strat = rm.getAllocator("DEVICE").getAllocationStrategy();
      auto op = op_registry.find("COPY",device_strat,host_strat);
      h_data = static_cast<T*>(host_allocator.allocate(bytes()));
      //h_data= (T*)::malloc(bytes());assert(h_data);

      try { 
        op->transform(data,h_data, nullptr, nullptr,bytes());
      }
      catch(...) {
        printf("RajaArray Print  host or device pointer not mapped with umpire\n");
        checkCudaErrors(cuMemcpyDtoH(h_data,(CUdeviceptr)data,bytes()));
      }  

    }else h_data=data;
#else
    T *h_data=data;
#endif
    for (size_t i=0; i<sz; i+=1) 
      if (sizeof(T)==8) printf("\n\t[%ld] %.15e",i,h_data[i]);
      else printf("\n\t[%ld] %d",i,h_data[i]);
  }
};

// Partial Specializations for xyz==FALSE ************************************
template <class T> class RajaArray<T,false> : public rmalloc<T>{
 private:
  static const int DIM = 4;
  T* data = NULL;
  size_t sz,d[DIM];
 public:
  RajaArray():data(NULL),sz(0),d{0,0,0,0} {}
  RajaArray(const size_t d0) {allocate(d0);}
  RajaArray(const RajaArray<T,false> &r) {assert(false);}
  ~RajaArray(){rdbg("\033[32m[~I"); rmalloc<T>::operator delete(data);}
  RajaArray& operator=(Array<T> &a){
#ifdef __NVCC__
    if (rconfig::Get().Cuda()) {
      try { 
        umpire::ResourceManager& rm = umpire::ResourceManager::getInstance();
        auto &op_registry = umpire::op::MemoryOperationRegistry::getInstance();
        auto host_strat = rm.getAllocator("HOST").getAllocationStrategy();
        auto device_strat = rm.getAllocator("DEVICE").getAllocationStrategy();
        auto op = op_registry.find("COPY",host_strat,device_strat);
        op->transform((void*)a.GetData(), data, nullptr, nullptr,a.Size()*sizeof(T));
      }
      catch(...) {
        printf("RajaArray operator=  host or device pointer not mapped with umpire\n");
        checkCudaErrors(cuMemcpyHtoD((CUdeviceptr)data,a.GetData(),a.Size()*sizeof(T)));
      }  
    }  
    else std::memcpy(data,a.GetData(),a.Size()*sizeof(T));
#else
    std::memcpy(data,a.GetData(),a.Size()*sizeof(T));
#endif
    return *this;
  }
  inline size_t* dim() { return &d[0]; }
  inline T* ptr() { return data; }
  inline T* GetData() const { return data; }
  inline const T* ptr() const { return data; }
  inline operator T* () { return data; }
  inline operator const T* () const { return data; }
  double operator* (const RajaArray& a) const { return vector_dot(sz, data, a.data); }
  inline size_t size() const { return sz; }
  inline size_t Size() const { return sz; }
  inline size_t bytes() const { return size()*sizeof(T); }
  void allocate(const size_t X, const size_t Y =1,
                const size_t Z =1, const size_t D =1,
                const bool transposed = false) {
    d[0]=X; d[1]=Y; d[2]=Z; d[3]=D;
    sz=d[0]*d[1]*d[2]*d[3];
    rdbg("\033[32m[I");
    data=(T*) rmalloc<T>::operator new(sz);
#define xsw(a,b) a^=b^=a^=b
    if (transposed) { xsw(d[0],d[1]); }
    for (size_t i=1,b=d[0]; i<DIM; xsw(d[i],b),++i) {
      d[i]*=d[i-1];
    }
    d[0]=1;
    if (transposed) { xsw(d[0],d[1]); }
  }
  inline T& operator[](const size_t x) { return data[x]; }
  inline T& operator()(const size_t x, const size_t y) {
    return data[d[0]*x + d[1]*y];
  }
  inline T& operator()(const size_t x, const size_t y, const size_t z) {
    return data[d[0]*x + d[1]*y + d[2]*z];
  }
  void Print(std::ostream& out= std::cout, int width = 8) const {
#ifdef __NVCC__
    T *h_data;
    if (rconfig::Get().Cuda()){
      umpire::ResourceManager& rm = umpire::ResourceManager::getInstance();
      umpire::Allocator host_allocator = rm.getAllocator("HOST");
      auto &op_registry = umpire::op::MemoryOperationRegistry::getInstance();
      auto host_strat = rm.getAllocator("HOST").getAllocationStrategy();
      auto device_strat = rm.getAllocator("DEVICE").getAllocationStrategy();
      auto op = op_registry.find("COPY",device_strat,host_strat);
      h_data = static_cast<T*>(host_allocator.allocate(bytes()));
      //h_data= (T*)::malloc(bytes());assert(h_data);

      try { 
        op->transform(data,h_data, nullptr, nullptr,bytes());
      }
      catch(...) {
        printf("RajaArray Print  host or device pointer not mapped with umpire\n");
        checkCudaErrors(cuMemcpyDtoH(h_data,(CUdeviceptr)data,bytes()));
      }  
    }else h_data=data;
#else
    T *h_data=data;
#endif
    for (size_t i=0; i<sz; i+=1) 
      printf("\n\t[%ld] %d",i,h_data[i]);
  }
};

} // mfem

#endif // LAGHOS_RAJA_ARRAY


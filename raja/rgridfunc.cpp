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
void RajaGridFunction::ToQuad(const IntegrationRule& ir,
                              RajaVector& quadValues) {
  const FiniteElement& fe = *(fes.GetFE(0));
  const int dim  = fe.GetDim();
  const int vdim = fes.GetVDim();
  const int elements = fes.GetNE();
  const int numQuad  = ir.GetNPoints();
  const RajaDofQuadMaps& maps = RajaDofQuadMaps::Get(fes, ir);
  const int quad1D  = IntRules.Get(Geometry::SEGMENT,ir.GetOrder()).GetNPoints();
  const int dofs1D =fes.GetFE(0)->GetOrder() + 1;

  quadValues.SetSize(numQuad * elements);

  if (dim==1) { assert(false); }
  if (dim==2){
    //maps.dofToQuad.Print("dofToQuad");
    //fes.GetLocalToGlobalMap().Print("GetLocalToGlobalMap");
    kGridFuncToQuad2D(vdim,
                      dofs1D,
                      quad1D,
                      elements,
                      maps.dofToQuad,
                      fes.GetLocalToGlobalMap(),
                      ptr(),
                      quadValues);
  }
  if (dim==3)
    kGridFuncToQuad3D(vdim,
                      dofs1D,
                      quad1D,
                      elements,
                      maps.dofToQuad,
                      fes.GetLocalToGlobalMap(),
                      ptr(),
                      quadValues);

}

} // mfem

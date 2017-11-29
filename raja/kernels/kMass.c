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
#include "defines.h"

// *****************************************************************************
void kMassAssemble2D(const int NUM_QUAD_2D,
                     const int numElements,
                     const double* quadWeights,
                     const double* J,
                     const double COEFF,
                     double* __restrict oper) {
  for (int e = 0; e < numElements; ++e) {
    for (int q = 0; q < NUM_QUAD_2D; ++q) {
      const double J11 = J[ijklNM(0,0,q,e,2,NUM_QUAD_2D)];
      const double J12 = J[ijklNM(1,0,q,e,2,NUM_QUAD_2D)];
      const double J21 = J[ijklNM(0,1,q,e,2,NUM_QUAD_2D)];
      const double J22 = J[ijklNM(1,1,q,e,2,NUM_QUAD_2D)];
      const double detJ = ((J11 * J22)-(J21 * J12));
      oper[ijN(q,e,NUM_QUAD_2D)] = quadWeights[q] * COEFF * detJ;
    }
  }
}

// *****************************************************************************
void kMassMultAdd2D(const int NUM_DOFS_1D,
                    const int NUM_QUAD_1D,
                    const int numElements,
                    const double* dofToQuad,
                    const double* dofToQuadD,
                    const double* quadToDof,
                    const double* quadToDofD,
                    const double* oper,
                    const double* solIn,
                    double* __restrict solOut) {
  //printf("\n\033[32m[kMassMultAdd2D]\033[m numElements=%d",numElements);
  //printf("\n\033[32m[kMassMultAdd2D]\033[m NUM_DOFS_1D=%d",NUM_DOFS_1D);
  //printf("\n\033[32m[kMassMultAdd2D]\033[m NUM_QUAD_1D=%d",NUM_QUAD_1D);
  for (int e = 0; e < numElements; ++e) {
    double sol_xy[NUM_QUAD_1D][NUM_QUAD_1D];
    for (int qy = 0; qy < NUM_QUAD_1D; ++qy) {
      for (int qx = 0; qx < NUM_QUAD_1D; ++qx) {
        sol_xy[qy][qx] = 0.0;
      }
    }
    for (int dy = 0; dy < NUM_DOFS_1D; ++dy) {
      double sol_x[NUM_QUAD_1D];
      for (int qy = 0; qy < NUM_QUAD_1D; ++qy) {
        sol_x[qy] = 0.0;
      }
      for (int dx = 0; dx < NUM_DOFS_1D; ++dx) {
        const double s = solIn[ijkN(dx,dy,e,NUM_DOFS_1D)];
        for (int qx = 0; qx < NUM_QUAD_1D; ++qx) {
          sol_x[qx] += dofToQuad[ijN(qx,dx,NUM_QUAD_1D)]* s;
        }
      }
      for (int qy = 0; qy < NUM_QUAD_1D; ++qy) {
        const double d2q = dofToQuad[ijN(qy,dy,NUM_QUAD_1D)];
        for (int qx = 0; qx < NUM_QUAD_1D; ++qx) {
          sol_xy[qy][qx] += d2q * sol_x[qx];
        }
      }
    }
    for (int qy = 0; qy < NUM_QUAD_1D; ++qy) {
      for (int qx = 0; qx < NUM_QUAD_1D; ++qx) {
        sol_xy[qy][qx] *= oper[ijkN(qx,qy,e,NUM_QUAD_1D)];
      }
    }
    for (int qy = 0; qy < NUM_QUAD_1D; ++qy) {
      double sol_x[NUM_DOFS_1D];
      for (int dx = 0; dx < NUM_DOFS_1D; ++dx) {
        sol_x[dx] = 0.0;
      }
      for (int qx = 0; qx < NUM_QUAD_1D; ++qx) {
        const double s = sol_xy[qy][qx];
        for (int dx = 0; dx < NUM_DOFS_1D; ++dx) {
          sol_x[dx] += quadToDof[ijN(dx,qx,NUM_DOFS_1D)] * s;
        }
      }
      for (int dy = 0; dy < NUM_DOFS_1D; ++dy) {
        const double q2d = quadToDof[ijN(dy,qy,NUM_DOFS_1D)];
        for (int dx = 0; dx < NUM_DOFS_1D; ++dx) {
          solOut[ijkN(dx,dy,e,NUM_DOFS_1D)] += q2d * sol_x[dx];
        }
      }
    }
  }
}


// *****************************************************************************
void kMassAssemble3D(const int NUM_QUAD_3D,
                     const double COEFF,
                     const int numElements,
                     const double* quadWeights,
                     const double* J,
                     double* __restrict oper) {
  for (int e = 0; e < numElements; ++e) {
    for (int q = 0; q < NUM_QUAD_3D; ++q) {
      const double J11 = J[ijklNM(0,0,q,e,3,NUM_QUAD_3D)];
      const double J12 = J[ijklNM(1,0,q,e,3,NUM_QUAD_3D)];
      const double J13 = J[ijklNM(2,0,q,e,3,NUM_QUAD_3D)];
      const double J21 = J[ijklNM(0,1,q,e,3,NUM_QUAD_3D)];
      const double J22 = J[ijklNM(1,1,q,e,3,NUM_QUAD_3D)];
      const double J23 = J[ijklNM(2,1,q,e,3,NUM_QUAD_3D)];
      const double J31 = J[ijklNM(0,2,q,e,3,NUM_QUAD_3D)];
      const double J32 = J[ijklNM(1,2,q,e,3,NUM_QUAD_3D)];
      const double J33 = J[ijklNM(2,2,q,e,3,NUM_QUAD_3D)];
      const double detJ = ((J11*J22*J33)+(J12*J23*J31)+
                           (J13*J21*J32)-(J13*J22*J31)-
                           (J12*J21*J33)-(J11*J23*J32));
      oper[ijN(q,e,NUM_QUAD_3D)] = quadWeights[q]*COEFF*detJ;
    }
  }
}

// *****************************************************************************
void kMassMultAdd3D(const int NUM_QUAD_1D,
                    const int NUM_DOFS_1D,
                    const int numElements,
                    const double* dofToQuad,
                    const double* dofToQuadD,
                    const double* quadToDof,
                    const double* quadToDofD,
                    const double* oper,
                    const double* solIn,
                    double* __restrict solOut) {
  for (int e = 0; e < numElements; ++e) {
    double sol_xyz[NUM_QUAD_1D][NUM_QUAD_1D][NUM_QUAD_1D];
    for (int qz = 0; qz < NUM_QUAD_1D; ++qz) {
      for (int qy = 0; qy < NUM_QUAD_1D; ++qy) {
        for (int qx = 0; qx < NUM_QUAD_1D; ++qx) {
          sol_xyz[qz][qy][qx] = 0;
        }
      }
    }
    for (int dz = 0; dz < NUM_DOFS_1D; ++dz) {
      double sol_xy[NUM_QUAD_1D][NUM_QUAD_1D];
      for (int qy = 0; qy < NUM_QUAD_1D; ++qy) {
        for (int qx = 0; qx < NUM_QUAD_1D; ++qx) {
          sol_xy[qy][qx] = 0;
        }
      }
      for (int dy = 0; dy < NUM_DOFS_1D; ++dy) {
        double sol_x[NUM_QUAD_1D];
        for (int qx = 0; qx < NUM_QUAD_1D; ++qx) {
          sol_x[qx] = 0;
        }
        for (int dx = 0; dx < NUM_DOFS_1D; ++dx) {
          const double s = solIn[ijklN(dx,dy,dz,e,NUM_DOFS_1D)];
          for (int qx = 0; qx < NUM_QUAD_1D; ++qx) {
            sol_x[qx] += dofToQuad[ijN(qx,dx,NUM_QUAD_1D)] * s;
          }
        }
        for (int qy = 0; qy < NUM_QUAD_1D; ++qy) {
          const double wy = dofToQuad[ijN(qy,dy,NUM_QUAD_1D)];
          for (int qx = 0; qx < NUM_QUAD_1D; ++qx) {
            sol_xy[qy][qx] += wy * sol_x[qx];
          }
        }
      }
      for (int qz = 0; qz < NUM_QUAD_1D; ++qz) {
        const double wz = dofToQuad[ijN(qz,dz,NUM_QUAD_1D)];
        for (int qy = 0; qy < NUM_QUAD_1D; ++qy) {
          for (int qx = 0; qx < NUM_QUAD_1D; ++qx) {
            sol_xyz[qz][qy][qx] += wz * sol_xy[qy][qx];
          }
        }
      }
    }
    for (int qz = 0; qz < NUM_QUAD_1D; ++qz) {
      for (int qy = 0; qy < NUM_QUAD_1D; ++qy) {
        for (int qx = 0; qx < NUM_QUAD_1D; ++qx) {
          sol_xyz[qz][qy][qx] *= oper[ijklN(qx,qy,qz,e,NUM_QUAD_1D)];
        }
      }
    }
    for (int qz = 0; qz < NUM_QUAD_1D; ++qz) {
      double sol_xy[NUM_DOFS_1D][NUM_DOFS_1D];
      for (int dy = 0; dy < NUM_DOFS_1D; ++dy) {
        for (int dx = 0; dx < NUM_DOFS_1D; ++dx) {
          sol_xy[dy][dx] = 0;
        }
      }
      for (int qy = 0; qy < NUM_QUAD_1D; ++qy) {
        double sol_x[NUM_DOFS_1D];
        for (int dx = 0; dx < NUM_DOFS_1D; ++dx) {
          sol_x[dx] = 0;
        }
        for (int qx = 0; qx < NUM_QUAD_1D; ++qx) {
          const double s = sol_xyz[qz][qy][qx];
          for (int dx = 0; dx < NUM_DOFS_1D; ++dx) {
            sol_x[dx] += quadToDof[ijN(dx,qx,NUM_DOFS_1D)] * s;
          }
        }
        for (int dy = 0; dy < NUM_DOFS_1D; ++dy) {
          const double wy = quadToDof[ijN(dy,qy,NUM_DOFS_1D)];
          for (int dx = 0; dx < NUM_DOFS_1D; ++dx) {
            sol_xy[dy][dx] += wy * sol_x[dx];
          }
        }
      }
      for (int dz = 0; dz < NUM_DOFS_1D; ++dz) {
        const double wz = quadToDof[ijN(dz,qz,NUM_DOFS_1D)];
        for (int dy = 0; dy < NUM_DOFS_1D; ++dy) {
          for (int dx = 0; dx < NUM_DOFS_1D; ++dx) {
            solOut[ijklN(dx,dy,dz,e,NUM_DOFS_1D)] += wz * sol_xy[dy][dx];
          }
        }
      }
    }
  }
}

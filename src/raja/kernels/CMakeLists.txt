include_directories(${CMAKE_SOURCE_DIR}/tpl/rajitify/include)
# DEBUG delete
set(LAGHOS_RAJA_KERNEL_SOURCES
  blas/vector_axpy.cpp
  blas/vector_clear_dofs.cpp
  blas/vector_dot.cpp
  blas/vector_get_subvector.cpp
  blas/vector_map_dofs.cpp
  blas/vector_min.cpp
  blas/vector_neg.cpp
  blas/vector_op_eq.cpp
  blas/vector_set_subvector.cpp
  blas/vector_set_subvector_const.cpp
  blas/vector_vec_add.cpp
  blas/vector_vec_mul.cpp
  blas/vector_vec_sub.cpp
  blas/vector_xpay.cpp
  blas/vector_xsy.cpp
  force/rForce.cpp
  geom/rInitGeom.cpp
  maps/rGlobalToLocal.cpp
  maps/rLocalToGlobal.cpp
  maps/rMapping.cpp
  mass/rMassAssemble.cpp
  mass/rMassMultAdd.cpp
  quad/rGridFuncToQuad.cpp
  quad/rQDataInit.cpp
  quad/rQDataUpdate.cpp
  share/rForceS.cpp
  share/rGridFuncToQuadS.cpp
  share/rMassAssembleS.cpp
  share/rMassMultAddS.cpp
  share/rQDataUpdateS.cpp
)
set_source_files_properties(${LAGHOS_RAJA_KERNEL_SOURCES} PROPERTIES LANGUAGE CUDA)

include_directories(/usr/workspace/wsrzd/dzpolia/src/cling/inst/include)
blt_add_library(
  NAME laghos_raja_kernels
  SOURCES ${LAGHOS_RAJA_KERNEL_SOURCES}
  DEPENDS_ON ${LAGHOS_DEPENDS} ${CLING_LIBS}
  )

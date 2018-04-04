set(LAGHOS_RAJA_SOURCES
  config/rconfig.cpp
  config/rnvvp.cpp
  fem/rbilinearform.cpp
  fem/rbilininteg.cpp
  fem/rconform.cpp
  fem/rfespace.cpp
  fem/rgridfunc.cpp
  fem/rprolong.cpp
  fem/rrestrict.cpp
  general/rcommd.cpp
  general/rcommunicator.cpp
  general/rtable.cpp
  linalg/rvector.cpp
  tests/dot.cpp
  tests/mult.cpp 
)
set_source_files_properties(${LAGHOS_RAJA_SOURCES} PROPERTIES LANGUAGE CUDA)
blt_add_library(
  NAME laghos_raja
  SOURCES ${LAGHOS_RAJA_SOURCES}
  DEPENDS_ON laghos_raja_kernels ${LAGHOS_DEPENDS} ${CLING_LIBS}
  )

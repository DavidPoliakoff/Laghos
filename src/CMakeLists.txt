set(CLING_DIR /usr/workspace/wsrzd/dzpolia/src/cling/inst/)
link_directories(${CLING_DIR}/lib)
include_directories(.)
include_directories(./raja)
include_directories(./raja/config)
include_directories(./raja/fem)
include_directories(./raja/general)
include_directories(./raja/kernels)
include_directories(./raja/kernels/include)
include_directories(./raja/linalg)
include_directories(./raja/tests)

include_directories(/usr/tce/packages/spectrum-mpi/ibm/spectrum-mpi-2017.04.03/include)

set(CLING_LIBS -Wl,--start-group
LTO
clang
cling
clangARCMigrate 
clangAST 
clangASTMatchers 
clangAnalysis 
clangBasic 
clangCodeGen 
clangDriver 
clangDynamicASTMatchers 
clangEdit 
clangFormat 
clangFrontend 
clangFrontendTool 
clangIndex 
clangLex 
clangParse 
clangRewrite 
clangRewriteFrontend 
clangSema 
clangSerialization 
clangStaticAnalyzerCheckers 
clangStaticAnalyzerCore 
clangStaticAnalyzerFrontend 
clangTooling 
clangToolingCore 
clangToolingRefactor 
clingInterpreter 
clingMetaProcessor 
clingUserInterface 
clingUtils 
-Wl,--end-group)

add_subdirectory(raja)
add_subdirectory(raja/kernels)

set(LAGHOS_EXECUTABLE_DEPENDS
    laghos_raja
    laghos_raja_kernels)


MESSAGE("Cling Libs are: ${CLING_LIBS}")  

list(APPEND LAGHOS_EXECUTABLE_DEPENDS ${LAGHOS_DEPENDS} rover-utils ${CLING_LIBS}) 

#add_executable(laghos.exe
#  laghos.cpp
#  laghos_assembly.cpp
#  laghos_solver.cpp
#  )
blt_add_executable(
  NAME laghos.exe
  SOURCES laghos.cpp 
  laghos_assembly.cpp
  laghos_solver.cpp
  DEPENDS_ON ${LAGHOS_EXECUTABLE_DEPENDS}
)

# Copyright (c) 2017, Lawrence Livermore National Security, LLC. Produced at
# the Lawrence Livermore National Laboratory. LLNL-CODE-734707. All Rights
# reserved. See files LICENSE and NOTICE for details.
#
# This file is part of CEED, a collection of benchmarks, miniapps, software
# libraries and APIs for efficient high-order finite element and spectral
# element discretizations for exascale applications. For more information and
# source code availability see http://github.com/ceed.
#
# The CEED research is supported by the Exascale Computing Project 17-SC-20-SC,
# a collaborative effort of two U.S. Department of Energy organizations (Office
# of Science and the National Nuclear Security Administration) responsible for
# the planning and preparation of a capable exascale ecosystem, including
# software, applications, hardware, advanced system engineering and early
# testbed platforms, in support of the nation's exascale computing imperative.

define LAGHOS_HELP_MSG

Laghos makefile targets:

   make
   make status/info
   make install
   make clean
   make distclean
   make style

Examples:

make -j 4
   Build Laghos using the current configuration options from MFEM.
   (Laghos requires the MFEM finite element library, and uses its compiler and
    linker options in its build process.)
make status
   Display information about the current configuration.
make install PREFIX=<dir>
   Install the Laghos executable in <dir>.
make clean
   Clean the Laghos executable, library and object files.
make distclean
   In addition to "make clean", remove the local installation directory and some
   run-time generated files.
make style
   Format the Laghos C++ source files using the Artistic Style (astyle) settings
   from MFEM.

endef

# fetch current/working directory
pwd = $(patsubst %/,%,$(dir $(abspath $(firstword $(MAKEFILE_LIST)))))
home = $(HOME)
raja = $(pwd)/raja
kernels = $(raja)/kernels

CPU = $(shell echo $(shell getconf _NPROCESSORS_ONLN)*2|bc -l)

# Default installation location
PREFIX = ./bin
INSTALL = /usr/bin/install

# Use the MFEM build directory
#MFEM_DIR = ../mfem-raja
MFEM_DIR = ../MFEM-github/blueos_build/mfem
CONFIG_MK = $(MFEM_DIR)/share/mfem/config.mk
TEST_MK = $(MFEM_DIR)/share/mfem/test.mk
#TEST_MK = $(MFEM_DIR)/config/test.mk
# Use the MFEM install directory
# MFEM_DIR = ../mfem/mfem
# CONFIG_MK = $(MFEM_DIR)/config.mk
# TEST_MK = $(MFEM_DIR)/test.mk

# Use two relative paths to MFEM: first one for compilation in '.' and second
# one for compilation in 'lib'.
MFEM_DIR1 := $(MFEM_DIR)
MFEM_DIR2 := $(realpath $(MFEM_DIR))

# Use the compiler used by MFEM. Get the compiler and the options for compiling
# and linking from MFEM's config.mk. (Skip this if the target does not require
# building.)
MFEM_LIB_FILE = mfem_is_not_built
ifeq (,$(filter help clean distclean style,$(MAKECMDGOALS)))
   -include $(CONFIG_MK)
endif

#################
# MFEM compiler #
#################
CXX = $(MFEM_CXX)
CPPFLAGS = $(MFEM_CPPFLAGS)
CXXFLAGS = $(MFEM_CXXFLAGS)
CXXFLAGS += -std=c++11 #-fopenmp #-Wall 
#-fsanitize=undefined -fsanitize=address -fno-omit-frame-pointer

# MFEM config does not define C compiler
CC     = gcc
CFLAGS = -O3 -Wall

# Optional link flags
LDFLAGS =

OPTIM_OPTS = -O3
DEBUG_OPTS = -g -Wall
LAGHOS_DEBUG = $(MFEM_DEBUG)
ifneq ($(LAGHOS_DEBUG),$(MFEM_DEBUG))
   ifeq ($(LAGHOS_DEBUG),YES)
      CXXFLAGS = $(DEBUG_OPTS)
   else
      CXXFLAGS = $(OPTIM_OPTS)
   endif
endif

#################
# CUDA compiler #
# comment these lines to use MFEM's compiler
#################
ifeq ($(LAGHOS_NVCC),YES)
	CXX = /usr/local/cuda/bin/nvcc
	CXXFLAGS = -std=c++11 -O2 -g -x=cu -m64 \
		-Xcompiler -fopenmp \
		--restrict --expt-extended-lambda \
		--gpu-architecture sm_60 \
		-DUSE_RAJA -DUSE_CUDA \
		-ccbin $(home)/usr/local/gcc/5.5.0/bin/g++
endif

#######################
# TPL INCLUDES & LIBS #
#######################
MPI_INC = -I$(home)/usr/local/openmpi/3.0.0/include 

CUDA_INC = -I/usr/local/cuda/include
CUDA_LIBS = /usr/local/cuda/lib64/libcudart_static.a

RAJA_INC = -I$(home)/usr/local/raja/0.4.1/include
RAJA_LIBS = $(home)/usr/local/raja/0.4.1/lib/libRAJA.a

LAGHOS_FLAGS = $(CPPFLAGS) $(CXXFLAGS) $(MFEM_INCFLAGS) $(RAJA_INC) $(CUDA_INC) $(MPI_INC)
LAGHOS_LIBS = $(MFEM_LIBS) -fopenmp $(RAJA_LIBS) $(CUDA_LIBS) -ldl 

ifeq ($(LAGHOS_DEBUG),YES)
   LAGHOS_FLAGS += -DLAGHOS_DEBUG
endif

#########################
# FINAL LIBS, CCC & Ccc #
#########################
LIBS = $(strip $(LAGHOS_LIBS) $(LDFLAGS))
CCC  = $(strip $(CXX) $(LAGHOS_FLAGS))
Ccc  = $(strip $(CC) $(CFLAGS) $(GL_OPTS))

################
# SOURCE FILES #
################
SOURCE_FILES  = $(wildcard $(pwd)/*.cpp)
KERNEL_FILES += $(wildcard $(kernels)/*.cpp)
  RAJA_FILES += $(wildcard $(raja)/*.cpp)

################
# OBJECT FILES #
################
OBJECT_FILES  = $(SOURCE_FILES:.cpp=.o)
OBJECT_FILES += $(KERNEL_FILES:.cpp=.o)
OBJECT_FILES += $(RAJA_FILES:.cpp=.o)
HEADER_FILES = laghos_solver.hpp laghos_assembly.hpp

################
# OUTPUT rules #
################
COLOR_OFFSET = 16
COLOR = $(shell echo $(rule_path)|cksum|cut -b1-2)
rule_path = $(notdir $(patsubst %/,%,$(dir $<)))
rule_file = $(basename $(notdir $@))
rule_dumb = @echo -e $(rule_path)/$(rule_file)
rule_xterm = @echo -e \\e[38\;5\;$(shell echo $(COLOR)+$(COLOR_OFFSET)|bc -l)\;1m\
             $(rule_path)\\033[m/\\033[\m$(rule_file)\\033[m
output = $(rule_${TERM})
quiet := --quiet -S

###########
# Targets #
###########
.PHONY: all clean distclean install status info opt debug test style clean-build clean-exec

.SUFFIXES: .c .cpp .o

$(pwd)/%.o:$(pwd)/%.cpp;$(output)
	$(CCC) -c -o $@ $<

$(raja)/%.o: $(raja)/%.cpp $(raja)/%.hpp $(raja)/raja.hpp $(raja)/rmanaged.hpp;$(output)
	$(CCC) -c -o $@ $<

$(kernels)/%.o: $(kernels)/%.cpp $(kernels)/kernels.hpp $(kernels)/defines.hpp;$(output)
	$(CCC) -c -o $@ $<

all: 
	@$(MAKE) $(quiet) -j $(CPU) laghos

laghos: override MFEM_DIR = $(MFEM_DIR1)
laghos:	$(OBJECT_FILES) $(CONFIG_MK) $(MFEM_LIB_FILE)
	$(MFEM_CXX) -o laghos $(OBJECT_FILES) $(LIBS)

opt:
	$(MAKE) "LAGHOS_DEBUG=NO"

debug:
	$(MAKE) "LAGHOS_DEBUG=YES"

nv nvcc:
	$(MAKE) "LAGHOS_NVCC=YES"

$(OBJECT_FILES): override MFEM_DIR = $(MFEM_DIR2)
$(OBJECT_FILES): $(HEADER_FILES) $(CONFIG_MK)

MFEM_TESTS = laghos
include $(TEST_MK)
# Testing: Specific execution options
RUN_MPI = $(MFEM_MPIEXEC) $(MFEM_MPIEXEC_NP) 4
test: laghos
	@$(call mfem-test,$<, $(RUN_MPI), Laghos miniapp,\
	-p 0 -m data/square01_quad.mesh -rs 3 -tf 0.1)
# Testing: "test" target and mfem-test* variables are defined in MFEM's
# config/test.mk

# Generate an error message if the MFEM library is not built and exit
$(CONFIG_MK) $(MFEM_LIB_FILE):
	$(error The MFEM library is not built)

clean cln: clean-build clean-exec

clean-build:
	rm -rf laghos *.o *~ *.dSYM raja/*.o raja/kernels/*.o
clean-exec:
	rm -rf ./results

distclean: clean
	rm -rf bin/

install: laghos
	mkdir -p $(PREFIX)
	$(INSTALL) -m 750 laghos $(PREFIX)

help:
	$(info $(value LAGHOS_HELP_MSG))
	@true

status info:
	$(info MFEM_DIR    = $(MFEM_DIR))
	$(info LAGHOS_FLAGS = $(LAGHOS_FLAGS))
	$(info LAGHOS_LIBS  = $(value LAGHOS_LIBS))
	$(info PREFIX      = $(PREFIX))
	@true

ASTYLE = astyle --options=$(MFEM_DIR1)/config/mfem.astylerc
FORMAT_FILES := $(SOURCE_FILES) $(HEADER_FILES)

style:
	@if ! $(ASTYLE) $(FORMAT_FILES) | grep Formatted; then\
	   echo "No source files were changed.";\
	fi

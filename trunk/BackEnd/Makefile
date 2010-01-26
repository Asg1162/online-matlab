LEX=flex
YACC=bison
CC=g++

ROOTDIR ?= .
SRC_DIR = $(ROOTDIR)/src
OBJ_DIR = $(ROOTDIR)/obj

SRC = $(wildcard $(SRC_DIR)/*.cpp)
SRC_OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, ${SRC})

CPPFLAGS = -g -Wno-write-strings

TARGET ?= $(ROOTDIR)/parser
LEX_YY_O = $(ROOTDIR)/lex.yy.o
LEX_YY_C = $(ROOTDIR)/lex.yy.c
LEXYA_L = $(ROOTDIR)/lexya.l

YACC_TAB_O = $(ROOTDIR)/yacc.tab.o
YACC_TAB_C = $(ROOTDIR)/yacc.tab.c
YACC_TAB_H = $(ROOTDIR)/yacc.tab.h
YACC_Y = $(ROOTDIR)/yacc.y

MAIN_O = $(ROOTDIR)/main.o
LoadBalancer_O = $(ROOTDIR)/LoadBalancer.o

ICE_HOME=/usr
ICE_HOME_LIB=${ICE_HOME}/lib64
ICE_HOME_INC=${ICE_HOME}/include

CULA_INSTALL_PATH ?= $(HOME)/cula
CULA_INC_PATH =$(CULA_INSTALL_PATH)/include
CULA_LIB_PATH_32 =$(CULA_INSTALL_PATH)/lib32
CULA_LIB_PATH_64 =$(CULA_INSTALL_PATH)/lib64

CUDA_INSTALL_PATH ?= /usr/local/cuda
CUDA_INC_PATH = $(CUDA_INSTALL_PATH)/include
CUDASDK_INSTALL_PATH ?=/home/yzhang25/NVIDIA_GPU_Computing_SDK/C
CUDASDK_INC_PATH=$(CUDASDK_INSTALL_PATH)/common/inc
CUDA_LIB_PATH=/usr/local/cuda/lib64
CUDASDK_LIB_PATH=$(CUDASDK_INSTALL_PATH)/common/lib/linux
NVCC=nvcc


INCLUDES=-I${CULA_INC_PATH} -I$(ROOTDIR)/. -I${CUDA_INC_PATH}
GPU_LIBS=-L${CULA_LIB_PATH_64} -L$(CUDA_LIB_PATH) -L$(CUDASDK_LIB_PATH) -L$(CUDASDK_INSTALL_PATH)/lib  -lcula -lcublas -lcudart -lcutil
ICE_LIBS=-L${ICE_HOME_LIB} -lIce -lIceUtil

CU_FILE=$(ROOTDIR)/src/GpuMath.cu
CU_KERNELFILE=$(ROOTDIR)/src/GpuMathKernel.cu
CU_O=$(OBJ_DIR)/GpuMath_cu.o

PNG_WRITER_FLAGS = -Wno-deprecated `freetype-config --cflags` `freetype-config --libs`
PNG_WRITER_SRC = $(ROOTDIR)/../3rdparty/pngwriter-0.5.4/src
PNG_WRITER_FLAGS += -I$(PNG_WRITER_SRC) -I/usr/include
PNG_WRITER_FLAGS += -L$(PNG_WRITER_SRC) -L/usr/lib64 -lz -lpngwriter -lpthread -lpng

INCLUDES +=  `freetype-config --cflags` `freetype-config --libs` -I$(PNG_WRITER_SRC) -I/usr/include -I$(ROOTDIR)/include
CPP_LIBS = $(GPU_LIBS) $(ICE_LIBS) -L$(PNG_WRITER_SRC) -L/usr/lib64 -lz -lpngwriter -lpthread -lpng

target: $(TARGET)

unittest: $(patsubst $(ROOTDIR)/unittest/%.cpp, $(TEST_BIN_DIR)/%, $(TEST_CASE_SRC))

$(CU_O) : $(CU_FILE) $(CU_KERNELFILE)
	$(NVCC) --compiler-options -fno-strict-aliasing -g -I. -I$(CUDA_INC_PATH) -I$(CUDASDK_INC_PATH) -DUNIX    -o $(CU_O) -c $(CU_FILE)


$(TEST_BIN_DIR)/%:  $(ROOTDIR)/unittest/%.cpp $(SRC_OBJ) $(LEX_YY_O) $(YACC_TAB_O) $(CU_O)
	mkdir -p $(ROOTDIR)/unittest/bin
	$(CC) -m64 $(CPPFLAGS) $(INCLUDES) $(LEX_YY_O) $(YACC_TAB_O)  ${LoadBalancer_O}  $< $(SRC_OBJ) $(CU_O) -o $@ $(CPP_LIBS) 
$

$(TARGET):$(LEX_YY_O) $(YACC_TAB_O) $(MAIN_O) $(SRC_OBJ) $(ROOTDIR)/Makefile ${LoadBalancer_O} $(CU_O)
	$(CC) -m64 $(CPPFLAGS) $(INCLUDES)  $(LEX_YY_O) $(YACC_TAB_O) $(MAIN_O) $(SRC_OBJ) $(CU_O) ${LoadBalancer_O} -o $(TARGET)  $(CPP_LIBS)

$(LEX_YY_O): $(LEX_YY_C) $(YACC_TAB_H)
	$(CC) $(CPPFLAGS) -I$(ROOTDIR)/include -c $(LEX_YY_C) -o $(LEX_YY_O)

$(ROOTDIR)/lexya_a.tab.o:$(YACC_TAB_C)
	$(CC) $(CPPFLAGS) -c $< -o $(ROOTDIR)/lexya_a.tab.o

$(YACC_TAB_O): $(YACC_TAB_C)
	$(CC) $(CPPFLAGS) -I$(ROOTDIR)/include -c $< -o $(YACC_TAB_O)

$(YACC_TAB_C) $(YACC_TAB_H): $(YACC_Y)
	$(YACC) -d -o $(YACC_TAB_C) $(YACC_Y)

$(LEX_YY_C):$(LEXYA_L)
	$(LEX) -d -o $(LEX_YY_C) $(LEXYA_L)

#	$(LEX) -d lexya.l

$(MAIN_O): $(ROOTDIR)/main.cpp  $(ROOTDIR)/LoadBalancer.cpp
	$(CC) -m64 -I$(ROOTDIR)/include -I$(ROOTDIR)/. $(CPPFLAGS) -c $< -o $(MAIN_O)

${LoadBalancer_O} $(ROOTDIR)/LoadBalancer.cpp: $(ROOTDIR)/../LoadBalancer/LoadBalancer.ice
	slice2cpp --output-dir $(ROOTDIR) $(ROOTDIR)/../LoadBalancer/LoadBalancer.ice 
	$(CC) -I${ICE_HOME_INC} -I$(ROOTDIR)/. -c $(ROOTDIR)/LoadBalancer.cpp -o ${LoadBalancer_O}

#rules:
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp 
	mkdir -p $(dir $@)
	$(VERBOSE)$(CC) -m64 $(CPPFLAGS) $(INCLUDES) -I$(ROOTDIR)/include -I$(CUDASDK_INC_PATH) $(CPP_LIBS) -o $@ -c $<




clean:
	rm -f $(ROOTDIR)/*.o $(ROOTDIR)/*.c $(ROOTDIR)/yacc.tab.h $(ROOTDIR)/parser $(ROOTDIR)/obj/*

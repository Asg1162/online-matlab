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

INCLUDES=-I${CULA_INC_PATH} -I$(ROOTDIR)/. -I${CUDA_INC_PATH}
GPU_LIBS=-L${CULA_LIB_PATH_64} -lcula -lcublas -lcudart
ICE_LIBS=-L${ICE_HOME_LIB} -lIce -lIceUtil

target: $(TARGET)

unittest: $(ROOTDIR)/unittest/bin/%


$(ROOTDIR)/unittest/bin/%: ${LoadBalancer_O} $(ROOTDIR)/unittest/src/%.cpp
	$(CC) -m64 $(CPPFLAGS) $(INCLUDES) $(GPU_LIBS) $(ICE_LIBS) $(LEX_YY_O) $(YACC_TAB_O) $(MAIN_O) $(SRC_OBJ) ${LoadBalancer_O} -lpthread -o $@ $(ROOTDIR)/unittest/$@.cpp
	

$(TARGET):$(LEX_YY_O) $(YACC_TAB_O) $(MAIN_O) $(SRC_OBJ) $(ROOTDIR)/Makefile ${LoadBalancer_O}
	$(CC) -m64 $(CPPFLAGS) $(INCLUDES) $(GPU_LIBS) $(ICE_LIBS) $(LEX_YY_O) $(YACC_TAB_O) $(MAIN_O) $(SRC_OBJ) ${LoadBalancer_O} -lpthread -o $(TARGET)

$(LEX_YY_O): $(LEX_YY_C) $(YACC_TAB_H)
	$(CC) $(CPPFLAGS) -I$(ROOTDIR)/include -c $(LEX_YY_C) -o $(LEX_YY_O)

$(ROOTDIR)/lexya_a.tab.o:$(YACC_TAB_C)
	$(CC) $(CPPFLAGS) -c $< -o $(ROOTDIR)/lexya_a.tab.o

$(YACC_TAB_O): $(YACC_TAB_C)
	$(CC) $(CPPFLAGS) -I$(ROOTDIR)/include -c $< -o $(YACC_TAB_O)

$(YACC_TAB_C) $(YACC_TAB_H): $(YACC_Y)
	$(YACC) -d -o $(YACC_TAB_C) $(YACC_Y)

$(LEX_YY_C):$(LEXYA_L)
	$(LEX) -o $(LEX_YY_C) $(LEXYA_L)

#	$(LEX) -d lexya.l

$(MAIN_O): $(ROOTDIR)/main.cpp  $(ROOTDIR)/LoadBalancer.cpp
	$(CC) -m64 -I$(ROOTDIR)/include -I$(ROOTDIR)/. -c $< -o $(MAIN_O)

${LoadBalancer_O} $(ROOTDIR)/LoadBalancer.cpp: $(ROOTDIR)/../LoadBalancer/LoadBalancer.ice
	slice2cpp --output-dir $(ROOTDIR) $(ROOTDIR)/../LoadBalancer/LoadBalancer.ice 
	$(CC) -I${ICE_HOME_INC} -I$(ROOTDIR)/. -c $(ROOTDIR)/LoadBalancer.cpp -o ${LoadBalancer_O}

#rules:
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp 
	mkdir -p $(dir $@)
	$(VERBOSE)$(CC) -m64 $(CPPFLAGS) $(INCLUDES) -I$(ROOTDIR)/include $(GPU_LIBS) -o $@ -c $<




clean:
	rm -f $(ROOTDIR)/*.o $(ROOTDIR)/*.c $(ROOTDIR)/yacc.tab.h $(ROOTDIR)/parser $(ROOTDIR)/obj/*

LEX=flex
YACC=bison
CC=g++

ROOTDIR ?= .
SRC_DIR = $(ROOTDIR)/src
OBJ_DIR = $(ROOTDIR)/obj

SRC = $(wildcard $(SRC_DIR)/*.cpp)
SRC_OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, ${SRC})

CPPFLAGS = -g -Wno-write-strings

TARGET = $(ROOTDIR)/parser
LEX_YY_O = $(ROOTDIR)/lex.yy.o
LEX_YY_C = $(ROOTDIR)/lex.yy.c
LEXYA_L = $(ROOTDIR)/lexya.l

YACC_TAB_O = $(ROOTDIR)/yacc.tab.o
YACC_TAB_C = $(ROOTDIR)/yacc.tab.c
YACC_TAB_H = $(ROOTDIR)/yacc.tab.h
YACC_Y = $(ROOTDIR)/yacc.y

MAIN_O = $(ROOTDIR)/main.o

CULA_INSTALL_PATH ?= $(HOME)/cula
CULA_INC_PATH =$(CULA_INSTALL_PATH)/include
CULA_LIB_PATH_32 =$(CULA_INSTALL_PATH)/lib32
CULA_LIB_PATH_64 =$(CULA_INSTALL_PATH)/lib64

INCLUDES=-I${CULA_INC_PATH}
GPU_LIBS=-L${CULA_LIB_PATH_64} -lcula -lcublas -lcudart

$(TARGET):$(LEX_YY_O) $(YACC_TAB_O) $(MAIN_O) $(SRC_OBJ) $(ROOTDIR)/Makefile
	$(CC) -m64 $(CPPFLAGS) $(INCLUDES) $(GPU_LIBS)  $(LEX_YY_O) $(YACC_TAB_O) $(MAIN_O) $(SRC_OBJ) -o $(TARGET)
#	$(CC) $(CPPFLAGS) lex.yy.o yacc.tab.o main.o $(SRC_OBJ) -o parser 

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


$(MAIN_O): $(ROOTDIR)/main.cpp
	$(CC) -m64 -I$(ROOTDIR)/include -c $< -o $(MAIN_O)

#rules:
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp 
	mkdir -p $(dir $@)
	$(VERBOSE)$(CC) -m64 $(CPPFLAGS) $(INCLUDES) -I$(ROOTDIR)/include $(GPU_LIBS) -o $@ -c $<




clean:
	rm -f $(ROOTDIR)/*.o $(ROOTDIR)/*.c $(ROOTDIR)/yacc.tab.h $(ROOTDIR)/parser $(ROOTDIR)/obj/*

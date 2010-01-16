ROOTDIR := ..

TEST_CASE_DIR := $(ROOTDIR)/unittest

TEST_CASE_SRC := $(wildcard $(TEST_CASE_DIR)/*.cpp)
TEST_BIN_DIR := $(ROOTDIR)/unittest/bin

TARGET := $(patsubst $(ROOTDIR)/unittest/%.cpp, $(TEST_BIN_DIR)/%, $(TEST_CASE_SRC))

include ../Makefile

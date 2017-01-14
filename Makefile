# Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file. See the AUTHORS file for names of contributors.

#-----------------------------------------------
# Modify prefix to specify the directory to install sofa-pbrpc.
#
PREFIX=./output
#-----------------------------------------------

#-----------------------------------------------
# Uncomment exactly one of the lines labelled (A), (B), and (C) below
# to switch between compilation modes.
#
OPT ?= -O2        # (A) Production use (optimized mode)
# OPT ?= -g2      # (B) Debug mode, generate full line-level debugging symbols
# OPT ?= -O2 -g2  # (C) Profiling mode: opt, but generate debugging symbols
#-----------------------------------------------

#-----------------------------------------------
# Customized macro switch:
#   SOFA_PBRPC_ENABLE_DETAILED_LOGGING : print current-time and thread-id in logging header
#   SOFA_PBRPC_ENABLE_FUNCTION_TRACE : print trace log when enter and leave function
#   SOFA_PBRPC_USE_SPINLOCK : use SpinLock as FastLock
#
CXXFLAGS ?= -DSOFA_PBRPC_ENABLE_DETAILED_LOGGING
#-----------------------------------------------

#-----------------------------------------------
# !!! Do not change the following lines !!!
#-----------------------------------------------

include depends.mk

LIB=libsofa-pbrpc.a
LIB_SRC=$(wildcard src/sofa/pbrpc/*.cc)
LIB_OBJ=$(patsubst %.cc,%.o,$(LIB_SRC))
PROTO=$(wildcard src/sofa/pbrpc/*.proto)
PROTO_SRC=$(patsubst %.proto,%.pb.cc,$(PROTO))
PROTO_HEADER=$(patsubst %.proto,%.pb.h,$(PROTO))
PROTO_OBJ=$(patsubst %.cc,%.o,$(PROTO_SRC))

BIN=sofa-pbrpc-client
BIN_SRC=$(wildcard src/sofa/pbrpc/http-agent/*.cc)
BIN_OBJ=$(patsubst %.cc,%.o,$(BIN_SRC))

PUB_INC=src/sofa/pbrpc/pbrpc.h src/sofa/pbrpc/closure_helper.h src/sofa/pbrpc/closure.h \
	src/sofa/pbrpc/ext_closure.h src/sofa/pbrpc/common.h src/sofa/pbrpc/rpc_channel.h \
	src/sofa/pbrpc/rpc_server.h src/sofa/pbrpc/rpc_client.h \
	src/sofa/pbrpc/rpc_controller.h src/sofa/pbrpc/rpc_error_code.h \
	src/sofa/pbrpc/mock_test_helper.h src/sofa/pbrpc/atomic.h src/sofa/pbrpc/counter.h \
	src/sofa/pbrpc/thread_group.h src/sofa/pbrpc/timeout_manager.h src/sofa/pbrpc/string_utils.h \
	src/sofa/pbrpc/locks.h src/sofa/pbrpc/mutex_lock.h src/sofa/pbrpc/spin_lock.h \
	src/sofa/pbrpc/fast_lock.h src/sofa/pbrpc/rw_lock.h src/sofa/pbrpc/scoped_locker.h \
	src/sofa/pbrpc/condition_variable.h src/sofa/pbrpc/wait_event.h src/sofa/pbrpc/http.h \
	src/sofa/pbrpc/buffer.h src/sofa/pbrpc/buf_handle.h src/sofa/pbrpc/profiling_linker.h \
	$(PROTO) $(PROTO_HEADER)

#-----------------------------------------------
ifeq ($(OS),Windows_NT)
    LDFLAGS += -lrt
    TARGET_DIRECTORY := --target-directory
else
    UNAME_S := $(shell uname -s)
    
    ifeq ($(UNAME_S),Linux)
        LDFLAGS += -lrt
        TARGET_DIRECTORY := --target-directory
    endif
    
    ifeq ($(UNAME_S),Darwin)
        TARGET_DIRECTORY := 
    endif
endif

ifndef CXX
    CXX=g++
endif

#-----------------------------------------------

INCPATH=-Isrc -I$(BOOST_HEADER_DIR) -I$(PROTOBUF_DIR)/include -I$(SNAPPY_DIR)/include -I$(ZLIB_DIR)/include
CXXFLAGS += $(OPT) -pipe -W -Wall -fPIC -D_GNU_SOURCE -D__STDC_LIMIT_MACROS -DHAVE_SNAPPY $(INCPATH)

LDFLAGS += -L$(ZLIB_DIR)/lib -L$(PROTOBUF_DIR)/lib/ -L$(SNAPPY_DIR)/lib/ -lprotobuf -lsnappy -lpthread -lz

.PHONY: check_depends build rebuild install clean

all: build

check_depends:
	@if [ ! -f "$(BOOST_HEADER_DIR)/boost/smart_ptr.hpp" ]; then echo "ERROR: need boost header"; exit 1; fi
	@if [ ! -f "$(PROTOBUF_DIR)/include/google/protobuf/message.h" ]; then echo "ERROR: need protobuf header"; exit 1; fi
	@if [ ! -f "$(PROTOBUF_DIR)/lib/libprotobuf.a" ]; then echo "ERROR: need protobuf lib"; exit 1; fi
	@if [ ! -f "$(SNAPPY_DIR)/include/snappy.h" ]; then echo "ERROR: need snappy header"; exit 1; fi
	@if [ ! -f "$(SNAPPY_DIR)/lib/libsnappy.a" ]; then echo "ERROR: need snappy lib"; exit 1; fi

clean:
	rm -f $(LIB) $(BIN) $(LIB_OBJ) $(PROTO_OBJ) $(BIN_OBJ) $(PROTO_HEADER) $(PROTO_SRC)

rebuild: clean all

$(PROTO_OBJ): $(PROTO_HEADER)

$(LIB_OBJ): $(PROTO_HEADER)

$(LIB): $(LIB_OBJ) $(PROTO_OBJ)
	ar crs $@ $(LIB_OBJ) $(PROTO_OBJ)

$(BIN): $(LIB) $(BIN_OBJ)
	$(CXX) $(BIN_OBJ) -o $@ $(LIB) $(LDFLAGS)

%.pb.cc %.pb.h: %.proto
	${PROTOBUF_DIR}/bin/protoc --proto_path=./src --proto_path=${PROTOBUF_DIR}/include --cpp_out=./src $<

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

build: $(LIB) $(BIN)
	@echo
	@echo 'Build succeed, run "make install" to install sofa-pbrpc to "'$(PREFIX)'".'

install: $(LIB) $(BIN)
	mkdir -p $(PREFIX)/include/sofa/pbrpc
	cp -r $(PUB_INC) $(TARGET_DIRECTORY) $(PREFIX)/include/sofa/pbrpc/
	mkdir -p $(PREFIX)/include/sofa/pbrpc/smart_ptr
	cp src/sofa/pbrpc/smart_ptr/*.hpp $(PREFIX)/include/sofa/pbrpc/smart_ptr
	mkdir -p $(PREFIX)/include/sofa/pbrpc/smart_ptr/detail
	cp src/sofa/pbrpc/smart_ptr/detail/*.hpp $(PREFIX)/include/sofa/pbrpc/smart_ptr/detail
	mkdir -p $(PREFIX)/lib
	cp $(LIB) $(PREFIX)/lib/
	mkdir -p $(PREFIX)/bin
	cp $(BIN) $(PREFIX)/bin/
	@echo
	@echo 'Install succeed, target directory is "'$(PREFIX)'".'


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
# !!! Do not change the following lines !!!
#-----------------------------------------------

include depends.mk

INC=src/sofa/pbrpc/pbrpc.h src/sofa/pbrpc/closure_helper.h src/sofa/pbrpc/closure.h \
	src/sofa/pbrpc/ext_closure.h src/sofa/pbrpc/common.h src/sofa/pbrpc/rpc_channel.h \
	src/sofa/pbrpc/rpc_client.h src/sofa/pbrpc/rpc_controller.h src/sofa/pbrpc/rpc_error_code.h \
	src/sofa/pbrpc/rpc_option.pb.h src/sofa/pbrpc/rpc_option.proto src/sofa/pbrpc/rpc_server.h \
	src/sofa/pbrpc/mock_test_helper.h src/sofa/pbrpc/atomic.h src/sofa/pbrpc/counter.h \
	src/sofa/pbrpc/thread_group.h src/sofa/pbrpc/timeout_manager.h \
	src/sofa/pbrpc/locks.h src/sofa/pbrpc/mutex_lock.h src/sofa/pbrpc/spin_lock.h \
	src/sofa/pbrpc/fast_lock.h src/sofa/pbrpc/rw_lock.h src/sofa/pbrpc/scoped_locker.h \
	src/sofa/pbrpc/condition_variable.h src/sofa/pbrpc/wait_event.h \
	src/sofa/pbrpc/builtin_service.proto src/sofa/pbrpc/builtin_service.pb.h

LIB=libsofa-pbrpc.a
LIB_SRC=$(wildcard src/sofa/pbrpc/*.cc)
LIB_OBJ=$(patsubst %.cc,%.o,$(LIB_SRC))
PROTO_SRC=$(wildcard src/sofa/pbrpc/*.pb.cc)
PROTO_HEADER=$(wildcard src/sofa/pbrpc/*.pb.h)

BIN=sofa-pbrpc-client
BIN_SRC=$(wildcard src/sofa/pbrpc/http-agent/*.cc)
BIN_OBJ=$(patsubst %.cc,%.o,$(BIN_SRC))

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

#-----------------------------------------------

CXX=g++
INCPATH=-Isrc -I$(BOOST_HEADER_DIR) -I$(PROTOBUF_DIR)/include -I$(SNAPPY_DIR)/include -I$(ZLIB_DIR)/include
CXXFLAGS += $(OPT) -pipe -W -Wall -fPIC -D_GNU_SOURCE -D__STDC_LIMIT_MACROS -DHAVE_SNAPPY $(INCPATH)

LDFLAGS += -L$(ZLIB_DIR)/lib -L$(PROTOBUF_DIR)/lib/ -L$(SNAPPY_DIR)/lib/ -lprotobuf -lsnappy -lpthread -lz

all: build

.PHONY: proto build install clean

clean:
	rm -f $(LIB) $(LIB_OBJ) $(BIN) $(BIN_OBJ) $(PROTO_HEADER) $(PROTO_SRC)

proto:
	cd src && sh compile_proto.sh

rebuild: clean all

$(LIB): $(LIB_OBJ)
	ar crs $@ $(LIB_OBJ)

$(BIN): $(LIB) $(BIN_OBJ)
	$(CXX) $(BIN_OBJ) -o $@ $(LIB) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c$< -o $@

build: proto $(LIB) $(BIN)
	@echo
	@echo 'Build succeed, run "make install" to install sofa-pbrpc.'

install: $(LIB) $(BIN)
	mkdir -p $(PREFIX)/include/sofa/pbrpc
	cp -r $(INC) $(TARGET_DIRECTORY) $(PREFIX)/include/sofa/pbrpc/
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


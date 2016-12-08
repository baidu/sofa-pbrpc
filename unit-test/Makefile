# Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file. See the AUTHORS file for names of contributors.

#-----------------------------------------------
# Uncomment exactly one of the lines labelled (A), (B), and (C) below
# to switch between compilation modes.
#
# OPT ?= -O2      # (A) Production use (optimized mode)
OPT ?= -g2        # (B) Debug mode, w/ full line-level debugging symbols
# OPT ?= -O2 -g2  # (C) Profiling mode: opt, but w/debugging symbols
#-----------------------------------------------

#-----------------------------------------------
# !!! Do not change the following lines !!!
#-----------------------------------------------

include ../depends.mk
include depends.mk

#-----------------------------------------------
CXX=g++
INCPATH=-I. -I../src -I$(BOOST_HEADER_DIR) -I$(PROTOBUF_DIR)/include \
		-I$(SNAPPY_DIR)/include -I$(ZLIB_DIR)/include -I$(GTEST_DIR)/include
CXXFLAGS += $(OPT) -pipe -W -Wall -fPIC -D_GNU_SOURCE -D__STDC_LIMIT_MACROS -fno-access-control $(INCPATH)

LDFLAGS += -L.. -L$(ZLIB_DIR)/lib -L$(PROTOBUF_DIR)/lib/ -L$(SNAPPY_DIR)/lib/ -lsofa-pbrpc -lprotobuf -lsnappy -lpthread -lz
ifeq ($(OS),Windows_NT)
    LDFLAGS += -lrt
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        LDFLAGS += -lrt
    endif
endif

PROTO_SRC=test_data.proto
PROTO_OBJ=$(patsubst %.proto,%.pb.o,$(PROTO_SRC))
PROTO_OPTIONS=--proto_path=. --proto_path=../src --proto_path=$(PROTOBUF_DIR)/include

TESTS = \
	test_common \
	test_atomic \
	test_epoll_support \
	test_tran_buf_pool \
	test_buffer \
	test_closure \
	test_ext_closure \
	test_thread_group \
	test_timeout_manager \
	test_io_service_pool \
	test_web_service

all: $(TESTS)

.PHONY: check_depends clean

check_depends:
	@if [ ! -f "$(BOOST_HEADER_DIR)/boost/smart_ptr.hpp" ]; then echo "ERROR: need boost header"; exit 1; fi
	@if [ ! -f "$(PROTOBUF_DIR)/include/google/protobuf/message.h" ]; then echo "ERROR: need protobuf header"; exit 1; fi
	@if [ ! -f "$(PROTOBUF_DIR)/lib/libprotobuf.a" ]; then echo "ERROR: need protobuf lib"; exit 1; fi
	@if [ ! -f "$(PROTOBUF_DIR)/bin/protoc" ]; then echo "ERROR: need protoc binary"; exit 1; fi
	@if [ ! -f "$(SNAPPY_DIR)/include/snappy.h" ]; then echo "ERROR: need snappy header"; exit 1; fi
	@if [ ! -f "$(SNAPPY_DIR)/lib/libsnappy.a" ]; then echo "ERROR: need snappy lib"; exit 1; fi
	@if [ ! -f "$(GTEST_DIR)/src/gtest-all.cc" ]; then echo "ERROR: need gtest"; exit 1; fi
	@if [ ! -f "../libsofa-pbrpc.a" ]; then echo "ERROR: should build sofa-pbrpc first"; exit 1; fi

clean:
	@rm -f $(TESTS) *.o *.pb.* libgtest.a

rebuild: clean all

test_common: test_common.o libgtest.a
	$(CXX) $^ -o $@ $(LDFLAGS)

test_atomic: test_atomic.o libgtest.a
	$(CXX) $^ -o $@ $(LIBRARY) $(LDFLAGS)

test_epoll_support: test_epoll_support.o libgtest.a
	$(CXX)  $^ -o $@ $(LIBRARY) $(LDFLAGS)

test_tran_buf_pool: test_tran_buf_pool.o libgtest.a
	$(CXX) $^ -o $@ $(LIBRARY) $(LDFLAGS)

test_buffer: test_data.pb.o test_buffer.o libgtest.a
	$(CXX) $^ -o $@ $(LIBRARY) $(LDFLAGS)

test_closure: test_closure.o libgtest.a
	$(CXX) $^ -o $@ $(LIBRARY) $(LDFLAGS)

test_ext_closure: test_ext_closure.o libgtest.a
	$(CXX) $^ -o $@ $(LIBRARY) $(LDFLAGS)

test_thread_group: test_thread_group.o libgtest.a
	$(CXX) $^ -o $@ $(LIBRARY) $(LDFLAGS)

test_timeout_manager: test_timeout_manager.o libgtest.a
	$(CXX) $^ -o $@ $(LIBRARY) $(LDFLAGS)

test_io_service_pool: test_io_service_pool.o libgtest.a
	$(CXX) $^ -o $@ $(LIBRARY) $(LDFLAGS)

test_web_service: test_web_service.o libgtest.a
	$(CXX) $^ -o $@ $(LIBRARY) $(LDFLAGS)

%.pb.o: %.pb.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.pb.cc: %.proto
	$(PROTOBUF_DIR)/bin/protoc $(PROTO_OPTIONS) --cpp_out=. $<

%.o: %.cc $(PROTO_OBJ)
	$(CXX) $(CXXFLAGS) -c $< -o $@

libgtest.a:
	g++ -isystem $(GTEST_DIR)/include -I$(GTEST_DIR) -pthread -c $(GTEST_DIR)/src/gtest-all.cc
	ar -rv libgtest.a gtest-all.o


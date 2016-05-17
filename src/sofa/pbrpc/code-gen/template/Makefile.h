// Copyright (c) 2016 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: shichengyi@baidu.com (Shi Chengyi)

#ifndef _SOFA_PBRPC_MAKEFILE_H_
#define _SOFA_PBRPC_MAKEFILE_H_

namespace sofa {
namespace pbrpc {
namespace code_gen {

static const char* Makefile = 
    "# Copyright (c) 2014 Baidu.com, Inc. All rights reserved."
    "\n# Use of this source code is governed by a BSD-style license that can be"
    "\n# found in the LICENSE file. See the AUTHORS file for names of contributors."
    "\n"
    "\n#-----------------------------------------------"
    "\n## Sofa-pbrpc path containing `include'and `lib'."
    "\n##"
    "\n## Check file exist:"
    "\n##   $(SOFA_PBRPC)/include/sofa/pbrpc/pbrpc.h "
    "\n##   $(SOFA_PBRPC)/lib/libsofa-pbrpc.a "
    "\n##"
    "\nSOFA_PBRPC=&PBRPC_ROOT&/output"
    "\n#-----------------------------------------------"
    "\n"
    "\n#-----------------------------------------------"
    "\n# Uncomment exactly one of the lines labelled (A), (B), and (C) below"
    "\n# to switch between compilation modes."
    "\n#"
    "\nOPT ?= -O2        # (A) Production use (optimized mode)"
    "\n# OPT ?= -g2      # (B) Debug mode, w/ full line-level debugging symbols"
    "\n# OPT ?= -O2 -g2  # (C) Profiling mode: opt, but w/debugging symbols"
    "\n#-----------------------------------------------"
    "\n"
    "\n#-----------------------------------------------"
    "\n# !!! Do not change the following lines !!!"
    "\n#-----------------------------------------------"
    "\n"
    "\ninclude &PBRPC_ROOT&/depends.mk"
    "\n"
    "\nCXX=g++"
    "\nINCPATH=-I. -I$(SOFA_PBRPC)/include -I$(PROTOBUF_DIR)/include \\"
    "\n		-I$(SNAPPY_DIR)/include -I$(ZLIB_DIR)/include"
    "\nCXXFLAGS += $(OPT) -pipe -W -Wall -fPIC -D_GNU_SOURCE -D__STDC_LIMIT_MACROS $(INCPATH)"
    "\n"
    "\nLIBRARY=$(SOFA_PBRPC)/lib/libsofa-pbrpc.a $(PROTOBUF_DIR)/lib/libprotobuf.a $(SNAPPY_DIR)/lib/libsnappy.a"
    "\nLDFLAGS += -L$(ZLIB_DIR)/lib -lpthread -lz"
    "\n"
    "\nUNAME_S := $(shell uname -s)"
    "\nifeq ($(UNAME_S),Linux)"
    "\n    LDFLAGS += -lrt"
    "\nendif"
    "\n"
    "\nPROTO_SRC=&PROTOS&"
    "\nPROTO_OBJ=$(patsubst %.proto,%.pb.o,$(PROTO_SRC))"
    "\nPROTO_OPTIONS=--proto_path=. --proto_path=$(SOFA_PBRPC)/include --proto_path=$(PROTOBUF_DIR)/include"
    "\n"
    "\nBIN=server"
    "\n"
    "\nall: check_depends $(BIN)"
    "\n"
    "\n.PHONY: check_depends clean"
    "\n"
    "\ncheck_depends:"
    "\n	@if [ ! -f \"$(PROTOBUF_DIR)/include/google/protobuf/message.h\" ]; then echo \"ERROR: need protobuf header\"; exit 1; fi"
    "\n	@if [ ! -f \"$(PROTOBUF_DIR)/lib/libprotobuf.a\" ]; then echo \"ERROR: need protobuf lib\"; exit 1; fi"
    "\n	@if [ ! -f \"$(PROTOBUF_DIR)/bin/protoc\" ]; then echo \"ERROR: need protoc binary\"; exit 1; fi"
    "\n	@if [ ! -f \"$(SNAPPY_DIR)/include/snappy.h\" ]; then echo \"ERROR: need snappy header\"; exit 1; fi"
    "\n	@if [ ! -f \"$(SNAPPY_DIR)/lib/libsnappy.a\" ]; then echo \"ERROR: need snappy lib\"; exit 1; fi"
    "\n	@if [ ! -f \"$(SOFA_PBRPC)/include/sofa/pbrpc/pbrpc.h\" ]; then echo \"ERROR: need sofa-pbrpc header\"; exit 1; fi"
    "\n	@if [ ! -f \"$(SOFA_PBRPC)/lib/libsofa-pbrpc.a\" ]; then echo \"ERROR: need sofa-pbrpc lib\"; exit 1; fi"
    "\n"
    "\nclean:"
    "\n	@rm -f $(BIN) *.o *.pb.*"
    "\n"
    "\nrebuild: clean all"
    "\n"
    "\nserver: $(PROTO_OBJ) main.o EchoServerImpl.o"
    "\n	$(CXX) $^ -o $@ $(LIBRARY) $(LDFLAGS)"
    "\n"
    "\nclient_sync: $(PROTO_OBJ) client_sync.o"
    "\n	$(CXX) $^ -o $@ $(LIBRARY) $(LDFLAGS)"
    "\n"
    "\nclient_async: $(PROTO_OBJ) client_async.o"
    "\n	$(CXX) $^ -o $@ $(LIBRARY) $(LDFLAGS)"
    "\n"
    "\n%.pb.o: %.pb.cc"
    "\n	$(CXX) $(CXXFLAGS) -c $< -o $@"
    "\n"
    "\n%.pb.cc: %.proto"
    "\n	$(PROTOBUF_DIR)/bin/protoc $(PROTO_OPTIONS) --cpp_out=. $<"
    "\n"
    "\n%.o: %.cc $(PROTO_OBJ)"
    "\n	$(CXX) $(CXXFLAGS) -c $< -o $@"
    "\n";

}
}
}

#endif

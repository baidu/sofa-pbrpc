#! /usr/bin/python
#
# See README for usage instructions.

from ez_setup import use_setuptools
use_setuptools()

from setuptools import setup
import sys
import os
import subprocess

# Find the Protocol Compiler.
proto_dir = None
protoc = None
for line in open("../depends.mk").readlines():
  line = line.strip()
  if line.startswith('PROTOBUF_DIR'):
    proto_dir = line[line.find('=')+1:]
    protoc = proto_dir + "/bin/protoc"
    if os.path.exists(protoc):
      break
    else:
      proto_dir = None
      protoc = None

if protoc == None:
  sys.stderr.write(
      "protoc is not found, please set PROTOBUF_DIR in `../depends.mk'.\n")
  sys.exit(-1)

def generate_proto(source):
  """Invokes the Protocol Compiler to generate a _pb2.py from the given
  .proto file.  Does nothing if the output already exists and is newer than
  the input."""

  output = source.replace(".proto", "_pb2.py").replace("../src/", "")

  if not os.path.exists(source):
    print "Can't find required file: " + source
    sys.exit(-1)

  if (not os.path.exists(output) or
      (os.path.exists(source) and
       os.path.getmtime(source) > os.path.getmtime(output))):
    print "Generating %s..." % output
    protoc_command = [ protoc, "-I../src", "-I.",
        "-I"+proto_dir+"/include", "--python_out=.", source ]
    if subprocess.call(protoc_command) != 0:
      sys.exit(-1)

if __name__ == '__main__':
  if len(sys.argv) >= 2 and sys.argv[1] == "clean":
    # Delete generated _pb2.py files and .pyc files in the code tree.
    for (dirpath, dirnames, filenames) in os.walk("."):
      for filename in filenames:
        filepath = os.path.join(dirpath, filename)
        if filepath.endswith("_pb2.py") or filepath.endswith(".pyc"):
          os.remove(filepath)
  else:
    generate_proto("../src/sofa/pbrpc/rpc_option.proto")
    generate_proto("../src/sofa/pbrpc/rpc_meta.proto")
    generate_proto("../src/sofa/pbrpc/builtin_service.proto")

  setup(name = 'sofa-pbrpc',
        version = '1.0.1',
        description = 'python client for sofa-pbrpc',
        maintainer = 'qinzuoyan@gmail.com',
        maintainer_email = 'qinzuoyan@gmail.com',
        url = 'https://github.com/BaiduPS/sofa-pbrpc',
        license = 'BSD License',
        packages = [ 'sofa' ],
        namespace_packages = [ 'sofa' ],
        py_modules = [
          'sofa.pbrpc.client',
          'sofa.pbrpc.rpc_option_pb2',
          'sofa.pbrpc.rpc_meta_pb2',
          'sofa.pbrpc.builtin_service_pb2' ],
        )

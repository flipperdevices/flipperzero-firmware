'''This file automatically rebuilds the proto definitions for Python.'''
from __future__ import absolute_import

import os.path
import sys

import pkg_resources

from ._utils import has_grpcio_protoc, invoke_protoc

dirname = os.path.dirname(__file__)
protosrc = os.path.join(dirname, "nanopb.proto")
protodst = os.path.join(dirname, "nanopb_pb2.py")

if os.path.isfile(protosrc):
    src_date = os.path.getmtime(protosrc)
    if not os.path.isfile(protodst) or os.path.getmtime(protodst) < src_date:

        cmd = [
            "protoc",
            "--python_out={}".format(dirname),
            protosrc,
            "-I={}".format(dirname),
        ]

        if has_grpcio_protoc():
            # grpcio-tools has an extra CLI argument
            # from grpc.tools.protoc __main__ invocation.
            _builtin_proto_include = pkg_resources.resource_filename('grpc_tools', '_proto')

            cmd.append("-I={}".format(_builtin_proto_include))
        try:
            invoke_protoc(argv=cmd)
        except:
            sys.stderr.write("Failed to build nanopb_pb2.py: " + ' '.join(cmd) + "\n")
            raise

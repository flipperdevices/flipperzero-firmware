import subprocess
import os.path

def has_grpcio_protoc():
    # type: () -> bool
    """ checks if grpcio-tools protoc is installed"""

    try:
        import grpc_tools.protoc
    except ImportError:
        return False
    return True


def invoke_protoc(argv):
    # type: (list) -> typing.Any
    """
    Invoke protoc.

    This routine will use grpcio-provided protoc if it exists,
    using system-installed protoc as a fallback.

    Args:
        argv: protoc CLI invocation, first item must be 'protoc'
    """

    # Add current directory to include path if nothing else is specified
    if not [x for x in argv if x.startswith('-I')]:
        argv.append("-I.")

    # Add default protoc include paths
    nanopb_include = os.path.dirname(os.path.abspath(__file__))
    argv.append('-I' + nanopb_include)

    if has_grpcio_protoc():
        import grpc_tools.protoc as protoc
        import pkg_resources
        proto_include = pkg_resources.resource_filename('grpc_tools', '_proto')
        argv.append('-I' + proto_include)

        return protoc.main(argv)
    else:
        return subprocess.call(argv)

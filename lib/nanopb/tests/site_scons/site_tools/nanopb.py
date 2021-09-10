'''
Scons Builder for nanopb .proto definitions.

This tool will locate the nanopb generator and use it to generate .pb.c and
.pb.h files from the .proto files.

Basic example
-------------
# Build myproto.pb.c and myproto.pb.h from myproto.proto
myproto = env.NanopbProto("myproto")

# Link nanopb core to the program
env.Append(CPPPATH = "$NANOB")
myprog = env.Program(["myprog.c", myproto, "$NANOPB/pb_encode.c", "$NANOPB/pb_decode.c"])

Configuration options
---------------------
Normally, this script is used in the test environment of nanopb and it locates
the nanopb generator by a relative path. If this script is used in another
application, the path to nanopb root directory has to be defined:

env.SetDefault(NANOPB = "path/to/nanopb")

Additionally, the path to protoc and the options to give to protoc can be
defined manually:

env.SetDefault(PROTOC = "path/to/protoc")
env.SetDefault(PROTOCFLAGS = "--plugin=protoc-gen-nanopb=path/to/protoc-gen-nanopb")
'''

import SCons.Action
import SCons.Builder
import SCons.Util
from SCons.Script import Dir, File
import os.path
import platform

try:
    warningbase = SCons.Warnings.SConsWarning
except AttributeError:
    warningbase = SCons.Warnings.Warning

class NanopbWarning(warningbase):
    pass
SCons.Warnings.enableWarningClass(NanopbWarning)

def _detect_nanopb(env):
    '''Find the path to nanopb root directory.'''
    if env.has_key('NANOPB'):
        # Use nanopb dir given by user
        return env['NANOPB']

    p = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
    if os.path.isdir(p) and os.path.isfile(os.path.join(p, 'pb.h')):
        # Assume we are running under tests/site_scons/site_tools
        return p

    raise SCons.Errors.StopError(NanopbWarning,
        "Could not find the nanopb root directory")

def _detect_python(env):
    '''Find Python executable to use.'''
    if env.has_key('PYTHON'):
        return env['PYTHON']

    p = env.WhereIs('python3')
    if p:
        return env['ESCAPE'](p)

    p = env.WhereIs('py.exe')
    if p:
        return env['ESCAPE'](p) + " -3"

    return env['ESCAPE'](sys.executable)

def _detect_nanopb_generator(env):
    '''Return command for running nanopb_generator.py'''
    generator_cmd = os.path.join(env['NANOPB'], 'generator-bin', 'nanopb_generator' + env['PROGSUFFIX'])
    if os.path.exists(generator_cmd):
        # Binary package
        return env['ESCAPE'](generator_cmd)
    else:
        # Source package
        return env['PYTHON'] + " " + env['ESCAPE'](os.path.join(env['NANOPB'], 'generator', 'nanopb_generator.py'))

def _detect_protoc(env):
    '''Find the path to the protoc compiler.'''
    if env.has_key('PROTOC'):
        # Use protoc defined by user
        return env['PROTOC']

    n = _detect_nanopb(env)
    p1 = os.path.join(n, 'generator-bin', 'protoc' + env['PROGSUFFIX'])
    if os.path.exists(p1):
        # Use protoc bundled with binary package
        return env['ESCAPE'](p1)

    p = os.path.join(n, 'generator', 'protoc')
    if os.path.exists(p):
        # Use the grcpio-tools protoc wrapper
        if env['PLATFORM'] == 'win32':
            return env['ESCAPE'](p + '.bat')
        else:
            return env['ESCAPE'](p)

    p = env.WhereIs('protoc')
    if p:
        # Use protoc from path
        return env['ESCAPE'](p)

    raise SCons.Errors.StopError(NanopbWarning,
        "Could not find the protoc compiler")

def _detect_protocflags(env):
    '''Find the options to use for protoc.'''
    if env.has_key('PROTOCFLAGS'):
        return env['PROTOCFLAGS']

    p = _detect_protoc(env)
    n = _detect_nanopb(env)
    p1 = os.path.join(n, 'generator', 'protoc' + env['PROGSUFFIX'])
    p2 = os.path.join(n, 'generator-bin', 'protoc' + env['PROGSUFFIX'])
    if p in [env['ESCAPE'](p1), env['ESCAPE'](p2)]:
        # Using the bundled protoc, no options needed
        return ''

    e = env['ESCAPE']
    if env['PLATFORM'] == 'win32':
        return e('--plugin=protoc-gen-nanopb=' + os.path.join(n, 'generator', 'protoc-gen-nanopb.bat'))
    else:
        return e('--plugin=protoc-gen-nanopb=' + os.path.join(n, 'generator', 'protoc-gen-nanopb'))

def _nanopb_proto_actions(source, target, env, for_signature):
    esc = env['ESCAPE']

    # Make protoc build inside the SConscript directory
    prefix = os.path.dirname(str(source[-1]))
    srcfile = esc(os.path.relpath(str(source[0]), prefix))
    include_dirs = '-I.'
    for d in env['PROTOCPATH']:
        d = env.GetBuildPath(d)
        if not os.path.isabs(d): d = os.path.relpath(d, prefix)
        include_dirs += ' -I' + esc(d)

    # when generating .pb.cpp sources, instead of pb.h generate .pb.hpp headers
    source_extension = os.path.splitext(str(target[0]))[1]
    header_extension = '.h' + source_extension[2:]
    nanopb_flags = env['NANOPBFLAGS']
    if nanopb_flags:
      nanopb_flags = '--source-extension=%s,--header-extension=%s,%s:.' % (source_extension, header_extension, nanopb_flags)
    else:
      nanopb_flags = '--source-extension=%s,--header-extension=%s:.' % (source_extension, header_extension)

    return SCons.Action.CommandAction('$PROTOC $PROTOCFLAGS %s --nanopb_out=%s %s' % (include_dirs, nanopb_flags, srcfile),
                                      chdir = prefix)

def _nanopb_proto_emitter(target, source, env):
    basename = os.path.splitext(str(source[0]))[0]
    source_extension = os.path.splitext(str(target[0]))[1]
    header_extension = '.h' + source_extension[2:]
    target.append(basename + '.pb' + header_extension)

    # This is a bit of a hack. protoc include paths work the sanest
    # when the working directory is the same as the source root directory.
    # To get that directory in _nanopb_proto_actions, we add SConscript to
    # the list of source files.
    source.append(File("SConscript"))

    if os.path.exists(basename + '.options'):
        source.append(basename + '.options')

    return target, source

_nanopb_proto_builder = SCons.Builder.Builder(
    generator = _nanopb_proto_actions,
    suffix = '.pb.c',
    src_suffix = '.proto',
    emitter = _nanopb_proto_emitter)

_nanopb_proto_cpp_builder = SCons.Builder.Builder(
    generator = _nanopb_proto_actions,
    suffix = '.pb.cpp',
    src_suffix = '.proto',
    emitter = _nanopb_proto_emitter)

def generate(env):
    '''Add Builder for nanopb protos.'''

    env['NANOPB'] = _detect_nanopb(env)
    env['PROTOC'] = _detect_protoc(env)
    env['PROTOCFLAGS'] = _detect_protocflags(env)
    env['PYTHON'] = _detect_python(env)
    env['NANOPB_GENERATOR'] = _detect_nanopb_generator(env)
    env.SetDefault(NANOPBFLAGS = '')

    env.SetDefault(PROTOCPATH = [".", os.path.join(env['NANOPB'], 'generator', 'proto')])

    env.SetDefault(NANOPB_PROTO_CMD = '$PROTOC $PROTOCFLAGS --nanopb_out=$NANOPBFLAGS:. $SOURCES')
    env['BUILDERS']['NanopbProto'] = _nanopb_proto_builder
    env['BUILDERS']['NanopbProtoCpp'] = _nanopb_proto_cpp_builder

def exists(env):
    return _detect_protoc(env) and _detect_protoc_opts(env)


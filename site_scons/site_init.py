import sys
import os
import atexit
from ansi.color import fg, fx
from SCons.Script import GetBuildFailures
import SCons.Errors

# necessary paths
sys.path.insert(0, os.path.join(os.getcwd(), "scripts"))
sys.path.insert(0, os.path.join(os.getcwd(), "lib/cxxheaderparser"))


def bf_to_str(bf):
    """Convert an element of GetBuildFailures() to a string
    in a useful way."""
    if bf is None:  # unknown targets produce None in the list
        return "(unknown tgt)"
    elif isinstance(bf, Errors.StopError):
        return fg.yellow(str(bf))
    elif bf.node:
        return f"{fg.yellow(str(bf.node))}: {bf.errstr}"
    elif bf.filename:
        return f"{fg.yellow(bf.filename)}: {bf.errstr}"
    return f"{fg.yellow('unknown failure')}: {bf.errstr}"


def display_build_status():
    """Display the build status.  Called by atexit.
    Here you could do all kinds of complicated things."""
    bf = GetBuildFailures()
    if bf:
        # bf is normally a list of build failures; if an element is None,
        # it's because of a target that SCons doesn't know anything about.
        failures_message = "\n".join([bf_to_str(x) for x in bf if x is not None])
        print()
        print(f"{fg.brightred(fx.bold('*' * 10 + ' FBT ERRORS ' + '*' * 10))}")
        print(failures_message)


atexit.register(display_build_status)

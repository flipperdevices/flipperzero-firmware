from SCons.Builder import Builder
from SCons.Action import Action


def generate(env):
    env.SetDefault(
        GDB="gdb",
        GDBPY="gdb-py",
        GDBFLAGS=[],
        GDBPYFLAGS=[],
        GDBCOM="$GDB $GDBFLAGS $SOURCES $TARGET",
        GDBPYCOM="$GDBPY $GDBFLAGS $GDBPYFLAGS $SOURCES",
    )
    env.Append(
        BUILDERS={
            "GDB": Builder(
                action=Action(
                    "${GDBCOM}",
                    "${GDBCOMSTR}",
                ),
            ),
            "GDBPy": Builder(
                action=Action(
                    "${GDBPYCOM}",
                    "${GDBPYCOMSTR}",
                ),
            ),
        }
    )


def exists(env):
    return True

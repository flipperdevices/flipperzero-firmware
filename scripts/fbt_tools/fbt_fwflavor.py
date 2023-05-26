"""
Use this file to introduce changes to firmware build environment
that are specific to your fork. For example, you can add new
defines that will be a part of SDK build, so applications can
use them for conditional compilation.
"""


def PreConfigureFwEnvionment(env):
    """
    This function is called on firmware environment (incl. updater)
    before any major configuration is done.
    """
    # print("PreConfigureFwEnvionment")
    env.Append(
        CPPDEFINES=["FW_OFFICIAL"],
    )


def PostConfigureFwEnvionment(env):
    """
    This function is called on firmware environment (incl. updater)
    after all configuration is done.
    """
    # print("PostConfigureFwEnvionment")
    pass


def PreConfigureUfbtEnvionment(env):
    """
    This function is called on ufbt environment at the beginning of
    its configuration, before dist environment is created.
    """
    # print("PreConfigureUfbtEnvionment")
    pass


def PostConfigureUfbtEnvionment(env):
    # print("PostConfigureUfbtEnvionment")
    """
    This function is called on ufbt dist_env environment after all
    configuration and target creation is done.
    """
    pass


def generate(env):
    env.AddMethod(PreConfigureFwEnvionment)
    env.AddMethod(PostConfigureFwEnvionment)
    env.AddMethod(PreConfigureUfbtEnvionment)
    env.AddMethod(PostConfigureUfbtEnvionment)


def exists():
    return True

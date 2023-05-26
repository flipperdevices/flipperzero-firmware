"""

To introduce changes to firmware build environment that are specific to your fork:

    create a file "scripts/fbt/fbt_hooks.py"

With it, you can define functions that will be called at specific points of
firmware build configuration, with environment as an argument.

For example, you can define a function `PreConfigureFwEnvionment(env)` that
defines that will be a part of SDK build, so applications can
use them for conditional compilation.

Here is a list of all available hooks:

    PreConfigureFwEnvionment(env):
        This function is called on firmware environment (incl. updater)
        before any major configuration is done.

    PostConfigureFwEnvionment(env):
        This function is called on firmware environment (incl. updater)
        after all configuration is done.

    PreConfigureUfbtEnvionment(env):
        This function is called on ufbt environment at the beginning of
        its configuration, before dist environment is created.

    PostConfigureUfbtEnvionment(env):
        This function is called on ufbt dist_env environment after all
        configuration and target creation is done.
"""

try:
    from fbt import fbt_hooks
except ImportError:

    class DefaultFbtHooks:
        @staticmethod
        def PreConfigureFwEnvionment(env):
            # print("PreConfigureFwEnvionment")
            env.Append(
                CPPDEFINES=["FW_OFFICIAL"],
            )

    fbt_hooks = DefaultFbtHooks()


def generate(env):
    stub_hook = lambda env: None
    control_hooks = [
        "PreConfigureFwEnvionment",
        "PostConfigureFwEnvionment",
        "PreConfigureUfbtEnvionment",
        "PostConfigureUfbtEnvionment",
    ]
    for hook_name in control_hooks:
        hook_fn = getattr(fbt_hooks, hook_name, stub_hook)
        env.AddMethod(hook_fn, hook_name)


def exists():
    return True

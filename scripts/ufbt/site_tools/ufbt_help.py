targets_help = """Configuration variables:
"""

tail_help = """

TASKS:
    (* - not supported yet)

    vscode_dist:
        Configure application in current directory for development in VSCode.
    app_template:
        Copy application template to current directory.

Building:
    faps:
        Build all FAP apps
    fap_{APPID}, launch_app APPSRC={APPID}:
        Build FAP app with appid={APPID}; upload & start it over USB

Flashing & debugging:
    flash, flash_blackmagic, *jflash:
        Flash firmware to target using debug probe
    flash_usb, flash_usb_full:
        Install firmware using self-update package
    debug, debug_other, blackmagic:
        Start GDB

Other:
    cli:
        Open a Flipper CLI session over USB
    *lint, *lint_py:
        run linters
    *format, *format_py:
        run code formatters
"""


def generate(env, **kw):
    vars = kw["vars"]
    basic_help = vars.GenerateHelpText(env)
    env.Help(targets_help + basic_help + tail_help)


def exists(env):
    return True

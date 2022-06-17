import os


def BuildAppElf(env, app):
    work_dir = env.subst("$EXT_APPS_WORK_DIR")
    app_elf = env.Program(
        os.path.join(work_dir, app.appid),
        env.GlobRecursive("*.c*", os.path.join(work_dir, app._appdir)),
        APP_ENTRY=app.entry_point,
    )
    app_stripped_elf = env.ELFStripper(
        os.path.join(env.subst("$PLUGIN_ELF_DIR"), app.appid), app_elf
    )
    env.Alias(f"{env['FIRMWARE_BUILD_CFG']}_{app.appid}", app_stripped_elf)
    return app_stripped_elf


def generate(env, **kw):
    env.SetDefault(EXT_APPS_WORK_DIR=kw.get("EXT_APPS_WORK_DIR", ".extapps"))
    env.VariantDir(env.subst("$EXT_APPS_WORK_DIR"), ".", duplicate=False)
    env.AddMethod(BuildAppElf)


def exists(env):
    return True

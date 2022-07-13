from SCons.Builder import Builder
from SCons.Action import Action
from SCons.Errors import UserError

import os
from fbt.elfmanifest import assemble_manifest_data
from fbt.sdk import SdkCache


def BuildAppElf(env, app):
    work_dir = env.subst("$EXT_APPS_WORK_DIR")
    app_target_name = os.path.join(work_dir, app.appid)
    app_alias = f"{env['FIRMWARE_BUILD_CFG']}_{app.appid}"
    app_original = app_target_name + "_raw"
    app_elf_raw = env.Program(
        app_original,
        env.GlobRecursive("*.c*", os.path.join(work_dir, app._appdir)),
        APP_ENTRY=app.entry_point,
    )

    app_elf_dump = env.ObjDump(app_elf_raw)
    env.Alias(f"{app_alias}_list", app_elf_dump)

    app_elf_augmented = env.EmbedAppMetadata(
        app_target_name,
        app_elf_raw,
        APP=app,
        SDK_DEF=env.File("#/firmware/targets/f${TARGET_HW}/api_symbols.csv"),
        # APPMETA=f"{app.appid}.meta",
    )

    app_stripped_elf = env.ELFStripper(
        os.path.join(env.subst("$PLUGIN_ELF_DIR"), app.appid), app_elf_augmented
    )
    env.Alias(app_alias, app_stripped_elf)
    return app_stripped_elf


def prepare_app_metadata(target, source, env):
    print(f"prepare_app_metadata: {target}, {source}")

    sdk_cache = SdkCache(env["SDK_DEF"].path, load_version_only=True)
    if not sdk_cache.is_buildable():
        raise UserError(
            "SDK version is not finalized, please review changes and re-run operation"
        )

    app = env["APP"]
    meta_file_name = target[0].path + ".meta"
    with open(meta_file_name, "wb") as f:

        # f.write(f"hello this is {app}")
        f.write(assemble_manifest_data(app, sdk_cache.version.as_int()))


def generate(env, **kw):
    env.SetDefault(EXT_APPS_WORK_DIR=kw.get("EXT_APPS_WORK_DIR", ".extapps"))
    env.VariantDir(env.subst("$EXT_APPS_WORK_DIR"), ".", duplicate=False)
    env.AddMethod(BuildAppElf)
    env.Append(
        BUILDERS={
            "EmbedAppMetadata": Builder(
                # generator=gen_embed_app_metadata,
                action=[
                    prepare_app_metadata,
                    # embed_app_metadata,
                    "${OBJCOPY} --add-section .fzmeta=${TARGET}.meta --set-section-flags .fzmeta=contents,data,readonly ${SOURCES} ${TARGET}",
                ],
                suffix=".elf",
                src_suffix=".elf",
            )
        }
    )


def exists(env):
    return True

from SCons.Builder import Builder
from SCons.Action import Action
from SCons.Errors import UserError

import os
from fbt.elfmanifest import assemble_manifest_data
from fbt.sdk import SdkCache


def BuildAppElf(env, app):
    work_dir = env.subst("$EXT_APPS_WORK_DIR")

    app_alias = f"{env['FIRMWARE_BUILD_CFG']}_{app.appid}"
    app_original = os.path.join(work_dir, f"{app.appid}_d")
    app_elf_raw = env.Program(
        app_original,
        env.GlobRecursive("*.c*", os.path.join(work_dir, app._appdir)),
        APP_ENTRY=app.entry_point,
    )

    app_elf_dump = env.ObjDump(app_elf_raw)
    env.Alias(f"{app_alias}_list", app_elf_dump)

    app_elf_augmented = env.EmbedAppMetadata(
        os.path.join(env.subst("$PLUGIN_ELF_DIR"), app.appid),
        app_elf_raw,
        APP=app,
    )
    env.Depends(app_elf_augmented, [env["SDK_DEFINITION"], env.Value(app)])
    env.Alias(app_alias, app_elf_augmented)
    return (app_elf_augmented, app_elf_raw)


def prepare_app_metadata(target, source, env):
    sdk_cache = SdkCache(env.subst("$SDK_DEFINITION"), load_version_only=True)
    if not sdk_cache.is_buildable():
        raise UserError(
            "SDK version is not finalized, please review changes and re-run operation"
        )

    app = env["APP"]
    meta_file_name = source[0].path + ".meta"
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
                    Action(prepare_app_metadata, "$APPMETA_COMSTR"),
                    # embed_app_metadata,
                    Action(
                        "${OBJCOPY} "
                        "--remove-section .ARM.attributes "
                        "--add-section .fzmeta=${SOURCE}.meta "
                        "--set-section-flags .fzmeta=contents,noload,readonly,data "
                        "--strip-debug --strip-unneeded "
                        "${SOURCES} ${TARGET}",
                        "$APPMETAEMBED_COMSTR",
                    ),
                ],
                suffix=".fap",
                src_suffix=".elf",
            )
        }
    )


def exists(env):
    return True

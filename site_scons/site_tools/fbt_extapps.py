from SCons.Builder import Builder
from SCons.Action import Action
from SCons.Errors import UserError
import SCons.Warnings

import os
import pathlib
from fbt.elfmanifest import assemble_manifest_data
from fbt.appmanifest import FlipperManifestException
from fbt.sdk import SdkCache
import itertools

from site_scons.fbt.appmanifest import FlipperApplication


def BuildAppElf(env, app):
    apps_work_dir = env.subst("$EXT_APPS_WORK_DIR")
    app_work_dir = os.path.join(apps_work_dir, app.appid)

    env.VariantDir(app_work_dir, app._appdir, duplicate=False)

    app_alias = f"{env['FIRMWARE_BUILD_CFG']}_{app.appid}"
    app_original_elf = os.path.join(app_work_dir, f"{app.appid}_d")

    if app.fap_assets:
        env.CompileIcons(
            env.Dir(app_work_dir),
            app._appdir.Dir(app.fap_assets),
            icon_bundle_name=f"{app.appid}_icons",
        )

    app_sources = list(
        itertools.chain.from_iterable(
            env.GlobRecursive(
                source_type,
                app_work_dir,
            )
            for source_type in app.sources
        )
    )

    app_env = env.Clone()
    app_env.Append(
        LIBS=app.fap_libs,
        CPPPATH=env.Dir(app_work_dir),
    )

    app_elf_raw = app_env.Program(
        app_original_elf,
        app_sources,
        APP_ENTRY=app.entry_point,
    )

    app_elf_dump = app_env.ObjDump(app_elf_raw)
    app_env.Alias(f"{app_alias}_list", app_elf_dump)

    app_elf_augmented = app_env.EmbedAppMetadata(
        os.path.join(app_env.subst("$PLUGIN_ELF_DIR"), app.appid),
        app_elf_raw,
        APP=app,
    )

    manifest_vals = {
        k: v
        for k, v in vars(app).items()
        if k.startswith(FlipperApplication.PRIVATE_FIELD_PREFIX)
    }

    app_env.Depends(
        app_elf_augmented,
        [app_env["SDK_DEFINITION"], app_env.Value(manifest_vals)],
    )
    if app.fap_icon:
        app_env.Depends(
            app_elf_augmented,
            app_env.File(f"{app._apppath}/{app.fap_icon}"),
        )
    app_env.Alias(app_alias, app_elf_augmented)

    app_elf_import_validator = app_env.ValidateAppImports(app_elf_augmented)
    app_env.AlwaysBuild(app_elf_import_validator)
    app_env.Alias(app_alias, app_elf_import_validator)
    return (app_elf_augmented, app_elf_raw, app_elf_import_validator)


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
        f.write(
            assemble_manifest_data(
                app_manifest=app,
                hardware_target=int(env.subst("$TARGET_HW")),
                sdk_version=sdk_cache.version.as_int(),
            )
        )


def validate_app_imports(target, source, env):
    sdk_cache = SdkCache(env.subst("$SDK_DEFINITION"), load_version_only=False)
    app_syms = set()
    with open(target[0].path, "rt") as f:
        for line in f:
            app_syms.add(line.split()[0])
    unresolved_syms = app_syms - sdk_cache.get_valid_names()
    if unresolved_syms:
        SCons.Warnings.warn(
            SCons.Warnings.LinkWarning,
            f"{source[0].path}: app won't run. Unresolved symbols: {unresolved_syms}",
        )


def GetExtAppFromPath(env, app_dir):
    if not app_dir:
        raise UserError("APPSRC= not set")

    appmgr = env["APPMGR"]

    app = None
    try:
        # Maybe used passed an appid?
        app = appmgr.get(app_dir)
    except FlipperManifestException as _:
        # Look up path components in known app dits
        for dir_part in reversed(pathlib.Path(app_dir).parts):
            if app := appmgr.find_by_appdir(dir_part):
                break

    if not app:
        raise UserError(f"Failed to resolve application for given APPSRC={app_dir}")

    app_elf = env["_extapps"]["compact"].get(app.appid, None)
    if not app_elf:
        raise UserError(
            f"Application {app.appid} is not configured for building as external"
        )

    app_validator = env["_extapps"]["validators"].get(app.appid, None)

    return (app, app_elf[0], app_validator[0])


def generate(env, **kw):
    env.SetDefault(EXT_APPS_WORK_DIR=kw.get("EXT_APPS_WORK_DIR"))
    # env.VariantDir(env.subst("$EXT_APPS_WORK_DIR"), env.Dir("#"), duplicate=False)

    env.AddMethod(BuildAppElf)
    env.AddMethod(GetExtAppFromPath)
    env.Append(
        BUILDERS={
            "EmbedAppMetadata": Builder(
                action=[
                    Action(prepare_app_metadata, "$APPMETA_COMSTR"),
                    Action(
                        "${OBJCOPY} "
                        "--remove-section .ARM.attributes "
                        "--add-section .fapmeta=${SOURCE}.meta "
                        "--set-section-flags .fapmeta=contents,noload,readonly,data "
                        "--strip-debug --strip-unneeded "
                        "--add-gnu-debuglink=${SOURCE} "
                        "${SOURCES} ${TARGET}",
                        "$APPMETAEMBED_COMSTR",
                    ),
                ],
                suffix=".fap",
                src_suffix=".elf",
            ),
            "ValidateAppImports": Builder(
                action=[
                    Action(
                        "@${NM} -P -u ${SOURCE} > ${TARGET}",
                        None,  # "$APPDUMP_COMSTR",
                    ),
                    Action(
                        validate_app_imports,
                        "$APPCHECK_COMSTR",
                    ),
                ],
                suffix=".impsyms",
                src_suffix=".fap",
            ),
        }
    )


def exists(env):
    return True

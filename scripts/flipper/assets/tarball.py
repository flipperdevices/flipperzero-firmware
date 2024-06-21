import tarfile

FLIPPER_TAR_FORMAT = tarfile.USTAR_FORMAT


def tar_sanitizer_filter(tarinfo: tarfile.TarInfo):
    tarinfo.gid = tarinfo.uid = 0
    tarinfo.mtime = 0
    tarinfo.uname = tarinfo.gname = "furippa"
    return tarinfo

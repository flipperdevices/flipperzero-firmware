import gdb


# Must match FuriHalRtcRegisterVersion index in FuriHalRtcRegister enum
RTC_BACKUP_VERSION_REGISTER_IDX = 0x2

RTC_BASE = 0x40002800
RTC_BACKUP_BASE = RTC_BASE + 0x50

VERSION_REGISTER_ADDRESS = RTC_BACKUP_BASE + RTC_BACKUP_VERSION_REGISTER_IDX * 4


class FlipperFwVersion(gdb.Command):
    """Print the version of Flipper's firmware."""

    def __init__(self):
        super(FlipperFwVersion, self).__init__("fw-version", gdb.COMMAND_USER)

    def invoke(self, arg, from_tty):
        # Get version struct from loaded elf
        version_struct_type = gdb.lookup_type("Version")
        # Get version ptr from RTC
        version_ptr_ptr = gdb.Value(VERSION_REGISTER_ADDRESS).cast(
            version_struct_type.pointer().pointer()
        )
        # Check if version ptr is valid
        if not version_ptr_ptr:
            print("RTC version register is NULL")
            return
        # Dereference version ptr
        version_ptr = version_ptr_ptr.dereference()
        # Check if version ptr is valid
        if not version_ptr:
            print("Pointer to version struct is NULL")
            return

        # Dereference version ptr
        version_struct = version_ptr.dereference()
        # Print version struct
        # print(version_struct)
        print("Running firmware info:")
        print(f"\tVersion:     {version_struct['version'].string()}")
        print(f"\tBuilt on:    {version_struct['build_date'].string()}")
        print(f"\tGit branch:  {version_struct['git_branch'].string()}")
        print(f"\tGit commit:  {version_struct['git_hash'].string()}")
        print(f"\tDirty:       {version_struct['build_is_dirty']}")
        print(f"\tHW Target:   {int(version_struct['target'])}")


FlipperFwVersion()

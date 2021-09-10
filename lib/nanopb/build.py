from conan.packager import ConanMultiPackager

if __name__ == "__main__":
    builder = ConanMultiPackager(build_policy="outdated")
    builder.add_common_builds(shared_option_name=None)
    builder.run()

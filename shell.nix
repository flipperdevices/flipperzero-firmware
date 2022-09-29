{ pkgs ? import <nixpkgs> {} }:
(pkgs.buildFHSUserEnv {
  name = "flipperzero";
  targetPkgs = pkgs: (with pkgs; [
    zlib
    python39
    python39Packages.pip
    python39Packages.virtualenv
    protobuf

    scons
    gcc-arm-embedded

    openocd
    dfu-util
  ]);
  runScript = ''
    bash --init-file <(echo "virtualenv venv; source venv/bin/activate; pip3 install -r scripts/requirements.txt; export FBT_NOENV=1")
  '';
}).env

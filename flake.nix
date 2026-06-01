{
  description = "Flipper Zero firmware build env (f6 backport)";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
  # flipper's build only accepts arm-gcc 12.3.x / 13.2.x; 24.05 ships 13.2.rel1
  inputs.nixpkgs-arm.url = "github:NixOS/nixpkgs/nixos-24.05";

  outputs = { self, nixpkgs, nixpkgs-arm }:
    let
      systems = [ "aarch64-darwin" "x86_64-darwin" "aarch64-linux" "x86_64-linux" ];
      forAll = nixpkgs.lib.genAttrs systems;
    in {
      devShells = forAll (system:
        let
          pkgs = import nixpkgs { inherit system; };
          pkgsArm = import nixpkgs-arm { inherit system; };
          py = pkgs.python3;
          # flipper's crosscc expects arm-none-eabi-gdb-py3; modern gdb has python built in
          armToolchain = pkgs.runCommand "arm-none-eabi-flipper" { } ''
            mkdir -p $out/bin
            for f in ${pkgsArm.gcc-arm-embedded-13}/bin/*; do ln -s "$f" "$out/bin/$(basename "$f")"; done
            ln -s $out/bin/arm-none-eabi-gdb $out/bin/arm-none-eabi-gdb-py3
          '';
          sconsModule = pkgs.python3Packages.toPythonModule pkgs.scons;
          pythonEnv = py.withPackages (ps: [
            sconsModule
            ps.pyelftools
            ps.heatshrink2
            ps.cxxheaderparser
            ps.pillow
            ps.protobuf
            ps.colorlog
            ps.ansi
            ps.requests
            ps.pyserial
            ps.oslex
            ps.lxml
            ps.setuptools
          ]);
        in {
          default = pkgs.mkShell {
            packages = [
              armToolchain
              pythonEnv
              pkgs.protobuf
              pkgs.git
              pkgs.dfu-util
            ];
            # use the nix-provided toolchain, skip fbt's downloader
            FBT_NOENV = "1";
            shellHook = ''
              echo "flipper f6 build shell — arm-gcc $(arm-none-eabi-gcc -dumpversion), $(python3 --version)"
              echo "build: ./fbt TARGET_HW=6 updater_package"
            '';
          };
        });
    };
}

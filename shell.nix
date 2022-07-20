{ pkgs ? import <nixpkgs> { } }:

(pkgs.callPackage ./default.nix {
  src = null;
}).overrideAttrs (old: {
  FBT_NO_SYNC = false;

  nativeBuildInputs = (old.nativeBuildInputs or [ ]) ++ [
    pkgs.openocd
    pkgs.dfu-util
  ];
})

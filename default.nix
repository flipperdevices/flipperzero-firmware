{ pkgs ? import <nixpkgs> { }
, lib ? pkgs.lib
, src ? lib.cleanSource ./.
, version ? "dev"
, rev ? let
    gitRepo = "${toString src.origSrc or ""}/.git";
    isGitRepo = lib.pathIsGitRepo gitRepo;
  in
  if isGitRepo then builtins.substring 0 8 (lib.commitIdFromGitRepo gitRepo) else ""
}:

let
  inherit (builtins) elem elemAt match filter split readFile typeOf;
  inherit (pkgs) stdenv lib;
  inherit (lib) toLower;

  ignoredPythonDeps = [
    "python3-protobuf" # Doesn't seem to actually be required?
  ];

  pythonDeps = (
    filter
      (x: ! elem x ignoredPythonDeps)
      (map
        (x: toLower (elemAt (match "([A-Za-z0-9._-]+).+" x) 0))
        (filter (line: typeOf line == "string" && line != "") (split "\n" (readFile ./scripts/requirements.txt))))
  );

  python = pkgs.python3.override {
    self = python;

    packageOverrides = self: super: {

      heatshrink2 =
        let
          pname = "heatshrink2";
          version = "0.11.0";
        in
        self.buildPythonPackage {
          inherit pname version;
          src = self.fetchPypi {
            inherit pname version;
            sha256 = "sha256-xihtHUuS+664Ow/YtfgxyTUNzRBG53blqqg9G+Q4Nfc=";
          };
        };

    };
  };

  pythonEnv = python.withPackages (ps: map (x: ps.${x}) pythonDeps);

in
stdenv.mkDerivation {
  pname = "flipperzero-firmware";
  inherit version src;

  GIT_COMMIT = rev;

  nativeBuildInputs = [
    pythonEnv
    pkgs.gcc-arm-embedded
    pkgs.imagemagick
  ];

  FBT_NOENV = true;
  FBT_NO_SYNC = true;

  dontConfigure = true;
  dontFixup = true;

  buildPhase = ''
    runHook preBuild
    ./fbt
    runHook postBuild
  '';

  installPhase = ''
    runHook preInstall
    mv dist $out
    runHook postInstall
  '';
}

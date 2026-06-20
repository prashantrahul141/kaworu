{
  pkgs ? import <nixpkgs> { },
}:
pkgs.stdenv.mkDerivation {
  name = "edk2-ovmf-stable-bins";
  src = pkgs.fetchurl {
    url = "https://github.com/osdev0/edk2-ovmf-stable-bins/releases/download/20260526T165842Z/edk2-ovmf-bins.tar.gz";
    sha256 = "sha256:9bf7998f0d691490c7c1245ed7edaec2f0b3b85fa8dc24223da617a912655677";
  };
  phases = [
    "installPhase"
  ];
  installPhase = ''
    mkdir extract
    tar -xzf "$src" -C extract

    mkdir -p "$out/share"
    cp -r extract/*/* "$out/share/"
  '';
}

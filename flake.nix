{
  description = "kaworu development environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-26.05";
    fenix = {
      url = "github:nix-community/fenix";
      inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs =
    {
      self,
      nixpkgs,
      fenix,
    }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs {
        inherit system;
        overlays = [ fenix.overlays.default ];
      };
      toolchain = pkgs.fenix.fromToolchainFile {
        file = ./rust-toolchain.toml;
        sha256 = "sha256-OATSZm98Es5kIFuqaba+UvkQtFsVgJEBMmS+t6od5/U=";
      };
      ovmf = import ./nix/edk2-ovmf-stable-bins.nix { inherit pkgs; };
    in
    {
      devShells.${system}.default = pkgs.mkShell {
        packages = with pkgs; [
          limine-full
          glibc_multi

          llvmPackages.clang-unwrapped
          llvmPackages.lld
          llvm
          toolchain

          xorriso
          ovmf

          qemu
          gdb
          dtc

          gnumake
          bear
          python313Packages.kconfiglib
        ];

        shellHook = ''
          export UEFI_FIRMWARE=${ovmf}/share/ovmf-code-aarch64.fd
          export LIMINE_PATH=${pkgs.limine-full}/share/limine
          export CC=clang
          export LD=ld.lld
          export OBJDUMP=llvm-objdump
          export READELF=llvm-readelf
        '';
      };
    };
}

{
  description = "kaworu development environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.11";
  };

  outputs =
    { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
    in
    {
      devShells.${system}.default = pkgs.mkShell {
        packages = with pkgs; [
          llvmPackages.clang-unwrapped
          llvmPackages.lld
          llvm

          qemu
          gdb
          dtc

          gnumake
          bear
          python313Packages.kconfiglib
        ];

        shellHook = ''
          export CC=clang
          export LD=ld.lld
          export OBJDUMP=llvm-objdump
          export READELF=llvm-readelf
        '';
      };
    };
}

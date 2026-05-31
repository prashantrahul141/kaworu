{
  description = "kaworu development environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-25.11";
  };

  outputs =
    { self, nixpkgs }:
    let
      systems = [
        "x86_64-linux"
        "aarch64-linux"
      ];

      forAllSystems = f: nixpkgs.lib.genAttrs systems (system: f (import nixpkgs { inherit system; }));
    in
    {
      devShells = forAllSystems (pkgs: {
        default = pkgs.mkShell {
          packages = with pkgs; [
            llvmPackages.clang-unwrapped
            llvmPackages.lld
            llvm
            gnumake
            qemu
            pkgsCross.aarch64-multiplatform.buildPackages.binutils
            bear
            python313Packages.kconfiglib
          ];

        };
      });
    };
}

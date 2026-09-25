{
  description = "C quorumlog development environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        devShells.default = pkgs.mkShell {
          name = "quorumlog";

          packages = with pkgs; [
            gcc
            gnumake
            gdb
            valgrind
            strace
            clang-tools
            cppcheck
            man-pages
            man-pages-posix
            zlib
          ];

          hardeningDisable = [ "all" ];

          shellHook = ''
            echo "quorumlog ready"
            echo "  gcc      : $(gcc --version | head -1)"
            echo "  valgrind : $(valgrind --version)"
          '';
        };
      }
    );
}

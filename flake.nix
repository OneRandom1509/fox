{
  description = "Fox programming language";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs =
    { self, nixpkgs }:
    let
      systems = [
        "x86_64-linux"
        "aarch64-linux"
        "x86_64-darwin"
        "aarch64-darwin"
      ];

      forAllSystems =
        function:
        nixpkgs.lib.genAttrs systems (
          system:
          function {
            pkgs = import nixpkgs { inherit system; };
          }
        );
    in
    {
      packages = forAllSystems (
        { pkgs }:
        {
          default = pkgs.stdenv.mkDerivation {
            pname = "fox";
            version = "0.1.0";

            src = ./.;

            nativeBuildInputs = [
              pkgs.cmake
              pkgs.pkg-config
            ];

            buildInputs = [
              pkgs.readline
            ];

            cmakeFlags = [
              "-DCMAKE_BUILD_TYPE=Release"
            ];

            installPhase = ''
              mkdir -p $out/bin
              cp fox $out/bin/
            '';

            meta = {
              description = "A toy programming language";
              license = pkgs.lib.licenses.mit;
              mainProgram = "fox";
            };
          };
        }
      );

      apps = forAllSystems (
        { pkgs }:
        {
          default = {
            type = "app";
            program = "${self.packages.${pkgs.system}.default}/bin/fox";
          };
        }
      );

      devShells = forAllSystems (
        { pkgs }:
        {
          default = pkgs.mkShell {
            packages = [
              pkgs.cmake
              pkgs.gnumake
              pkgs.pkg-config
              pkgs.clang-tools
              pkgs.ccache
            ];

            buildInputs = [
              pkgs.readline
            ];
          };
        }
      );
    };
}

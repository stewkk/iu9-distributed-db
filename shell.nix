{ pkgs ? import <nixpkgs> {} }:

let
  tex = (pkgs.texlive.combine {
    inherit (pkgs.texlive) scheme-full
      latexmk;
  });
  pythonWithPytest = pkgs.python312.buildEnv.override {
    extraLibs = with pkgs.python312Packages; [
      pip
      virtualenv
    ];
    ignoreCollisions = true;
  };
in
pkgs.mkShell.override {stdenv = pkgs.llvmPackages_18.stdenv;} {
  buildInputs = [
    pkgs.pandoc
    tex
    pkgs.cmake
    pkgs.pre-commit
    pythonWithPytest
    pkgs.nodePackages.pyright
    pkgs.plantuml
    pkgs.systemd
    pkgs.gdb
    pkgs.killall
    pkgs.valgrind
    pkgs.clojure
    pkgs.clojure-lsp
    pkgs.leiningen

    # keep this line if you use bash
    pkgs.bashInteractive
  ];

  nativeBuildInputs = [
    pkgs.clang-tools_18
    pkgs.pkg-config
  ];

  NIX_LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath [
    pkgs.stdenv.cc.cc
    pkgs.zlib
  ];
  NIX_LD = pkgs.lib.fileContents "${pkgs.stdenv.cc}/nix-support/dynamic-linker";
  shellHook = ''
    export VENV_DIR="$PWD/.venv"
    if [ ! -d "$VENV_DIR" ]; then
      ${pythonWithPytest}/bin/python -m venv $VENV_DIR
      source $VENV_DIR/bin/activate
      pip install pip setuptools wheel
    else
      source $VENV_DIR/bin/activate
    fi

    export LD_LIBRARY_PATH=$NIX_LD_LIBRARY_PATH
    export PYTHONPATH="${pythonWithPytest}/lib/python3.13/site-packages:$PYTHONPATH"
    export PATH="$PWD/build/bin:$PATH"

    if [ -f requirements.txt ]; then
      pip install -r requirements.txt
    fi

    if [ -f pytests/requirements.txt ]; then
      pip install -r pytests/requirements.txt
    fi

    python --version
    resolvectl dns wlp0s20f3 10.0.100.1
    resolvectl domain wlp0s20f3 ~incus
  '';
}

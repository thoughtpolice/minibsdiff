{ minibsdiff ? { outPath = ./.; revCount = 0; shortRev = "abcdef"; rev = "HEAD"; }
, officialRelease ? false
}:

let
  pkgs = import <nixpkgs> { };

  systems = [ "x86_64-linux" ];

  version = "0.0.1" + (pkgs.lib.optionalString (!officialRelease)
    "-pre${toString minibsdiff.revCount}_${minibsdiff.shortRev}");
in
rec {

  ## -- Tarballs ---------------------------------------------------------------
  tarball = pkgs.releaseTools.sourceTarball {
    name = "minibsdiff-tarball";
    src  = minibsdiff;
    inherit version;
    officialRelease = true; # hack
    buildInputs = [ pkgs.git pkgs.xz ];

    distPhase = ''
      relname=minibsdiff-${version}
      mkdir ../$relname
      cp -prd . ../$relname
      rm -rf ../$relname/.git ../$relname/svn-revision

      mkdir $out/tarballs
      tar cvfJ $out/tarballs/$relname.tar.xz -C .. $relname
    '';
  };

  ## -- Build ------------------------------------------------------------------
  build = pkgs.lib.genAttrs systems (system:
    with import <nixpkgs> { inherit system; };

    releaseTools.nixBuild {
      name = "minibsdiff";
      src  = tarball;
      enableParallelBuilding = true;
      doCheck = false;

      installPhase = "make install PREFIX=$out";
    }
  );

  ## -- Release build ----------------------------------------------------------
  release = pkgs.releaseTools.aggregate
    { name = "patchelf-${tarball.version}";
      constituents =
        [ tarball
          build.x86_64-linux
        ];
      meta.description = "Release-critical builds";
    };

}

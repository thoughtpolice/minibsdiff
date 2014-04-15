{ minibsdiff ? { outPath = ./.; revCount = 0; shortRev = "abcdef"; rev = "HEAD"; }
, officialRelease ? false
}:

let
  pkgs = import <nixpkgs> { };

  systems = [ "i686-linux" "x86_64-linux" ];

  version = "0.0.1" + (pkgs.lib.optionalString (!officialRelease)
    "-pre${toString minibsdiff.revCount}_${minibsdiff.shortRev}");

  jobs = rec {
    ## -- Tarballs -------------------------------------------------------------
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

    ## -- Build ----------------------------------------------------------------
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

    ## -- Release build --------------------------------------------------------
    release = pkgs.releaseTools.aggregate
      { name = "minibsdiff-${tarball.version}";
        constituents =
          [ tarball
            build.x86_64-linux
          ];
        meta.description = "Release-critical builds";
      };
  };

  ## -- Utilities --------------------------------------------------------------
  makeRPM_i686   = makeRPM "i686-linux";
  makeRPM_x86_64 = makeRPM "x86_64-linux";

  makeRPM =
    system: diskImageFun: prio:

    with import <nixpkgs> { inherit system; };

    releaseTools.rpmBuild rec {
      name = "minibsdiff-rpm";
      src = jobs.tarball;
      diskImage = diskImageFun vmTools.diskImages;
      meta = { schedulingPriority = prio; };
    };

  makeDeb_i686   = makeDeb "i686-linux";
  makeDeb_x86_64 = makeDeb "x86_64-linux";

  makeDeb =
    system: diskImageFun: prio:

    with import <nixpkgs> { inherit system; };

    releaseTools.debBuild {
      name = "minibsdiff-deb";
      src = jobs.tarball;
      diskImage = diskImageFun vmTools.diskImages;
      meta = { schedulingPriority = prio; };
      debMaintainer = "Austin Seipp <aseipp@pobox.com>";
    };

in jobs

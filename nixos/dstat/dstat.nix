{stdenv, fetchurl, libX11, libXinerama, libXft, zlib}:

with stdenv.lib;

stdenv.mkDerivation rec {
  name = "dstat-0.4.55";

  unpackCmd="cp -rvd $curSrc .";
  src = "/etc/nixos/dstat";

  buildInputs = [ libX11 libXinerama zlib libXft];
  buildPhase = ''
    gcc -c -o dstat.o dstat.c
    gcc -lX11 dstat.o -o dstat
  '';

  installPhase = ''
     mkdir -p $out/bin
     cp dstat $out/bin
  '';

  meta = { 
      description = "a generic, highly customizable, and efficient menu for the X Window System";
      homepage = http://tools.suckless.org/dmenu;
      license = stdenv.lib.licenses.mit;
      maintainers = with stdenv.lib.maintainers; [viric];
      platforms = with stdenv.lib.platforms; all;
  };
}

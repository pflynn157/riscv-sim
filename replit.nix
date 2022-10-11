{ pkgs }: {
	deps = [
		pkgs.unixtools.util-linux
  pkgs.cmake
  pkgs.clang_12
		pkgs.ccls
		pkgs.gdb
		pkgs.gnumake
	];
}
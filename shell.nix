with (import <nixpkgs> { overlays = [ (import ./nix/overlay.nix) ]; });

mkShell {
	packages = [
		git
		ninja
		cmake
		pkg-config
		mesonNoPatch
	] ++ lib.optionals stdenv.hostPlatform.isLinux [
		libglvnd
		libgbm
		libxau
		libxdmcp
		libxcb
		libx11
		libxrandr
		libffi
		libxkbcommon
		libdecor
		wayland-scanner
		wayland-protocols
		egl-wayland
		wayland
		cairo
	] ++ lib.optionals stdenv.hostPlatform.isDarwin [
	] ++ lib.optionals stdenv.hostPlatform.isWindows [
	];

	LD_LIBRARY_PATH = lib.makeLibraryPath ([
	] ++ lib.optionals stdenv.hostPlatform.isLinux [
		libxkbcommon
		wayland
	] ++ lib.optionals stdenv.hostPlatform.isDarwin [
	] ++ lib.optionals stdenv.hostPlatform.isWindows [
	]);
}

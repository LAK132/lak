final: prev: {
	mesonNoPatch = prev.meson.overrideAttrs (old: {
		patches = (old.patches or []) ++ [
			./0001-gnupatch-shipped-with-nixos-cannot-handle-differing-.patch
		];
		nativeBuildInputs = (old.nativeBuildInputs or []) ++ [
			prev.git
		];
	});
}

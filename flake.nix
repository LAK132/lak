{
	inputs = {
		nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
	};

	outputs = { self, nixpkgs }:
	let
		lib = nixpkgs.lib;
		forAllSystems = lib.genAttrs lib.platforms.linux;
	in
	{
		packages = forAllSystems (system:
		let
			pkgs = nixpkgs.legacyPackages.${system};
			wrap2nix-pkg = pkgs.callPackage ./package.nix {
				lak_install_targets = [ "wrap2nix" ];
			};
			wrap2nix = "${wrap2nix-pkg}/bin/wrap2nix";
		in
		{
			lak = pkgs.callPackage ./package.nix {
				wrap2nix = wrap2nix;
				lak_install_targets = [
					"basic_program"
					"colour-demo"
					"hello-cobalt"
					"mdc-view"
					"nbt-view"
					"tiff-view"
					"x3f-view"
					"asc2csv"
					"ebnf2cpp"
					"lisk-repl"
					"mdc2png"
					"wrap2nix"
					"lak_test"
				];
				lak_enable_tests = true;
				lak_enable_examples = true;
				lak_enable_windowing = true;
				lak_backend = "sdl3";
				lak_renderer = [
					"softrender"
					"cobalt"
				];
				cobalt_renderer = [
					"OpenGL4"
				];
				lak_enable_glm = true;
				lak_enable_imgui = true;
				lak_enable_stb = true;
				lak_enable_stb_image = true;
				lak_enable_stb_image_write = true;
			};
			default = self.packages.${system}.lak;
		});
	};
}

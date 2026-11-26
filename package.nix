{
	pkgs,
	stdenv,
	lib,
	applyPatches,
	fetchgit,
	fetchzip,
	fetchurl,
	fetchpatch,
	fetchFromGitHub,
	git,
	meson,
	ninja,
	gcc,
	cmake,
	pkg-config,
	python3,
	sdl3,
	libglvnd,
	libgbm,
	libxau,
	libxdmcp,
	libxcb,
	libx11,
	libxrandr,
	libffi,
	libxkbcommon,
	libdecor,
	wayland-scanner,
	wayland-protocols,
	egl-wayland,
	wayland,
	vulkan-loader,
	cairo,
	makeWrapper,

	wrap2nix ? null,

	lak_install_targets ? [],
	lak_enable_tests ? false,
	lak_enable_examples ? false,
	lak_bounds_checks ? "debug",
	lak_enable_windowing ? false,
	lak_use_native_file_dialog ? true,
	lak_run_windowing_tests ? false,
	lak_backend ? "sdl3",
	lak_renderer ? [],
	cobalt_renderer ? [ "OpenGL4" ],
	cobalt_use_vvl ? false,
	lak_enable_llvm ? false,
	llvm_from_source ? true,
	llvm_components ? [ "Native" ],
	llvm_install_components ? [ "Core" "ExecutionEngine" "OrcJIT" "Support" ],
	llvm_parallel_link_jobs ? "4",
	lak_enable_lmdb ? false,
	lak_enable_lua ? false,
	lak_enable_glm ? false,
	lak_enable_imgui ? false,
	lak_enable_libfive ? false,
	lak_enable_libgphoto2 ? false,
	lak_enable_openvr ? false,
	lak_enable_stb ? false,
	lak_enable_stb_image ? false,
	lak_enable_stb_image_write ? false,
	lak_enable_godot_cpp ? false,
}:

let
	subprojectsDir = ./subprojects;
	cobaltCacheDir = "External/Cache";

	needsCobalt = lak_enable_windowing && (lib.filter (rnd: rnd == "cobalt") lak_renderer) != [];
	needsVulkan = needsCobalt && (lib.filter (rnd: rnd == "Vulkan") cobalt_renderer) != [];

	readWrap = w: import (pkgs.runCommand "wrap2nix" {} ''${wrap2nix} ${w} $out'');

	wrapDiff = name: wrap: src: if wrap?diff_files then (applyPatches {
		name = "patched-${name}";
		src = src;
		patches = lib.forEach (lib.flatten [wrap.diff_files]) (diff: "${subprojectsDir}/packagefiles/${diff}");
	}) else src;

	copyPatchDir = name: dir: "cp -r ${dir}/. $out/\n";

	wrapPatch = name: wrap: if wrap?patch_directory then copyPatchDir name "${subprojectsDir}/packagefiles/${wrap.patch_directory}" else "";

	wrapGit = name: wrap-git:
		wrapDiff name wrap-git (fetchgit {
			url = wrap-git.url;
			rev = wrap-git.revision;
			hash = wrap-git.nix-hash;
			deepClone = if wrap-git?depth then wrap-git.depth != "1" else false;
			postFetch = wrapPatch name wrap-git;
		});

	wrapFile = name: wrap-file:
		wrapDiff name wrap-file (fetchzip {
			name = name;
			url = wrap-file.source_url;
			hash = wrap-file.nix-hash;
			postFetch = wrapPatch name wrap-file;
			stripRoot = true; # :TODO:
		});

	wrap = name:
		let
			wrap-nix1 = readWrap "${./subprojects}/${name}.wrap";
			wrap-nix = if wrap-nix1?wrap-redirect
				then readWrap "${./subprojects}/${wrap-nix1.wrap-redirect.filename}"
				else wrap-nix1;
			src =
				if wrap-nix?wrap-git then (wrapGit name wrap-nix.wrap-git) else
				if wrap-nix?wrap-file then (wrapFile name wrap-nix.wrap-file) else
			null;
		in
		{
			name = name;
			src = src;
		};

	symlinkWrap = d: w: ''
		ln -s "${w.src}" "${d}/${w.name}"
	'';

	symlinkSubproject = w: symlinkWrap "subprojects" w;
	symlinkCobaltDep = w: symlinkWrap "$out/${cobaltCacheDir}" w;

	cobaltWrap = wrap "Cobalt";

	cobaltDep = name: src: { name = name; src = src; };

	cobaltDepFromGit = out: repo:
		cobaltDep out (builtins.fetchGit repo);
	cobaltDepFromGitPatched = name: repo: patches:
		cobaltDep name (applyPatches {
			name = "patched-${name}";
			src = builtins.fetchGit repo;
			patches = patches;
		});
	cobaltDepFromGitHub = out: repo:
		cobaltDep out (fetchFromGitHub repo);
	cobaltDepFromGitHubPatched = name: repo: patches:
		cobaltDep name (applyPatches {
			name = "patched-${name}";
			src = fetchFromGitHub repo;
			patches = patches;
		});
	cobaltDepZip = name: zip: cobaltDep name (fetchzip zip);
	cobaltDepUrl = name: url: cobaltDep name (fetchurl url);
	cobaltDeps = [
		# General Utility Libraries
		(cobaltDepFromGitHubPatched "zlib" {
			owner = "madler";
			repo = "zlib";
			rev = "v1.3.1";
			hash = "sha256-TkPLWSN5QcPlL9D0kc/yhH0/puE9bFND24aj5NVDKYs=";
		} [ (fetchpatch {
			url = "https://github.com/savushkin-r-d/zlib/commit/feee7dd9d6559571e9821493f890df73422f9f8e.patch";
			hash = "sha256-4g1esOXzwi8uKaZqbgWRvCioE53AKU4XeCvYa26qklA=";
		}) ])
		(cobaltDepFromGitHub "libpng" {
			owner = "pnggroup";
			repo = "libpng";
			rev = "v1.6.51";
			hash = "sha256-4txg9W8LqdCtvVZ2hZGyWRVTDxTIn7Mtl9joBrUtFZI=";
		})
		(cobaltDepFromGitHubPatched "gli" {
			owner = "g-truc";
			repo = "gli";
			rev = "3542f8830178061e0661f5df1e89d36ba7d7b0ab";
			hash = "sha256-EbDwbIsf4qu60FCMJUv4KNu/UoyM7grpEwd0+9A4O6I=";
		} [ (fetchpatch {
			url = "https://github.com/g-truc/gli/pull/164.patch";
			hash = "sha256-PX3wXLSjtvhKK6Or9rRTgb3OPFbOxUyr30sIaPp3+VY=";
		}) ])

		# Math Libraries
		(cobaltDepFromGitHub "glm" {
			owner = "g-truc";
			repo = "glm";
			rev = "1.0.2";
			hash = "sha256-2xKv1nO+OdwA0r+I9OZ+OCL9dJFg/LJsQfIvIF76vc0=";
		})
		(cobaltDepZip "half" {
			url = "mirror://sourceforge/half/2.2.0/half-2.2.0.zip";
			hash = "sha256-ZdGgBMZylFgkvs/XVBnvgBY2EYSHRLY3S4YwXjshpOY=";
			stripRoot = false;
		})

		# OpenGL Libraries
		(cobaltDepFromGitHub "OpenGLRegistry" {
			owner = "KhronosGroup";
			repo = "OpenGL-Registry";
			rev = "8f9b0ca1a8eafa2cebde3ff982629e7913d2817f";
			hash = "sha256-1iNsFYyQGiptEGJMi9XtF0zETZLtCiM9uN06/Pnno5I=";
		})

		# Shader Utilities
		(cobaltDepFromGitHub "SPIRV-Cross" {
			owner = "KhronosGroup";
			repo = "SPIRV-Cross";
			rev = "9daa7fe0a36dbe5c5b810d74d2cc43b202407d57";
			hash = "sha256-VPTwAfxu2bY22D+HdzmvmSUwTMaBApmY76N7vHBfzVY=";
		})
		(cobaltDepFromGitHub "SPIRV-Headers" {
			owner = "KhronosGroup";
			repo = "SPIRV-Headers";
			rev = "sdk-1.3.216.0";
			hash = "sha256-qYhFoRrQOlvYvVXhIFsa3dZuORDpZyVC5peeYmGNimw=";
		})
		(cobaltDepFromGitHubPatched "SPIRV-Tools" {
			owner = "KhronosGroup";
			repo = "SPIRV-Tools";
			rev = "sdk-1.3.216.0";
			hash = "sha256-NWpFSRoxtYWi+hLUt9gpw0YScM3shcUwv9yUmbivRb0=";
		} [
			"${cobaltWrap.src}/External/ShaderUtilities/SPIRV-Tools.patch"
		])
		(cobaltDepFromGitHub "glslang" {
			owner = "KhronosGroup";
			repo = "glslang";
			rev = "11.10.0";
			hash = "sha256-sjidkiPtRADhyOEKDb2cHCBXnFjLwk2F5Lppv5/fwNQ=";
		})

		# Vulkan Libraries
		(cobaltDepFromGitHub "Vulkan-Headers" {
			owner = "KhronosGroup";
			repo = "Vulkan-Headers";
			rev = "v1.3.261";
			hash = "sha256-zKHew7SGUq1C3XGp/HrCle6KyqB4cziPcTYVqAr814s=";
		})
		(cobaltDepFromGitHubPatched "Vulkan-Loader" {
			owner = "KhronosGroup";
			repo = "Vulkan-Loader";
			rev = "v1.3.261";
			hash = "sha256-5QCVHfvjE98EnL2Dr7g9fdrJAg+np1Q6hgqcuZCWReQ=";
		} [
			"${cobaltWrap.src}/External/VulkanLibraries/Vulkan-Loader.patch"
		])
		(cobaltDepFromGitHub "VulkanMemoryAllocator" {
			owner = "GPUOpen-LibrariesAndSDKs";
			repo = "VulkanMemoryAllocator";
			rev = "v3.4.0";
			hash = "sha256-LBZJcom7G7maF9wpUVeVEJQAJwGy6365INk3VD0/0PM=";
		})
	] ++ lib.optionals cobalt_use_vvl [
		# Vulkan Libraries
		(cobaltDepFromGitHubPatched "Vulkan-ValidationLayers" {
			owner = "KhronosGroup";
			repo = "Vulkan-ValidationLayers";
			rev = "v1.4.336";
			hash = "sha256-EuXKMGZ9gaTySwX1/6iON5owLaBo1vodDWcOz69mP/4=";
		} [
			"${cobaltWrap.src}/External/VulkanLibraries/Vulkan-ValidationLayers.patch"
		])
	] ++ lib.optionals (true || sdl3 == null) [
		(cobaltDepFromGitHub "sdl3" {
			owner = "libsdl-org";
			repo = "SDL";
			rev = "release-3.4.0";
			hash = "sha256-/A1y/NaZVebzI58F4TlwtDwuzlcA33Y1YuZqd5lz/Sk=";
		})
	] ++ lib.optionals stdenv.hostPlatform.isLinux [
	] ++ lib.optionals stdenv.hostPlatform.isDarwin [
		# Vulkan Libraries
		(cobaltDepUrl "VulkanSDK" {
			url = "https://sdk.lunarg.com/sdk/download/1.4.341.0/mac/vulkan_sdk.zip";
			hash = "sha256-O4r399t0tL2gLckVA87CiKtwrr/ic/+Yu4zrq/BLGW4=";
		})
	] ++ lib.optionals stdenv.hostPlatform.isWindows [
		# Direct3D12 Libraries
		(cobaltDepFromGitHubPatched "PixEvents" {
			owner = "Microsoft";
			repo = "PixEvents";
			rev = "3a7e70dde7bf54f02f9d2e9dd6d3350c6cfb962f";
			hash = "sha256-KvJjdAwtCb+vibu3F+kiivtelsYMSZgg/mysa4K/2CE=";
		} [
			"${cobaltWrap.src}/External/Direct3D12Libraries/PixEvents/PixEvents.patch"
		])
		(cobaltDepFromGitHubPatched "DXSamples" {
			owner = "Microsoft";
			repo = "DirectX-Graphics-Samples";
			rev = "1704d8883ea74a73d332516bb6d990a134abaebd";
			hash = "sha256-b6isfN3PjzW/K3A5/HGS7FSn6cj0MsYpST+KNSxpsXM=";
		} [
			"${cobaltWrap.src}/External/Direct3D12Libraries/D3DX12Residency.patch"
		])
		(cobaltDepFromGitHub "DirectX-Headers" {
			owner = "Microsoft";
			repo = "DirectX-Headers";
			rev = "v1.606.3-r1";
			hash = "sha256-b6isfN3PjzW/K3A5/HGS7FSn6cj0MsYpST+KNSxpsXM=";
		})
		(cobaltDepFromGitHub "D3D12MemoryAllocator" {
			owner = "GPUOpen-LibrariesAndSDKs";
			repo = "D3D12MemoryAllocator";
			rev = "eace86c42911c8812841043d8f34c7574acee7da";
			hash = "sha256-b6isfN3PjzW/K3A5/HGS7FSn6cj0MsYpST+KNSxpsXM=";
		})
		(cobaltDepZip "DirectXShaderCompilerOfficial" {
			url = "https://github.com/microsoft/DirectXShaderCompiler/releases/download/v1.9.2602/dxc_2026_02_20.zip";
			hash = "sha256-lc3yr3BQs7qiFClJJNM4PBkYfwvQenHko0jzCy/8ZS8=";
			stripRoot = false;
		})
	];

	symlinkCobaltDeps = lib.concatMapStrings symlinkCobaltDep cobaltDeps;

	cobalt = {
		name = "Cobalt";
		src = pkgs.runCommand "modified-cobalt" { } ''
			mkdir -p "$out"
			mkdir -p "$out/${cobaltCacheDir}"
			cp -r ${cobaltWrap.src}/. "$out/"
			${symlinkCobaltDeps}
		'';
	};

	wraps = lib.optionals needsCobalt [
		cobalt
	] ++ (lib.forEach ([
		# "emsdk"
		# "libpng-1.6.38"
		# "zlib-1.2.13"
	] ++ lib.optionals lak_enable_windowing [
	] ++ lib.optionals (lak_enable_llvm && llvm_from_source) [
		"llvm-15.0.1"
	] ++ lib.optionals lak_enable_lmdb [
		"lmdb-0.9.31"
	] ++ lib.optionals lak_enable_lua [
		"lua-5.4.4"
	] ++ lib.optionals lak_enable_glm [
		"glm-1.0.2"
	] ++ lib.optionals lak_enable_imgui [
		"imgui_club"
		"imgui-node-editor"
		"imgui"
		"ImGuiColorTextEdit"
		"implot"
		"implot3d"
		"ImFileDialog"
	] ++ lib.optionals lak_enable_libfive [
		"boost-1.80.0"
		"eigen-3.4.0"
		"libfive-6f844b1"
	] ++ lib.optionals lak_enable_libgphoto2 [
		"libgphoto2"
	] ++ lib.optionals lak_enable_openvr [
		"openvr-2.12.14"
	] ++ lib.optionals (lak_enable_stb || lak_enable_imgui) [
		"stb-f1c79c0"
	] ++ lib.optionals lak_enable_godot_cpp [
		"godot-cpp-4.5.0"
	]) wrap);

	symlinkSubprojects = lib.concatMapStrings symlinkSubproject wraps;

	nativeBuildInputs = [
		git
		meson
		ninja
		gcc
		cmake
		pkg-config
		python3
	] ++ lib.optionals lak_enable_windowing ([
		sdl3
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
	]);
	installTargets = lak_install_targets ++ lib.optionals needsCobalt [
		"cobalt"
	];
	mesonFlags = [
		(lib.mesonOption "lak_install_targets" (lib.strings.join "," installTargets))
		(lib.mesonBool "lak_enable_tests" lak_enable_tests)
		(lib.mesonBool "lak_enable_examples" lak_enable_examples)
		(lib.mesonOption "lak_bounds_checks" lak_bounds_checks)
		(lib.mesonBool "lak_enable_windowing" lak_enable_windowing)
		(lib.mesonBool "lak_use_native_file_dialog" lak_use_native_file_dialog)
		(lib.mesonBool "lak_run_windowing_tests" lak_run_windowing_tests)
		(lib.mesonBool "sdl2_from_source" false)
		(lib.mesonOption "lak_backend" lak_backend)
		(lib.mesonOption "lak_renderer" (lib.strings.join "," lak_renderer))
		(lib.mesonOption "cobalt_renderer" (lib.strings.join "," cobalt_renderer))
		(lib.mesonBool "lak_enable_llvm" lak_enable_llvm)
		(lib.mesonBool "llvm_from_source" llvm_from_source)
		(lib.mesonOption "llvm_components" (lib.strings.join "," llvm_components))
		(lib.mesonOption "llvm_install_components" (lib.strings.join "," llvm_install_components))
		(lib.mesonOption "llvm_parallel_link_jobs" llvm_parallel_link_jobs)
		(lib.mesonBool "lak_enable_lmdb" lak_enable_lmdb)
		(lib.mesonBool "lak_enable_lua" lak_enable_lua)
		(lib.mesonBool "lak_enable_glm" lak_enable_glm)
		(lib.mesonBool "lak_enable_imgui" lak_enable_imgui)
		(lib.mesonBool "lak_enable_libfive" lak_enable_libfive)
		(lib.mesonBool "lak_enable_libgphoto2" lak_enable_libgphoto2)
		(lib.mesonBool "lak_enable_openvr" lak_enable_openvr)
		(lib.mesonBool "lak_enable_stb" lak_enable_stb)
		(lib.mesonBool "lak_enable_stb_image" lak_enable_stb_image)
		(lib.mesonBool "lak_enable_stb_image_write" lak_enable_stb_image_write)
		(lib.mesonBool "lak_enable_godot_cpp" lak_enable_godot_cpp)
	];
in
stdenv.mkDerivation (finalAttrs: {
	pname = "lak";
	version = "v0.1.5";
	meta.maintainers = [{
		name = "LAK132";
		github = "LAK132";
		githubId = 1386467;
	}];

	src = ./.;

	# runs out of ram in release builds
	mesonBuildType = "debug";

	nativeBuildInputs = nativeBuildInputs ++ [
		makeWrapper
	];

	buildInputs = lib.optionals lak_enable_windowing ([
		sdl3
	] ++ lib.optionals stdenv.hostPlatform.isLinux [
		libxkbcommon
		wayland
	] ++ lib.optionals needsVulkan [
		vulkan-loader
	]);

	mesonFlags = mesonFlags ++ [
		(lib.mesonBool "cobalt_cmake_no_download" true)
	];

	postPatch = lib.optionalString (wrap2nix != null) ''
		${symlinkSubprojects}
	'';

	postFixup = lib.optionalString needsVulkan (lib.strings.join " " (lib.forEach lak_install_targets (b: ''
		wrapProgram $out/bin/${b} \
			--set LD_LIBRARY_PATH "${lib.makeLibraryPath [ vulkan-loader ]}"
	'')));
})

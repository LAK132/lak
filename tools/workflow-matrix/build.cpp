#include "main.hpp"

int main()
{
	using namespace std::literals::string_literals;

	std::vector<target_t> common_targets = {
	  {
	    // examples
	    .setups =
	      {
	        "--buildtype=debugoptimized "s +
	          meson_option("lak_enable_examples", {"true"}) +
	          meson_option("lak_enable_windowing", {"true"}) +
	          meson_option("lak_enable_glm", {"true"}) +
	          meson_option("lak_enable_imgui", {"true"}),
	      },
	    .target = "basic_program "
	              "nbt-view "
	              "template"s,
	    .run    = {},
	  },
	  {
	    // tests
	    .setups =
	      {
	        "--buildtype=debugoptimized "s +
	          meson_option("lak_enable_tests", {"true"}),
	      },
	    .target = "lak_test"s,
	    .run    = "build/lak_test --help ; "
	              "build/lak_test --testall"s,
	  },
	};

	std::vector<target_t> native_targets =
	  std::vector<target_t>{
	    {
	      // tests
	      .setups =
	        {
	          "--buildtype=debugoptimized "s +
	            meson_option("lak_enable_tests", {"true"}) +
	            meson_option("lak_enable_godot_cpp", {"true"}) +
	            meson_option("lak_enable_lua", {"true"}) +
	            meson_option("lak_enable_stb", {"true"}) +
	            meson_option("lak_enable_stb_image", {"true"}) +
	            meson_option("lak_enable_stb_image_write", {"true"}),

	          "--buildtype=debugoptimized "s +
	            meson_option("lak_enable_tests", {"true"}) +
	            meson_option("lak_enable_windowing", {"true"}) +
	            meson_option("lak_use_native_file_dialog", {"false"}) +
	            meson_option("sdl2_from_source", {"true"}) +
	            meson_option("lak_backend", {"sdl2"}) +
	            meson_option("lak_enable_glm", {"true"}) +
	            meson_option("lak_enable_imgui", {"true"}),
	        },
	      .target = "lak_test"s,
	      .run    = "build/lak_test --help ; "
	                "build/lak_test --testall"s,
	    },
	    {
	      // test building with cobalt
	      .setups =
	        {
	          "--buildtype=release "s +
	            meson_option("lak_enable_tests", {"false"}) +
	            meson_option("lak_enable_examples", {"true"}) +
	            meson_option("lak_enable_windowing", {"true"}) +
	            meson_option("sdl2_from_source", {"true"}) +
	            meson_option("lak_backend", {"sdl2"}) +
	            meson_option("lak_renderer", {"cobalt"}) +
	            meson_option("lak_enable_glm", {"true"}) +
	            meson_option("lak_enable_imgui", {"true"}),
	        },
	      .target = "hello-cobalt"s,
	      .run    = {},
	    },
	  } +
	  common_targets;

	std::vector<target_t> windows_targets =
	  std::vector<target_t>{
	    {
	      .setups =
	        {
	          "--buildtype=debugoptimized "s +
	            meson_option("lak_enable_tests", {"true"}) +
	            // "Compatibility with CMake < 3.5 has been removed from CMake."
	            // meson_option("lak_enable_libfive", {"true"}) +
	            meson_option("lak_enable_lmdb", {"true"}),

	          "--buildtype=debugoptimized "s +
	            meson_option("lak_enable_tests", {"true"}) +
	            meson_option("lak_enable_windowing", {"true"}) +
	            meson_option("lak_use_native_file_dialog", {"true"}) +
	            meson_option("lak_backend", {"win32"}) +
	            meson_option("lak_enable_glm", {"true"}) +
	            meson_option("lak_enable_imgui", {"true"}),
	        },
	      .target = "lak_test"s,
	      .run    = "build/lak_test --help ; "
	                "build/lak_test --testall"s,
	    },
	    {
	      // test building with cobalt
	      .setups =
	        {
	          "--buildtype=release "s +
	            meson_option("lak_enable_tests", {"false"}) +
	            meson_option("lak_enable_examples", {"true"}) +
	            meson_option("lak_enable_windowing", {"true"}) +
	            meson_option("lak_backend", {"win32"}) +
	            meson_option("lak_renderer", {"cobalt"}) +
	            meson_option("lak_enable_glm", {"true"}) +
	            meson_option("lak_enable_imgui", {"true"}),
	        },
	      .target = "hello-cobalt"s,
	      .run    = {},
	    },
	  } +
	  common_targets;
	for (auto &t : windows_targets)
		for (auto &s : t.setups) s += " --vsenv"s;

	std::vector<matrix_entry_t> entries = {
	  // --- x64 ---
	  {
	    .os      = {.runner = "windows-2025-vs2026"s,
	                .system = "windows"s,
	                .arch   = "x64"s},
	    .targets = windows_targets,
	  },
	  {
	    .os = {.runner = "ubuntu-26.04"s, .system = "ubuntu"s, .arch = "x64"s},
	    .targets = native_targets,
	  },
	  {
	    .os = {.runner = "macos-26-intel"s, .system = "macos"s, .arch = "x64"s},
	    .targets = native_targets,
	  },

	  // --- arm64 ---
	  {
	    .os      = {.runner = "windows-11-vs2026-arm"s,
	                .system = "windows"s,
	                .arch   = "arm64"s},
	    .targets = windows_targets,
	  },
	  // {
	  //   .os      = {.runner = "ubuntu-26.04-arm"s,
	  //               .system = "ubuntu"s,
	  //               .arch   = "arm64"s},
	  //   .targets = native_targets,
	  // },
	  {
	    .os      = {.runner = "macos-26"s, .system = "macos"s, .arch = "arm64"s},
	    .targets = native_targets,
	  },

	  // --- x86 ---
	  {
	    .os      = {.runner = "windows-2025-vs2026"s,
	                .system = "windows"s,
	                .arch   = "x86"s},
	    .targets = common_targets,
	  },
	  {
	    .os = {.runner = "ubuntu-26.04"s, .system = "ubuntu"s, .arch = "x86"s},
	    .targets = common_targets,
	  },

	  // --- wasm32 ---
	  {
	    .os      = {.runner = "ubuntu-26.04"s,
	                .system = "ubuntu"s,
	                .arch   = "wasm32"s},
	    .targets = {{
	      .setups = {"--cross-file=cross/emscripten.txt "
	                 "--cross-file=cross/wasm32.txt "
	                 "--buildtype=debugoptimized "s +
	                 meson_option("lak_enable_tests", {"true"})},
	      .target = "lak_test"s,
	      .run    = "node build/lak_test.js --help ;"
	                "node build/lak_test.js --testall"s,
	    }},
	  },

	  // --- wasm64 ---
	  {
	    .os      = {.runner = "ubuntu-26.04"s,
	                .system = "ubuntu"s,
	                .arch   = "wasm64"s},
	    .targets = {{
	      .setups = {"--cross-file=cross/emscripten.txt "
	                 "--cross-file=cross/wasm64.txt "
	                 "--buildtype=debugoptimized "s +
	                 meson_option("lak_enable_tests", {"true"})},
	      .target = "lak_test"s,
	      .run    = "node build/lak_test.js --help ;"
	                "node build/lak_test.js --testall"s,
	    }},
	  },
	};

	print_matrix(entries);

	return EXIT_SUCCESS;
}

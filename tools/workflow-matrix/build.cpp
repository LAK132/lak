#include "main.hpp"

int main()
{
	using namespace std::literals::string_literals;

	std::vector<target_t> common_targets = {
	  {
	    // examples
	    .setups = {{
	      .setup_options = meson_option("lak_enable_examples", {"true"}) +
	                       meson_option("lak_enable_windowing", {"true"}) +
	                       meson_option("lak_enable_glm", {"true"}) +
	                       meson_option("lak_enable_imgui", {"true"}),
	      .build_type = build_type_t::debug_optimised,
	    }},
	    .target = "basic_program "
	              "nbt-view "
	              "template"s,
	    .run    = {},
	  },
	  {
	    // tests
	    .setups = {{
	      .setup_options = meson_option("lak_enable_tests", {"true"}),
	      .build_type    = build_type_t::debug_optimised,
	    }},
	    .target = "lak_test"s,
	    .run    = "build/lak_test --help ; "
	              "build/lak_test --testall"s,
	  },
	};

	std::vector<target_t> native_targets =
	  std::vector<target_t>{
	    {
	      // tests
	      .setups = {{.setup_options =
	                    meson_option("lak_enable_tests", {"true"}) +
	                    meson_option("lak_enable_godot_cpp", {"true"}) +
	                    meson_option("lak_enable_lua", {"true"}) +
	                    meson_option("lak_enable_stb", {"true"}) +
	                    meson_option("lak_enable_stb_image", {"true"}) +
	                    meson_option("lak_enable_stb_image_write", {"true"}),
	                  .build_type = build_type_t::debug_optimised},
	                 {.setup_options =
	                    meson_option("lak_enable_tests", {"true"}) +
	                    meson_option("lak_enable_windowing", {"true"}) +
	                    meson_option("lak_use_native_file_dialog", {"false"}) +
	                    meson_option("sdl2_from_source", {"true"}) +
	                    meson_option("lak_backend", {"sdl2"}) +
	                    meson_option("lak_enable_glm", {"true"}) +
	                    meson_option("lak_enable_imgui", {"true"}),
	                  .build_type = build_type_t::debug_optimised}},
	      .target = "lak_test"s,
	      .run    = "build/lak_test --help ; "
	                "build/lak_test --testall"s,
	    },
	    {
	      // test building with cobalt
	      .setups = {{
	        .setup_options = meson_option("lak_enable_tests", {"false"}) +
	                         meson_option("lak_enable_examples", {"true"}) +
	                         meson_option("lak_enable_windowing", {"true"}) +
	                         meson_option("sdl2_from_source", {"true"}) +
	                         meson_option("lak_backend", {"sdl2"}) +
	                         meson_option("lak_renderer", {"cobalt"}) +
	                         meson_option("lak_enable_glm", {"true"}) +
	                         meson_option("lak_enable_imgui", {"true"}),
	        .build_type = build_type_t::debug_optimised,
	      }},
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
	          {.setup_options =
	             meson_option("lak_enable_tests", {"true"}) +
	             // "Compatibility with CMake < 3.5 has been removed from CMake."
	             // meson_option("lak_enable_libfive", {"true"}) +
	             meson_option("lak_enable_lmdb", {"true"}),
	           .build_type = build_type_t::debug_optimised},
	          {.setup_options =
	             meson_option("lak_enable_tests", {"true"}) +
	             meson_option("lak_enable_windowing", {"true"}) +
	             meson_option("lak_use_native_file_dialog", {"true"}) +
	             meson_option("lak_backend", {"win32"}) +
	             meson_option("lak_enable_glm", {"true"}) +
	             meson_option("lak_enable_imgui", {"true"}),
	           .build_type = build_type_t::debug_optimised},
	        },
	      .target = "lak_test"s,
	      .run    = "build/lak_test --help ; "
	                "build/lak_test --testall"s,
	    },
	    {
	      // test building with cobalt
	      .setups = {{
	        .setup_options = meson_option("lak_enable_tests", {"false"}) +
	                         meson_option("lak_enable_examples", {"true"}) +
	                         meson_option("lak_enable_windowing", {"true"}) +
	                         meson_option("lak_backend", {"win32"}) +
	                         meson_option("lak_renderer", {"cobalt"}) +
	                         meson_option("lak_enable_glm", {"true"}) +
	                         meson_option("lak_enable_imgui", {"true"}),
	        .build_type = build_type_t::debug_optimised,
	      }},
	      .target = "hello-cobalt"s,
	      .run    = {},
	    },
	  } +
	  common_targets;
	for (auto &t : windows_targets)
		for (auto &s : t.setups) s.setup_options += " --vsenv"s;

	std::vector<target_t> windows_x86_targets = common_targets;

	std::vector<target_t> ubuntu_targets = native_targets;
	for (auto &t : ubuntu_targets)
		for (auto &s : t.setups) s.compile_options += " -j 1"s;

	std::vector<target_t> ubuntu_x86_targets = common_targets;
	for (auto &t : ubuntu_x86_targets)
		for (auto &s : t.setups) s.compile_options += " -j 1"s;

	std::vector<target_t> macos_targets = native_targets;

	std::vector<matrix_entry_t> entries = {
	  // --- x64 ---
	  {
	    .os      = {.runner = windows_intel_runner,
	                .system = "windows"s,
	                .arch   = "x64"s},
	    .targets = windows_targets,
	  },
	  {
	    .os      = {.runner = ubuntu_intel_runner,
	                .system = "ubuntu"s,
	                .arch   = "x64"s},
	    .targets = ubuntu_targets,
	  },
	  {
	    .os = {.runner = macos_intel_runner, .system = "macos"s, .arch = "x64"s},
	    .targets = macos_targets,
	  },

	  // --- arm64 ---
	  {
	    .os      = {.runner = windows_arm_runner,
	                .system = "windows"s,
	                .arch   = "arm64"s},
	    .targets = windows_targets,
	  },
	  // {
	  //   .os      = {.runner = ubuntu_arm_runner,
	  //               .system = "ubuntu"s,
	  //               .arch   = "arm64"s},
	  //   .targets = native_targets,
	  // },
	  {
	    .os = {.runner = macos_arm_runner, .system = "macos"s, .arch = "arm64"s},
	    .targets = macos_targets,
	  },

	  // --- x86 ---
	  {
	    .os      = {.runner = windows_intel_runner,
	                .system = "windows"s,
	                .arch   = "x86"s},
	    .targets = windows_x86_targets,
	  },
	  {
	    .os      = {.runner = ubuntu_intel_runner,
	                .system = "ubuntu"s,
	                .arch   = "x86"s},
	    .targets = ubuntu_x86_targets,
	  },

	  // --- wasm32 ---
	  {
	    .os      = {.runner = ubuntu_intel_runner,
	                .system = "ubuntu"s,
	                .arch   = "wasm32"s},
	    .targets = {{
	      .setups = {{.setup_options =
	                    "--cross-file=cross/emscripten.txt "
	                    "--cross-file=cross/wasm32.txt "
	                    "--buildtype=debugoptimized "s +
	                    meson_option("lak_enable_tests", {"true"})}},
	      .target = "lak_test"s,
	      .run    = "node build/lak_test.js --help ;"
	                "node build/lak_test.js --testall"s,
	    }},
	  },

	  // --- wasm64 ---
	  {
	    .os      = {.runner = ubuntu_intel_runner,
	                .system = "ubuntu"s,
	                .arch   = "wasm64"s},
	    .targets = {{
	      .setups = {{.setup_options =
	                    "--cross-file=cross/emscripten.txt "
	                    "--cross-file=cross/wasm64.txt "
	                    "--buildtype=debugoptimized "s +
	                    meson_option("lak_enable_tests", {"true"})}},
	      .target = "lak_test"s,
	      .run    = "node build/lak_test.js --help ;"
	                "node build/lak_test.js --testall"s,
	    }},
	  },
	};

	print_matrix(entries);

	return EXIT_SUCCESS;
}

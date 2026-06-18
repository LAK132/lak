#include <iostream>
#include <optional>
#include <string>
#include <vector>

struct os_t
{
	std::string runner;
	std::string system;
	std::string arch;

	friend std::ostream &operator<<(std::ostream &strm, const os_t &os)
	{
		strm << "os:{";
		strm << "runner:\"" << os.runner << "\",";
		strm << "system:\"" << os.system << "\",";
		strm << "arch:\"" << os.arch << "\"";
		strm << "},";
		return strm;
	}
};

struct target_t
{
	std::vector<std::string> setups;
	std::string target;
	std::string run;
};

std::vector<target_t> operator+(std::vector<target_t> a,
                                std::vector<target_t> b)
{
	std::vector<target_t> result;
	result.reserve(a.size() + b.size());
	for (auto &t : a) result.push_back(std::move(t));
	for (auto &t : b) result.push_back(std::move(t));
	return result;
}

struct matrix_entry_t
{
	os_t os;
	std::vector<target_t> targets;

	friend std::ostream &operator<<(std::ostream &strm, const matrix_entry_t &e)
	{
		size_t i = e.targets.size();
		for (const auto &t : e.targets)
		{
			--i;
			i += t.setups.size();
			for (const auto &s : t.setups)
			{
				strm << "{";
				strm << e.os;
				strm << "setup_options:\"" << s << "\",";
				strm << "target:\"" << t.target << "\",";
				strm << "run:\"" << t.run << "\"";
				strm << "}";
				if (--i != 0) strm << ",";
			}
		}
		return strm;
	}
};

int main()
{
	using namespace std::literals::string_literals;

	std::vector<target_t> common_targets = {
	  {
	    // examples
	    .setups =
	      {
	        "-Dlak_enable_examples=true "
	        "-Dlak_enable_windowing=true "
	        "-Dlak_enable_glm=true "
	        "-Dlak_enable_imgui=true"s,
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
	        "-Dlak_enable_tests=true"s,
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
	          "-Dlak_enable_tests=true "
	          "-Dlak_enable_godot_cpp=true "
	          "-Dlak_enable_lua=true "
	          "-Dlak_enable_stb=true "
	          "-Dlak_enable_stb_image=true "
	          "-Dlak_enable_stb_image_write=true"s,

	          "-Dlak_enable_tests=true "
	          "-Dlak_enable_windowing=true "
	          "-Dlak_use_native_file_dialog=false "
	          "-Dsdl2_from_source=true "
	          "-Dlak_backend=sdl "
	          "-Dlak_enable_glm=true "
	          "-Dlak_enable_imgui=true"s,
	        },
	      .target = "lak_test"s,
	      .run    = "build/lak_test --help ; "
	                "build/lak_test --testall"s,
	    },
	  } +
	  common_targets;

	std::vector<target_t> windows_targets =
	  std::vector<target_t>{
	    {
	      .setups =
	        {
	          "-Dlak_enable_tests=true "
	          // "Compatibility with CMake < 3.5 has been removed from CMake."
	          // "-Dlak_enable_libfive=true "
	          "-Dlak_enable_lmdb=true"s,

	          "-Dlak_enable_tests=true "
	          "-Dlak_enable_windowing=true "
	          "-Dlak_use_native_file_dialog=true "
	          "-Dlak_backend=win32 "
	          "-Dlak_enable_glm=true "
	          "-Dlak_enable_imgui=true"s,
	        },
	      .target = "lak_test"s,
	      .run    = "build/lak_test --help ; "
	                "build/lak_test --testall"s,
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
	    .os = {.runner = "ubuntu-24.04"s, .system = "ubuntu"s, .arch = "x64"s},
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
	  //   .os      = {.runner = "ubuntu-24.04-arm"s,
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
	    .os = {.runner = "ubuntu-24.04"s, .system = "ubuntu"s, .arch = "x86"s},
	    .targets = common_targets,
	  },

	  // --- wasm32 ---
	  {
	    .os      = {.runner = "ubuntu-24.04"s,
	                .system = "ubuntu"s,
	                .arch   = "wasm32"s},
	    .targets = {{
	      .setups = {"--cross-file=cross/emscripten.txt "
	                 "-Dlak_enable_tests=true"s},
	      .target = "lak_test"s,
	      .run    = "node build/lak_test.js --help ;"
	                "node build/lak_test.js --testall"s,
	    }},
	  },
	};

	std::cout << "matrix=[";
	size_t i = entries.size();
	for (const auto &e : entries)
	{
		std::cout << e;
		if (--i != 0) std::cout << ",";
		std::cout << "";
	}
	std::cout << "]\n";

	return EXIT_SUCCESS;
}

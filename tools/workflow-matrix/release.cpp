#include "main.hpp"

int main()
{
	using namespace std::literals::string_literals;

	std::vector<matrix_entry_t> entries = {
	  {
	    .os      = {.runner = "windows-2025-vs2026"s,
	                .system = "windows"s,
	                .arch   = "x64"s},
	    .targets = {{
	      .setups =
	        {
	          "-Dprefix=\"$pwd/install\" "
	          "-Dbindir=bin "
	          "-Dlibdir=bin "
	          "-Dcpp_args=\"-D_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR\" "
	          "--buildtype=release "
	          "-Dlak_bounds_checks=always "
	          "-Dlak_enable_examples=true "
	          "-Dlak_enable_tests=true "
	          "-Dlak_enable_windowing=true "
	          "-Dlak_enable_imgui=true "
	          "-Dlak_enable_stb=true "
	          "-Dlak_enable_stb_image=true "
	          "-Dlak_enable_stb_image_write=true "
	          "-Dlak_use_native_file_dialog=true "
	          "-Dlak_backend=win32 "
	          "-Dlak_renderer=softrender,opengl,cobalt "
	          "-Dcobalt_renderer=OpenGL3,OpenGL4,Vulkan,Direct3D11,Direct3D12"s,
	        },
	      .target = "lak_test "
	                "asc2csv "
	                "ebnf2cpp "
	                "lisk-repl "
	                "mdc2png "
	                "basic_program "
	                "hello-cobalt "
	                "mdc-view "
	                "nbt-view"s,
	      .run    = "install/bin/lak_test --help ; "
	                "install/bin/lak_test --testall"s,
	    }},
	  },
	};

	print_matrix(entries);

	return EXIT_SUCCESS;
}

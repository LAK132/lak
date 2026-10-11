#include "main.hpp"

int main()
{
	using namespace std::literals::string_literals;

	std::string windows_setup =
	  "--vsenv "s +
	  meson_option("cpp_args",
	               {quote("-D_DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR")}) +
	  meson_option("lak_bounds_checks", {"always"}) +
	  meson_option("lak_enable_examples", {"true"}) +
	  meson_option("lak_enable_tests", {"true"}) +
	  meson_option("lak_enable_windowing", {"true"}) +
	  meson_option("lak_enable_imgui", {"true"}) +
	  meson_option("lak_enable_stb", {"true"}) +
	  meson_option("lak_enable_stb_image", {"true"}) +
	  meson_option("lak_enable_stb_image_write", {"true"}) +
	  meson_option("lak_use_native_file_dialog", {"true"}) +
	  meson_option("lak_backend", {"win32"}) +
	  meson_option("lak_renderer", {"softrender", "opengl", "cobalt"}) +
	  meson_option("cobalt_renderer",
	               {"OpenGL3", "OpenGL4", "Vulkan", "Direct3D11", "Direct3D12"});
	std::string windows_run =
	  "install/bin/lak_test --help ; "
	  "install/bin/lak_test --testall"s;

	std::vector<matrix_entry_t> entries = {
	  {
	    .os = {.runner = windows_intel_runner,
	           .system = "windows"s,
	           .arch   = "x64"s},
	    .targets =
	      {
	        {
	          .setups = {{.setup_options = windows_setup,
	                      .build_type    = build_type_t::release}},
	          .target = "lak_test"s,
	          .run    = windows_run,
	        },
	      },
	  },
	};

	print_matrix(entries);

	return EXIT_SUCCESS;
}

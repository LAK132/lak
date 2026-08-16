project(
	'lak',
	['c', 'cpp'],
	meson_version: '>=1.4.0',
	license: 'MIT OR Unlicense',
	license_files: [
		'LICENSE',
		'UNLICENSE',
	],
	default_options: {
		'c_std': 'c11',
		'build.c_std': 'c11',
		'cpp_std': ['c++26', 'c++23', 'c++latest'],
		'build.cpp_std': ['c++26', 'c++23', 'c++latest'],
		'b_vscrt': 'static_from_buildtype',
		'warning_level': '3',
		'werror': false,
		'prefix': meson.global_build_root()/'install',
		'bindir': 'bin',
		'libdir': 'lib',
		'licensedir': 'share'/'licenses',
	},
)
if host_machine.system() == 'darwin'
	add_languages('objcpp', native: false, required: true)
endif

compilers = {
	'build': {
		'native': true,
		'hosted': meson.get_compiler('cpp', native: true).get_define('__STDC_HOSTED__') == '1',
		'machine': build_machine,
		'c': meson.get_compiler('c', native: true),
		'cpp': meson.get_compiler('cpp', native: true),
	},
	'host': {
		'native': false,
		'hosted': meson.get_compiler('cpp', native: false).get_define('__STDC_HOSTED__') == '1',
		'machine': host_machine,
		'c': meson.get_compiler('c', native: false),
		'cpp': meson.get_compiler('cpp', native: false),
	},
}
cxx = compilers['host']['cpp']

lak_natvis = meson.project_source_root()/'.natvis'

lak_reflection_argument = []
foreach type, comps : compilers
	if comps['cpp'].get_id() == 'msvc'
		add_project_arguments(
			'-DUNICODE',
			'-DWIN32_LEAN_AND_MEAN',
			'-DNOMINMAX',
			'-wd4315',
			'-wd4366',
			'-wd4250',
			'-Wv:18',
			language: ['c', 'cpp'],
			native: comps['native'])

		add_project_arguments(
			'-Zc:__cplusplus',
			'-Zc:rvalueCast',
			'-Zc:wchar_t',
			'-Zc:ternary',
			'-Zc:preprocessor',
			'-Zc:u8EscapeEncoding',
			language: ['cpp'],
			native: comps['native'])

		add_project_link_arguments(
			'Winmm.lib',
			'Setupapi.lib',
			'Version.lib',
			'Imm32.lib',
			'/NATVIS:@0@'.format(lak_natvis),
			language: ['c', 'cpp'],
			native: comps['native'])
	else
		add_project_arguments(
			'-Wno-unknown-warning-option',
			'-Wno-gnu-zero-variadic-macro-arguments',
			'-Wno-language-extension-token',
			'-Wno-dangling-else',
			'-Wno-microsoft-cast',
			'-Wno-pragma-pack',
			'-Wno-deprecated-declarations',
			'-Wno-unused-private-field',
			'-Wno-unused-variable',
			'-Wno-init-list-lifetime',
			'-Wno-invalid-partial-specialization',
			'-Wno-missing-field-initializers',
			'-Wno-maybe-uninitialized',
			'-Wfatal-errors',
			'-Wno-psabi',
			# '-mavx',
			language: ['c', 'cpp', 'objcpp'],
			native: comps['native'])

		if comps['cpp'].get_id() == 'gcc'
			if comps['cpp'].has_multi_arguments('-std=c++26', '-freflection')
				# GCC >= 16.1
				lak_reflection_argument += '-freflection'
			endif
			if comps['cpp'].has_link_argument('-Wl,-lstdc++_libbacktrace')
				# GCC <14
				add_project_link_arguments(
					'-Wl,-lstdc++_libbacktrace',
					language: ['cpp'],
					native: comps['native'])
			elif comps['cpp'].has_link_argument('-Wl,-lstdc++exp')
				# GCC >=14
				add_project_link_arguments(
					'-Wl,-lstdc++exp',
					language: ['cpp'],
					native: comps['native'])
			else
				warning('Unable to determine backtrace library link argument')
			endif
		endif
	endif
endforeach

if get_option('lak_bounds_checks') == 'never' or (get_option('lak_bounds_checks') == 'debug' and not get_option('debug'))
	add_project_arguments(
		'-DLAK_NO_BOUNDS_CHECKS',
		language: ['c', 'cpp', 'objcpp'])
endif

add_project_arguments(
	'-D_CRT_SECURE_NO_WARNINGS',
	language: ['c', 'cpp', 'objcpp'])

if not get_option('debug')
	add_project_arguments(
		'-DNDEBUG',
		language: ['c', 'cpp', 'objcpp'])
endif

if get_option('buildtype') == 'debug'
	# needed for cobalt (and *only* in the debug buildtype specifically)
	add_project_arguments(
		'-D_DEBUG',
		language: ['c', 'cpp', 'objcpp'])
endif

if get_option('lak_backend') == 'sdl2'
	add_project_arguments(
		'-DLAK_USE_SDL2',
		'-DSDL_MAIN_HANDLED',
		language: ['c', 'cpp', 'objcpp'])
elif get_option('lak_backend') == 'sdl3'
	add_project_arguments(
		'-DLAK_USE_SDL3',
		'-DSDL_MAIN_HANDLED',
		language: ['c', 'cpp', 'objcpp'])
elif get_option('lak_backend') == 'win32'
	add_project_arguments(
		'-DLAK_USE_WINAPI',
		language: ['c', 'cpp'])
elif get_option('lak_backend') == 'xlib'
	add_project_arguments(
		'-DLAK_USE_XLIB',
		language: ['c', 'cpp'])
elif get_option('lak_backend') == 'xcb'
	add_project_arguments(
		'-DLAK_USE_XCB',
		language: ['c', 'cpp'])
endif

if get_option('lak_renderer').contains('softrender')
	add_project_arguments(
		'-DLAK_ENABLE_SOFTRENDER',
		language: ['c', 'cpp', 'objcpp'])
endif

if get_option('lak_renderer').contains('opengl')
	add_project_arguments(
		'-DLAK_ENABLE_OPENGL',
		language: ['c', 'cpp', 'objcpp'])
endif

# get_option('cobalt_renderer') filtered by host machine system requirements
lak_cobalt_enabled_renderers = []
foreach renderer : get_option('cobalt_renderer')
	renderer_warning = 'Cobalt @0@ renderer is not available on @1@ systems'
	if renderer == 'OpenGL3' or renderer == 'Vulkan'
		lak_cobalt_enabled_renderers += renderer
	elif renderer == 'OpenGL4'
		if host_machine.system() != 'darwin'
			lak_cobalt_enabled_renderers += renderer
		else
			warning(renderer_warning.format(renderer, host_machine.system()))
		endif
	elif renderer == 'Direct3D11'
		if host_machine.system() == 'windows'
			lak_cobalt_enabled_renderers += renderer
		else
			warning(renderer_warning.format(renderer, host_machine.system()))
		endif
	elif renderer == 'Direct3D12'
		if host_machine.system() == 'windows'
			lak_cobalt_enabled_renderers += renderer
		else
			warning(renderer_warning.format(renderer, host_machine.system()))
		endif
	elif renderer == 'Metal'
		if host_machine.system() == 'darwin'
			lak_cobalt_enabled_renderers += renderer
		else
			warning(renderer_warning.format(renderer, host_machine.system()))
		endif
	else
		error('Invalid Cobalt renderer "@0@"'.format(renderer))
	endif
endforeach

if get_option('lak_renderer').contains('cobalt')
	add_project_arguments(
		'-DLAK_ENABLE_COBALT',
		language: ['c', 'cpp', 'objcpp'])

	if lak_cobalt_enabled_renderers.contains('OpenGL3')
		add_project_arguments(
			'-DLAK_ENABLE_COBALT_OGL3',
			language: ['c', 'cpp', 'objcpp'])
	endif

	if lak_cobalt_enabled_renderers.contains('OpenGL4')
		add_project_arguments(
			'-DLAK_ENABLE_COBALT_OGL4',
			language: ['c', 'cpp'])
	endif

	if lak_cobalt_enabled_renderers.contains('Direct3D11')
		add_project_arguments(
			'-DLAK_ENABLE_COBALT_D3D11',
			language: ['c', 'cpp'])
	endif

	if lak_cobalt_enabled_renderers.contains('Direct3D12')
		add_project_arguments(
			'-DLAK_ENABLE_COBALT_D3D12',
			language: ['c', 'cpp'])
	endif

	if lak_cobalt_enabled_renderers.contains('Vulkan')
		add_project_arguments(
			'-DLAK_ENABLE_COBALT_VK',
			language: ['c', 'cpp', 'objcpp'])
	endif

	if lak_cobalt_enabled_renderers.contains('Metal')
		add_project_arguments(
			'-DLAK_ENABLE_COBALT_MTL',
			language: ['c', 'cpp', 'objcpp'])
	endif
endif

if get_option('lak_run_windowing_tests')
	# even if building the windowing tests is enabled, we may not want to run
	# them, particularly if the test machine is headless
	add_project_arguments(
		'-DLAK_RUN_WINDOWING_TESTS',
		language: ['c', 'cpp', 'objcpp'])
endif

if get_option('lak_use_native_file_dialog')
	add_project_arguments(
		'-DLAK_USE_NATIVE_FILE_DIALOG',
		language: ['c', 'cpp', 'objcpp'])
endif

if get_option('lak_enable_windowing') or get_option('lak_enable_glm')
	add_project_arguments(
		'-DLAK_ENABLE_GLM',
		language: ['c', 'cpp', 'objcpp'])
endif

dependencies = []
dll_depends = []
dlls = []

subdir('src')

dependencies += lakcore_dep

subdir('tools'/'ebnf2cpp')

subdir('src'/'lisk')
dependencies += laklisk_dep

subdir('src'/'file')
dependencies += lakfile_dep

if get_option('lak_enable_tests')
	subdir('src'/'tests')
	dependencies += laktestcore_dep
endif

if get_option('lak_enable_windowing') or get_option('lak_enable_glm')
	subproject('glm-1.0.2')
	glm_dep = dependency('glm-1.0.2')
	dependencies += glm_dep
endif

if get_option('lak_enable_windowing')
	if get_option('lak_renderer').contains('softrender')
		laksoftrender_dep = declare_dependency(
			compile_args: '-DLAK_ENABLE_SOFTRENDER')
		dependencies += laksoftrender_dep
	endif

	if get_option('lak_renderer').contains('opengl')
		subdir('src'/'system'/'opengl')
		dependencies += lakopengl_dep
	endif

	if get_option('lak_renderer').contains('cobalt')
		subdir('src'/'system'/'cobalt')
		dependencies += lakcobalt_dep
	endif

	subdir('src'/'system'/'windowing')
	dependencies += lakwindowing_dep

	if get_option('lak_enable_tests')
		subdir('src'/'tests'/'windowing')
		dependencies += laktestwindowing_dep
	endif
endif

if get_option('lak_enable_imgui')
	subproject('imgui')
	imgui_dep = dependency('imgui')
	imgui_node_editor_dep = dependency('imgui-node-editor')
	imgui_color_text_edit_dep = dependency('ImGuiColorTextEdit')
	imgui_file_dialog_dep = dependency('ImFileDialog')
	imgui_memory_editor_dep = dependency('imgui_memory_editor')
	implot_dep = dependency('implot')
	implot3d_dep = dependency('implot3d')
	dependencies += imgui_dep

	if get_option('lak_enable_windowing')
		subdir('src'/'imgui')
		dependencies += lakimgui_dep

		if get_option('lak_enable_tests')
				subdir('src'/'tests'/'imgui')
				dependencies += laktestimgui_dep
		endif
	endif
endif

if get_option('lak_enable_godot_cpp')
	subproject('godot-cpp-4.5.0')
	godot_cpp_dep = dependency('godot-cpp-4.5.0')

	subdir('src'/'godot')
	# dependencies += lakgodot_dep

	# if get_option('lak_enable_tests')
	# 	subdir('src'/'tests'/'godot')
	# 	dependencies += laktestgodot_dep
	# endif
endif

if get_option('lak_enable_libfive')
	subproject('libfive-6f844b1')
	libfive_dep = dependency('libfive')

	subdir('src'/'libfive')
	dependencies += laklibfive_dep

	if get_option('lak_enable_tests')
		subdir('src'/'tests'/'libfive')
		dependencies += laktestlibfive_dep
	endif
endif

if get_option('lak_enable_libgphoto2')
	libgphoto2_subprj = subproject('libgphoto2')
	libgphoto2_dep = libgphoto2_subprj.get_variable('libgphoto2_dep')
	dependencies += libgphoto2_dep

	if get_option('lak_enable_tests')
		subdir('src'/'tests'/'libgphoto2')
		dependencies += laktestlibgphoto2_dep
	endif

	if host_machine.system() == 'windows'
		foreach dll : libgphoto2_subprj.get_variable('libgphoto2_dlls')
			dlls += dll
		endforeach
		dll_depends += libgphoto2_subprj.get_variable('libgphoto2_dlls_dep')
	endif
endif

if get_option('lak_enable_llvm')
	llvm_subprj = subproject('llvm-15.0.1')
	llvm_dep = llvm_subprj.get_variable('llvm_dep')

	subdir('src'/'llvm')
	dependencies += lakllvm_dep

	if get_option('lak_enable_tests')
		subdir('src'/'tests'/'llvm')
		dependencies += laktestllvm_dep
	endif
endif

if get_option('lak_enable_lmdb')
	subproject('lmdb-0.9.31')
	lmdb_dep = dependency('lmdb-0.9.31')

	subdir('src'/'lmdb')
	dependencies += laklmdb_dep

	if get_option('lak_enable_tests')
		subdir('src'/'tests'/'lmdb')
		dependencies += laktestlmdb_dep
	endif
endif

if get_option('lak_enable_lua')
	subproject('lua-5.4.4')
	lua_dep = dependency('lua-5.4.4')

	subdir('src'/'lua')
	dependencies += laklua_dep

	if get_option('lak_enable_tests')
		subdir('src'/'tests'/'lua')
		dependencies += laktestlua_dep
	endif
endif

if get_option('lak_enable_openvr')
	subdir('src'/'openvr')
	dependencies += lakopenvr_dep
endif

if get_option('lak_enable_stb')
	subproject('stb-f1c79c0')

	if get_option('lak_enable_stb_image')
		stb_image_dep = dependency('stb_image-2.30')
		dependencies += stb_image_dep
	endif

	if get_option('lak_enable_stb_image_write')
		stb_image_write_dep = dependency('stb_image_write-1.16')
		dependencies += stb_image_write_dep
	endif
endif

if host_machine.system() == 'windows'
	lakdlls_h = custom_target(
		'configure-dlls',
		depends: dll_depends,
		output: 'lakdlls.hpp',
		capture: true,
		command: [
			find_program('python3'),
			'-c',
			'import shutil;' +
			'import sys;' +
			'[shutil.copy(x, \'@OUTDIR@\') for x in sys.argv[4:]]',
			dlls,
		]
	)
else
	lakdlls_h = []
endif

lak_dep = declare_dependency(
	dependencies: dependencies,
	sources: lakdlls_h,
)

subdir('tools')

if get_option('lak_enable_examples')
	subdir('examples')
endif

if get_option('lak_enable_tests')
	lak_test_prog = executable(
		'lak_test',
		[
			'src'/'tests'/'test_main.cpp',
		],
		include_directories: include_directories('include'),
		dependencies: lak_dep,
		build_by_default: false,
		install: get_option('lak_install_targets').contains('lak_test'),
		install_rpath: get_option('prefix')/get_option('libdir'),
	)
endif

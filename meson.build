project(
	'lak',
	['c', 'cpp'],
)

cxx = meson.get_compiler('cpp')

if cxx.get_id() == 'msvc'
	version = 'c++latest'

	c_cpp_args = [
		'-DUNICODE',
		'-DWIN32_LEAN_AND_MEAN',
		'-DNOMINMAX',
		'-wd4315',
		'-wd4366',
		'-Wv:18',
	]

	cpp_args = [
		'-Zc:__cplusplus',
		'-Zc:rvalueCast',
		'-Zc:wchar_t',
		'-Zc:ternary',
		'-Zc:preprocessor',
	]

	c_args = [
		'-W0',
	]

	system_libraries = [
		'Winmm.lib',
		'Setupapi.lib',
		'Version.lib',
		'Imm32.lib',
	]

	macro_test_args = c_cpp_args + cpp_args + ['-std:' + version]
else
	version = 'c++20'

	c_cpp_args = [
		'-Wno-gnu-zero-variadic-macro-arguments',
		'-Wno-language-extension-token',
		'-Wno-dangling-else',
		'-Wno-microsoft-cast',
		'-Wno-pragma-pack',
		'-Wno-deprecated-declarations',
		'-Wno-unused-private-field',
		'-Wno-unused-variable',
		'-Wfatal-errors',
		'-Wno-psabi',
		# '-mavx',
	]

	cpp_args = []

	c_args = []

	system_libraries = []

	macro_test_args = c_cpp_args + cpp_args + ['-std=' + version]
endif

c_cpp_args += [
	'-D_CRT_SECURE_NO_WARNINGS',
]

if get_option('buildtype') != 'debug'
	c_cpp_args += [
		'-DNDEBUG',
	]
endif

if get_option('lak_backend') == 'sdl'
	c_cpp_args += [
		'-DLAK_USE_SDL',
		'-DSDL_MAIN_HANDLED',
	]
elif get_option('lak_backend') == 'win32'
	c_cpp_args += [
		'-DLAK_USE_WINAPI',
	]
endif

if get_option('lak_enable_softrender')
	c_cpp_args += '-DLAK_ENABLE_SOFTRENDER'
endif

if get_option('lak_enable_opengl')
	c_cpp_args += '-DLAK_ENABLE_OPENGL'
endif

if get_option('lak_enable_vulkan')
	c_cpp_args += '-DLAK_ENABLE_VULKAN'
endif

if get_option('lak_enable_metal')
	c_cpp_args += '-DLAK_ENABLE_METAL'
endif

if get_option('lak_run_windowing_tests')
	c_cpp_args += [
		'-DLAK_RUN_WINDOWING_TESTS',
	]
endif

if get_option('lak_use_native_file_dialog')
	c_cpp_args += '-DLAK_USE_NATIVE_FILE_DIALOG'
endif

add_project_arguments(c_cpp_args + cpp_args, language: ['cpp'])
add_project_arguments(c_cpp_args + c_args, language: ['c'])

cplusplus = cxx.get_define('__cplusplus', args: macro_test_args)

if cxx.compute_int(cplusplus) < 202002
	error('C++ version insufficient, expected 202002L got ' + cplusplus)
endif

dependencies = []
dll_depends = []
dlls = []

subdir('src')
dependencies += lakcore_dep

if get_option('lak_enable_tests')
	subdir('src/tests')
	dependencies += laktestcore_dep
endif

if get_option('lak_enable_windowing')
	if get_option('lak_enable_softrender')
		laksoftrender_dep = []
		dependencies += laksoftrender_dep
	endif

	if get_option('lak_enable_opengl')
		subdir('src/opengl')
		dependencies += lakopengl_dep
	endif

	if get_option('lak_enable_vulkan')
		lakvulkan_dep = []
		dependencies += lakvulkan_dep
	endif

	if get_option('lak_enable_metal')
		lakmetal_dep = []
		dependencies += lakmetal_dep
	endif

	glm_dep = subproject('glm').get_variable('glm_dep')
	dependencies += glm_dep

	subdir('src/windowing')
	dependencies += lakwindowing_dep

	if get_option('lak_enable_tests')
		subdir('src/tests/windowing')
		dependencies += laktestwindowing_dep
	endif
elif get_option('lak_enable_glm')
	glm_dep = subproject('glm').get_variable('glm_dep')
	dependencies += glm_dep
endif

if get_option('lak_enable_llvm')
	subdir('src/llvm')
	dependencies += lakllvm_dep

	if get_option('lak_enable_tests')
		subdir('src/tests/llvm')
		dependencies += laktestllvm_dep
	endif
endif

if get_option('lak_enable_lmdb')
	subdir('src/lmdb')
	dependencies += laklmdb_dep

	if get_option('lak_enable_tests')
		subdir('src/tests/lmdb')
		dependencies += laktestlmdb_dep
	endif
endif

if get_option('lak_enable_lua')
	subdir('src/lua')
	dependencies += laklua_dep

	if get_option('lak_enable_tests')
		subdir('src/tests/lua')
		dependencies += laktestlua_dep
	endif
endif

if get_option('lak_enable_imgui')
	imgui_dep = subproject('imgui').get_variable('imgui_dep')
	dependencies += imgui_dep

	if get_option('lak_enable_windowing')
		subdir('src/imgui')
		dependencies += lakimgui_dep

		if get_option('lak_enable_tests')
			subdir('src/tests/imgui')
			dependencies += laktestimgui_dep
		endif
	endif
endif

if get_option('lak_enable_libfive')
	subdir('src/libfive')
	dependencies += laklibfive_dep

	if get_option('lak_enable_tests')
		subdir('src/tests/libfive')
		dependencies += laktestlibfive_dep
	endif
endif

if get_option('lak_enable_libgphoto2')
	libgphoto2_subprj = subproject('libgphoto2')
	libgphoto2_dep = libgphoto2_subprj.get_variable('libgphoto2_dep')
	dependencies += libgphoto2_dep

	if get_option('lak_enable_tests')
		subdir('src/tests/libgphoto2')
		dependencies += laktestlibgphoto2_dep
	endif

	if target_machine.system() == 'windows'
		foreach dll : libgphoto2_subprj.get_variable('libgphoto2_dlls')
			dlls += dll
		endforeach
		dll_depends += libgphoto2_subprj.get_variable('libgphoto2_dlls_dep')
	endif
endif

if get_option('lak_enable_stb')
	stb_subprj = subproject('stb')

	if get_option('lak_enable_stb_image')
		dependencies += stb_subprj.get_variable('stb_image_dep')
	endif

	if get_option('lak_enable_stb_image_write')
		dependencies += stb_subprj.get_variable('stb_image_write_dep')
	endif
endif

if get_option('lak_enable_godot_cpp')
	subdir('src/godot')
	dependencies += lakgodot_dep
endif

if target_machine.system() == 'windows'
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
	link_args: system_libraries,
	sources: lakdlls_h,
)

if get_option('lak_enable_tests')
	lak_test_prog = executable(
		'lak_test',
		[
			'src/tests/test_main.cpp',
		],
		override_options: 'cpp_std=' + version,
		include_directories: include_directories('include'),
		dependencies: lak_dep,
	)

	lak_test_bigint_standalone_prog = executable(
		'lak_test_bigint_standalone',
		[
			'src/tests/bigint_standalone.cpp',
		],
		override_options: 'cpp_std=' + version,
		include_directories: include_directories('include'),
	)
endif

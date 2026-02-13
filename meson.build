project(
	'lak',
	['c', 'cpp', 'objcpp'],
	meson_version: '>=1.1.0',
)

cxx = meson.get_compiler('cpp')

if cxx.get_id() == 'msvc'
	version = 'c++latest'

	add_project_arguments(
		'-DUNICODE',
		'-DWIN32_LEAN_AND_MEAN',
		'-DNOMINMAX',
		'-wd4315',
		'-wd4366',
		'-wd4250',
		'-Wv:18',
		language: ['c', 'cpp'])

	add_project_arguments(
		'-Zc:__cplusplus',
		'-Zc:rvalueCast',
		'-Zc:wchar_t',
		'-Zc:ternary',
		'-Zc:preprocessor',
		language: ['cpp'])

	add_project_arguments(
		'-W0',
		language: ['c'])

	system_libraries = [
		'Winmm.lib',
		'Setupapi.lib',
		'Version.lib',
		'Imm32.lib',
	]
else
	version = 'c++20'

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
		language: ['c', 'cpp', 'objcpp'])

	system_libraries = []
endif

if get_option('debug')
	override_options = [
		'cpp_std=' + version,
		'werror=false',
	]
	override_options_w0 = [
		'cpp_std=' + version,
		'warning_level=0',
		'werror=false',
	]
	override_options_w1 = [
		'cpp_std=' + version,
		'warning_level=1',
		'werror=false',
	]
	override_options_w2 = [
		'cpp_std=' + version,
		'warning_level=2',
		'werror=false',
	]
	override_options_w3 = [
		'cpp_std=' + version,
		'warning_level=3',
		'werror=false',
	]
	override_options_werror = [
		'cpp_std=' + version,
		'warning_level=3',
		'werror=true',
	]
else
	override_options = [
		'cpp_std=' + version,
		'warning_level=0',
		'werror=false',
	]
	override_options_w0 = override_options
	override_options_w1 = override_options
	override_options_w2 = override_options
	override_options_w3 = override_options
	override_options_werror = override_options
endif

add_project_arguments(
	'-D_CRT_SECURE_NO_WARNINGS',
	language: ['c', 'cpp', 'objcpp'])

if get_option('buildtype') != 'debug'
	add_project_arguments(
		'-DNDEBUG',
		language: ['c', 'cpp', 'objcpp'])
endif

if get_option('lak_backend') == 'sdl'
	add_project_arguments(
		'-DLAK_USE_SDL',
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

if get_option('lak_enable_softrender')
	add_project_arguments(
		'-DLAK_ENABLE_SOFTRENDER',
		language: ['c', 'cpp', 'objcpp'])
endif

if get_option('lak_enable_opengl')
	add_project_arguments(
		'-DLAK_ENABLE_OPENGL',
		language: ['c', 'cpp', 'objcpp'])
endif

if get_option('lak_enable_cobalt')
	add_project_arguments(
		'-DLAK_ENABLE_COBALT',
		language: ['c', 'cpp', 'objcpp'])

	if get_option('lak_enable_cobalt_ogl3')
		add_project_arguments(
			'-DLAK_ENABLE_COBALT_OGL3',
			language: ['c', 'cpp', 'objcpp'])
	endif

	if get_option('lak_enable_cobalt_ogl4')
		add_project_arguments(
			'-DLAK_ENABLE_COBALT_OGL4',
			language: ['c', 'cpp'])
	endif

	if get_option('lak_enable_cobalt_d3d11')
		add_project_arguments(
			'-DLAK_ENABLE_COBALT_D3D11',
			language: ['c', 'cpp'])
	endif

	if get_option('lak_enable_cobalt_d3d12')
		add_project_arguments(
			'-DLAK_ENABLE_COBALT_D3D12',
			language: ['c', 'cpp'])
	endif

	if get_option('lak_enable_cobalt_vk')
		add_project_arguments(
			'-DLAK_ENABLE_COBALT_VK',
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

subdir('tools')

if get_option('lak_enable_tests')
	subdir('src'/'tests')
	dependencies += laktestcore_dep
endif

if get_option('lak_enable_glm')
	subproject('glm-1.0.2')
	glm_dep = dependency('glm-1.0.2')
	dependencies += glm_dep
endif

if get_option('lak_enable_windowing')
	if get_option('lak_enable_softrender')
		laksoftrender_dep = declare_dependency(
			compile_args: '-DLAK_ENABLE_SOFTRENDER')
		dependencies += laksoftrender_dep
	endif

	if get_option('lak_enable_opengl')
		subdir('src'/'system'/'opengl')
		dependencies += lakopengl_dep
	endif

	if get_option('lak_enable_cobalt')
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

	if target_machine.system() == 'windows'
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

if get_option('lak_enable_examples')
	subdir('examples')
endif

if get_option('lak_enable_tests')
	lak_test_prog = executable(
		'lak_test',
		[
			'src'/'tests'/'test_main.cpp',
		],
		override_options: override_options,
		include_directories: include_directories('include'),
		dependencies: lak_dep,
	)

	lak_test_bigint_standalone_prog = executable(
		'lak_test_bigint_standalone',
		[
			'src'/'tests'/'bigint_standalone.cpp',
		],
		override_options: override_options,
		include_directories: include_directories('include'),
	)
endif

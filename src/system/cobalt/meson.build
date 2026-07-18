cmake = import('cmake')

cobalt_opts = cmake.subproject_options()

cobalt_source_root = meson.global_source_root()/'subprojects'/'Cobalt'
cobalt_build_root = meson.global_build_root()/'subprojects'/'Cobalt'

cobalt_opts.set_override_option('cpp_std', 'c++17')
cobalt_opts.set_override_option('warning_level', '0')
if get_option('optimization') == '0'
	cobalt_opts.set_override_option('optimization', '1')
endif

cobalt_opts.add_cmake_defines({
	'COBALT_BUILD_TESTS': 'OFF',
	'COBALT_BUILD_UTILITIES': 'OFF',
	'COBALT_BUILD_EXAMPLES': 'OFF',
	'COBALT_OPENGL_USE_EGL': 'ON',
	'COBALT_BUILD_VULKAN_VALIDATION_LAYERS': 'OFF',
	'COBALT_USE_SDL3': 'OFF',
	'COBALT_USE_LOCAL_SDL3': 'ON',
	'COBALT_RUN_MSVC_STATIC_ANALYSIS': 'OFF',
	'COBALT_CLANG_FORMAT_ON_ALL': 'OFF',
	'COBALT_CLANG_TIDY_ON_ALL': 'OFF',

	'COBALT_EXTERNAL_ROOT': cobalt_source_root/'External',

	'COBALT_INHERIT_SYSTEM_INSTALL_PATH': 'ON',
	'CMAKE_INSTALL_PREFIX': get_option('prefix'),
	'COBALT_SDK_BIN_DIR': get_option('prefix')/get_option('bindir'),
	'COBALT_SDK_LIB_DIR': get_option('prefix')/get_option('libdir'),
	'COBALT_SDK_INCLUDE_DIR': get_option('prefix')/get_option('includedir')/'Cobalt',
})

# Vulkan-Loader
cobalt_opts.add_cmake_defines({
	'USE_MASM': 'OFF',
	'ENABLE_WERROR': 'OFF',
})

if host_machine.system() == 'linux'
	cobalt_opts.add_cmake_defines({'BUILD_WSI_XLIB_SUPPORT': 'ON'})
	cobalt_opts.add_cmake_defines({'BUILD_WSI_XCB_SUPPORT': 'ON'})
	cobalt_opts.add_cmake_defines({'BUILD_WSI_WAYLAND_SUPPORT': 'ON'})
else
	cobalt_opts.add_cmake_defines({'BUILD_WSI_XLIB_SUPPORT': 'OFF'})
	cobalt_opts.add_cmake_defines({'BUILD_WSI_XCB_SUPPORT': 'OFF'})
	cobalt_opts.add_cmake_defines({'BUILD_WSI_WAYLAND_SUPPORT': 'OFF'})
endif

vk_support_directfb = false
vk_support_qnx = false
if vk_support_directfb
	cobalt_opts.add_cmake_defines({'BUILD_WSI_DIRECTFB_SUPPORT': 'ON'})
else
	cobalt_opts.add_cmake_defines({'BUILD_WSI_DIRECTFB_SUPPORT': 'OFF'})
endif
if vk_support_qnx
	cobalt_opts.add_cmake_defines({'BUILD_WSI_QNX_SUPPORT': 'ON'})
else
	cobalt_opts.add_cmake_defines({'BUILD_WSI_QNX_SUPPORT': 'OFF'})
endif

cobalt_opts.set_install(false)

cobalt_opts.set_install(true, target: 'ShaderSupport')
cobalt_opts.set_install(true, target: 'TextureFormatConversion')
if lak_cobalt_enabled_renderers.contains('OpenGL3')
	cobalt_opts.set_install(true, target: 'OpenGL3Renderer')
endif
if lak_cobalt_enabled_renderers.contains('OpenGL4')
	cobalt_opts.set_install(true, target: 'OpenGL4Renderer')
endif
if lak_cobalt_enabled_renderers.contains('Direct3D11')
	cobalt_opts.set_install(true, target: 'Direct3D11Renderer')
endif
if lak_cobalt_enabled_renderers.contains('Direct3D12')
	cobalt_opts.set_install(true, target: 'Direct3D12Renderer')
	cobalt_opts.set_install(true, target: 'WinPixEventRuntime')
	dxc_bin_dir = cobalt_source_root/'External'/'Cache'/'DirectXShaderCompilerOfficial'/'bin'
	if host_machine.cpu_family() == 'x86'
		meson.add_install_script('install_dxc_x86.bat')
	elif host_machine.cpu_family() == 'x86_64'
		meson.add_install_script('install_dxc_x64.bat')
	elif host_machine.cpu_family() == 'aarch64'
		meson.add_install_script('install_dxc_arm64.bat')
	endif
endif
if lak_cobalt_enabled_renderers.contains('Vulkan')
	cobalt_opts.set_install(true, target: 'VulkanRenderer')
	if host_machine.system() == 'darwin'
		cobalt_opts.set_install(true, target: 'vulkan')
		meson.add_install_script('install_moltenvk.sh')
	endif
	if host_machine.system() == 'windows'
		cobalt_opts.set_install(true, target: 'vulkan')
		meson.add_install_script('install_vulkan.bat')
	endif
endif
if lak_cobalt_enabled_renderers.contains('Metal')
	cobalt_opts.set_install(true, target: 'MetalRenderer')
endif

vis_hidden = '-fvisibility=hidden'
if meson.get_compiler('cpp').has_argument(vis_hidden)
	cobalt_opts.append_compile_args('cpp', vis_hidden)
endif
if host_machine.system() == 'darwin' and meson.get_compiler('objcpp').has_argument(vis_hidden)
	cobalt_opts.append_compile_args('objcpp', vis_hidden)
endif

if host_machine.system() == 'darwin'
	cobalt_opts.append_compile_args('objcpp', '-Wno-builtin-macro-redefined')
endif

if host_machine.system() == 'windows'
	cobalt_opts.append_link_args(
		'/DEF:@0@'.format(cobalt_source_root/'External'/'Direct3D12Libraries'/'PixEvents'/'WinPixEventRuntime.def'),
		target: 'WinPixEventRuntime',
	)
	cobalt_opts.append_link_args(
		'/DEF:@0@'.format(cobalt_source_root/'External'/'Cache'/'Vulkan-Loader'/'loader'/'vulkan-1.def'),
		target: 'vulkan',
	)
endif

if get_option('buildtype') != 'release'
	cobalt_opts.append_compile_args('cpp', '-D_DEBUG')
	cobalt_opts.append_compile_args('objcpp', '-D_DEBUG')
endif

disabled_warnings = []
if meson.get_compiler('cpp').get_id() == 'gcc'
	disabled_warnings = [
		'-Wno-delete-non-virtual-dtor',
		'-Wno-cast-function-type',
		'-Wno-switch-default',
		'-Wno-double-promotion',
		'-Wno-extra-semi',
		'-Wno-float-equal',
		'-Wno-implicit-fallthrough',
		'-Wno-non-virtual-dtor',
		'-Wno-old-style-cast',
		'-Wno-reorder',
		'-Wno-shadow',
		'-Wno-sign-conversion',
		'-Wno-suggest-override',
		'-Wno-switch',
		'-Wno-switch-enum',
		'-Wno-unused-macros',
		'-Wno-unused-parameter',
		'-Wno-unused-variable',
		'-Wno-zero-as-null-pointer-constant',
	]
elif meson.get_compiler('cpp').get_id() == 'clang'
	disabled_warnings = [
		'-Wno-unknown-warning-option',
		'-Wno-c++98-compat',
		'-Wno-c++98-compat-pedantic',
		'-Wno-cast-function-type',
		'-Wno-covered-switch-default',
		'-Wno-ctad-maybe-unsupported',
		'-Wno-declaration-after-statement',
		'-Wno-delete-non-virtual-dtor',
		'-Wno-disabled-macro-expansion',
		'-Wno-documentation-unknown-command',
		'-Wno-double-promotion',
		'-Wno-exit-time-destructors',
		'-Wno-extra-semi',
		'-Wno-float-equal',
		'-Wno-global-constructors',
		'-Wno-implicit-fallthrough',
		'-Wno-missing-noreturn',
		'-Wno-non-virtual-dtor',
		'-Wno-old-style-cast',
		'-Wno-padded',
		'-Wno-reorder-ctor',
		'-Wno-reserved-identifier',
		'-Wno-reserved-macro-identifier',
		'-Wno-shadow',
		'-Wno-sign-conversion',
		'-Wno-suggest-override',
		'-Wno-switch',
		'-Wno-switch-default',
		'-Wno-switch-enum',
		'-Wno-unneeded-internal-declaration',
		'-Wno-unsafe-buffer-usage',
		'-Wno-unused-command-line-argument',
		'-Wno-unused-macros',
		'-Wno-unused-parameter',
		'-Wno-unused-variable',
		'-Wno-unused-private-field',
		'-Wno-zero-as-null-pointer-constant',
	]
elif meson.get_compiler('cpp').get_id() == 'msvc'
	disabled_warnings = [
		'-Wv:19.50.35718',
		'-wd4061',
		'-wd4062',
		'-wd4100',
		'-wd4127',
		'-wd4191',
		'-wd4250',
		'-wd4255',
		'-wd4265',
		'-wd4302',
		'-wd4324',
		'-wd4339',
		'-wd4342',
		'-wd4347',
		'-wd4350',
		'-wd4365',
		'-wd4371',
		'-wd4412',
		'-wd4428',
		'-wd4435',
		'-wd4464',
		'-wd4471',
		'-wd4514',
		'-wd4571',
		'-wd4574',
		'-wd4610',
		'-wd4619',
		'-wd4623',
		'-wd4625',
		'-wd4626',
		'-wd4628',
		'-wd4640',
		'-wd4641',
		'-wd4668',
		'-wd4682',
		'-wd4692',
		'-wd4710',
		'-wd4711',
		'-wd4738',
		'-wd4746',
		'-wd4774',
		'-wd4777',
		'-wd4820',
		'-wd4865',
		'-wd4868',
		'-wd4917',
		'-wd4946',
		'-wd4987',
		'-wd5026',
		'-wd5027',
		'-wd5038',
		'-wd5039',
		'-wd5045',
		'-wd5204',
		'-wd5220',
		'-wd5246',
	]
endif
cobalt_opts.append_compile_args('cpp', disabled_warnings)
cobalt_opts.append_compile_args('objcpp', disabled_warnings)

cobalt_subprj = cmake.subproject('Cobalt', options: cobalt_opts)

_lakcobalt_srcs = [
	'log_target.cpp',
]
_lakcobalt_deps = []
_lakcobalt_compile_args = [
	'-DLAK_ENABLE_COBALT',
]
_lakcobalt_link_args = []

if host_machine.system() == 'windows'
	_lakcobalt_compile_args += '-DCOBALT_RENDERER_WIN32_SUPPORT'
elif host_machine.system() == 'darwin'
	_lakcobalt_compile_args += '-DCOBALT_RENDERER_APPKIT_SUPPORT'
elif host_machine.system() == 'linux'
	_lakcobalt_compile_args += '-DCOBALT_RENDERER_WAYLAND_SUPPORT'
	_lakcobalt_compile_args += '-DCOBALT_RENDERER_XLIB_SUPPORT'
	_lakcobalt_compile_args += '-DCOBALT_RENDERER_XCB_SUPPORT'
else
	_lakcobalt_compile_args += '-DCOBALT_RENDERER_MANUAL_PLATFORM_BINDINGS'
endif

cobalt_shader_support = cobalt_subprj.dependency('ShaderSupport')
cobalt_texture_format_conversion = cobalt_subprj.dependency('TextureFormatConversion')

if lak_cobalt_enabled_renderers.contains('OpenGL3')
	_lakcobalt_srcs += 'opengl3.cpp'
	_lakcobalt_deps += cobalt_subprj.dependency('OpenGL3Renderer')
	_lakcobalt_compile_args += '-DLAK_ENABLE_COBALT_OGL3'
	if meson.get_compiler('cpp').get_id() == 'msvc'
		_lakcobalt_link_args += '/delayload:OpenGL3Renderer.dll'
	endif
endif

if lak_cobalt_enabled_renderers.contains('OpenGL4')
	_lakcobalt_srcs += 'opengl4.cpp'
	_lakcobalt_deps += cobalt_subprj.dependency('OpenGL4Renderer')
	_lakcobalt_compile_args += '-DLAK_ENABLE_COBALT_OGL4'
	if meson.get_compiler('cpp').get_id() == 'msvc'
		_lakcobalt_link_args += '/delayload:OpenGL4Renderer.dll'
	endif
endif

if lak_cobalt_enabled_renderers.contains('Direct3D11')
	_lakcobalt_srcs += 'direct3d11.cpp'
	_lakcobalt_deps += cobalt_subprj.dependency('Direct3D11Renderer')
	_lakcobalt_compile_args += '-DLAK_ENABLE_COBALT_D3D11'
	if meson.get_compiler('cpp').get_id() == 'msvc'
		_lakcobalt_link_args += '/delayload:Direct3D11Renderer.dll'
	endif
endif

if lak_cobalt_enabled_renderers.contains('Direct3D12')
	_lakcobalt_srcs += 'direct3d12.cpp'
	_lakcobalt_deps += cobalt_subprj.dependency('Direct3D12Renderer')
	_lakcobalt_compile_args += '-DLAK_ENABLE_COBALT_D3D12'
	if meson.get_compiler('cpp').get_id() == 'msvc'
		_lakcobalt_link_args += '/delayload:Direct3D12Renderer.dll'
	endif
endif

if lak_cobalt_enabled_renderers.contains('Vulkan')
	_lakcobalt_srcs += 'vulkan.cpp'
	_lakcobalt_deps += cobalt_subprj.dependency('VulkanRenderer')
	if host_machine.system() == 'darwin'
		vk_sdk = meson.global_build_root()/'subprojects'/'Cobalt'/'__CMake_build'/'_deps'/'vulkan-sdk-install'/'macOS'
		_lakcobalt_compile_args += '-DLAK_VK_DRIVER_PREFIX="@0@"'.format(get_option('prefix'))
		_lakcobalt_compile_args += '-DLAK_MOLTENVK_JSON="@0@"'.format('share'/'vulkan'/'icd.d'/'MoltenVK_icd.json')
	endif
	_lakcobalt_compile_args += '-DLAK_ENABLE_COBALT_VK'
	if meson.get_compiler('cpp').get_id() == 'msvc'
		_lakcobalt_link_args += '/delayload:VulkanRenderer.dll'
	endif
endif

if lak_cobalt_enabled_renderers.contains('Metal')
	_lakcobalt_srcs += 'metal.cpp'
	_lakcobalt_deps += cobalt_subprj.dependency('MetalRenderer')
	_lakcobalt_compile_args += '-DLAK_ENABLE_COBALT_MTL'
	if meson.get_compiler('cpp').get_id() == 'msvc'
		_lakcobalt_link_args += '/delayload:MetalRenderer.dll'
	endif
endif

_lakcobalt_dep = declare_dependency(
	dependencies: _lakcobalt_deps,
)

lakcobalt = static_library(
	'lakcobalt',
	_lakcobalt_srcs,
	override_options: {
		'warning_level': '2',
	},
	include_directories: lakcore_inc,
	dependencies: [
		lakcore_dep,
		_lakcobalt_dep,
	],
)

lakcobalt_dep = declare_dependency(
	link_with: lakcobalt,
	dependencies: _lakcobalt_dep.partial_dependency(includes: true),
	compile_args: _lakcobalt_compile_args,
	link_args: _lakcobalt_link_args,
)
meson.override_dependency('lak-cobalt', lakcobalt_dep)

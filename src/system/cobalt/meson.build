cmake = import('cmake')

cobalt_opts = cmake.subproject_options()

cobalt_source_root = meson.global_source_root()/'subprojects'/'Cobalt'
cobalt_build_root = meson.global_build_root()/'subprojects'/'Cobalt'

cobalt_opts.add_cmake_defines({
	# Cobalt
	'COBALT_BUILD_TESTS': 'OFF',
	'COBALT_BUILD_UTILITIES': 'OFF',
	'COBALT_BUILD_EXAMPLES': 'OFF',
	'COBALT_BUILD_VULKAN_VALIDATION_LAYERS': 'OFF',
	'COBALT_OPENGL_USE_EGL': 'ON',
	# 'COBALT_ARCH': '',
	# 'COBALT_ARCH_SHORT': '',
	'COBALT_EXTERNAL_ROOT': cobalt_source_root/'External',

	'COBALT_INSTALL_PREFIX': get_option('prefix'),
	'COBALT_SDK_BIN_DIR': get_option('prefix')/get_option('bindir'),
	'COBALT_SDK_LIB_DIR': get_option('prefix')/get_option('libdir'),
	'COBALT_SDK_INCLUDE_DIR': get_option('prefix')/get_option('includedir')/'Cobalt',

	# Vulkan-Loader
	'USE_MASM': 'OFF',
	'ENABLE_WERROR': 'OFF',
})

cobalt_opts.set_install(false)

cobalt_opts.set_install(true, target: 'ShaderSupport')
cobalt_opts.set_install(true, target: 'TextureFormatConversion')
if get_option('lak_enable_cobalt_ogl3')
	cobalt_opts.set_install(true, target: 'OpenGL3Renderer')
endif
if get_option('lak_enable_cobalt_ogl4')
	cobalt_opts.set_install(true, target: 'OpenGL4Renderer')
endif
if get_option('lak_enable_cobalt_d3d11')
	cobalt_opts.set_install(true, target: 'Direct3D11Renderer')
endif
if get_option('lak_enable_cobalt_d3d12')
	cobalt_opts.set_install(true, target: 'Direct3D12Renderer')
	cobalt_opts.set_install(true, target: 'WinPixEventRuntime')
endif
if get_option('lak_enable_cobalt_vk')
	cobalt_opts.set_install(true, target: 'VulkanRenderer')
endif

if target_machine.system() == 'windows'
	cobalt_opts.append_link_args(
		'/DEF:@0@'.format(cobalt_source_root/'External'/'Direct3D12Libraries'/'PixEvents'/'WinPixEventRuntime.def'),
		target: 'WinPixEventRuntime',
	)
	cobalt_opts.append_link_args(
		'/DEF:@0@'.format(cobalt_source_root/'External'/'Vulkan-Loader'/'loader'/'vulkan-1.def'),
		target: 'vulkan',
	)
endif

cobalt_subprj = cmake.subproject('Cobalt', options: cobalt_opts)

_lakcobalt_srcs = [
	'log_target.cpp',
]
_lakcobalt_deps = []

cobalt_shader_support = cobalt_subprj.dependency('ShaderSupport')
cobalt_texture_format_conversion = cobalt_subprj.dependency('TextureFormatConversion')

if get_option('lak_enable_cobalt_ogl3')
	_lakcobalt_srcs += 'opengl3.cpp'
	_lakcobalt_deps += cobalt_subprj.dependency('OpenGL3Renderer')
endif

if get_option('lak_enable_cobalt_ogl4')
	_lakcobalt_srcs += 'opengl4.cpp'
	_lakcobalt_deps += cobalt_subprj.dependency('OpenGL4Renderer')
endif

if get_option('lak_enable_cobalt_d3d11')
	_lakcobalt_srcs += 'direct3d11.cpp'
	_lakcobalt_deps += cobalt_subprj.dependency('Direct3D11Renderer')
endif

if get_option('lak_enable_cobalt_d3d12')
	_lakcobalt_srcs += 'direct3d12.cpp'
	_lakcobalt_deps += cobalt_subprj.dependency('Direct3D12Renderer')
endif

if get_option('lak_enable_cobalt_vk')
	_lakcobalt_srcs += 'vulkan.cpp'
	_lakcobalt_deps += cobalt_subprj.dependency('VulkanRenderer')
endif

_lakcobalt_dep = declare_dependency(
	dependencies: _lakcobalt_deps,
)

lakcobalt = static_library(
	'lakcobalt',
	_lakcobalt_srcs,
	override_options: override_options_w2,
	include_directories: lakcore_inc,
	dependencies: [
		lakcore_dep,
		_lakcobalt_dep,
	],
	link_args: lakcore_linkargs,
)

lakcobalt_dep = declare_dependency(
	link_with: lakcobalt,
	dependencies: _lakcobalt_dep.partial_dependency(includes: true),
)
meson.override_dependency('lak-cobalt', lakcobalt_dep)

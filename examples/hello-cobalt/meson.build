if not get_option('lak_enable_windowing')
	warning('hello-cobalt requires windowing')
	subdir_done()
endif
if get_option('lak_backend') != 'win32'
	warning('hello-cobalt requires the win32 backend')
	subdir_done()
endif
if not get_option('lak_enable_cobalt')
	warning('hello-cobalt requires cobalt')
	subdir_done()
endif
if not get_option('lak_enable_glm')
	warning('hello-cobalt requires glm')
	subdir_done()
endif
if not get_option('lak_enable_imgui')
	warning('hello-cobalt requires imgui')
	subdir_done()
endif

hello_cobalt_deps = [
	cobalt_subprj.target('ShaderSupport'),
	cobalt_subprj.target('TextureFormatConversion'),
]

if get_option('lak_enable_cobalt_ogl3')
	hello_cobalt_deps += [
		cobalt_subprj.target('OpenGL3Renderer'),
	]
endif

if get_option('lak_enable_cobalt_ogl4')
	hello_cobalt_deps += [
		cobalt_subprj.target('OpenGL4Renderer'),
	]
endif

if get_option('lak_enable_cobalt_d3d11')
	hello_cobalt_deps += [
		cobalt_subprj.target('Direct3D11Renderer'),
	]
endif

if get_option('lak_enable_cobalt_d3d12')
	hello_cobalt_deps += [
		cobalt_subprj.target('Direct3D12Renderer'),
		cobalt_subprj.target('WinPixEventRuntime'),
	]
endif

if get_option('lak_enable_cobalt_vk')
	hello_cobalt_deps += [
		cobalt_subprj.target('VulkanRenderer'),
	]
endif

hello_cobalt = files([
	'main.cpp',
])

hello_cobalt_dll_copy = []
foreach dll : lakcobalt_dlls
hello_cobalt_dll_copy += custom_target(
	dll + '-copy',
	depends: hello_cobalt_deps,
	output: dll+'.dll',
	command: [
		import('python').find_installation('python3'),
		meson.project_source_root()/'copy.py',
		cobalt_build_root/(dll+'.dll'),
		'@OUTPUT@',
	]
)
endforeach

executable(
	'hello-cobalt',
	hello_cobalt,
	override_options: override_options,
	dependencies: [
		lak_dep,
	],
	link_depends: [
		hello_cobalt_dll_copy,
	],
)

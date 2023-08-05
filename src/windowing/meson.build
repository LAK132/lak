subdir(get_option('lak_backend'))

lakwindowing_compileargs = []

if get_option('lak_enable_softrender')
	lakwindowing_compileargs += '-DLAK_ENABLE_SOFTRENDER'
endif

if get_option('lak_enable_opengl')
	lakwindowing_compileargs += '-DLAK_ENABLE_OPENGL'
endif

if get_option('lak_enable_vulkan')
	lakwindowing_compileargs += '-DLAK_ENABLE_VULKAN'
endif

if get_option('lak_enable_metal')
	lakwindowing_compileargs += '-DLAK_ENABLE_METAL'
endif

lakwindowing_dep = declare_dependency(
	link_with: lakwindowing,
	dependencies: lakwindowing_dependencies.partial_dependency(
		includes: true, compile_args: true, sources: true),
	compile_args: lakwindowing_compileargs,
)

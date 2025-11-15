cmake = import('cmake')

openvr_opts = cmake.subproject_options()

openvr_opts.add_cmake_defines({
	'BUILD_SHARED': 'OFF',
})
openvr_opts.set_install(false)
if get_option('b_vscrt') == 'mt'
	openvr_opts.append_compile_args('cpp', '/MT')
elif get_option('b_vscrt') == 'mtd'
	openvr_opts.append_compile_args('cpp', '/MTd')
elif get_option('b_vscrt') == 'static_from_buildtype'
	if get_option('buildtype') == 'debug'
		openvr_opts.append_compile_args('cpp', '/MTd')
	else
		openvr_opts.append_compile_args('cpp', '/MT')
	endif
elif get_option('b_vscrt') == 'md'
	openvr_opts.append_compile_args('cpp', '/MD')
elif get_option('b_vscrt') == 'mdd'
	openvr_opts.append_compile_args('cpp', '/MDd')
elif get_option('b_vscrt') == 'from_buildtype'
	if get_option('buildtype') == 'debug'
		openvr_opts.append_compile_args('cpp', '/MDd')
	else
		openvr_opts.append_compile_args('cpp', '/MD')
	endif
elif get_option('b_vscrt') != 'none'
	error('unsupported vscrt link mode')
endif

openvr_subprj = cmake.subproject('openvr', options: openvr_opts)

if host_machine.system() == 'windows'
	openvr_dep = openvr_subprj.dependency('openvr_api64').as_link_whole()
else
	openvr_dep = openvr_subprj.dependency('openvr_api').as_link_whole()
endif

lakopenvr = static_library(
	'lakopenvr',
	[
		'openvr.cpp',
	],
	override_options: override_options,
	dependencies: [
		lakcore_dep,
		openvr_dep,
		glm_dep,
	]
)

lakopenvr_dep = declare_dependency(
	link_with: lakopenvr,
	dependencies: [
		openvr_dep,
		glm_dep,
	]
)

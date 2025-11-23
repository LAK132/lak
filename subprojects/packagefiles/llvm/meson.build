project(
	'llvm',
	'cpp',
	license: 'Apache-2.0 WITH LLVM-exception',
	version: '15.0.1',
)

cmake = find_program('cmake')

if get_option('buildtype') == 'debug'
	llvm_build_type = 'Debug'
elif get_option('buildtype') == 'release'
	llvm_build_type = 'Release'
elif get_option('buildtype') == 'debugoptimized'
	llvm_build_type = 'RelWithDebInfo'
elif get_option('buildtype') == 'minsize'
	llvm_build_type = 'MinSizeRel'
else
	error('unsupported build type')
endif

if get_option('b_vscrt') == 'mt'
	llvm_vscrt_link = 'MT'
elif get_option('b_vscrt') == 'mtd'
	llvm_vscrt_link = 'MTd'
elif get_option('b_vscrt') == 'static_from_buildtype'
	if get_option('buildtype') == 'debug'
		llvm_vscrt_link = 'MTd'
	else
		llvm_vscrt_link = 'MT'
	endif
elif get_option('b_vscrt') == 'md'
	llvm_vscrt_link = 'MD'
elif get_option('b_vscrt') == 'mdd'
	llvm_vscrt_link = 'MDd'
elif get_option('b_vscrt') == 'from_buildtype'
	if get_option('buildtype') == 'debug'
		llvm_vscrt_link = 'MDd'
	else
		llvm_vscrt_link = 'MD'
	endif
elif get_option('b_vscrt') != 'none'
	error('unsupported vscrt link mode')
endif

llvm_build = custom_target(
	'configure-llvm',
	output: ['llvm.spec'],
	console: true,
	command: [
		cmake,
		'-DLLVM_PARALLEL_LINK_JOBS=' + get_option('llvm_parallel_link_jobs'),
		'-DLLVM_TARGETS_TO_BUILD=host',
		'-DLLVM_ENABLE_PROJECTS=llvm;clang;compiler-rt',
		'-DLLVM_USE_CRT_RELEASE=' + llvm_vscrt_link,
		'-DLLVM_USE_CRT_DEBUG=' + llvm_vscrt_link,
		'-DLLVM_USE_CRT_RELWITHDEBINFO=' + llvm_vscrt_link,
		'-DLLVM_USE_CRT_MINSIZEREL=' + llvm_vscrt_link,
		'-DCMAKE_BUILD_TYPE=' + llvm_build_type,
		'-DCMAKE_INSTALL_PREFIX=@CURRENT_SOURCE_DIR@/build',
		'-S', '@CURRENT_SOURCE_DIR@/llvm',
		'-B', '@OUTDIR@',
		'-G', 'Ninja',
	],
)

llvm_build_dir = llvm_build[0].full_path()/'..'

llvm_build_targets = [
	'install-llvm-config',
	'install-llvm-headers',
	'install-LLVM-C',
]
foreach target : get_option('llvm_install_components')
	llvm_build_targets += 'install-LLVM' + target
endforeach

subdir('lib')
subdir('build/include/llvm/Config')

llvm_inc = include_directories(
	'build/include',
	'llvm/include',
)

llvm_dep = declare_dependency(
	link_whole: llvm_c,
	include_directories: llvm_inc,
	sources: cmake_generated_headers,
	version: meson.project_version(),
)

# not working for some reason
# meson.override_dependency(
# 	'llvm-@0@'.format(meson.project_version()), llvm_dep)
# meson.override_dependency('llvm', llvm_dep)

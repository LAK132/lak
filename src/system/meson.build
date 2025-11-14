lakcore_system_common = static_library(
	'lakcore_system_common',
	[
		'architecture.cpp',
		'file.cpp',
		'page_memory.cpp',
		'profile.cpp',
	],
	override_options: override_options_w3,
	include_directories: lakcore_inc,
	dependencies: [],
	link_args: lakcore_linkargs,
)

if target_machine.system() == 'linux'
	subdir('posix')
	subdir('linux')
	lakcore_system = [
		lakcore_system_common,
		lakcore_system_posix,
		lakcore_system_macos,
	]
elif target_machine.system() == 'darwin'
	subdir('posix')
	subdir('macos')
	lakcore_system = [
		lakcore_system_common,
		lakcore_system_posix,
		lakcore_system_macos,
	]
elif target_machine.system() == 'windows'
	subdir('win32')
	lakcore_system = [
		lakcore_system_common,
		lakcore_system_win32,
	]
else
	error('Unknown system @0@'.format(target_machine.system()))
endif

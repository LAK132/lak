if target_machine.system() == 'windows'
	libgphoto2_msys = custom_target(
		'build_libgphoto2',
		input: ['win_build.sh', 'build.sh'],
		output: [
			'libgphoto2-6.def',
			'libgphoto2_port-12.def',
			'libgphoto2-6.dll',
			'libgphoto2_port-12.dll',
		],
		console: true,
		command: [
			msys_bash,
			'-c',
			(meson.current_source_dir())/'win_build.sh' + ' ' +
			(meson.current_source_dir()/'..') + ' ' +
			(meson.current_build_dir()/'..') + ' ' +
			'lakgphoto2',
		],
	)

	libgphoto2_def = libgphoto2_msys[0]
	libgphoto2_port_def = libgphoto2_msys[1]
	libgphoto2_dll = libgphoto2_msys[2]
	libgphoto2_port_dll = libgphoto2_msys[3]

	libgphoto2_dlls = [
		libgphoto2_dll,
		libgphoto2_port_dll,
		meson.current_build_dir()/'libiconv-2.dll',
		meson.current_build_dir()/'libintl-8.dll',
		meson.current_build_dir()/'libltdl-7.dll',
		meson.current_build_dir()/'libsystre-0.dll',
		meson.current_build_dir()/'libtre-5.dll',
	]

	libgphoto2_dll_lib = custom_target(
		'libgphoto2-dll-lib',
		input: libgphoto2_def,
		output: 'libgphoto2.lib',
		console: true,
		command: [
			find_program('lib'),
			'-nologo',
			'-def:@INPUT@',
			'-out:@OUTPUT@',
		],
	)

	libgphoto2_port_dll_lib = custom_target(
		'libgphoto2-port-dll-lib',
		input: libgphoto2_port_def,
		output: 'libgphoto2_port.lib',
		console: true,
		command: [
			find_program('lib'),
			'-nologo',
			'-def:@INPUT@',
			'-out:@OUTPUT@',
		],
	)

	liblakgphoto2 = static_library(
		'liblakgphoto2',
		[
			libgphoto2_dll_lib,
			libgphoto2_port_dll_lib,
		],
	)
else
	error('todo')
endif

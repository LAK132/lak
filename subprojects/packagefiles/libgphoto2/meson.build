project(
	'libgphoto2',
	'c',
	meson_version: '>=0.59',
)

includes = include_directories('.', 'libgphoto2_port')

cc = meson.get_compiler('c')

if target_machine.system() == 'windows'
	if build_machine.system() != 'windows'
		error('cross compiling libgphoto2 is not supported')
	endif

	msys_bash = find_program(get_option('lak_msys_prefix')/'usr'/'bin'/'bash')

	custom_target(
		'install-msys-libgphoto2-dependencies',
		output: 'nah',
		console: true,
		env: {
			'MSYSTEM': 'UCRT64',
		},
		command: [
			msys_bash,
			'--login',
			'-c',
			'pacman -S --needed --noconfirm ' +
			'grep ' +
			'mingw-w64-ucrt-x86_64-autotools ' +
			'mingw-w64-ucrt-x86_64-make ' +
			'mingw-w64-ucrt-x86_64-gcc ' +
			'mingw-w64-ucrt-x86_64-libusb ' +
			'mingw-w64-ucrt-x86_64-libltdl ' +
			'mingw-w64-ucrt-x86_64-pkgconf ' +
			'mingw-w64-ucrt-x86_64-tools-git ' +
			'mingw-w64-x86_64-autotools ' +
			'mingw-w64-x86_64-make ' +
			'mingw-w64-x86_64-gcc ' +
			'mingw-w64-x86_64-libusb ' +
			'mingw-w64-x86_64-libltdl ' +
			'mingw-w64-x86_64-pkgconf ',
		],
	)
endif

subdir('lib')

libgphoto2_dep = declare_dependency(
	link_with: liblakgphoto2,
	include_directories: includes,
)

meson.override_dependency('libgphoto2', libgphoto2_dep)

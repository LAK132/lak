laktestlibgphoto2 = static_library(
	'laktestlibgphoto2',
	[
		'libgphoto2.cpp',
	],
	override_options: [
		'cpp_std=' + version,
		'warning_level=0',
		'werror=false',
	],
	dependencies: [
		lakcore_dep,
		libgphoto2_dep,
	],
)

laktestlibgphoto2_dep = declare_dependency(
	link_whole: laktestlibgphoto2,
)

lakfile = static_library(
	'lakfile',
	[
		'meson'/'wrap.cpp',
		ebnf2cpp.process('meson'/'wrap.ebnf', extra_args: [
			'LAK_MESON_WRAP_EBNF_HPP', 'lak', 'wrap_parse']),
		'ebpf.cpp',
		ebnf2cpp.process('ebpf.ebnf', extra_args: [
			'LAK_EBPF_EBNF_HPP', 'lak', 'ebpf_parse']),
		'json.cpp',
		'mdc.cpp',
		'obj.cpp',
		ebnf2cpp.process('obj.ebnf', extra_args: [
			'LAK_OBJ_EBNF_HPP', 'lak', 'obj_parse']),
		'pnm.cpp',
		'tiff.cpp',
		'x3f.cpp',
	],
	dependencies: [
		lakcore_dep,
	],
	override_options: {
		'optimization': '1',
	},
)

lakfile_dep = declare_dependency(
	link_with: lakfile,
	dependencies: [
	],
)

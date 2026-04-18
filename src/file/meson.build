lakfile = static_library(
	'lakfile',
	[
		'json.cpp',
		'obj.cpp',
		ebnf2cpp.process('obj.ebnf', extra_args: [
			'LAK_OBJ_EBNF_HPP', 'lak', 'obj_parse']),
		'pnm.cpp',
	],
	dependencies: [
		lakcore_dep,
	],
)

lakfile_dep = declare_dependency(
	link_with: lakfile,
	dependencies: [
	],
)

lak_asc2csv = executable(
	'asc2csv',
	[
		'main.cpp',
		ebnf2cpp.process('asc2csv.ebnf', extra_args: [
			'LAK_ASC2CSV_EBNF_HPP', 'lak', 'asc2csv']),
	],
	dependencies: [
		lakcore_dep,
		lakfile_dep,
	],
	build_by_default: false,
	install_rpath: get_option('prefix')/get_option('bindir'),
)

laklisk = static_library(
	'laklisk',
	[
		'lisk.cpp',
		ebnf2cpp.process('lisk.ebnf', extra_args: [
			'LAK_LISK_EBNF_HPP', 'lak', 'lisk_parse']),
	],
	dependencies: [
		lakcore_dep,
	],
)

laklisk_dep = declare_dependency(
	link_with: laklisk,
	dependencies: [
	],
)

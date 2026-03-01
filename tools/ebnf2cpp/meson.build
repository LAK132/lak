lak_ebnf2cpp = executable(
	'ebnf2cpp',
	[
		'main.cpp',
	],
	override_options: override_options_w3,
	dependencies: lakcore_dep,
	build_by_default: false,
)
ebnf2cpp = generator(
	lak_ebnf2cpp,
	output: '@BASENAME@.hpp',
	arguments: ['@INPUT@', '@OUTPUT@', '@EXTRA_ARGS@'],
)

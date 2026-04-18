foreach type, comps : compilers
	_ebnf2cpp_name = 'ebnf2cpp'
	if type == 'build'
		_ebnf2cpp_name += '-bootstrap'
	endif
	_lak_ebnf2cpp = executable(
		_ebnf2cpp_name,
		[
			'main.cpp',
		],
		dependencies: lakcore_dep2[type],
		build_by_default: false,
		native: comps['native'],
	)
	if type == 'build'
		lak_ebnf2cpp = _lak_ebnf2cpp
	endif
endforeach
ebnf2cpp = generator(
	lak_ebnf2cpp,
	output: '@BASENAME@.ebnf.hpp',
	arguments: ['@INPUT@', '@OUTPUT@', '@EXTRA_ARGS@'],
)

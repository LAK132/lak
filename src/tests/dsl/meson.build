laktest_sources += files('ebnf2cpp.cpp')
laktest_dependencies += declare_dependency(
	include_directories: include_directories('.'),
	sources: custom_target(
		'ebnf2cpp_test.hpp',
		capture: true,
		input: 'ebnf2cpp_test.ebnf',
		output: 'ebnf2cpp_test.hpp',
		command: [lak_ebnf2cpp, '@INPUT@', 'LAK_TEST_EBNF_HPP', 'ebnf_test'],
	),
)

laktest_sources += files('dsl.cpp', 'ebnf.cpp', 'json.cpp', 'sexpr.cpp')

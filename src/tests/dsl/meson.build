laktest_sources += files('ebnf2cpp.cpp')
laktest_dependencies += declare_dependency(
	include_directories: include_directories('.'),
	sources: ebnf2cpp.process(
		'ebnf2cpp_test.ebnf', extra_args: ['LAK_TEST_EBNF_HPP', 'ebnf_test']),
)

laktest_sources += files('dsl.cpp', 'ebnf.cpp', 'json.cpp', 'sexpr.cpp')

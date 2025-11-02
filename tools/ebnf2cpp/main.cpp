#include <lak/array.hpp>
#include <lak/debug.hpp>
#include <lak/file.hpp>
#include <lak/span.hpp>
#include <lak/string_view.hpp>

#include <lak/dsl/ebnf.hpp>

int main(int argc, char **argv)
{
	if (argc < 3) return EXIT_FAILURE;

	lak::debugger.live_output_enabled = true;
	lak::debugger.live_errors_only    = true;

	lak::fs::path source      = argv[1];
	lak::astring header_guard = argv[2];
	lak::array<lak::astring> namespaces;
	lak::astring prefix;
	for (int i = 3; i < argc; ++i)
	{
		namespaces.push_back(argv[i]);
		prefix += argv[i];
		prefix += "::";
	}

	auto src = lak::read_file(source).UNWRAP();

	std::cerr << "input:\n"
	          << lak::u8string_view(lak::span<char8_t>(lak::span(src))) << "\n";

	auto grammar =
	  lak::dsl::ebnf
	    .parse(lak::u8string_view(lak::span<char8_t>(lak::span(src))))
	    .EXPECT("parse failed")
	    .value;

	std::cerr << "\noutput:\n";

	std::cout << "#ifndef " << header_guard << "\n";
	std::cout << "#define " << header_guard << "\n\n";
	std::cout << "#include <lak/dsl/dsl.hpp>\n";
	std::cout << "#include <lak/dsl/utility.hpp>\n\n";
	for (const auto &ns : namespaces) std::cout << "namespace " << ns << " { ";
	if (!namespaces.empty()) std::cout << "\n\n";

	for (const auto &rule : grammar.rules)
	{
		std::cout << "constexpr auto " << rule.name << " =";

		if (rule.transform)
		{
			std::cout << " lak::dsl::transform<";
		}

		std::cout << "\n";

		lak::array<lak::pair<lak::dsl::ebnf_rule_value, size_t>> stack;

		auto indent_str = [&]() { return lak::astring(stack.size(), '\t'); };

		stack.push_back({grammar.rule_values[rule.definition], 0});

		while (!stack.empty())
		{
			auto &[val, index] = stack.back();
			switch (val.type)
			{
				using enum lak::dsl::ebnf_rule_value::value_type;

				case string:
				{
					std::cout << indent_str() << "lak::dsl::str_literal<u8\"";
					auto [first, second] = lak::split_before<const char8_t>(
					  lak::span<const char8_t>(grammar.strings[val.index]), u8'"');
					std::cout << lak::string_view(first);
					while (!second.empty())
					{
						second = second.subspan(1U);
						std::cout << "\\\"";
						if (second.empty()) break;
						lak::tie(first, second) =
						  lak::split_before<const char8_t>(second, u8'"');
						std::cout << lak::string_view(first);
					}
					std::cout << "\">";
					stack.pop_back();
				}
				break;

				case rule:
					std::cout << indent_str() << prefix
					          << grammar.identifiers[val.index];
					stack.pop_back();
					break;

				case concatenation:
					if (index == 0U)
					{
						std::cout << indent_str();
						bool contains_captures = false;
						for (size_t i = 0U; !contains_captures &&
						                    i < grammar.concatenations[val.index].size();
						     ++i)
							if (grammar
							      .rule_values[grammar.concatenations[val.index].begin + i]
							      .type == capture)
								contains_captures = true;
						if (contains_captures)
							std::cout << "lak::dsl::capture_sequence<\n";
						else
							std::cout << "lak::dsl::sequence<\n";
					}
					if (index >= grammar.concatenations[val.index].size())
					{
						std::cout << ">";
						stack.pop_back();
					}
					else
					{
						if (index != 0U) std::cout << ",\n";
						stack.push_back(
						  {grammar.rule_values[grammar.concatenations[val.index].begin +
						                       (index++)],
						   0U});
					}
					break;

				case alternation:
					if (index == 0U)
						std::cout << indent_str() << "lak::dsl::disjunction<\n";
					if (index >= grammar.alternations[val.index].size())
					{
						std::cout << ">";
						stack.pop_back();
					}
					else
					{
						if (index != 0U) std::cout << ",\n";
						stack.push_back(
						  {grammar.rule_values[grammar.alternations[val.index].begin +
						                       (index++)],
						   0U});
					}
					break;

				case optional:
					if (index == 0U)
					{
						std::cout << indent_str() << "lak::dsl::optional<\n";
						++index;
						stack.push_back(
						  {grammar.rule_values[grammar.optionals[val.index].index], 0U});
					}
					else
					{
						std::cout << ">";
						stack.pop_back();
					}
					break;

				case repetition:
					if (index == 0U)
					{
						std::cout << indent_str();
						if (grammar.repetitions[val.index].count)
							std::cout << "lak::dsl::repeat_exact<\n";
						else
							std::cout << "lak::dsl::repeat<\n";
						++index;
						stack.push_back(
						  {grammar.rule_values[grammar.repetitions[val.index].index], 0U});
					}
					else
					{
						if (grammar.repetitions[val.index].count)
							std::cout << ", " << std::dec
							          << *grammar.repetitions[val.index].count << "U";
						std::cout << ">";
						stack.pop_back();
					}
					break;

				case match_case:
					if (index == 0U)
					{
						std::cout << indent_str() << "lak::dsl::match<\n";
						++index;
						stack.push_back(
						  {grammar.rule_values[grammar.match_cases[val.index].condition],
						   0U});
					}
					else if (index == 1U)
					{
						std::cout << ",\n";
						++index;
						stack.push_back(
						  {grammar.rule_values[grammar.match_cases[val.index].matched],
						   0U});
					}
					else
					{
						std::cout << ">";
						stack.pop_back();
					}
					break;

				case match_sequence:
					if (index == 0U)
						std::cout << indent_str() << "lak::dsl::match_sequence<\n";
					if (index >= grammar.match_sequences[val.index].size())
					{
						std::cout << ">";
						stack.pop_back();
					}
					else
					{
						if (index != 0U) std::cout << ",\n";
						stack.push_back(
						  {grammar.rule_values[grammar.match_sequences[val.index].begin +
						                       (index++)],
						   0U});
					}
					break;

				case grouping:
				{
					lak::dsl::ebnf_rule_value v =
					  grammar.rule_values[grammar.groupings[val.index].index];
					stack.pop_back();
					stack.push_back({v, 0U});
				}
				break;

				case capture:
					if (index == 0U)
					{
						std::cout << indent_str() << "lak::dsl::capture<\n";
						++index;
						stack.push_back(
						  {grammar.rule_values[grammar.captures[val.index].index], 0U});
					}
					else
					{
						std::cout << ">";
						stack.pop_back();
					}
					break;

				case special:
					std::cout << indent_str() << grammar.specials[val.index];
					stack.pop_back();
					break;

				case exception:
					if (index == 0U)
					{
						std::cout << indent_str() << "lak::dsl::conjunction<\n";
						++index;
						stack.push_back(
						  {grammar.rule_values[grammar.exceptions[val.index].rule], 0U});
					}
					else if (index == 1U)
					{
						std::cout << ", lak::dsl::negative_lookahead<\n";
						++index;
						stack.push_back(
						  {grammar.rule_values[grammar.exceptions[val.index].except_rule],
						   0U});
					}
					else
					{
						std::cout << ">>";
						stack.pop_back();
					}
					break;

				default:
					ASSERT_UNREACHABLE();
			}
		}

		if (rule.transform)
		{
			lak::u8string transform =
			  grammar.transforms[grammar.rule_values[*rule.transform].index];
			// strip \r
			transform.erase(lak::stable_partition(transform.begin(),
			                                      transform.end(),
			                                      [](char8_t c)
			                                      { return c != u8'\r'; }),
			                transform.end());
			std::cout << ",\n" << lak::u8string_view(transform) << ">";
		}

		std::cout << ";\n\n";
	}

	if (!namespaces.empty())
		std::cout << lak::astring(namespaces.size(), '}') << "\n\n";

	std::cout << "#endif\n";

	return EXIT_SUCCESS;
}

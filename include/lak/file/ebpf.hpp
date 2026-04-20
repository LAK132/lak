#ifndef LAK_FILE_EBPF_HPP
#define LAK_FILE_EBPF_HPP

#include "lak/ebpf/ebpf.hpp"

#include "lak/dsl/dsl.hpp"
#include "lak/dsl/utility.hpp"

namespace lak
{
	namespace ebpf
	{
		constexpr auto label_token_parser =
		  (lak::dsl::char_range<U'a', U'z'> | lak::dsl::char_range<U'A', U'Z'>)+(*(
		    lak::dsl::char_literal<U'_'> | lak::dsl::char_range<U'a', U'z'> |
		    lak::dsl::char_range<U'A', U'Z'> | lak::dsl::char_range<U'0', U'9'>));

		struct instruction_parser_t
		{
			static constexpr bool is_pure_match = false;

			using value_type =
			  lak::pair<lak::ebpf::instruction_t,
			            lak::optional<lak::variant<lak::ebpf::instruction_t,
			                                       lak::u8string_view>>>;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const;
		};

		inline constexpr instruction_parser_t instruction_parser;

		template<lak::dsl::concepts::pure_match_parser auto whitespace_parser>
		struct program_parser_t
		{
			static constexpr bool is_pure_match = false;

			using value_type = lak::array<lak::ebpf::instruction_t>;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			{
				lak::u8string_view rem = str;

				value_type result;

				struct _code_label
				{
					lak::u8string_view label;
				};

				lak::array<lak::pair<lak::u8string_view, size_t>> labels;
				lak::array<lak::pair<size_t, lak::u8string_view>> jumps_to_patch;

				auto move_str = [&]<typename T>(const lak::dsl::parse_result<T> &res)
				{ rem = res.remaining; };

				constexpr auto label_parser = lak::dsl::transform<
				  lak::dsl::
				    capture_nth<0U, label_token_parser, lak::dsl::char_literal<U':'>>,
				  [](lak::u8string_view label) { return _code_label{label}; }>;

				auto inst_parser =
				  lak::dsl::capture_nth<1U,
				                        *whitespace_parser,
				                        (label_parser | instruction_parser),
				                        *whitespace_parser>;

				while (!rem.empty())
				{
					RES_TRY_ASSIGN(auto inst =, inst_parser.parse(rem).if_ok(move_str));
					inst.value.visit(lak::overloaded{
					  [&](const _code_label &label)
					  { labels.push_back({label.label, result.size()}); },
					  [&](const lak::pair<
					      lak::ebpf::instruction_t,
					      lak::optional<lak::variant<lak::ebpf::instruction_t,
					                                 lak::u8string_view>>> &isn)
					  {
						  result.push_back(isn.first);
						  if (isn.second)
							  isn.second->visit(lak::overloaded{
							    [&](lak::ebpf::instruction_t i) { result.push_back(i); },
							    [&](lak::u8string_view label)
							    { jumps_to_patch.push_back({result.size() - 1U, label}); },
							  });
					  }});
				}

				for (const auto &[index, label] : jumps_to_patch)
				{
					auto inst = lak::ebpf::instruction::make(result[index]);

					lak::optional<int64_t> jump;
					for (const auto &[dst_label, dst_index] : labels)
					{
						if (dst_label == label)
						{
							jump = static_cast<int64_t>(dst_index) -
							       static_cast<int64_t>(index + 1U);
							break;
						}
					}
					if (!jump) return lak::err_t{u8"label not found"_str};

					if (inst.offset == UINT16_MAX)
						if (*jump > INT16_MAX || *jump < INT16_MIN)
							return lak::err_t{u8"jump out of range"_str};
						else
							inst.offset = static_cast<uint16_t>(static_cast<int16_t>(*jump));
					else if (inst.immediate == UINT32_MAX)
						if (*jump > INT32_MAX || *jump < INT32_MIN)
							return lak::err_t{u8"jump out of range"_str};
						else
							inst.immediate =
							  static_cast<uint32_t>(static_cast<int32_t>(*jump));
					else
						return lak::err_t{u8"bad jump patch"_str};

					result[index] = inst;
				}

				return lak::dsl::result<value_type>::make_ok({
				  .consumed  = str.substr(rem.begin() - str.begin()),
				  .remaining = rem,
				  .value     = lak::move(result),
				});
			}
		};

		template<lak::dsl::concepts::pure_match_parser auto whitespace_parser>
		constexpr program_parser_t<whitespace_parser> program_parser;
	}
}

#endif

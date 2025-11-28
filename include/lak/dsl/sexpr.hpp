#ifndef LAK_DSL_SEXPR_HPP
#define LAK_DSL_SEXPR_HPP

#include "lak/array.hpp"
#include "lak/dsl/dsl.hpp"
#include "lak/result.hpp"
#include "lak/string_view.hpp"

namespace lak
{
	namespace dsl
	{
		struct sexpr_cell
		{
			enum struct value_type
			{
				token,
				list,
			} type;
			size_t index;
		};

		struct sexpr_list
		{
			size_t begin;
			size_t end;

			size_t size() const { return end - begin; }
		};

		struct sexpr_block
		{
			lak::array<lak::u8string_view> tokens;
			lak::array<sexpr_cell> cells;
			lak::array<sexpr_list> lists;

			inline lak::span<const sexpr_cell> list_cells(const sexpr_list &list)
			{
				return lak::span(cells).subspan(list.begin, list.size());
			}

			inline lak::span<const sexpr_cell> list_cells(size_t list_index)
			{
				return list_cells(lists[list_index]);
			}

			lak::result<const sexpr_list &> as_list(const sexpr_cell &cell)
			{
				if (cell.type == sexpr_cell::value_type::list)
					return lak::ok_t<const sexpr_list &>{lists[cell.index]};
				else
					return lak::err_t{};
			}

			lak::result<lak::u8string_view> as_token(const sexpr_cell &cell)
			{
				if (cell.type == sexpr_cell::value_type::token)
					return lak::ok_t<lak::u8string_view>{tokens[cell.index]};
				else
					return lak::err_t{};
			}

			inline auto visit_cell(const sexpr_cell &cell, auto &&func)
			{
				if (cell.type == sexpr_cell::value_type::token)
					return func(tokens[cell.index]);
				else
					return func(lists[cell.index]);
			}

			inline auto visit_cell(size_t index, auto &&func)
			{
				return visit_cell(cells[index], func);
			}
		};

		template<lak::dsl::concepts::pure_match_parser auto token_parser,
		         lak::dsl::concepts::pure_match_parser auto whitespace_parser>
		struct sexpr_t
		{
			static constexpr bool is_pure_match = false;

			using value_type = sexpr_block;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			{
				lak::u8string_view rem = str;

				sexpr_block result;

				auto move_str =
				  [&](const lak::dsl::parse_result<lak::u8string_view> &res)
				{ rem = res.remaining; };

				constexpr auto lparen = lak::dsl::char_literal<U'('>;
				constexpr auto rparen = lak::dsl::char_literal<U')'>;

				struct working_data
				{
					size_t index;
					size_t size;
				};

				lak::array<sexpr_cell> working_cells;
				lak::array<working_data> working_tree;

				RES_TRY((*whitespace_parser).parse(rem).if_ok(move_str));

				RES_TRY(lparen.parse(rem).if_ok(move_str));

				working_tree.push_back({.index = result.lists.size(), .size = 0U});
				result.lists.emplace_back();

				while (!working_tree.empty())
				{
					RES_TRY((*whitespace_parser).parse(rem).if_ok(move_str));

					if (rem.empty())
						return lak::err_t{
						  lak::dsl::err::parse{.message = u8"out of data"}};

					if (lparen.parse(rem).if_ok(move_str).is_ok())
					{
						working_tree.push_back({.index = result.lists.size(), .size = 0U});
						result.lists.emplace_back();
					}
					else if (rparen.parse(rem).if_ok(move_str).is_ok())
					{
						// get the cells associated with the popped list
						auto list_cells =
						  lak::span(working_cells).last(working_tree.back().size);

						size_t begin = result.cells.size();

						// commit the popped list's cells to the result
						result.cells.reserve(result.cells.size() + list_cells.size());
						for (auto &cell : list_cells) result.cells.push_back(cell);
						working_cells.resize(working_cells.size() - list_cells.size());

						// set the bounds of the list based on the newly moved cells
						result.lists[working_tree.back().index] = {
						  .begin = begin,
						  .end   = result.cells.size(),
						};

						// add the popped list to the working cells
						working_cells.push_back({
						  .type  = sexpr_cell::value_type::list,
						  .index = working_tree.back().index,
						});

						working_tree.pop_back();
						if (!working_tree.empty()) ++working_tree.back().size;
					}
					else
					{
						// add a token to the working cells
						RES_TRY_ASSIGN(lak::dsl::parse_result<lak::u8string_view> tok =,
						               token_parser.parse(rem).if_ok(move_str));
						working_cells.push_back({
						  .type  = sexpr_cell::value_type::token,
						  .index = result.tokens.size(),
						});
						++working_tree.back().size;
						result.tokens.push_back(tok.value);
					}
				}

				return lak::ok_t<lak::dsl::parse_result<value_type>>{{
				  .consumed  = str.substr(str.size() - rem.size()),
				  .remaining = rem,
				  .value     = lak::move(result),
				}};
			}
		};

		template<lak::dsl::concepts::pure_match_parser auto token_parser,
		         lak::dsl::concepts::pure_match_parser auto whitespace_parser>
		inline constexpr lak::dsl::sexpr_t<token_parser, whitespace_parser> sexpr;

		static_assert(lak::dsl::concepts::parser<
		              lak::dsl::sexpr_t<lak::dsl::bottom, lak::dsl::bottom>>);
	}
}

#endif

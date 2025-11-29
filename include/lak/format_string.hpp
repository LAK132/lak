#ifndef LAK_FORMAT_STRING_HPP
#define LAK_FORMAT_STRING_HPP

#include "lak/c_array.hpp"
#include "lak/char.hpp"
#include "lak/concepts.hpp"
#include "lak/const_string.hpp"
#include "lak/tuple.hpp"

namespace lak
{
	template<lak::const_string const_str>
	consteval size_t format_specifier_offset(size_t specifier_index)
	{
		using CHAR        = typename decltype(const_str)::char_type;
		size_t index      = 0U;
		bool in_specifier = false;
		for (size_t i = 0U; i < const_str.size(); ++i)
		{
			if (const_str[i] == CHAR('{'))
			{
				if (i + 1U < const_str.size() && const_str[i + 1U] == CHAR('{'))
				{
					++i;
					continue;
				}
				if (in_specifier) throw "unexpected '{'";
				if (index == specifier_index) return i + 1U;
				in_specifier = true;
			}
			else if (const_str[i] == CHAR('}'))
			{
				if (i + 1U < const_str.size() && const_str[i + 1U] == CHAR('}'))
				{
					++i;
					continue;
				}
				if (!in_specifier) throw "unexpected '}'";
				in_specifier = false;
				++index;
			}
		}
		if (in_specifier) throw "missing '}'";
		if (index == specifier_index)
			return const_str.size();
		else
			throw "invalid specifier index";
	}

	template<lak::const_string const_str>
	consteval size_t format_specifier_size(size_t specifier_index)
	{
		using CHAR = typename decltype(const_str)::char_type;
		const size_t begin =
		  lak::format_specifier_offset<const_str>(specifier_index);
		for (size_t i = begin; i < const_str.size(); ++i)
			if (const_str[i] == CHAR('}')) return i - begin;
		throw "invalid specifier index";
	}

	template<lak::const_string const_str, size_t specifier_index>
	consteval lak::pair<size_t, lak::pair<size_t, size_t>> format_specifier()
	{
		using CHAR   = typename decltype(const_str)::char_type;
		size_t begin = lak::format_specifier_offset<const_str>(specifier_index);
		size_t size  = lak::format_specifier_size<const_str>(specifier_index);
		size_t end   = begin + size;
		size_t index = 0U;
		for (size_t i = begin; i < end; ++i)
		{
			if (const_str[i] == CHAR(':'))
			{
				return lak::pair(i == begin ? lak::dynamic_extent : index,
				                 lak::pair(i + 1U, end - (i + 1U)));
			}
			if (const_str[i] < CHAR('0') || const_str[i] > CHAR('9'))
				throw "expected decimal digit";
			index *= 10;
			index += const_str[i] - CHAR('0');
		}
		return lak::pair(size == 0U ? lak::dynamic_extent : index,
		                 lak::pair(end, size_t(0U)));
	}

	template<lak::const_string const_str>
	consteval size_t format_specifier_prefix_offset(size_t specifier_index)
	{
		using CHAR        = typename decltype(const_str)::char_type;
		size_t index      = 0U;
		bool in_specifier = false;
		for (size_t i = 0U; i < const_str.size(); ++i)
		{
			if (index == specifier_index) return i;
			if (const_str[i] == CHAR('{'))
			{
				if (i + 1U < const_str.size() && const_str[i + 1U] == CHAR('{'))
				{
					++i;
					continue;
				}
				if (in_specifier) throw "unexpected '{'";
				in_specifier = true;
			}
			else if (const_str[i] == CHAR('}'))
			{
				if (i + 1U < const_str.size() && const_str[i + 1U] == CHAR('}'))
				{
					++i;
					continue;
				}
				if (!in_specifier) throw "unexpected '}'";
				in_specifier = false;
				++index;
			}
		}
		if (in_specifier) throw "missing '}'";
		if (index == specifier_index)
			return const_str.size();
		else
			throw "invalid specifier index";
	}

	template<lak::const_string const_str>
	consteval size_t format_specifier_prefix_size(size_t specifier_index)
	{
		using CHAR   = typename decltype(const_str)::char_type;
		size_t index = 0U;
		size_t begin =
		  lak::format_specifier_prefix_offset<const_str>(specifier_index);
		for (size_t i = begin; i < const_str.size(); ++i)
		{
			if (const_str[i] == CHAR('{'))
			{
				if (i + 1U < const_str.size() && const_str[i + 1U] == CHAR('{'))
				{
					++i;
					continue;
				}
				return i - begin;
			}
			if (const_str[i] == CHAR('}'))
			{
				if (i + 1U < const_str.size() && const_str[i + 1U] == CHAR('}'))
				{
					++i;
					continue;
				}
				throw "unexpected '}";
			}
		}
		return const_str.size() - begin;
	}

	template<lak::const_string const_str>
	consteval lak::pair<size_t, size_t> format_specifier_prefix(
	  size_t specifier_index)
	{
		return lak::pair(
		  lak::format_specifier_prefix_offset<const_str>(specifier_index),
		  lak::format_specifier_prefix_size<const_str>(specifier_index));
	}

	template<lak::const_string const_str>
	consteval size_t format_specifier_count()
	{
		using CHAR        = typename decltype(const_str)::char_type;
		size_t count      = 0U;
		bool in_specifier = false;
		for (size_t i = 0U; i < const_str.size(); ++i)
		{
			if (const_str[i] == CHAR('{'))
			{
				if (i + 1U < const_str.size() && const_str[i + 1U] == CHAR('{'))
				{
					++i;
					continue;
				}
				if (in_specifier) throw "unexpected '{'";
				in_specifier = true;
			}
			else if (const_str[i] == CHAR('}'))
			{
				if (i + 1U < const_str.size() && const_str[i + 1U] == CHAR('}'))
				{
					++i;
					continue;
				}
				if (!in_specifier) throw "unexpected '}'";
				in_specifier = false;
				++count;
			}
		}
		if (in_specifier) throw "missing '}'";
		return count;
	}

	template<lak::const_string const_str, size_t begin, size_t count>
	consteval size_t format_specifier_deduped_size()
	{
		using CHAR    = typename decltype(const_str)::char_type;
		size_t result = 0U;
		size_t end    = begin + count;
		for (size_t i = begin; i < begin + count; ++i)
		{
			if (const_str[i] == CHAR('{') && i + 1U < end &&
			    i + 1U < const_str.size() && const_str[i + 1U] == CHAR('{'))
			{
				++i;
				++result;
			}
			else if (const_str[i] == CHAR('}') && i + 1U < end &&
			         i + 1U < const_str.size() && const_str[i + 1U] == CHAR('}'))
			{
				++i;
				++result;
			}
		}
		return count - result;
	}

	template<lak::const_string const_str, size_t begin, size_t count>
	requires(lak::format_specifier_deduped_size<const_str, begin, count>() != 0U)
	consteval auto format_specifier_dedup()
	{
		using CHAR = typename decltype(const_str)::char_type;
		lak::c_array<CHAR,
		             lak::format_specifier_deduped_size<const_str, begin, count>()>
		  result;
		size_t offset = begin;
		size_t end    = begin + count;
		for (size_t i = 0; i < result.size(); ++i)
		{
			if (const_str[i + offset] == CHAR('{') && (i + offset) + 1U < end &&
			    (i + offset) + 1U < const_str.size() &&
			    const_str[(i + offset) + 1U] == CHAR('{'))
			{
				++offset;
			}
			else if (const_str[i + offset] == CHAR('}') && (i + offset) + 1U < end &&
			         (i + offset) + 1U < const_str.size() &&
			         const_str[(i + offset) + 1U] == CHAR('}'))
			{
				++offset;
			}
			result[i] = const_str[i + offset];
		}
		return result;
	}

	template<lak::const_string const_str, size_t begin, size_t count>
	requires(lak::format_specifier_deduped_size<const_str, begin, count>() == 0U)
	consteval auto format_specifier_dedup()
	{
		return lak::c_array<typename decltype(const_str)::char_type, 0U>{};
	}

	template<lak::const_string const_str, size_t index>
	consteval auto format_specifier_deduped()
	{
		constexpr auto specifier = lak::format_specifier<const_str, index>();
		return lak::pair(specifier.first,
		                 lak::format_specifier_dedup<const_str,
		                                             specifier.second.first,
		                                             specifier.second.second>());
	}

	template<lak::const_string const_str, size_t... I>
	consteval auto format_specifiers_deduped(lak::index_sequence<I...>)
	{
		using CHAR = typename decltype(const_str)::char_type;

		lak::c_array<
		  CHAR,
		  ((lak::format_specifier_deduped<const_str, I>().second.size()) + ... +
		   0U)>
		  buffer;

		lak::c_array<lak::pair<size_t, lak::pair<size_t, size_t>>, sizeof...(I)>
		  offsets;

		// [[maybe_unused]] to suppress warning on gcc. even though we're clearly
		// using them
		[[maybe_unused]] size_t offset  = 0U;
		[[maybe_unused]] size_t dyn_idx = 0U;
		((
		   [&]<size_t J>(lak::size_type<J>)
		   {
			   static_assert(J < sizeof...(I));
			   constexpr auto deduped =
			     lak::format_specifier_deduped<const_str, J>();
			   offsets[J].first =
			     deduped.first == lak::dynamic_extent ? dyn_idx++ : deduped.first;
			   offsets[J].second.first  = offset;
			   offsets[J].second.second = deduped.second.size();
			   for (size_t i = 0; i < deduped.second.size(); ++i)
				   buffer[i + offset] = deduped.second[i];
			   offset += deduped.second.size();
		   }(lak::size_type<I>{})),
		 ...);

		return lak::pair(buffer, offsets);
	}

	template<lak::const_string const_str, size_t index>
	consteval auto format_specifier_prefix_deduped()
	{
		constexpr auto spec = lak::format_specifier_prefix<const_str>(index);
		return lak::format_specifier_dedup<const_str, spec.first, spec.second>();
	}

	template<lak::const_string const_str, size_t... I>
	consteval auto format_specifier_prefixes_deduped(lak::index_sequence<I...>)
	{
		using CHAR = typename decltype(const_str)::char_type;
		lak::c_array<lak::pair<size_t, size_t>, sizeof...(I)> offsets;

		lak::c_array<
		  CHAR,
		  ((lak::format_specifier_prefix_deduped<const_str, I>().size()) + ... +
		   0U)>
		  buffer;

		size_t offset = 0U;
		((
		   [&]<size_t J>(lak::size_type<J>)
		   {
			   constexpr auto deduped =
			     lak::format_specifier_prefix_deduped<const_str, J>();
			   offsets[J].first  = offset;
			   offsets[J].second = deduped.size();
			   for (size_t i = 0; i < deduped.size(); ++i)
				   buffer[i + offset] = deduped[i];
			   offset += deduped.size();
		   }(lak::size_type<I>{})),
		 ...);

		return lak::pair(buffer, offsets);
	}

	template<lak::const_string const_str>
	struct format_string
	{
		using char_type = typename decltype(const_str)::char_type;

		static constexpr size_t specifier_count =
		  lak::format_specifier_count<const_str>();

		static constexpr auto _specifiers_buffer =
		  lak::format_specifiers_deduped<const_str>(
		    lak::make_index_sequence<specifier_count>{})
		    .first;
		static_assert(_specifiers_buffer.size() <= const_str.size());

		static constexpr auto specifiers =
		  lak::format_specifiers_deduped<const_str>(
		    lak::make_index_sequence<specifier_count>{})
		    .second;
		static_assert(specifiers.size() == specifier_count);

		static constexpr auto _prefixes_buffer =
		  lak::format_specifier_prefixes_deduped<const_str>(
		    lak::make_index_sequence<specifier_count + 1U>{})
		    .first;
		static_assert(_prefixes_buffer.size() <= const_str.size());

		static constexpr auto prefixes =
		  lak::format_specifier_prefixes_deduped<const_str>(
		    lak::make_index_sequence<specifier_count + 1U>{})
		    .second;
		static_assert(prefixes.size() == specifier_count + 1U);

		static consteval lak::string_view<char_type> specifiers_buffer()
		{
			if (_specifiers_buffer.size() == 0U) return {};
			return lak::string_view<char_type>(_specifiers_buffer.begin(),
			                                   _specifiers_buffer.size());
		}

		static consteval lak::string_view<char_type> prefixes_buffer()
		{
			if (_prefixes_buffer.size() == 0U) return {};
			return lak::string_view<char_type>(_prefixes_buffer.begin(),
			                                   _prefixes_buffer.size());
		}

		static constexpr lak::pair<size_t, lak::string_view<char_type>> specifier(
		  size_t index)
		{
			if (!std::is_constant_evaluated())
			{
				ASSERT_LESS(index, specifiers.size());
			}
			else if (index >= specifiers.size())
				throw "index out of range";
			const auto spec = specifiers[index];
			return lak::pair(
			  spec.first,
			  specifiers_buffer().substr(spec.second.first, spec.second.second));
		}

		static constexpr lak::string_view<char_type> prefix(size_t index)
		{
			if (!std::is_constant_evaluated())
			{
				ASSERT_LESS(index, prefixes.size());
			}
			else if (index >= prefixes.size())
				throw "index out of range";
			const auto spec = prefixes[index];
			return prefixes_buffer().substr(spec.first, spec.second);
		}
	};

}

#endif

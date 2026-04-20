#include <lak/array.hpp>
#include <lak/dsl/reader.hpp>
#include <lak/generator.hpp>
#include <lak/string_literals/string.hpp>
#include <lak/system/file.hpp>

#include "asc2csv.ebnf.hpp"

#include <iostream>

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: asc2csv <input> <output>\n";
		return EXIT_FAILURE;
	}

	lak::debugger.live_output_enabled = true;
	lak::debugger.live_errors_only    = true;

	lak::fs::path source = argv[1];
	lak::fs::path dest   = argv[2];

	std::cerr << "Loading " << source << "\n";

	auto file = lak::ro_mmap_file(source).UNWRAP();

	auto str = lak::span<const char8_t>(lak::span(*file));

	std::cerr << "Converting to CSV\n";

	auto lines = lak::make_generator(
	  [&]() -> lak::optional<lak::u8string_view>
	  {
		  lak::span<const char8_t> result;
		  for (size_t i = 0U; i < str.size(); ++i)
		  {
			  if (str[i] != '\n') continue;
			  lak::tie(result, str) = lak::split(str, i + 1U);
			  return lak::u8string_view(result);
		  }
		  if (str.empty()) return lak::nullopt;
		  lak::swap(result, str);
		  return lak::u8string_view(result);
	  });

	lak::u8string result = u8"PX,PY,PZ,NX,NY,NZ\n"_str;
	result.reserve(result.size() + str.size());

	result = lak::transform_reduce(
	  lak::execution::par,
	  lines.first,
	  lines.second,
	  lak::move(result),
	  [](lak::u8string &&buff, lak::u8string str)
	  {
		  lak::u8string result = lak::exchange(buff, {});
		  result += str;
		  return result;
	  },
	  [](lak::u8string_view str)
	  {
		  auto res = lak::asc2csv::line.parse(str).unwrap().value;
		  return lak::fmt<u8"{},{},{},{},{},{}\n">(res[0U][0U],
		                                           res[0U][1U],
		                                           res[0U][2U],
		                                           res[1U][0U],
		                                           res[1U][1U],
		                                           res[1U][2U]);
	  });

	std::cerr << "Saving to " << dest << "\n";

	if (!lak::save_file(dest, result))
	{
		std::cerr << "Failed to save file " << dest << "\n";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

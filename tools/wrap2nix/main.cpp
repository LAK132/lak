#include <lak/array.hpp>
#include <lak/dsl/reader.hpp>
#include <lak/file/meson/wrap.hpp>
#include <lak/format.hpp>
#include <lak/main.inl>
#include <lak/string_literals/view.hpp>
#include <lak/string_utils.hpp>
#include <lak/system/file.hpp>

lak::error_code<int> lak_main(lak::span<char *> argv)
{
	argv = argv.subspan(1U);

	if (argv.size() < 1U || argv.size() > 2U)
	{
		std::cerr << "Usage: wrap2nix <input> [<output>]\n";
		return lak::err_t{EXIT_FAILURE};
	}

	lak::debugger.live_output_enabled = true;
	lak::debugger.live_errors_only    = true;

	lak::fs::path src = argv[0U];
	lak::fs::path dst;
	if (argv.size() < 2U)
	{
		dst = src;
		dst.replace_extension("nix");
	}
	else
		dst = argv[1U];

	std::cerr << "Loading " << src << "\n";

	auto file = lak::ro_mmap_file(src).UNWRAP();

	auto str = lak::u8string_view(lak::span<const char8_t>(lak::span(*file)));

	auto wrap = lak::dsl::meson_wrap.parse(str).UNWRAP().value;

	std::cerr << "Converting to nix\n";

	lak::u8string out = u8"{\n";

	for (auto &[section, entries] : wrap)
	{
		out += lak::fmt<u8"\t\"{}\" = {{\n">(section);
		for (auto &[key, values] : entries)
		{
			if (values.empty()) continue;

			for (auto &value : values) value = lak::fmt<u8"\"{}\"">(value);

			if (values.size() == 1U)
			{
				out += lak::fmt<u8"\t\t\"{}\" = {};\n">(key, values[0U]);
			}
			else
			{
				out += lak::fmt<u8"\t\t\"{}\" = [\n\t\t\t{}\n\t\t];\n">(
				  key,
				  lak::join_strings<char8_t>(u8"\n\t\t\t"_view, lak::span(values)));
			}
		}
		out += u8"\t};\n";
	}

	out += u8"}\n";

	std::cerr << "Saving to " << dst << "\n";

	if (!lak::save_file(dst, out))
	{
		std::cerr << "Failed to save file " << dst << "\n";
		return lak::err_t{EXIT_FAILURE};
	}

	return lak::ok_t{};
}

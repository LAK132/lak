#ifndef LAK_FILE_WRAP_HPP
#define LAK_FILE_WRAP_HPP

#include "lak/array.hpp"
#include "lak/dsl/concepts.hpp"
#include "lak/dsl/dsl.hpp"
#include "lak/string.hpp"

namespace lak
{
	namespace wrap
	{
		/*
		[wrap-file]
		directory =
		patch_url =
		patch_fallback_url =
		patch_filename =
		patch_hash =
		patch_directory =
		diff_files =
		method = [meson, cmake, cargo]
		source_url =
		source_fallback_url =
		source_filename =
		source_hash =
		lead_directory_missing =

		[wrap-redirect]
		filename =

		[wrap-git]
		directory =
		patch_url =
		patch_fallback_url =
		patch_filename =
		patch_hash =
		patch_directory =
		diff_files =
		method = [meson, cmake, cargo]
		url =
		revision =
		depth =
		push-url =
		clone-recursive =

		[wrap-svn]
		directory =
		patch_url =
		patch_fallback_url =
		patch_filename =
		patch_hash =
		patch_directory =
		diff_files =
		method = [meson, cmake, cargo]
		url =
		revision =

		[wrap-hg]
		directory =
		patch_url =
		patch_fallback_url =
		patch_filename =
		patch_hash =
		patch_directory =
		diff_files =
		method = [meson, cmake, cargo]
		url =
		revision =

		[provide]
		...
		*/
		struct section_entry
		{
			lak::u8string key;
			lak::array<lak::u8string> values;
		};
		struct section
		{
			lak::u8string name;
			lak::array<lak::wrap::section_entry> entries;
		};
	}

	namespace dsl
	{
		struct wrap_t
		{
			static constexpr auto is_pure_match = false;

			using value_type = lak::array<lak::wrap::section>;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const;
		};

		constexpr wrap_t wrap;

		static_assert(lak::dsl::concepts::parser<lak::dsl::wrap_t>);
	}
}

#endif

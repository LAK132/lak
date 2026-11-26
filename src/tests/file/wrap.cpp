#include "lak/test.hpp"

#include "lak/file/meson/wrap.hpp"

#include "lak/string_literals/string.hpp"
#include "lak/string_literals/view.hpp"

BEGIN_TEST(file_wrap)
{
	{
		auto file = u8""_view;
		auto wrap = lak::dsl::meson_wrap.parse(file).UNWRAP().value;

		ASSERT_EQUAL(wrap.size(), 0U);
	}

	{
		auto file = u8"[wrap-file]"_view;
		auto wrap = lak::dsl::meson_wrap.parse(file).UNWRAP().value;

		ASSERT_EQUAL(wrap.size(), 1U);
		ASSERT_EQUAL(wrap[0U].name, u8"wrap-file"_str);
		ASSERT_EQUAL(wrap[0U].entries.size(), 0U);
	}
	{
		auto file = u8"[wrap-redirect]"_view;
		auto wrap = lak::dsl::meson_wrap.parse(file).UNWRAP().value;

		ASSERT_EQUAL(wrap.size(), 1U);
		ASSERT_EQUAL(wrap[0U].name, u8"wrap-redirect"_str);
		ASSERT_EQUAL(wrap[0U].entries.size(), 0U);
	}
	{
		auto file = u8"[wrap-git]"_view;
		auto wrap = lak::dsl::meson_wrap.parse(file).UNWRAP().value;

		ASSERT_EQUAL(wrap.size(), 1U);
		ASSERT_EQUAL(wrap[0U].name, u8"wrap-git"_str);
		ASSERT_EQUAL(wrap[0U].entries.size(), 0U);
	}
	{
		auto file = u8"[wrap-svn]"_view;
		auto wrap = lak::dsl::meson_wrap.parse(file).UNWRAP().value;

		ASSERT_EQUAL(wrap.size(), 1U);
		ASSERT_EQUAL(wrap[0U].name, u8"wrap-svn"_str);
		ASSERT_EQUAL(wrap[0U].entries.size(), 0U);
	}
	{
		auto file = u8"[wrap-hg]"_view;
		auto wrap = lak::dsl::meson_wrap.parse(file).UNWRAP().value;

		ASSERT_EQUAL(wrap.size(), 1U);
		ASSERT_EQUAL(wrap[0U].name, u8"wrap-hg"_str);
		ASSERT_EQUAL(wrap[0U].entries.size(), 0U);
	}

	{
		auto file = u8"[provide]"_view;
		auto wrap = lak::dsl::meson_wrap.parse(file).UNWRAP().value;

		ASSERT_EQUAL(wrap.size(), 1U);
		ASSERT_EQUAL(wrap[0U].name, u8"provide"_str);
		ASSERT_EQUAL(wrap[0U].entries.size(), 0U);
	}

	{
		auto file = u8"[wrap-file]\n[provide]"_view;
		auto wrap = lak::dsl::meson_wrap.parse(file).UNWRAP().value;

		ASSERT_EQUAL(wrap.size(), 2U);
		ASSERT_EQUAL(wrap[0U].name, u8"wrap-file"_str);
		ASSERT_EQUAL(wrap[0U].entries.size(), 0U);
		ASSERT_EQUAL(wrap[1U].name, u8"provide"_str);
		ASSERT_EQUAL(wrap[1U].entries.size(), 0U);
	}
	{
		auto file = u8"[wrap-git]\n[provide]"_view;
		auto wrap = lak::dsl::meson_wrap.parse(file).UNWRAP().value;

		ASSERT_EQUAL(wrap.size(), 2U);
		ASSERT_EQUAL(wrap[0U].name, u8"wrap-git"_str);
		ASSERT_EQUAL(wrap[0U].entries.size(), 0U);
		ASSERT_EQUAL(wrap[1U].name, u8"provide"_str);
		ASSERT_EQUAL(wrap[1U].entries.size(), 0U);
	}
	{
		auto file = u8"[wrap-svn]\n[provide]"_view;
		auto wrap = lak::dsl::meson_wrap.parse(file).UNWRAP().value;

		ASSERT_EQUAL(wrap.size(), 2U);
		ASSERT_EQUAL(wrap[0U].name, u8"wrap-svn"_str);
		ASSERT_EQUAL(wrap[0U].entries.size(), 0U);
		ASSERT_EQUAL(wrap[1U].name, u8"provide"_str);
		ASSERT_EQUAL(wrap[1U].entries.size(), 0U);
	}
	{
		auto file = u8"[wrap-hg]\n[provide]"_view;
		auto wrap = lak::dsl::meson_wrap.parse(file).UNWRAP().value;

		ASSERT_EQUAL(wrap.size(), 2U);
		ASSERT_EQUAL(wrap[0U].name, u8"wrap-hg"_str);
		ASSERT_EQUAL(wrap[0U].entries.size(), 0U);
		ASSERT_EQUAL(wrap[1U].name, u8"provide"_str);
		ASSERT_EQUAL(wrap[1U].entries.size(), 0U);
	}

	{
		auto file =
		  u8R"([wrap-redirect]
		  filename = mysubproj/subprojects/othersubproj.wrap)"_view;
		auto wrap = lak::dsl::meson_wrap.parse(file).UNWRAP().value;

		ASSERT_EQUAL(wrap.size(), 1U);
		ASSERT_EQUAL(wrap[0U].name, u8"wrap-redirect"_str);
		ASSERT_EQUAL(wrap[0U].entries.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[0U].key, u8"filename"_str);
		ASSERT_EQUAL(wrap[0U].entries[0U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[0U].values[0U],
		             u8"mysubproj/subprojects/othersubproj.wrap"_str);
	}

	{
		auto file =
		  u8R"([wrap-file]
		  directory = /hello/world/
		  patch_url = https://example.com
		  patch_fallback_url = http://example.com/example/
		  patch_hash = asoidfcujaoswfdu9df8uwa9e8fuw9e8f
		  patch_directory = C:\waow\meow\
		  diff_files = file1,file2, file3.diff , file4.asdf
		  method = meson
		  source_url = https://example.com/example.tar.gz
		  source_fallback_url = https://example.com/example.zip
		  source_filename = asdasda
		  source_hash = 123123
		  lead_directory_missing = true)"_view;
		auto wrap = lak::dsl::meson_wrap.parse(file).UNWRAP().value;

		ASSERT_EQUAL(wrap.size(), 1U);
		ASSERT_EQUAL(wrap[0U].name, u8"wrap-file"_str);
		ASSERT_EQUAL(wrap[0U].entries.size(), 12U);
		ASSERT_EQUAL(wrap[0U].entries[0U].key, u8"directory"_str);
		ASSERT_EQUAL(wrap[0U].entries[0U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[0U].values[0U], u8"/hello/world/"_str);
		ASSERT_EQUAL(wrap[0U].entries[1U].key, u8"patch_url"_str);
		ASSERT_EQUAL(wrap[0U].entries[1U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[1U].values[0U], u8"https://example.com"_str);
		ASSERT_EQUAL(wrap[0U].entries[2U].key, u8"patch_fallback_url"_str);
		ASSERT_EQUAL(wrap[0U].entries[2U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[2U].values[0U],
		             u8"http://example.com/example/"_str);
		ASSERT_EQUAL(wrap[0U].entries[3U].key, u8"patch_hash"_str);
		ASSERT_EQUAL(wrap[0U].entries[3U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[3U].values[0U],
		             u8"asoidfcujaoswfdu9df8uwa9e8fuw9e8f"_str);
		ASSERT_EQUAL(wrap[0U].entries[4U].key, u8"patch_directory"_str);
		ASSERT_EQUAL(wrap[0U].entries[4U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[4U].values[0U], u8"C:\\waow\\meow\\"_str);
		ASSERT_EQUAL(wrap[0U].entries[5U].key, u8"diff_files"_str);
		ASSERT_EQUAL(wrap[0U].entries[5U].values.size(), 4U);
		ASSERT_EQUAL(wrap[0U].entries[5U].values[0U], u8"file1"_str);
		ASSERT_EQUAL(wrap[0U].entries[5U].values[1U], u8"file2"_str);
		ASSERT_EQUAL(wrap[0U].entries[5U].values[2U], u8"file3.diff"_str);
		ASSERT_EQUAL(wrap[0U].entries[5U].values[3U], u8"file4.asdf"_str);
		ASSERT_EQUAL(wrap[0U].entries[6U].key, u8"method"_str);
		ASSERT_EQUAL(wrap[0U].entries[6U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[6U].values[0U], u8"meson"_str);
		ASSERT_EQUAL(wrap[0U].entries[7U].key, u8"source_url"_str);
		ASSERT_EQUAL(wrap[0U].entries[7U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[7U].values[0U],
		             u8"https://example.com/example.tar.gz"_str);
		ASSERT_EQUAL(wrap[0U].entries[8U].key, u8"source_fallback_url"_str);
		ASSERT_EQUAL(wrap[0U].entries[8U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[8U].values[0U],
		             u8"https://example.com/example.zip"_str);
		ASSERT_EQUAL(wrap[0U].entries[9U].key, u8"source_filename"_str);
		ASSERT_EQUAL(wrap[0U].entries[9U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[9U].values[0U], u8"asdasda"_str);
		ASSERT_EQUAL(wrap[0U].entries[10U].key, u8"source_hash"_str);
		ASSERT_EQUAL(wrap[0U].entries[10U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[10U].values[0U], u8"123123"_str);
		ASSERT_EQUAL(wrap[0U].entries[11U].key, u8"lead_directory_missing"_str);
		ASSERT_EQUAL(wrap[0U].entries[11U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[11U].values[0U], u8"true"_str);
	}

	{
		auto file =
		  u8R"([wrap-git]
		  directory = /hello/world/
		  patch_url = https://example.com
		  patch_fallback_url = http://example.com/example/
		  patch_hash = asoidfcujaoswfdu9df8uwa9e8fuw9e8f
		  patch_directory = C:\waow\meow\
		  diff_files = file1,file2, file3.diff , file4.asdf
		  method = meson
		  url = https://example.com/example/repo
		  revision = v1.3.2
		  depth = 2
		  push-url = git:example/repo
		  clone-recursive = true)"_view;
		auto wrap = lak::dsl::meson_wrap.parse(file).UNWRAP().value;

		ASSERT_EQUAL(wrap.size(), 1U);
		ASSERT_EQUAL(wrap[0U].name, u8"wrap-git"_str);
		ASSERT_EQUAL(wrap[0U].entries.size(), 12U);
		ASSERT_EQUAL(wrap[0U].entries[0U].key, u8"directory"_str);
		ASSERT_EQUAL(wrap[0U].entries[0U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[0U].values[0U], u8"/hello/world/"_str);
		ASSERT_EQUAL(wrap[0U].entries[1U].key, u8"patch_url"_str);
		ASSERT_EQUAL(wrap[0U].entries[1U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[1U].values[0U], u8"https://example.com"_str);
		ASSERT_EQUAL(wrap[0U].entries[2U].key, u8"patch_fallback_url"_str);
		ASSERT_EQUAL(wrap[0U].entries[2U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[2U].values[0U],
		             u8"http://example.com/example/"_str);
		ASSERT_EQUAL(wrap[0U].entries[3U].key, u8"patch_hash"_str);
		ASSERT_EQUAL(wrap[0U].entries[3U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[3U].values[0U],
		             u8"asoidfcujaoswfdu9df8uwa9e8fuw9e8f"_str);
		ASSERT_EQUAL(wrap[0U].entries[4U].key, u8"patch_directory"_str);
		ASSERT_EQUAL(wrap[0U].entries[4U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[4U].values[0U], u8"C:\\waow\\meow\\"_str);
		ASSERT_EQUAL(wrap[0U].entries[5U].key, u8"diff_files"_str);
		ASSERT_EQUAL(wrap[0U].entries[5U].values.size(), 4U);
		ASSERT_EQUAL(wrap[0U].entries[5U].values[0U], u8"file1"_str);
		ASSERT_EQUAL(wrap[0U].entries[5U].values[1U], u8"file2"_str);
		ASSERT_EQUAL(wrap[0U].entries[5U].values[2U], u8"file3.diff"_str);
		ASSERT_EQUAL(wrap[0U].entries[5U].values[3U], u8"file4.asdf"_str);
		ASSERT_EQUAL(wrap[0U].entries[6U].key, u8"method"_str);
		ASSERT_EQUAL(wrap[0U].entries[6U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[6U].values[0U], u8"meson"_str);
		ASSERT_EQUAL(wrap[0U].entries[7U].key, u8"url"_str);
		ASSERT_EQUAL(wrap[0U].entries[7U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[7U].values[0U],
		             u8"https://example.com/example/repo"_str);
		ASSERT_EQUAL(wrap[0U].entries[8U].key, u8"revision"_str);
		ASSERT_EQUAL(wrap[0U].entries[8U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[8U].values[0U], u8"v1.3.2"_str);
		ASSERT_EQUAL(wrap[0U].entries[9U].key, u8"depth"_str);
		ASSERT_EQUAL(wrap[0U].entries[9U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[9U].values[0U], u8"2"_str);
		ASSERT_EQUAL(wrap[0U].entries[10U].key, u8"push-url"_str);
		ASSERT_EQUAL(wrap[0U].entries[10U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[10U].values[0U], u8"git:example/repo"_str);
		ASSERT_EQUAL(wrap[0U].entries[11U].key, u8"clone-recursive"_str);
		ASSERT_EQUAL(wrap[0U].entries[11U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[11U].values[0U], u8"true"_str);
	}

	{
		auto file =
		  u8R"([wrap-svn]
		  directory = /hello/world/
		  patch_url = https://example.com
		  patch_fallback_url = http://example.com/example/
		  patch_hash = asoidfcujaoswfdu9df8uwa9e8fuw9e8f
		  patch_directory = C:\waow\meow\
		  diff_files = file1,file2, file3.diff , file4.asdf
		  method = meson
		  url = https://example.com/example/repo
		  revision = v1.3.2)"_view;
		auto wrap = lak::dsl::meson_wrap.parse(file).UNWRAP().value;

		ASSERT_EQUAL(wrap.size(), 1U);
		ASSERT_EQUAL(wrap[0U].name, u8"wrap-svn"_str);
		ASSERT_EQUAL(wrap[0U].entries.size(), 9U);
		ASSERT_EQUAL(wrap[0U].entries[0U].key, u8"directory"_str);
		ASSERT_EQUAL(wrap[0U].entries[0U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[0U].values[0U], u8"/hello/world/"_str);
		ASSERT_EQUAL(wrap[0U].entries[1U].key, u8"patch_url"_str);
		ASSERT_EQUAL(wrap[0U].entries[1U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[1U].values[0U], u8"https://example.com"_str);
		ASSERT_EQUAL(wrap[0U].entries[2U].key, u8"patch_fallback_url"_str);
		ASSERT_EQUAL(wrap[0U].entries[2U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[2U].values[0U],
		             u8"http://example.com/example/"_str);
		ASSERT_EQUAL(wrap[0U].entries[3U].key, u8"patch_hash"_str);
		ASSERT_EQUAL(wrap[0U].entries[3U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[3U].values[0U],
		             u8"asoidfcujaoswfdu9df8uwa9e8fuw9e8f"_str);
		ASSERT_EQUAL(wrap[0U].entries[4U].key, u8"patch_directory"_str);
		ASSERT_EQUAL(wrap[0U].entries[4U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[4U].values[0U], u8"C:\\waow\\meow\\"_str);
		ASSERT_EQUAL(wrap[0U].entries[5U].key, u8"diff_files"_str);
		ASSERT_EQUAL(wrap[0U].entries[5U].values.size(), 4U);
		ASSERT_EQUAL(wrap[0U].entries[5U].values[0U], u8"file1"_str);
		ASSERT_EQUAL(wrap[0U].entries[5U].values[1U], u8"file2"_str);
		ASSERT_EQUAL(wrap[0U].entries[5U].values[2U], u8"file3.diff"_str);
		ASSERT_EQUAL(wrap[0U].entries[5U].values[3U], u8"file4.asdf"_str);
		ASSERT_EQUAL(wrap[0U].entries[6U].key, u8"method"_str);
		ASSERT_EQUAL(wrap[0U].entries[6U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[6U].values[0U], u8"meson"_str);
		ASSERT_EQUAL(wrap[0U].entries[7U].key, u8"url"_str);
		ASSERT_EQUAL(wrap[0U].entries[7U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[7U].values[0U],
		             u8"https://example.com/example/repo"_str);
		ASSERT_EQUAL(wrap[0U].entries[8U].key, u8"revision"_str);
		ASSERT_EQUAL(wrap[0U].entries[8U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[8U].values[0U], u8"v1.3.2"_str);
	}

	{
		auto file =
		  u8R"([wrap-hg]
		  directory = /hello/world/
		  patch_url = https://example.com
		  patch_fallback_url = http://example.com/example/
		  patch_hash = asoidfcujaoswfdu9df8uwa9e8fuw9e8f
		  patch_directory = C:\waow\meow\
		  diff_files = file1,file2, file3.diff , file4.asdf
		  method = meson
		  url = https://example.com/example/repo
		  revision = v1.3.2)"_view;
		auto wrap = lak::dsl::meson_wrap.parse(file).UNWRAP().value;

		ASSERT_EQUAL(wrap.size(), 1U);
		ASSERT_EQUAL(wrap[0U].name, u8"wrap-hg"_str);
		ASSERT_EQUAL(wrap[0U].entries.size(), 9U);
		ASSERT_EQUAL(wrap[0U].entries[0U].key, u8"directory"_str);
		ASSERT_EQUAL(wrap[0U].entries[0U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[0U].values[0U], u8"/hello/world/"_str);
		ASSERT_EQUAL(wrap[0U].entries[1U].key, u8"patch_url"_str);
		ASSERT_EQUAL(wrap[0U].entries[1U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[1U].values[0U], u8"https://example.com"_str);
		ASSERT_EQUAL(wrap[0U].entries[2U].key, u8"patch_fallback_url"_str);
		ASSERT_EQUAL(wrap[0U].entries[2U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[2U].values[0U],
		             u8"http://example.com/example/"_str);
		ASSERT_EQUAL(wrap[0U].entries[3U].key, u8"patch_hash"_str);
		ASSERT_EQUAL(wrap[0U].entries[3U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[3U].values[0U],
		             u8"asoidfcujaoswfdu9df8uwa9e8fuw9e8f"_str);
		ASSERT_EQUAL(wrap[0U].entries[4U].key, u8"patch_directory"_str);
		ASSERT_EQUAL(wrap[0U].entries[4U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[4U].values[0U], u8"C:\\waow\\meow\\"_str);
		ASSERT_EQUAL(wrap[0U].entries[5U].key, u8"diff_files"_str);
		ASSERT_EQUAL(wrap[0U].entries[5U].values.size(), 4U);
		ASSERT_EQUAL(wrap[0U].entries[5U].values[0U], u8"file1"_str);
		ASSERT_EQUAL(wrap[0U].entries[5U].values[1U], u8"file2"_str);
		ASSERT_EQUAL(wrap[0U].entries[5U].values[2U], u8"file3.diff"_str);
		ASSERT_EQUAL(wrap[0U].entries[5U].values[3U], u8"file4.asdf"_str);
		ASSERT_EQUAL(wrap[0U].entries[6U].key, u8"method"_str);
		ASSERT_EQUAL(wrap[0U].entries[6U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[6U].values[0U], u8"meson"_str);
		ASSERT_EQUAL(wrap[0U].entries[7U].key, u8"url"_str);
		ASSERT_EQUAL(wrap[0U].entries[7U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[7U].values[0U],
		             u8"https://example.com/example/repo"_str);
		ASSERT_EQUAL(wrap[0U].entries[8U].key, u8"revision"_str);
		ASSERT_EQUAL(wrap[0U].entries[8U].values.size(), 1U);
		ASSERT_EQUAL(wrap[0U].entries[8U].values[0U], u8"v1.3.2"_str);
	}

	return EXIT_SUCCESS;
}
END_TEST()

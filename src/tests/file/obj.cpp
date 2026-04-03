#include "lak/test.hpp"

#include "lak/file/obj.hpp"

#include "lak/string_literals/span.hpp"

BEGIN_TEST(obj)
{
	{
		DEBUG("face bounds checking")
		auto source =
		  u8"f 1/1 2/2/1 3//1\n"
		  ""_view;

		DEBUG(lak::dsl::obj.parse(source).UNWRAP_ERR());
	}

	{
		DEBUG("line bounds checking")
		auto source =
		  u8"l 1 2\n"
		  ""_view;

		DEBUG(lak::dsl::obj.parse(source).UNWRAP_ERR());
	}

	{
		DEBUG("vertex coord test");
		auto source =
		  u8"v -1.0 0.1 0.5 4.5\n"
		  "v -1.0 0.0 0.5"
		  ""_view;
		lak::dsl::obj.parse(source).UNWRAP();
	}

	{
		DEBUG("vertex normal test");
		auto source = u8"vn 0.0 0.0 1.0"_view;
		lak::dsl::obj.parse(source).UNWRAP();
	}

	{
		DEBUG("texture coord test");
		auto source =
		  u8"vt 1.0 2.0 3.0\n"
		  "vt +1.0 2.0\n"
		  "vt 1.0"
		  ""_view;
		lak::dsl::obj.parse(source).UNWRAP();
	}

	{
		DEBUG("group test");
		auto source =
		  u8"v -1.0 0.1 0.5 4.5\n"
		  "v -1.0 0.1 0.5\n"
		  "v -1.0 0.1 0.5\n"
		  "g hello, world!\n"
		  "f 1 2 3\n"
		  ""_view;
		lak::dsl::obj.parse(source).UNWRAP();
	}

	{
		DEBUG("object test");
		auto source =
		  u8"v -1.0 0.1 0.5 4.5\n"
		  "v -1.0 0.1 0.5\n"
		  "v -1.0 0.1 0.5\n"
		  "o hello, world!\n"
		  "f 1 2 3\n"
		  ""_view;
		lak::dsl::obj.parse(source).UNWRAP();
	}

	{
		DEBUG("big test")
		auto source =
		  u8"# comm\tent\n"
		  "v -1.0 0.0 0.5\n"
		  "v 0.0 +2.0 0.0\n"
		  "v +1.0 0.0 -0.5\n"
		  "vt 0.0 0.0\n"
		  "vt 1.0 1.0\n"
		  "vn 0.0 0.0 1.0\n"
		  "o my object\n"
		  "f 1/1 2/2/1 3//1\n"
		  "g group 1\n"
		  "f 1/1 2/2/1 3//1\n"
		  "g group 2\n"
		  "f 1/1 2/2/1 3//1\n"
		  ""_view;

		auto obj = lak::dsl::obj.parse(source).UNWRAP().value;

		ASSERT_EQUAL(obj.vertex_coords.size(), 3U);
		ASSERT_EQUAL(obj.texture_coords.size(), 2U);
		ASSERT_EQUAL(obj.vertex_normals.size(), 1U);
		ASSERT_EQUAL(obj.face_coords.size(), 9U);
		ASSERT_EQUAL(obj.faces.size(), 3U);
		ASSERT_EQUAL(obj.objects.size(), 1U);
		ASSERT_EQUAL(obj.groups.size(), 3U);

		ASSERT_EQUAL(obj.vertex_coords[0].x, -1.0);
		ASSERT_EQUAL(obj.vertex_coords[0].y, 0.0);
		ASSERT_EQUAL(obj.vertex_coords[0].z, 0.5);
		ASSERT_EQUAL(obj.vertex_coords[0].w, 1.0);

		ASSERT_EQUAL(obj.vertex_coords[1].x, 0.0);
		ASSERT_EQUAL(obj.vertex_coords[1].y, 2.0);
		ASSERT_EQUAL(obj.vertex_coords[1].z, 0.0);
		ASSERT_EQUAL(obj.vertex_coords[1].w, 1.0);

		ASSERT_EQUAL(obj.vertex_coords[2].x, 1.0);
		ASSERT_EQUAL(obj.vertex_coords[2].y, 0.0);
		ASSERT_EQUAL(obj.vertex_coords[2].z, -0.5);
		ASSERT_EQUAL(obj.vertex_coords[2].w, 1.0);

		ASSERT_EQUAL(obj.texture_coords[0].u, 0.0);
		ASSERT_EQUAL(obj.texture_coords[0].v, 0.0);
		ASSERT_EQUAL(obj.texture_coords[0].w, 0.0);

		ASSERT_EQUAL(obj.texture_coords[1].u, 1.0);
		ASSERT_EQUAL(obj.texture_coords[1].v, 1.0);
		ASSERT_EQUAL(obj.texture_coords[1].w, 0.0);

		ASSERT_EQUAL(obj.vertex_normals[0].x, 0.0);
		ASSERT_EQUAL(obj.vertex_normals[0].y, 0.0);
		ASSERT_EQUAL(obj.vertex_normals[0].z, 1.0);

		for (size_t i = 0U; i < 3U; ++i)
		{
			ASSERT_EQUAL(obj.faces[i + 0].offset, i * 3U);
			ASSERT_EQUAL(obj.faces[i + 0].count, 3U);

			ASSERT_EQUAL(obj.face_coords[(i * 3U) + 0].v, 0U);
			ASSERT_EQUAL(obj.face_coords[(i * 3U) + 0].vt, 0U);
			ASSERT_EQUAL(obj.face_coords[(i * 3U) + 0].vn, lak::dynamic_extent);

			ASSERT_EQUAL(obj.face_coords[(i * 3U) + 1].v, 1U);
			ASSERT_EQUAL(obj.face_coords[(i * 3U) + 1].vt, 1U);
			ASSERT_EQUAL(obj.face_coords[(i * 3U) + 1].vn, 0U);

			ASSERT_EQUAL(obj.face_coords[(i * 3U) + 2].v, 2U);
			ASSERT_EQUAL(obj.face_coords[(i * 3U) + 2].vt, lak::dynamic_extent);
			ASSERT_EQUAL(obj.face_coords[(i * 3U) + 2].vn, 0U);
		}

		ASSERT_EQUAL(obj.objects[0].name, u8"my object"_view);

		ASSERT_EQUAL(obj.groups[0].name, u8""_view);
		ASSERT_EQUAL(obj.groups[1].name, u8"group 1"_view);
		ASSERT_EQUAL(obj.groups[2].name, u8"group 2"_view);
	}

	return 0;
}
END_TEST()

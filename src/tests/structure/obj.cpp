#include "lak/test.hpp"

#include "lak/structure/obj.hpp"

#include "lak/string_literals.hpp"

BEGIN_TEST(obj)
{
	{
		auto source =
		  "# comm\tent\n"
		  "v -1.0 0.0 0.0\n"
		  "v 0.0 +2.0 0.0\n"
		  "v +1.0 0.0 0.0\n"
		  "vt 0.0 0.0\n"
		  "vt 1.0 1.0\n"
		  "vn 0.0 0.0 1.0\n"
		  "f 1/1 2/2/1 3//1\n"
		  ""_span;

		lak::binary_reader strm{lak::span<const byte_t>(source)};

		auto obj = strm.template read<lak::obj::obj>().UNWRAP();

		ASSERT_EQUAL(obj.vertex_coords.size(), 3U);
		ASSERT_EQUAL(obj.texture_coords.size(), 2U);
		ASSERT_EQUAL(obj.vertex_normals.size(), 1U);
		ASSERT_EQUAL(obj.face_coords.size(), 3U);
		ASSERT_EQUAL(obj.faces.size(), 1U);

		ASSERT_EQUAL(obj.vertex_coords[0].x, -1.0);
		ASSERT_EQUAL(obj.vertex_coords[0].y, 0.0);
		ASSERT_EQUAL(obj.vertex_coords[0].z, 0.0);
		ASSERT_EQUAL(obj.vertex_coords[0].w, 1.0);

		ASSERT_EQUAL(obj.vertex_coords[1].x, 0.0);
		ASSERT_EQUAL(obj.vertex_coords[1].y, 2.0);
		ASSERT_EQUAL(obj.vertex_coords[1].z, 0.0);
		ASSERT_EQUAL(obj.vertex_coords[1].w, 1.0);

		ASSERT_EQUAL(obj.vertex_coords[2].x, 1.0);
		ASSERT_EQUAL(obj.vertex_coords[2].y, 0.0);
		ASSERT_EQUAL(obj.vertex_coords[2].z, 0.0);
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

		ASSERT_EQUAL(obj.faces[0].offset, 0U);
		ASSERT_EQUAL(obj.faces[0].count, 3U);

		ASSERT_EQUAL(obj.face_coords[0].v, 0U);
		ASSERT_EQUAL(obj.face_coords[0].vt, 0U);
		ASSERT_EQUAL(obj.face_coords[0].vn, lak::dynamic_extent);

		ASSERT_EQUAL(obj.face_coords[1].v, 1U);
		ASSERT_EQUAL(obj.face_coords[1].vt, 1U);
		ASSERT_EQUAL(obj.face_coords[1].vn, 0U);

		ASSERT_EQUAL(obj.face_coords[2].v, 2U);
		ASSERT_EQUAL(obj.face_coords[2].vt, lak::dynamic_extent);
		ASSERT_EQUAL(obj.face_coords[2].vn, 0U);
	}

	return 0;
}
END_TEST()

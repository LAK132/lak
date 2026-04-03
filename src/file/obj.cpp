#include "lak/file/obj.hpp"

#include "lak/dsl/reader.hpp"

#include "lak/optional.hpp"

#include "obj.ebnf.hpp"

lak::dsl::result<lak::dsl::obj_t::value_type> lak::dsl::obj_t::parse(
  lak::u8string_view str) const
{
	lak::obj::obj result;

	lak::dsl::reader strm{str};

	lak::optional<lak::obj::group> working_group   = lak::obj::group{};
	lak::optional<lak::obj::object> working_object = lak::obj::object{};

	auto pop_working_group = [&]()
	{
		if (working_group)
		{
			working_group->face_count =
			  result.faces.size() - working_group->face_offset;
			working_group->line_count =
			  result.lines.size() - working_group->line_offset;

			if (!working_group->name.empty() || working_group->face_count != 0U ||
			    working_group->line_count != 0U)
			{
				result.groups.push_back(lak::move(*working_group));
			}

			working_group.reset();
		}
	};

	auto pop_working_object = [&]()
	{
		pop_working_group();

		if (working_object)
		{
			working_object->count = result.groups.size() - working_object->offset;

			if (!working_object->name.empty() || working_object->count != 0U)
			{
				result.objects.push_back(lak::move(*working_object));
			}

			working_object.reset();
		}
	};

	RES_TRY(strm.parse<lak::obj_parse::nlws>());

	while (!strm.empty())
	{
		RES_TRY_ASSIGN(auto parsed_line =, strm.parse<lak::obj_parse::entry>());

		parsed_line.visit(lak::overloaded{
		  [&](const lak::obj::vertex_coord &v)
		  { result.vertex_coords.push_back(v); },
		  [&](const lak::obj::texture_coord &vt)
		  { result.texture_coords.push_back(vt); },
		  [&](const lak::obj::vertex_normal &vn)
		  { result.vertex_normals.push_back(vn); },
		  [&](const lak::array<lak::obj::face_coord> &f)
		  {
			  auto &face  = result.faces.emplace_back();
			  face.offset = result.face_coords.size();
			  face.count  = f.size();
			  result.face_coords.insert(result.face_coords.end(), f);
		  },
		  [&](const lak::array<lak::obj::line_coord> &l)
		  {
			  auto &line  = result.lines.emplace_back();
			  line.offset = result.line_coords.size();
			  line.count  = l.size();
			  result.line_coords.insert(result.line_coords.end(), l);
		  },
		  [&](const lak::obj::object &o)
		  {
			  pop_working_object();
			  working_group          = lak::obj::group{}; // dummy group
			  working_object         = o;
			  working_object->offset = result.groups.size();
		  },
		  [&](const lak::obj::group &g)
		  {
			  pop_working_group();
			  working_group              = g;
			  working_group->face_offset = result.faces.size();
			  working_group->line_offset = result.lines.size();
		  },
		  [&](const lak::obj::smooth_shading &) {},
		});

		RES_TRY(strm.parse<lak::obj_parse::nlws>());
	}

	pop_working_object();

	for (const auto &f : result.face_coords)
		if (f.v >= result.vertex_coords.size() ||
		    (f.vt != lak::dynamic_extent &&
		     f.vt >= result.texture_coords.size()) ||
		    (f.vn != lak::dynamic_extent && f.vn >= result.vertex_normals.size()))
			return lak::err_t<lak::dsl::err::parse>{
			  {.message = u8"facet coord out of range"}};

	for (const auto &l : result.line_coords)
		if (l.v >= result.vertex_coords.size())
			return lak::err_t<lak::dsl::err::parse>{
			  {.message = u8"line coord out of range"}};

	return lak::ok_t<lak::dsl::parse_result<lak::obj::obj>>{{
	  .consumed  = strm.consumed(),
	  .remaining = strm.remaining(),
	  .value     = lak::move(result),
	}};
}

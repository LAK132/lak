#include "lak/structure/obj.hpp"

#include "lak/dsl/binary_reader.hpp"
#include "lak/dsl/utility.hpp"

template<lak::endian E>
lak::error_codes<lak::out_of_data_error,
                 lak::value_out_of_range_error,
                 lak::string_to_numeric_error,
                 lak::dsl::parse_error>
lak::obj::obj::read(lak::binary_reader &strm)
{
	lak::dsl::binary_reader parser{strm};

	auto empty_lines = *(lak::dsl::pound_line_comment | lak::dsl::whitespace);

	auto uint_parser =
	  lak::dsl::capture_nth<1U,
	                        (*lak::dsl::ascii_nonnewline_whitespace),
	                        lak::dsl::parsed_dec_uint<size_t>>;

	auto double_parser = lak::dsl::capture_nth<
	  1U,
	  (*lak::dsl::ascii_nonnewline_whitespace),
	  lak::dsl::parsed_dec_float<double,
	                             lak::dsl::char_literal<U'.'>,
	                             lak::dsl::bottom>>;

	auto vertex_coord_parser = lak::dsl::transform<
	  lak::dsl::capture_nth<
	    1U,
	    (*lak::dsl::ascii_nonnewline_whitespace) + lak::dsl::str_literal<u8"v"> +
	      (+lak::dsl::ascii_nonnewline_whitespace),
	    double_parser + double_parser + double_parser + ~double_parser>,
	  [](const lak::tuple<double, double, double, lak::optional<double>> &value)
	  {
		  auto [x, y, z, w] = value;
		  return lak::obj::vertex_coord{
		    .x = x,
		    .y = y,
		    .z = z,
		    .w = w ? *w : 1.0,
		  };
	  }>;

	auto texture_coord_parser = lak::dsl::transform<
	  lak::dsl::capture_nth<1U,
	                        (*lak::dsl::ascii_nonnewline_whitespace) +
	                          lak::dsl::str_literal<u8"vt"> +
	                          (+lak::dsl::ascii_nonnewline_whitespace),
	                        double_parser + ~double_parser + ~double_parser>,
	  [](const lak::tuple<double, lak::optional<double>, lak::optional<double>>
	       &value)
	  {
		  auto [u, v, w] = value;
		  return lak::obj::texture_coord{
		    .u = u,
		    .v = v ? *v : 0.0,
		    .w = w ? *w : 0.0,
		  };
	  }>;

	auto vertex_normal_parser = lak::dsl::transform<
	  lak::dsl::capture_nth<1U,
	                        (*lak::dsl::ascii_nonnewline_whitespace) +
	                          lak::dsl::str_literal<u8"vn"> +
	                          (+lak::dsl::ascii_nonnewline_whitespace),
	                        double_parser + double_parser + double_parser>,
	  [](const lak::tuple<double, double, double> &value)
	  {
		  auto [x, y, z] = value;
		  return lak::obj::vertex_normal{
		    .x = x,
		    .y = y,
		    .z = z,
		  };
	  }>;

	// [[maybe_unused]] auto vertex_pspace_parser = lak::dsl::transform<
	//   lak::dsl::capture_nth<1U,
	//                         (*lak::dsl::ascii_nonnewline_whitespace) +
	//                           lak::dsl::str_literal<u8"vp"> +
	//                           (+lak::dsl::ascii_nonnewline_whitespace),
	//                         double_parser + ~double_parser + ~double_parser>,
	//   [](const lak::tuple<double, lak::optional<double>,
	//   lak::optional<double>>
	//        &) { return lak::bottom{}; }>;

	auto polyface_parser = lak::dsl::capture_nth<
	  1U,
	  (*lak::dsl::ascii_nonnewline_whitespace) + lak::dsl::str_literal<u8"f"> +
	    (+lak::dsl::ascii_nonnewline_whitespace),
	  lak::dsl::repeat_at_least<
	    lak::dsl::transform<
	      (uint_parser +
	       ~(lak::dsl::
	           capture_nth<1U, lak::dsl::char_literal<U'/'>, ~uint_parser> +
	         ~lak::dsl::
	           capture_nth<1U, lak::dsl::char_literal<U'/'>, uint_parser>)),
	      [](const lak::tuple<size_t,
	                          lak::optional<lak::tuple<lak::optional<size_t>,
	                                                   lak::optional<size_t>>>>
	           &value)
	      {
		      auto [v, vtvn] = value;
		      if (vtvn)
		      {
			      auto [vt, vn] = *vtvn;
			      return lak::obj::face_coord{
			        .v  = v - 1U,
			        .vt = vt ? (*vt) - 1U : lak::dynamic_extent,
			        .vn = vn ? (*vn) - 1U : lak::dynamic_extent,
			      };
		      }
		      else
			      return lak::obj::face_coord{.v = v - 1U};
	      }>,
	    3U>>;

	auto polyline_parser = lak::dsl::capture_nth<
	  1U,
	  (*lak::dsl::ascii_nonnewline_whitespace) + lak::dsl::str_literal<u8"l"> +
	    (+lak::dsl::ascii_nonnewline_whitespace),
	  lak::dsl::repeat_at_least<
	    lak::dsl::transform<uint_parser,
	                        [](size_t v)
	                        { return lak::obj::line_coord{.v = v - 1U}; }>,
	    2U>>;

	auto data_parser = vertex_coord_parser | texture_coord_parser |
	                   vertex_normal_parser /*| vertex_pspace_parser*/ |
	                   polyface_parser | polyline_parser;

	auto lines_parser = lak::dsl::capture_nth<1U, empty_lines, data_parser>;

	RES_TRY(parser.read(empty_lines));
	while (!strm.empty())
	{
		RES_TRY_ASSIGN(auto parsed_line =, parser.read(data_parser));
		parsed_line.visit(lak::overloaded{
		  [&](const lak::obj::vertex_coord &v) { vertex_coords.push_back(v); },
		  [&](const lak::obj::texture_coord &vt) { texture_coords.push_back(vt); },
		  [&](const lak::obj::vertex_normal &vn) { vertex_normals.push_back(vn); },
		  [&](const lak::array<lak::obj::face_coord> &f)
		  {
			  auto &face  = faces.emplace_back();
			  face.offset = face_coords.size();
			  face.count  = f.size();
			  face_coords.reserve(face_coords.size() + f.size());
			  for (const auto &_f : f) face_coords.push_back(_f);
		  },
		  [&](const lak::array<lak::obj::line_coord> &l)
		  {
			  auto &line  = lines.emplace_back();
			  line.offset = line_coords.size();
			  line.count  = l.size();
			  line_coords.reserve(line_coords.size() + l.size());
			  for (const auto &_l : l) line_coords.push_back(_l);
		  },
		});
		RES_TRY(parser.read(empty_lines));
	}

	for (const auto &f : face_coords)
		if (f.v >= vertex_coords.size() ||
		    (f.vt != lak::dynamic_extent && f.vt >= texture_coords.size()) ||
		    (f.vn != lak::dynamic_extent && f.vn >= vertex_normals.size()))
			return lak::err_t{lak::value_out_of_range_error{}};

	for (const auto &l : line_coords)
		if (l.v >= vertex_coords.size())
			return lak::err_t{lak::value_out_of_range_error{}};

	return lak::ok_t{};
}

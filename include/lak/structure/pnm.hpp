#ifndef LAK_STRUCTURE_PNM_HPP
#define LAK_STRUCTURE_PNM_HPP

/*
P1: PBM ASCII bitmap (b&w)
P2: PGM ASCII greymap (monochrome)
P3: PPM ASCII pixmap (RGB)
P4: PBM binary bitmap (b&w)
P5: PGM binary greymap (monochrome)
P6: PPM binary pixmap (RGB)
Pf: f32 greymapmap (monochrome)
PF: f32 pixmap (RGB)
PF4: f32 pixmap (RGBA)
P7: PAM binary anymap
*/

#include "lak/array.hpp"
#include "lak/binary_reader.hpp"
#include "lak/binary_traits.hpp"
#include "lak/bit_reader.hpp"
#include "lak/endian.hpp"
#include "lak/image.hpp"
#include "lak/numeric.hpp"
#include "lak/variant.hpp"
#include "lak/vec.hpp"

#include "lak/dsl/dsl.hpp"

namespace lak
{
	namespace pnm
	{
		template<typename T, typename U>
		struct pnm_data : public ::lak::array<T>
		{
			U max_value;
		};

#define LAK_FOREACH_PNM_TYPE(MACRO, ...)                                      \
	MACRO(P1, __VA_ARGS__)                                                      \
	MACRO(P2, __VA_ARGS__)                                                      \
	MACRO(P3, __VA_ARGS__)                                                      \
	MACRO(P4, __VA_ARGS__)                                                      \
	MACRO(P5, __VA_ARGS__)                                                      \
	MACRO(P6, __VA_ARGS__)                                                      \
	MACRO(Pf, __VA_ARGS__)                                                      \
	MACRO(PF, __VA_ARGS__)                                                      \
	MACRO(PF4, __VA_ARGS__)                                                     \
	MACRO(P7, __VA_ARGS__)

		enum struct pnm_type : uint8_t
		{
#define LAK_PNM_ENUM(NAME, ...) NAME,
			LAK_FOREACH_PNM_TYPE(LAK_PNM_ENUM)
#undef LAK_PNM_ENUM
		};

		struct pnm
		{
			uint32_t width;
			::lak::variant<::lak::monostate,
			               pnm_data<bool, bool>,
			               pnm_data<uint8_t, uint8_t>,
			               pnm_data<uint16_t, uint16_t>,
			               pnm_data<f32_t, f32_t>,
			               pnm_data<::lak::vec3u8_t, uint8_t>,
			               pnm_data<::lak::vec4u8_t, uint8_t>,
			               pnm_data<::lak::vec3u16_t, uint16_t>,
			               pnm_data<::lak::vec4u16_t, uint16_t>,
			               pnm_data<::lak::vec3f32_t, f32_t>,
			               pnm_data<::lak::vec4f32_t, f32_t>>
			  value;

			uint8_t channels() const;

			operator ::lak::image3_t() const;
			operator ::lak::image4_t() const;

			template<::lak::endian E>
			::lak::error_codes<::lak::out_of_data_error,
			                   ::lak::value_out_of_range_error,
			                   ::lak::string_to_numeric_error,
			                   ::lak::dsl::parse_error>
			read(::lak::binary_reader &strm)
			{
				// pnms have their on way of handling endianness
				return _read(strm);
			}

			::lak::error_codes<::lak::out_of_data_error,
			                   ::lak::value_out_of_range_error,
			                   ::lak::string_to_numeric_error,
			                   ::lak::dsl::parse_error>
			_read(::lak::binary_reader &strm);
		};
	}
}

static_assert(
  lak::concepts::from_bytes_readable<lak::pnm::pnm, lak::endian::native>);

#endif

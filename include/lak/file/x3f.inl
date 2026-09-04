#include "lak/file/x3f.hpp"

LAK_MEMCPY_BYTE_TRAITS_IMPL(lak::x3f::extended_data_type);

LAK_FIXED_STRUCT_BYTES_TRAITS(lak::x3f::version_number,
                              &lak::x3f::version_number::minor,
                              &lak::x3f::version_number::major);
LAK_FIXED_STRUCT_BYTES_TRAITS(lak::x3f::section_header,
                              &lak::x3f::section_header::fourcc,
                              &lak::x3f::section_header::version);

LAK_FIXED_STRUCT_BYTES_TRAITS(lak::x3f::header_1_0,
                              &lak::x3f::header_1_0::unique_id,
                              &lak::x3f::header_1_0::mark_bits,
                              &lak::x3f::header_1_0::image_columns,
                              &lak::x3f::header_1_0::image_rows,
                              &lak::x3f::header_1_0::rotation);
LAK_FIXED_STRUCT_BYTES_TRAITS(lak::x3f::header_2_0,
                              &lak::x3f::header_2_0::unique_id,
                              &lak::x3f::header_2_0::mark_bits,
                              &lak::x3f::header_2_0::image_columns,
                              &lak::x3f::header_2_0::image_rows,
                              &lak::x3f::header_2_0::rotation,
                              &lak::x3f::header_2_0::white_balance,
                              &lak::x3f::header_2_0::extended_data_types,
                              &lak::x3f::header_2_0::extended_data);
LAK_FIXED_STRUCT_BYTES_TRAITS(lak::x3f::header_4_0,
                              &lak::x3f::header_4_0::unknown1,
                              &lak::x3f::header_4_0::unknown2,
                              &lak::x3f::header_4_0::unknown3,
                              &lak::x3f::header_4_0::unique_id,
                              &lak::x3f::header_4_0::mark_bits,
                              &lak::x3f::header_4_0::image_columns,
                              &lak::x3f::header_4_0::image_rows,
                              &lak::x3f::header_4_0::rotation
                              // &lak::x3f::header_4_0::white_balance,
                              // &lak::x3f::header_4_0::extended_data_types,
                              // &lak::x3f::header_4_0::extended_data
);

LAK_FIXED_STRUCT_BYTES_TRAITS(lak::x3f::directory_entry_2_0,
                              &lak::x3f::directory_entry_2_0::offset,
                              &lak::x3f::directory_entry_2_0::size,
                              &lak::x3f::directory_entry_2_0::type);

LAK_MEMCPY_BYTE_TRAITS_IMPL(lak::x3f::image_type);
LAK_MEMCPY_BYTE_TRAITS_IMPL(lak::x3f::image_format);
LAK_FIXED_STRUCT_BYTES_TRAITS(lak::x3f::image_data_2_0,
                              &lak::x3f::image_data_2_0::type,
                              &lak::x3f::image_data_2_0::format,
                              &lak::x3f::image_data_2_0::columns,
                              &lak::x3f::image_data_2_0::rows,
                              &lak::x3f::image_data_2_0::row_bytes);

LAK_FIXED_STRUCT_BYTES_TRAITS(lak::x3f::camf_data_header,
                              &lak::x3f::camf_data_header::type,
                              &lak::x3f::camf_data_header::unknown,
                              &lak::x3f::camf_data_header::columns,
                              &lak::x3f::camf_data_header::rows);

template<typename CHAR>
struct lak::format_traits<lak::x3f::extended_data_type, CHAR>
{
	static lak::string<CHAR> to_string(const lak::x3f::extended_data_type &value)
	{
		return lak::fmt<CHAR, "{:d}">(static_cast<uint32_t>(value));
	}
};

template<typename CHAR>
struct lak::format_traits<lak::x3f::version_number, CHAR>
{
	static lak::string<CHAR> to_string(const lak::x3f::version_number &value)
	{
		return lak::fmt<CHAR, "{:d}.{:d}">(value.major, value.minor);
	}
};

template<typename CHAR>
struct lak::format_traits<lak::x3f::image_type, CHAR>
{
	static lak::string<CHAR> to_string(const lak::x3f::image_type &value)
	{
		switch (value)
		{
#define LAK_X3F_IMAGE_TYPE_TO_STRING(VAL, NAME, ...)                          \
	case lak::x3f::image_type::NAME:                                            \
		return lak::fmt<CHAR, #NAME " ({:#X})">(VAL);
			LAK_FOREACH_X3F_IMAGE_TYPE(LAK_X3F_IMAGE_TYPE_TO_STRING)
#undef LAK_X3F_IMAGE_TYPE_TO_STRING
			default: return lak::fmt<CHAR, "{:#X}">(static_cast<uint32_t>(value));
		}
	}
};

template<typename CHAR>
struct lak::format_traits<lak::x3f::image_format, CHAR>
{
	static lak::string<CHAR> to_string(const lak::x3f::image_format &value)
	{
		switch (value)
		{
#define LAK_X3F_IMAGE_FORMAT_TO_STRING(VAL, NAME, ...)                        \
	case lak::x3f::image_format::NAME:                                          \
		return lak::fmt<CHAR, #NAME " ({:#X})">(VAL);
			LAK_FOREACH_X3F_IMAGE_FORMAT(LAK_X3F_IMAGE_FORMAT_TO_STRING)
#undef LAK_X3F_IMAGE_FORMAT_TO_STRING
			default: return lak::fmt<CHAR, "{:#X}">(static_cast<uint32_t>(value));
		}
	}
};

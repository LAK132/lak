#include "lak/structure/pnm.hpp"

lak::pnm::pnm::operator ::lak::image3_t() const
{
	return value.visit(::lak::overloaded{
	  [](const ::lak::monostate &) -> ::lak::image3_t { ASSERT_UNREACHABLE(); },
	  [this](const pnm_data<bool, bool> &data) -> ::lak::image3_t
	  {
		  ::lak::image3_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  for (const auto &v : data)
		  {
			  result[i].r = v ? 0U : UINT8_MAX;
			  result[i].g = v ? 0U : UINT8_MAX;
			  result[i].b = v ? 0U : UINT8_MAX;
			  ++i;
		  }
		  return result;
	  },
	  [this](const pnm_data<uint8_t, uint8_t> &data) -> ::lak::image3_t
	  {
		  ::lak::image3_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  if (data.max_value == UINT8_MAX)
			  for (const auto &v : data)
			  {
				  result[i].r = v;
				  result[i].g = v;
				  result[i].b = v;
				  ++i;
			  }
		  else
			  for (const auto &v : data)
			  {
				  result[i].r = uint8_t((uintmax_t(v) * UINT8_MAX) / data.max_value);
				  result[i].g = uint8_t((uintmax_t(v) * UINT8_MAX) / data.max_value);
				  result[i].b = uint8_t((uintmax_t(v) * UINT8_MAX) / data.max_value);
				  ++i;
			  }
		  return result;
	  },
	  [this](const pnm_data<uint16_t, uint16_t> &data) -> ::lak::image3_t
	  {
		  ::lak::image3_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  for (const auto &v : data)
		  {
			  result[i].r = uint8_t((uintmax_t(v) * UINT8_MAX) / data.max_value);
			  result[i].g = uint8_t((uintmax_t(v) * UINT8_MAX) / data.max_value);
			  result[i].b = uint8_t((uintmax_t(v) * UINT8_MAX) / data.max_value);
			  ++i;
		  }
		  return result;
	  },
	  [this](const pnm_data<f32_t, f32_t> &data) -> ::lak::image3_t
	  {
		  ::lak::image3_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  for (const auto &v : data)
		  {
			  result[i].r = uint8_t((v * UINT8_MAX) / data.max_value);
			  result[i].g = uint8_t((v * UINT8_MAX) / data.max_value);
			  result[i].b = uint8_t((v * UINT8_MAX) / data.max_value);
			  ++i;
		  }
		  return result;
	  },
	  [this](const pnm_data<::lak::vec3u8_t, uint8_t> &data) -> ::lak::image3_t
	  {
		  ::lak::image3_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  if (data.max_value == UINT8_MAX)
			  for (const auto &v : data) result[i++] = v;
		  else
			  for (const auto &v : data)
			  {
				  result[i].r = uint8_t((uintmax_t(v.r) * UINT8_MAX) / data.max_value);
				  result[i].g = uint8_t((uintmax_t(v.g) * UINT8_MAX) / data.max_value);
				  result[i].b = uint8_t((uintmax_t(v.b) * UINT8_MAX) / data.max_value);
				  ++i;
			  }
		  return result;
	  },
	  [this](const pnm_data<::lak::vec4u8_t, uint8_t> &data) -> ::lak::image3_t
	  {
		  ::lak::image3_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  if (data.max_value == UINT8_MAX)
			  for (const auto &v : data)
			  {
				  result[i].r = v.r;
				  result[i].g = v.g;
				  result[i].b = v.b;
				  ++i;
			  }
		  else
			  for (const auto &v : data)
			  {
				  result[i].r = uint8_t((uintmax_t(v.r) * UINT8_MAX) / data.max_value);
				  result[i].g = uint8_t((uintmax_t(v.g) * UINT8_MAX) / data.max_value);
				  result[i].b = uint8_t((uintmax_t(v.b) * UINT8_MAX) / data.max_value);
				  ++i;
			  }
		  return result;
	  },
	  [this](const pnm_data<::lak::vec3u16_t, uint16_t> &data) -> ::lak::image3_t
	  {
		  ::lak::image3_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  for (const auto &v : data)
		  {
			  result[i].r = uint8_t((uintmax_t(v.r) * UINT8_MAX) / data.max_value);
			  result[i].g = uint8_t((uintmax_t(v.g) * UINT8_MAX) / data.max_value);
			  result[i].b = uint8_t((uintmax_t(v.b) * UINT8_MAX) / data.max_value);
			  ++i;
		  }
		  return result;
	  },
	  [this](const pnm_data<::lak::vec4u16_t, uint16_t> &data) -> ::lak::image3_t
	  {
		  ::lak::image3_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  for (const auto &v : data)
		  {
			  result[i].r = uint8_t((uintmax_t(v.r) * UINT8_MAX) / data.max_value);
			  result[i].g = uint8_t((uintmax_t(v.g) * UINT8_MAX) / data.max_value);
			  result[i].b = uint8_t((uintmax_t(v.b) * UINT8_MAX) / data.max_value);
			  ++i;
		  }
		  return result;
	  },
	  [this](const pnm_data<::lak::vec3f32_t, f32_t> &data) -> ::lak::image3_t
	  {
		  ::lak::image3_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  for (const auto &v : data)
		  {
			  result[i].r = uint8_t((v.r * UINT8_MAX) / data.max_value);
			  result[i].g = uint8_t((v.g * UINT8_MAX) / data.max_value);
			  result[i].b = uint8_t((v.b * UINT8_MAX) / data.max_value);
			  ++i;
		  }
		  return result;
	  },
	  [this](const pnm_data<::lak::vec4f32_t, f32_t> &data) -> ::lak::image3_t
	  {
		  ::lak::image3_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  for (const auto &v : data)
		  {
			  result[i].r = uint8_t((v.r * UINT8_MAX) / data.max_value);
			  result[i].g = uint8_t((v.g * UINT8_MAX) / data.max_value);
			  result[i].b = uint8_t((v.b * UINT8_MAX) / data.max_value);
			  ++i;
		  }
		  return result;
	  },
	});
}

lak::pnm::pnm::operator ::lak::image4_t() const
{
	return value.visit(::lak::overloaded{
	  [](const ::lak::monostate &) -> ::lak::image4_t { ASSERT_UNREACHABLE(); },
	  [this](const pnm_data<bool, bool> &data) -> ::lak::image4_t
	  {
		  ::lak::image4_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  for (const auto &v : data)
		  {
			  result[i].r = v ? 0U : UINT8_MAX;
			  result[i].g = v ? 0U : UINT8_MAX;
			  result[i].b = v ? 0U : UINT8_MAX;
			  result[i].a = UINT8_MAX;
			  ++i;
		  }
		  return result;
	  },
	  [this](const pnm_data<uint8_t, uint8_t> &data) -> ::lak::image4_t
	  {
		  ::lak::image4_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  if (data.max_value == UINT8_MAX)
			  for (const auto &v : data)
			  {
				  result[i].r = v;
				  result[i].g = v;
				  result[i].b = v;
				  result[i].a = UINT8_MAX;
				  ++i;
			  }
		  else
			  for (const auto &v : data)
			  {
				  result[i].r = uint8_t((uintmax_t(v) * UINT8_MAX) / data.max_value);
				  result[i].g = uint8_t((uintmax_t(v) * UINT8_MAX) / data.max_value);
				  result[i].b = uint8_t((uintmax_t(v) * UINT8_MAX) / data.max_value);
				  result[i].a = UINT8_MAX;
				  ++i;
			  }
		  return result;
	  },
	  [this](const pnm_data<uint16_t, uint16_t> &data) -> ::lak::image4_t
	  {
		  ::lak::image4_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  for (const auto &v : data)
		  {
			  result[i].r = uint8_t((uintmax_t(v) * UINT8_MAX) / data.max_value);
			  result[i].g = uint8_t((uintmax_t(v) * UINT8_MAX) / data.max_value);
			  result[i].b = uint8_t((uintmax_t(v) * UINT8_MAX) / data.max_value);
			  result[i].a = UINT8_MAX;
			  ++i;
		  }
		  return result;
	  },
	  [this](const pnm_data<f32_t, f32_t> &data) -> ::lak::image4_t
	  {
		  ::lak::image4_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  for (const auto &v : data)
		  {
			  result[i].r = uint8_t((v * UINT8_MAX) / data.max_value);
			  result[i].g = uint8_t((v * UINT8_MAX) / data.max_value);
			  result[i].b = uint8_t((v * UINT8_MAX) / data.max_value);
			  result[i].a = UINT8_MAX;
			  ++i;
		  }
		  return result;
	  },
	  [this](const pnm_data<::lak::vec3u8_t, uint8_t> &data) -> ::lak::image4_t
	  {
		  ::lak::image4_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  if (data.max_value == UINT8_MAX)
			  for (const auto &v : data)
			  {
				  result[i].r = v.r;
				  result[i].g = v.g;
				  result[i].b = v.b;
				  result[i].a = UINT8_MAX;
				  ++i;
			  }
		  else
			  for (const auto &v : data)
			  {
				  result[i].r = uint8_t((uintmax_t(v.r) * UINT8_MAX) / data.max_value);
				  result[i].g = uint8_t((uintmax_t(v.g) * UINT8_MAX) / data.max_value);
				  result[i].b = uint8_t((uintmax_t(v.b) * UINT8_MAX) / data.max_value);
				  result[i].a = UINT8_MAX;
				  ++i;
			  }
		  return result;
	  },
	  [this](const pnm_data<::lak::vec4u8_t, uint8_t> &data) -> ::lak::image4_t
	  {
		  ::lak::image4_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  if (data.max_value == UINT8_MAX)
			  for (const auto &v : data) result[i++] = v;
		  else
			  for (const auto &v : data)
			  {
				  result[i].r = uint8_t((uintmax_t(v.r) * UINT8_MAX) / data.max_value);
				  result[i].g = uint8_t((uintmax_t(v.g) * UINT8_MAX) / data.max_value);
				  result[i].b = uint8_t((uintmax_t(v.b) * UINT8_MAX) / data.max_value);
				  result[i].a = uint8_t((uintmax_t(v.a) * UINT8_MAX) / data.max_value);
				  ++i;
			  }
		  return result;
	  },
	  [this](const pnm_data<::lak::vec3u16_t, uint16_t> &data) -> ::lak::image4_t
	  {
		  ::lak::image4_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  for (const auto &v : data)
		  {
			  result[i].r = uint8_t((uintmax_t(v.r) * UINT8_MAX) / data.max_value);
			  result[i].g = uint8_t((uintmax_t(v.g) * UINT8_MAX) / data.max_value);
			  result[i].b = uint8_t((uintmax_t(v.b) * UINT8_MAX) / data.max_value);
			  result[i].a = UINT8_MAX;
			  ++i;
		  }
		  return result;
	  },
	  [this](const pnm_data<::lak::vec4u16_t, uint16_t> &data) -> ::lak::image4_t
	  {
		  ::lak::image4_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  for (const auto &v : data)
		  {
			  result[i].r = uint8_t((uintmax_t(v.r) * UINT8_MAX) / data.max_value);
			  result[i].g = uint8_t((uintmax_t(v.g) * UINT8_MAX) / data.max_value);
			  result[i].b = uint8_t((uintmax_t(v.b) * UINT8_MAX) / data.max_value);
			  result[i].a = uint8_t((uintmax_t(v.a) * UINT8_MAX) / data.max_value);
			  ++i;
		  }
		  return result;
	  },
	  [this](const pnm_data<::lak::vec3f32_t, f32_t> &data) -> ::lak::image4_t
	  {
		  ::lak::image4_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  for (const auto &v : data)
		  {
			  result[i].r = uint8_t((v.r * UINT8_MAX) / data.max_value);
			  result[i].g = uint8_t((v.g * UINT8_MAX) / data.max_value);
			  result[i].b = uint8_t((v.b * UINT8_MAX) / data.max_value);
			  result[i].a = UINT8_MAX;
			  ++i;
		  }
		  return result;
	  },
	  [this](const pnm_data<::lak::vec4f32_t, f32_t> &data) -> ::lak::image4_t
	  {
		  ::lak::image4_t result;
		  result.resize({width, data.size() / width});
		  size_t i = 0U;
		  for (const auto &v : data)
		  {
			  result[i].r = uint8_t((v.r * UINT8_MAX) / data.max_value);
			  result[i].g = uint8_t((v.g * UINT8_MAX) / data.max_value);
			  result[i].b = uint8_t((v.b * UINT8_MAX) / data.max_value);
			  result[i].a = uint8_t((v.a * UINT8_MAX) / data.max_value);
			  ++i;
		  }
		  return result;
	  },
	});
}

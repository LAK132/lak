#ifndef LAK_COL_SRGB_HPP
#define LAK_COL_SRGB_HPP

#include "lak/col/cie.hpp"

namespace lak
{
	namespace col
	{
		constexpr lak::col::cie::xyY_primaries sRGB_primaries{
		  .r = {.x = 0.640f, .y = 0.330f, .Y = 0.2126f},
		  .g = {.x = 0.300f, .y = 0.600f, .Y = 0.7152f},
		  .b = {.x = 0.150f, .y = 0.060f, .Y = 0.0722f},
		  .w = {.x = 0.3127f, .y = 0.3290f},
		};

		struct sRGB
		{
			float R; // R'
			float G; // G'
			float B; // B'
		};

		struct sRGB_linear
		{
			float R;
			float G;
			float B;
		};

		constexpr lak::col::sRGB to_sRGB(lak::col::sRGB_linear colour)
		{
			auto from_linear = [](float value)
			{
				if (value <= 0.0031308f)
					return 12.92f * value;
				else
					return (1.055f * std::pow(value, 1.f / 2.4f)) - 0.055f;
			};
			return {
			  .R = from_linear(colour.R),
			  .G = from_linear(colour.G),
			  .B = from_linear(colour.B),
			};
		}

		constexpr lak::col::sRGB_linear to_sRGB_linear(lak::col::cie::XYZ colour)
		{
			// https://en.wikipedia.org/wiki/SRGB#Primaries
			const auto from_vec = lak::vec3f64_t(colour.X, colour.Y, colour.Z);
			const auto mat =
			  lak::mat3f64_t(lak::vec3f64_t(3.2406255, -1.5372080, -0.4986286),
			                 lak::vec3f64_t(-0.9689307, 1.8757561, 0.0415175),
			                 lak::vec3f64_t(0.0557101, -0.2040211, 1.0569959));
			const auto to_vec = mat * from_vec;
			return {
			  .R = float(to_vec.r),
			  .G = float(to_vec.g),
			  .B = float(to_vec.b),
			};
		}

		constexpr lak::col::sRGB_linear to_sRGB_linear(lak::col::sRGB colour)
		{
			auto to_linear = [](float value)
			{
				if (value <= 0.04045f)
					return value / 12.92f;
				else
					return std::pow((value + 0.055f) / 1.055f, 2.4f);
			};
			return {
			  .R = to_linear(colour.R),
			  .G = to_linear(colour.G),
			  .B = to_linear(colour.B),
			};
		}

		constexpr lak::col::cie::XYZ to_XYZ(lak::col::sRGB_linear colour)
		{
			// https://en.wikipedia.org/wiki/SRGB#Primaries
			const auto from_vec = lak::vec3f64_t(colour.R, colour.G, colour.B);
			const auto mat = lak::mat3f64_t(lak::vec3f64_t(0.4124, 0.3576, 0.1805),
			                                lak::vec3f64_t(0.2126, 0.7152, 0.0722),
			                                lak::vec3f64_t(0.0193, 0.1192, 0.9505));
			const auto to_vec = mat * from_vec;
			return {
			  .X = float(to_vec.r),
			  .Y = float(to_vec.g),
			  .Z = float(to_vec.b),
			};
		}

		// ---

		constexpr lak::col::cie::XYZ to_XYZ(lak::col::sRGB colour)
		{
			return to_XYZ(to_sRGB_linear(colour));
		}

		constexpr lak::col::sRGB to_sRGB(lak::col::cie::XYZ colour)
		{
			return to_sRGB(to_sRGB_linear(colour));
		}
	}
}

#endif

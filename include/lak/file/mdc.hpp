#ifndef MDC_RAW_MDC_HPP
#define MDC_RAW_MDC_HPP

#include "lak/array.hpp"
#include "lak/binary_traits.hpp"
#include "lak/image.hpp"
#include "lak/result.hpp"
#include "lak/span.hpp"
#include "lak/stdint.hpp"

// - sensors are 768 * 494 pixels, active area is 760 * 490.
//
// - 2 sensors are green (offset diagonally by half a pixel), 1 sensor is
//   striped red and blue. this particular combination allows the 3CCD system
//   to approximate a bayer CFA (if i remember my research correctly, this
//   came at the cost of an apparently expensive alignment process)
//
// - pixel aspect ratio is approximately 1.16 (29:25).
//
// - minolta's tool produces a final image size of 1528 * 1146, our's produces
//   a final image of 1900 * 1421 in order to maintain interger scaling of the
//   active area with the 1.16 aspect ratio (760,490 * 25,29 / 10 = 1900,1421).
//
// - the raw preamble contains "EXIF" information that to the best of my
//   knowledge is largely undocumented (dcraw/libraw only handles the image
//   data). with considerable experimentation (changing only a single
//   body/lens setting at a time!), i have managed to discover the meaning of
//   some of these values:
//
// --- autoness ---
//
//   0x04/0x02: no redeye/redeye
//   0x12/0x14: flash/flash redeye
//   0x04: M
//   0x02: A/S/P
//   0x0X: daylight/tungsten/flash/fluro/auto
//   0x1X: external flash
//
// --- focal_length ---
//
//   0x1E: 16mm
//   0x1F: 17mm (unk7: 0x00)
//   0x21:      (unk7: 0x01)
//   0x22:      (unk7: 0x1F)
//   0x23: 20mm (unk7: 0x27)
//   0x27: 24mm (unk7: 0x15)
//   0x2B: 28mm (unk7: 0x20, 0x63)
//   0x30: 35mm (unk7: 0x1C)
//   0x32: 35mm
//   0x34:
//   0x3A: 50mm
//   0x38: 50mm
//   0x39: 50mm
//   0x3C: 60mm
//   0x3E:
//   0x3F: 70mm (unk7: 0x77)
//   0x40: 70mm (unk7: 0x63)
//   0x41: 75mm (unk7: 0x7D)
//   0x48: 100mm
//   0x4F: 135mm
//   0x58: 200mm
//   0x61: 300mm
//   0x68: 400mm
//
// --- aperture ---
//
//   0x2C: f/4.5
//   0x30: f/5.6
//   0x34: f/6.7
//   0x38: f/8
//   0x3C: f/9.5
//   0x40: f/11
//   0x44: f/13
//   0x48: f/16
//   0x4C: f/19
//   0x50: f/22
//
// --- shutter_speed ---
//
//   0x38: 1/2s
//   0x3D: 1/3s
//   0x40: 1/4s
//   0x45: 1/6s
//   0x48: 1/8s
//   0x4D: 1/10s
//   0x50: 1/15s
//   0x55: 1/20s
//   0x58: 1/30s
//   0x5D?
//   0x60: 1/60s
//   0x64: 1/90s
//   0x68: 1/125s
//   0x6D?
//   0x70: 1/250s
//   0x74?
//   0x78: 1/500s
//   0x7D?
//   0x80: 1/1000s
//   0x85?: 1/1500s
//   0x88: 1/2000s
//
// --- exposure_compensation ---
//
//   0xE8: -3
//   0xEC: -2.5
//   0xF0: -2
//   0xF4: -1.5
//   0xF8: -1
//   0xFC: -0.5
//   0x00: 0
//   0x04: +0.5
//   0x08: +1
//   0x0C: +1.5
//   0x10: +2
//   0x14: +2.5
//   0x18: +3

namespace lak
{
	namespace mdc
	{
		struct mdc
		{
		public:
			// offset values discovered experimentally. 8u=1pixel
			lak::vec2i16_t red_offset    = {8, 0};  // 1.0, 0.0
			lak::vec2i16_t green1_offset = {8, 0};  // 1.0, 0.0
			lak::vec2i16_t green2_offset = {12, 4}; // 1.5, 0.5
			lak::vec2i16_t blue_offset   = {0, 0};  // 0.0, 0.0

			lak::array<uint8_t> red;
			lak::array<uint8_t> green1;
			lak::array<uint8_t> green2;
			lak::array<uint8_t> blue;

			// 0x200 bytes, most seems to be random/garbage
			lak::array<uint8_t> preamble;

			struct shot_settings
			{
				uint8_t autoness;
				uint8_t focal_length;
				uint8_t aperture;
				uint8_t shutter_speed;
				int8_t exposure_compensation; // int8 / 8.f

				uint16_t unk1; // 0x00
				uint16_t unk2; // 0x02
				uint16_t unk3; // 0x04
				uint16_t unk4; // 0x06
				uint8_t unk5;  // 0x09
				uint8_t unk6;  // 0x0A
				uint16_t unk7; // 0x0D something to do with focus?
				uint8_t unk8;  // 0x0F
				uint16_t unk9; // 0x12 affected below f/6.7?
			};

			shot_settings settings;

			operator lak::image3_t() const;

			template<lak::endian E>
			inline lak::error_code<lak::err::out_of_data> read(
			  lak::binary_reader &strm)
			{
				// ignore endianness
				return _read(strm);
			}

			lak::error_code<lak::err::out_of_data> _read(lak::binary_reader &strm);
		};
	}
}

LAK_FIXED_STRUCT_BYTES_TRAITS_EX(
  lak::endian::little,
  lak::mdc::mdc::shot_settings,
  &lak::mdc::mdc::shot_settings::unk1,
  &lak::mdc::mdc::shot_settings::unk2,
  &lak::mdc::mdc::shot_settings::unk3,
  &lak::mdc::mdc::shot_settings::unk4,
  &lak::mdc::mdc::shot_settings::autoness,
  &lak::mdc::mdc::shot_settings::unk5,
  &lak::mdc::mdc::shot_settings::unk6,
  &lak::mdc::mdc::shot_settings::focal_length,
  &lak::mdc::mdc::shot_settings::aperture,
  &lak::mdc::mdc::shot_settings::unk7,
  &lak::mdc::mdc::shot_settings::unk8,
  &lak::mdc::mdc::shot_settings::shutter_speed,
  &lak::mdc::mdc::shot_settings::exposure_compensation,
  &lak::mdc::mdc::shot_settings::unk9);

#endif

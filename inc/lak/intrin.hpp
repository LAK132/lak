#ifndef LAK_INTRIN_HPP
#define LAK_INTRIN_HPP

#include "lak/compiler.hpp"
#include "lak/stdint.hpp"

#if defined(LAK_COMPILER_EMSCRIPTEN)
#elif defined(LAK_COMPILER_GNUC) || defined(LAK_COMPILER_CLANG)
#	include <cpuid.h>
#	include <immintrin.h>
#elif defined(LAK_COMPILER_MSVC)
#	include <immintrin.h>
#else
#	error "Compiler not supported"
#endif

namespace lak
{
	void cpuid(unsigned int index, unsigned int info[4]);

	struct instruction_set
	{
		bool MMX;
		bool SSE;
		bool SSE2;
		bool SSE3;
		bool SSSE3;
		bool SSE41;
		bool SSE42;
		bool AVX;
		bool AVX2;
		bool AVX512F;
		bool AVX512PF;
		bool AVX512ER;
		bool AVX512CD;

		static instruction_set get();
	};

#if defined(LAK_COMPILER_EMSCRIPTEN)
#elif defined(LAK_COMPILER_MSVC)
	using m128_t  = __m128;
	using m256_t  = __m256;
	using m256d_t = __m256d;
	using m128i_t = __m128i;
	using m256i_t = __m256i;
#elif defined(LAK_COMPILER_GNUC)
	union alignas(__m128) m128_t
	{
		__m128 m128;

		f32_t m128_f32[4];

		m128_t() = default;
		m128_t(const __m128 &v) : m128(v) {}
		operator __m128() const { return m128; }
		operator __m128 &() { return m128; }
		operator const __m128 &() const { return m128; }
	};

	union alignas(__m256) m256_t
	{
		__m256 m256;

		f32_t m256_f32[8];

		m256_t() = default;
		m256_t(const __m256 &v) : m256(v) {}
		operator __m256() const { return m256; }
		operator __m256 &() { return m256; }
		operator const __m256 &() const { return m256; }
	};

	union alignas(__m256d) m256d_t
	{
		__m256d m256d;

		f64_t m256d_f64[4];

		m256d_t() = default;
		m256d_t(const __m256d &v) : m256d(v) {}
		operator __m256d() const { return m256d; }
		operator __m256d &() { return m256d; }
		operator const __m256d &() const { return m256d; }
	};

	union alignas(__m128i) m128i_t
	{
		__m128i m128i;

		int8_t m128i_i8[16];
		int16_t m128i_i16[8];
		int32_t m128i_i32[4];
		int64_t m128i_i64[2];
		uint8_t m128i_u8[16];
		uint16_t m128i_u16[8];
		uint32_t m128i_u32[4];
		uint64_t m128i_u64[2];

		m128i_t() = default;
		m128i_t(const __m128i &v) : m128i(v) {}
		operator __m128i() const { return m128i; }
		operator __m128i &() { return m128i; }
		operator const __m128i &() const { return m128i; }
	};

	union alignas(__m256i) m256i_t
	{
		__m256i m256i;

		int8_t m256i_i8[32];
		int16_t m256i_i16[16];
		int32_t m256i_i32[8];
		int64_t m256i_i64[4];
		uint8_t m256i_u8[32];
		uint16_t m256i_u16[16];
		uint32_t m256i_u32[8];
		uint64_t m256i_u64[4];

		m256i_t() = default;
		m256i_t(const __m256i &v) : m256i(v) {}
		operator __m256i() const { return m256i; }
		operator __m256i &() { return m256i; }
		operator const __m256i &() const { return m256i; }
	};
#else
#	error "Compiler not supported"
#endif
}

#endif

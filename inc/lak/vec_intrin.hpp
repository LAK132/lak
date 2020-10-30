#ifndef LAK_VEC_INTRIN_HPP
#define LAK_VEC_INTRIN_HPP

#include "lak/compiler.hpp"
#include "lak/intrin.hpp"
#include "lak/vec.hpp"

namespace lak
{
  /* --- f32 --- */

  force_inline m128_t to_wide(const lak::vec3f32_t &vec)
  {
    return {_mm_setr_ps(vec.x, vec.y, vec.z, 1.0)};
  }

  force_inline lak::vec3f32_t to_vec3(m128_t m128)
  {
    return {m128.m128_f32[0], m128.m128_f32[1], m128.m128_f32[2]};
  }

  force_inline m128_t to_wide(const lak::vec4f32_t &vec)
  {
    return {_mm_setr_ps(vec.x, vec.y, vec.z, vec.w)};
  }

  force_inline lak::vec4f32_t to_vec4(m128_t m128)
  {
    return {
      m128.m128_f32[0], m128.m128_f32[1], m128.m128_f32[2], m128.m128_f32[3]};
  }

  /* --- f64 --- */

  force_inline m256d_t to_wide(const lak::vec3f64_t &vec)
  {
    return {_mm256_setr_pd(vec.x, vec.y, vec.z, 1.0)};
  }

  force_inline lak::vec3f64_t to_vec3(m256d_t m256d)
  {
    return {m256d.m256d_f64[0], m256d.m256d_f64[1], m256d.m256d_f64[2]};
  }

  force_inline m256d_t to_wide(const lak::vec4f64_t &vec)
  {
    return {_mm256_setr_pd(vec.x, vec.y, vec.z, vec.w)};
  }

  force_inline lak::vec4f64_t to_vec4(m256d_t m256d)
  {
    return {m256d.m256d_f64[0],
            m256d.m256d_f64[1],
            m256d.m256d_f64[2],
            m256d.m256d_f64[3]};
  }

  /* --- int32_t --- */

  force_inline m128i_t to_wide(const lak::vec3i32_t &vec)
  {
    return {_mm_setr_epi32(vec.x, vec.y, vec.z, 0)};
  }

  force_inline lak::vec3i32_t to_vec3(m128i_t m128i)
  {
    return {m128i.m128i_i32[0], m128i.m128i_i32[1], m128i.m128i_i32[2]};
  }

  force_inline m128i_t to_wide(const lak::vec4i32_t &vec)
  {
    return {_mm_setr_epi32(vec.x, vec.y, vec.z, vec.w)};
  }

  force_inline lak::vec4i32_t to_vec4(m128i_t m128i)
  {
    return {m128i.m128i_i32[0],
            m128i.m128i_i32[1],
            m128i.m128i_i32[2],
            m128i.m128i_i32[3]};
  }

  /* --- int64_t --- */

  force_inline m256i_t to_wide(const lak::vec3i64_t &vec)
  {
    return {_mm256_setr_epi64x(vec.x, vec.y, vec.z, 0)};
  }

  force_inline lak::vec3i64_t to_vec3(m256i_t m256i)
  {
    return {m256i.m256i_i64[0], m256i.m256i_i64[1], m256i.m256i_i64[2]};
  }

  force_inline m256i_t to_wide(const lak::vec4i64_t &vec)
  {
    return {_mm256_setr_epi64x(vec.x, vec.y, vec.z, vec.w)};
  }

  force_inline lak::vec4i64_t to_vec4(m256i_t m256i)
  {
    return {m256i.m256i_i64[0],
            m256i.m256i_i64[1],
            m256i.m256i_i64[2],
            m256i.m256i_i64[3]};
  }
}

#endif
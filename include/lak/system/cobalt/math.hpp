#ifndef LAK_SYSTEM_COBALT_MATH_HPP
#define LAK_SYSTEM_COBALT_MATH_HPP

#include <RendererInterface/RendererInterface.pkg>

#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace lak
{
	namespace cobalt
	{
		inline ::cobalt::graphics::M4Float32 from_glm(glm::mat4x4 m)
		{
			::cobalt::graphics::M4Float32 result;
			for (size_t y = 0; y < 4; ++y)
				for (size_t x = 0; x < 4; ++x) result.data()[x + (y * 4)] = m[x][y];
			return result;
		}

		inline ::cobalt::graphics::M3Float32 from_glm(glm::mat3x3 m)
		{
			::cobalt::graphics::M3Float32 result;
			for (size_t y = 0; y < 3; ++y)
				for (size_t x = 0; x < 3; ++x) result.data()[x + (y * 3)] = m[x][y];
			return result;
		}

		inline ::cobalt::graphics::M2Float32 from_glm(glm::mat2x2 m)
		{
			::cobalt::graphics::M2Float32 result;
			for (size_t y = 0; y < 2; ++y)
				for (size_t x = 0; x < 2; ++x) result.data()[x + (y * 2)] = m[x][y];
			return result;
		}

		inline ::cobalt::graphics::V4Float32 from_glm(glm::vec4 v)
		{
			::cobalt::graphics::V4Float32 result;
			for (size_t x = 0; x < 4; ++x) result.data()[x] = v[x];
			return result;
		}

		inline ::cobalt::graphics::V3Float32 from_glm(glm::vec3 v)
		{
			::cobalt::graphics::V3Float32 result;
			for (size_t x = 0; x < 3; ++x) result.data()[x] = v[x];
			return result;
		}

		inline ::cobalt::graphics::V2Float32 from_glm(glm::vec2 v)
		{
			::cobalt::graphics::V2Float32 result;
			for (size_t x = 0; x < 2; ++x) result.data()[x] = v[x];
			return result;
		}
		inline glm::mat4x4 to_glm(::cobalt::graphics::M4Float32 m)
		{
			glm::mat4x4 result;
			for (size_t y = 0; y < 4; ++y)
				for (size_t x = 0; x < 4; ++x) result[x][y] = m.data()[x + (y * 4)];
			return result;
		}

		inline glm::mat3x3 to_glm(::cobalt::graphics::M3Float32 m)
		{
			glm::mat3x3 result;
			for (size_t y = 0; y < 3; ++y)
				for (size_t x = 0; x < 3; ++x) result[x][y] = m.data()[x + (y * 3)];
			return result;
		}

		inline glm::mat2x2 to_glm(::cobalt::graphics::M2Float32 m)
		{
			glm::mat2x2 result;
			for (size_t y = 0; y < 2; ++y)
				for (size_t x = 0; x < 2; ++x) result[x][y] = m.data()[x + (y * 2)];
			return result;
		}

		inline glm::vec4 to_glm(::cobalt::graphics::V4Float32 v)
		{
			glm::vec4 result;
			for (size_t x = 0; x < 4; ++x) result[x] = v.data()[x];
			return result;
		}

		inline glm::vec3 to_glm(::cobalt::graphics::V3Float32 v)
		{
			glm::vec3 result;
			for (size_t x = 0; x < 3; ++x) result[x] = v.data()[x];
			return result;
		}

		inline glm::vec2 to_glm(::cobalt::graphics::V2Float32 v)
		{
			glm::vec2 result;
			for (size_t x = 0; x < 2; ++x) result[x] = v.data()[x];
			return result;
		}
	}
}

#endif

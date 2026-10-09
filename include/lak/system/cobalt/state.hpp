#ifndef LAK_COBALT_STATE_HPP
#define LAK_COBALT_STATE_HPP

#include "lak/com_ptr.hpp"
#include "lak/const_string.hpp"
#include "lak/string_literals/string.hpp"
#include "lak/string_view.hpp"
#include "lak/tuple.hpp"
#include "lak/type_pack.hpp"

#include "lak/system/cobalt/math.hpp"
#include "lak/system/cobalt/token.hpp"

#include <Cobalt/RendererInterface/RendererInterface.pkg>

namespace lak
{
	namespace cobalt
	{
		template<typename STATE_VALUES    = lak::type_pack<>,
		         typename TEXTURES        = lak::type_pack<>,
		         typename STATE_BUFFERS   = lak::type_pack<>,
		         typename RESOURCE_ARRAYS = lak::type_pack<>>
		struct program_state;

		template<typename... STATE_VALUES,
		         typename... TEXTURES,
		         typename... STATE_BUFFERS,
		         typename... RESOURCE_ARRAYS>
		struct program_state<lak::type_pack<STATE_VALUES...>,
		                     lak::type_pack<TEXTURES...>,
		                     lak::type_pack<STATE_BUFFERS...>,
		                     lak::type_pack<RESOURCE_ARRAYS...>>
		{
			static_assert(((lak::cobalt::is_state_value_binding_v<STATE_VALUES>) &&
			               ...));
			static_assert(((lak::cobalt::is_texture_binding_v<TEXTURES>) && ...));
			static_assert(((lak::cobalt::is_state_buffer_binding_v<STATE_BUFFERS>) &&
			               ...));
			static_assert(
			  ((lak::cobalt::is_resource_array_binding_v<RESOURCE_ARRAYS>) && ...));

			::cobalt::graphics::IShaderProgram *shader_program        = nullptr;
			::cobalt::graphics::IProgramNode::unique_ptr program_node = {};
			::cobalt::graphics::IStateGroupNode::unique_ptr state_group_node = {};
			lak::tuple<STATE_VALUES...> state_values;
			lak::tuple<TEXTURES...> textures;
			lak::tuple<STATE_BUFFERS...> state_buffers;
			lak::tuple<RESOURCE_ARRAYS...> resource_arrays;

			program_state() = default;
			program_state(program_state &&other)
			: shader_program(lak::exchange(other.shader_program, nullptr)),
			  program_node(lak::exchange(other.program_node, {})),
			  state_group_node(lak::exchange(other.state_group_node, {})),
			  state_values(lak::exchange(other.state_values, {})),
			  textures(lak::exchange(other.textures, {})),
			  state_buffers(lak::exchange(other.state_buffers, {})),
			  resource_arrays(lak::exchange(other.resource_arrays, {}))
			{
			}
			program_state &operator=(program_state &&other)
			{
				lak::swap(shader_program, other.shader_program);
				lak::swap(program_node, other.program_node);
				lak::swap(state_group_node, other.state_group_node);
				lak::swap(state_values, other.state_values);
				lak::swap(textures, other.textures);
				lak::swap(state_buffers, other.state_buffers);
				lak::swap(resource_arrays, other.resource_arrays);
				return *this;
			}

			static lak::result<program_state, lak::u8string> make(
			  ::cobalt::graphics::IRenderer *renderer,
			  ::cobalt::graphics::IShaderProgram *program)
			{
				program_state result;

				result.shader_program = program;
				if (!result.shader_program)
					return lak::err_t{u8"Invalid shader program"_str};

				result.program_node = renderer->CreateProgramNode();
				if (!result.program_node)
					return lak::err_t{u8"Failed to create program node"_str};

				result.state_group_node = renderer->CreateStateGroupNode();
				if (!result.state_group_node)
					return lak::err_t{u8"Failed to create state group node"_str};

				if (!result.program_node->BindShaderProgram(result.shader_program))
					return lak::err_t{u8"Failed to bind shader program"_str};

				result.program_node->AddChildNode(result.state_group_node.get());

				result.bind();

				return lak::move_ok(result);
			}

			bool bind()
			{
				ASSERT(shader_program);
				ASSERT(program_node);
				ASSERT(state_group_node);

				bool all_bound = true;

				state_values.foreach (
				  [&](auto &state_value)
				  { all_bound &= state_value.bind(shader_program); });

				textures.foreach ([&](auto &texture)
				                  { all_bound &= texture.bind(shader_program); });

				state_buffers.foreach (
				  [&](auto &state_buffer)
				  { all_bound &= state_buffer.bind(shader_program); });

				resource_arrays.foreach (
				  [&](auto &resource_array)
				  { all_bound &= resource_array.bind(shader_program); });

				return all_bound;
			}

			/* --- state values --- */

			template<lak::aconst_string NAME>
			static consteval size_t index_of_state_value()
			{
				size_t index = sizeof...(STATE_VALUES);
				decltype(state_values)::foreach_type(
				  [&]<size_t I, typename T>(lak::size_type<I>, lak::type_identity<T>)
				  {
					  if (T::name == NAME) index = I;
				  });
				return index;
			}
			template<lak::aconst_string NAME>
			using type_of_state_value = typename decltype(state_values)::type_of<
			  index_of_state_value<NAME>()>::value_type;
			template<lak::aconst_string NAME>
			const auto &state_value() const
			{
				constexpr size_t index = index_of_state_value<NAME>();
				static_assert(index < sizeof...(STATE_VALUES));
				return state_values.template get<index>();
			}
			template<lak::aconst_string NAME>
			auto &state_value()
			{
				constexpr size_t index = index_of_state_value<NAME>();
				static_assert(index < sizeof...(STATE_VALUES));
				return state_values.template get<index>();
			}
			template<lak::aconst_string NAME>
			bool unset_state_value() const
			{
				return this->template state_value<NAME>().unset(
				  state_group_node.get());
			}
			template<lak::aconst_string NAME>
			bool set_state_value(const type_of_state_value<NAME> &value) const
			{
				return this->template state_value<NAME>().set(state_group_node.get(),
				                                              value);
			}

			/* --- textures --- */

			template<lak::aconst_string NAME>
			static consteval size_t index_of_texture()
			{
				size_t index = sizeof...(TEXTURES);
				decltype(textures)::foreach_type(
				  [&]<size_t I, typename T>(lak::size_type<I>, lak::type_identity<T>)
				  {
					  if (T::name == NAME) index = I;
				  });
				return index;
			}
			template<lak::aconst_string NAME>
			using type_of_texture_buffer = typename decltype(textures)::type_of<
			  index_of_texture<NAME>()>::buffer_type;
			template<lak::aconst_string NAME>
			using type_of_texture_sampler = typename decltype(textures)::type_of<
			  index_of_texture<NAME>()>::sampler_type;
			template<lak::aconst_string NAME>
			const auto &texture() const
			{
				constexpr size_t index = index_of_texture<NAME>();
				static_assert(index < sizeof...(TEXTURES));
				return textures.template get<index>();
			}
			template<lak::aconst_string NAME>
			auto &texture()
			{
				constexpr size_t index = index_of_texture<NAME>();
				static_assert(index < sizeof...(TEXTURES));
				return textures.template get<index>();
			}
			template<lak::aconst_string NAME>
			bool unset_texture() const
			{
				return this->template texture<NAME>().unset(state_group_node.get());
			}
			template<lak::aconst_string NAME>
			bool set_texture(type_of_texture_buffer<NAME> *buffer) const
			{
				return this->template texture<NAME>().set(state_group_node.get(),
				                                          buffer);
			}
			template<lak::aconst_string NAME>
			bool set_texture(type_of_texture_buffer<NAME> *buffer,
			                 type_of_texture_sampler<NAME> *sampler) const
			{
				return this->template texture<NAME>().set(
				  state_group_node.get(), buffer, sampler);
			}

			/* --- state buffers --- */

			template<lak::aconst_string NAME>
			static consteval size_t index_of_state_buffer()
			{
				size_t index = sizeof...(STATE_BUFFERS);
				decltype(state_buffers)::foreach_type(
				  [&]<size_t I, typename T>(lak::size_type<I>, lak::type_identity<T>)
				  {
					  if (T::name == NAME) index = I;
				  });
				return index;
			}
			template<lak::aconst_string NAME>
			const auto &state_buffer() const
			{
				constexpr size_t index = index_of_state_buffer<NAME>();
				static_assert(index < sizeof...(STATE_BUFFERS));
				return state_buffers.template get<index>();
			}
			template<lak::aconst_string NAME>
			auto &state_buffer()
			{
				constexpr size_t index = index_of_state_buffer<NAME>();
				static_assert(index < sizeof...(STATE_BUFFERS));
				return state_buffers.template get<index>();
			}

			/* --- resource arrays --- */

			template<lak::aconst_string NAME>
			static consteval size_t index_of_resource_array()
			{
				size_t index = sizeof...(RESOURCE_ARRAYS);
				decltype(resource_arrays)::foreach_type(
				  [&]<size_t I, typename T>(lak::size_type<I>, lak::type_identity<T>)
				  {
					  if (T::name == NAME) index = I;
				  });
				return index;
			}
			template<lak::aconst_string NAME>
			const auto &resource_array() const
			{
				constexpr size_t index = index_of_resource_array<NAME>();
				static_assert(index < sizeof...(STATE_BUFFERS));
				return resource_arrays.template get<index>();
			}
			template<lak::aconst_string NAME>
			auto &resource_array()
			{
				constexpr size_t index = index_of_resource_array<NAME>();
				static_assert(index < sizeof...(STATE_BUFFERS));
				return resource_arrays.template get<index>();
			}
		};
	}
}

#endif

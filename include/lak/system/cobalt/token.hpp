#ifndef LAK_COBALT_TOKEN_HPP
#define LAK_COBALT_TOKEN_HPP

#include "lak/com_ptr.hpp"
#include "lak/type_traits.hpp"

#include <Cobalt/RendererInterface/RendererInterface.pkg>

namespace lak
{
	namespace cobalt
	{
		template<typename T>
		struct token_com_ptr_traits
		{
			using handle_type                = T;
			using exposed_type               = T;
			static constexpr auto null_value = T::Null;
			inline static lak::infallible_result<handle_type> ctor(handle_type id)
			{
				return lak::ok_t{id};
			}
			inline static void dtor(handle_type &handle) { handle = null_value; }
			inline static bool valid(const handle_type &handle)
			{
				return handle != null_value;
			}
		};
	}
}

template<>
struct lak::unique_com_ptr_traits<::cobalt::graphics::VertexAttributeId>
: public lak::cobalt::token_com_ptr_traits<
    ::cobalt::graphics::VertexAttributeId>
{
};
template<>
struct lak::unique_com_ptr_traits<::cobalt::graphics::TextureId>
: public lak::cobalt::token_com_ptr_traits<::cobalt::graphics::TextureId>
{
};
template<>
struct lak::unique_com_ptr_traits<::cobalt::graphics::SamplerId>
: public lak::cobalt::token_com_ptr_traits<::cobalt::graphics::SamplerId>
{
};
template<>
struct lak::unique_com_ptr_traits<::cobalt::graphics::StateValueId>
: public lak::cobalt::token_com_ptr_traits<::cobalt::graphics::StateValueId>
{
};
template<>
struct lak::unique_com_ptr_traits<::cobalt::graphics::StateBufferId>
: public lak::cobalt::token_com_ptr_traits<::cobalt::graphics::StateBufferId>
{
};
template<>
struct lak::unique_com_ptr_traits<::cobalt::graphics::ResourceArrayId>
: public lak::cobalt::token_com_ptr_traits<::cobalt::graphics::ResourceArrayId>
{
};

namespace lak
{
	namespace cobalt
	{
		using vertex_attribute_id =
		  lak::unique_com_ptr<::cobalt::graphics::VertexAttributeId>;
		using texture_id = lak::unique_com_ptr<::cobalt::graphics::TextureId>;
		using sampler_id = lak::unique_com_ptr<::cobalt::graphics::SamplerId>;
		using state_value_id =
		  lak::unique_com_ptr<::cobalt::graphics::StateValueId>;
		using state_buffer_id =
		  lak::unique_com_ptr<::cobalt::graphics::StateBufferId>;
		using resource_array_id =
		  lak::unique_com_ptr<::cobalt::graphics::ResourceArrayId>;

		/* --- texture --- */

		template<typename T>
		struct is_texture_buffer
		: lak::bool_type<
		    lak::concepts::one_of<T,
		                          ::cobalt::graphics::ITextureBuffer1D,
		                          ::cobalt::graphics::ITextureBuffer2D,
		                          ::cobalt::graphics::ITextureBuffer3D,
		                          ::cobalt::graphics::ITextureBufferCube,
		                          ::cobalt::graphics::ITextureBuffer1DArray,
		                          ::cobalt::graphics::ITextureBuffer2DArray,
		                          ::cobalt::graphics::ITextureBufferCubeArray>>
		{
		};
		template<typename T>
		inline constexpr bool is_texture_buffer_v =
		  lak::cobalt::is_texture_buffer<T>::value;

		template<typename T>
		struct is_texture_sampler
		: lak::bool_type<
		    lak::concepts::one_of<T,
		                          ::cobalt::graphics::ITextureSampler1D,
		                          ::cobalt::graphics::ITextureSampler2D,
		                          ::cobalt::graphics::ITextureSampler3D,
		                          ::cobalt::graphics::ITextureSamplerCube,
		                          ::cobalt::graphics::ITextureSampler1DArray,
		                          ::cobalt::graphics::ITextureSampler2DArray,
		                          ::cobalt::graphics::ITextureSamplerCubeArray>>
		{
		};
		template<typename T>
		inline constexpr bool is_texture_sampler_v =
		  lak::cobalt::is_texture_sampler<T>::value;

		template<typename T>
		struct sampler_for_buffer;
		template<>
		struct sampler_for_buffer<::cobalt::graphics::ITextureBuffer1D>
		: lak::type_identity<::cobalt::graphics::ITextureSampler1D>
		{
		};
		template<>
		struct sampler_for_buffer<::cobalt::graphics::ITextureBuffer2D>
		: lak::type_identity<::cobalt::graphics::ITextureSampler2D>
		{
		};
		template<>
		struct sampler_for_buffer<::cobalt::graphics::ITextureBufferCube>
		: lak::type_identity<::cobalt::graphics::ITextureSamplerCube>
		{
		};
		template<>
		struct sampler_for_buffer<::cobalt::graphics::ITextureBuffer1DArray>
		: lak::type_identity<::cobalt::graphics::ITextureSampler1DArray>
		{
		};
		template<>
		struct sampler_for_buffer<::cobalt::graphics::ITextureBuffer2DArray>
		: lak::type_identity<::cobalt::graphics::ITextureSampler2DArray>
		{
		};
		template<>
		struct sampler_for_buffer<::cobalt::graphics::ITextureBufferCubeArray>
		: lak::type_identity<::cobalt::graphics::ITextureSamplerCubeArray>
		{
		};
		template<typename T>
		using sampler_for_buffer_t =
		  typename lak::cobalt::sampler_for_buffer<T>::type;

		template<typename BUFFER, lak::aconst_string NAME>
		struct texture_binding
		{
			using buffer_type  = BUFFER;
			using sampler_type = lak::cobalt::sampler_for_buffer_t<BUFFER>;
			static_assert(lak::cobalt::is_texture_buffer_v<buffer_type> ||
			              lak::cobalt::is_texture_sampler_v<sampler_type>);

			static constexpr auto name = NAME;
			lak::cobalt::texture_id id;

			texture_binding() = default;
			texture_binding(texture_binding &&other)
			: id(lak::exchange(other.id, lak::cobalt::texture_id{}))
			{
			}
			texture_binding &operator=(texture_binding &&other)
			{
				lak::swap(id, other.id);
				return *this;
			}

			explicit operator bool() const { return (bool)id; }

			bool bind(::cobalt::graphics::IShaderProgram *program)
			{
				BOUNDS_ASSERT(program != nullptr);
				id.emplace(program->GetTextureId((std::string)name));
				return (bool)id;
			}

			auto get() const { return id.get(); }

			bool unset(::cobalt::graphics::IStateGroupNode *state_group) const
			{
				if (!id) return false;
				state_group->UnbindTexture(id.get());
				return true;
			}

			bool set(::cobalt::graphics::IStateGroupNode *state_group,
			         buffer_type *buffer) const
			{
				if (!id) return false;
				state_group->BindTexture(id.get(), buffer);
				return true;
			}
			bool set(::cobalt::graphics::IStateGroupNode *state_group,
			         buffer_type *buffer,
			         sampler_type *sampler) const
			{
				if (!id) return false;
				state_group->BindTextureWithCombinedSampler(id.get(), buffer, sampler);
				return true;
			}
		};
		template<typename T>
		struct is_texture_binding : lak::false_type
		{
		};
		template<typename BUFFER, auto NAME>
		struct is_texture_binding<lak::cobalt::texture_binding<BUFFER, NAME>>
		: lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_texture_binding_v =
		  lak::cobalt::is_texture_binding<T>::value;

		/* --- state value --- */

		template<typename TYPE, lak::aconst_string NAME>
		struct state_value_binding
		{
			using value_type = TYPE;
			static_assert(lak::cobalt::is_vector_v<value_type> ||
			              lak::cobalt::is_matrix_v<value_type>);

			static constexpr auto name = NAME;
			lak::cobalt::state_value_id id;

			state_value_binding() = default;
			state_value_binding(state_value_binding &&other)
			: id(lak::exchange(other.id, lak::cobalt::state_value_id{}))
			{
			}
			state_value_binding &operator=(state_value_binding &&other)
			{
				lak::swap(id, other.id);
				return *this;
			}

			explicit operator bool() const { return (bool)id; }

			bool bind(::cobalt::graphics::IShaderProgram *program)
			{
				BOUNDS_ASSERT(program != nullptr);
				id.emplace(program->GetStateValueId((std::string)name));
				return (bool)id;
			}

			auto get() const { return id.get(); }

			bool unset(::cobalt::graphics::IStateGroupNode *state_group) const
			{
				if (!id) return false;
				state_group->ResetStateValue(id.get());
				return true;
			}
			bool set(::cobalt::graphics::IStateGroupNode *state_group,
			         const value_type &value) const
			{
				if (!id) return false;
				state_group->SetStateValue(id.get(), value);
				return true;
			}
		};
		template<typename T>
		struct is_state_value_binding : lak::false_type
		{
		};
		template<typename TYPE, auto NAME>
		struct is_state_value_binding<lak::cobalt::state_value_binding<TYPE, NAME>>
		: lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_state_value_binding_v =
		  lak::cobalt::is_state_value_binding<T>::value;

		/* --- state buffer --- */

		template<lak::aconst_string NAME>
		struct state_buffer_binding
		{
			static constexpr auto name = NAME;
			lak::cobalt::state_buffer_id id;

			state_buffer_binding() = default;
			state_buffer_binding(state_buffer_binding &&other)
			: id(lak::exchange(other.id, lak::cobalt::state_buffer_id{}))
			{
			}
			state_buffer_binding &operator=(state_buffer_binding &&other)
			{
				lak::swap(id, other.id);
				return *this;
			}

			explicit operator bool() const { return (bool)id; }

			bool bind(::cobalt::graphics::IShaderProgram *program)
			{
				BOUNDS_ASSERT(program != nullptr);
				id.emplace(program->GetStateBufferId((std::string)name));
				return (bool)id;
			}

			auto get() const { return id.get(); }
		};
		template<typename T>
		struct is_state_buffer_binding : lak::false_type
		{
		};
		template<auto NAME>
		struct is_state_buffer_binding<lak::cobalt::state_buffer_binding<NAME>>
		: lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_state_buffer_binding_v =
		  lak::cobalt::is_state_buffer_binding<T>::value;

		/* --- resource array --- */

		template<lak::aconst_string NAME>
		struct resource_array_binding
		{
			static constexpr auto name = NAME;
			lak::cobalt::resource_array_id id;

			resource_array_binding() = default;
			resource_array_binding(resource_array_binding &&other)
			: id(lak::exchange(other.id, lak::cobalt::resource_array_id{}))
			{
			}
			resource_array_binding &operator=(resource_array_binding &&other)
			{
				lak::swap(id, other.id);
				return *this;
			}

			explicit operator bool() const { return (bool)id; }

			bool bind(::cobalt::graphics::IShaderProgram *program)
			{
				BOUNDS_ASSERT(program != nullptr);
				id.emplace(program->GetResourceArrayId((std::string)name));
				return (bool)id;
			}

			auto get() const { return id.get(); }
		};
		template<typename T>
		struct is_resource_array_binding : lak::false_type
		{
		};
		template<auto NAME>
		struct is_resource_array_binding<lak::cobalt::resource_array_binding<NAME>>
		: lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_resource_array_binding_v =
		  lak::cobalt::is_resource_array_binding<T>::value;
	}
}

#endif

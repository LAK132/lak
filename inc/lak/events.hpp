#ifndef LAK_EVENTS_HPP
#define LAK_EVENTS_HPP

#include "lak/platform.hpp"

#include "lak/buffer.hpp"
#include "lak/debug.hpp"
#include "lak/memory.hpp"
#include "lak/string.hpp"
#include "lak/vec.hpp"

#include <variant>

namespace lak
{
	struct window;
	struct window_handle;

	enum struct event_type
	{
		platform_event = 0,

		quit_program,

		close_window,
		window_closed,
		window_changed,
		window_exposed,

		dropfile,

		key_down,
		key_up,

		character,

		button_down,
		button_up,

		motion,

		wheel
	};

	struct window_event
	{
		lak::vec2l_t position;
		lak::vec2l_t size;
	};

	struct dropfile_event
	{
		lak::vec2l_t position;
		std::u8string path;
	};

	enum struct mod_key : uint32_t
	{
		none  = 0,
		shift = 1 << 0,
		ctrl  = 1 << 1,
		alt   = 1 << 2,
		super = 1 << 3
	};

	static inline lak::mod_key operator|(lak::mod_key m1, lak::mod_key m2)
	{
		return static_cast<lak::mod_key>(static_cast<uint32_t>(m1) |
		                                 static_cast<uint32_t>(m2));
	}

	static inline lak::mod_key operator&(lak::mod_key m1, lak::mod_key m2)
	{
		return static_cast<lak::mod_key>(static_cast<uint32_t>(m1) &
		                                 static_cast<uint32_t>(m2));
	}

	struct key_event
	{
		uint32_t scancode;
		lak::mod_key mod;
	};

	struct character_event
	{
		char32_t code;
	};

	enum struct mouse_button : uint32_t
	{
		none   = 0,
		left   = 1 << 0,
		middle = 1 << 1,
		right  = 1 << 2,
		x1     = 1 << 3,
		x2     = 1 << 4
	};

	static inline lak::mouse_button operator|(lak::mouse_button m1,
	                                          lak::mouse_button m2)
	{
		return static_cast<lak::mouse_button>(static_cast<uint32_t>(m1) |
		                                      static_cast<uint32_t>(m2));
	}

	struct button_event
	{
		lak::mouse_button button;
	};

	struct motion_event
	{
		lak::vec2l_t position;
	};

	struct wheel_event
	{
		lak::vec2f_t wheel;
	};

	struct platform_event;
	using platform_event_ptr = lak::unique_ptr<lak::platform_event>;
	extern template struct lak::unique_ptr<lak::platform_event>;

	struct event
	{
		using state_t = lak::variant<lak::monostate,
		                             lak::window_event,
		                             lak::dropfile_event,
		                             lak::key_event,
		                             lak::character_event,
		                             lak::button_event,
		                             lak::motion_event,
		                             lak::wheel_event>;

		lak::event_type type             = lak::event_type::platform_event;
		const lak::window_handle *handle = nullptr;
		mutable lak::platform_event_ptr _platform_event;
		state_t _state;

		const lak::window_event &window() const
		{
			return *_state.template get<state_t::index_of<lak::window_event>>();
		}

		const lak::dropfile_event &dropfile() const
		{
			return *_state.template get<state_t::index_of<lak::dropfile_event>>();
		}

		const lak::key_event &key() const
		{
			return *_state.template get<state_t::index_of<lak::key_event>>();
		}

		const lak::character_event &character() const
		{
			return *_state.template get<state_t::index_of<lak::character_event>>();
		}

		const lak::motion_event &motion() const
		{
			return *_state.template get<state_t::index_of<lak::motion_event>>();
		}

		const lak::button_event &button() const
		{
			return *_state.template get<state_t::index_of<lak::button_event>>();
		}

		const lak::wheel_event &wheel() const
		{
			return *_state.template get<state_t::index_of<lak::wheel_event>>();
		}

		// implementations should initialise _platform_event if it is not already
		const lak::platform_event &platform() const;
		lak::platform_event &platform();

		event(lak::event_type t, lak::platform_event_ptr &&e, const state_t &s);

		event(lak::event_type t, lak::platform_event_ptr &&e);

		event(lak::event_type t,
		      const lak::window_handle *w,
		      lak::platform_event_ptr &&e,
		      const state_t &s);

		event(lak::event_type t,
		      const lak::window_handle *w,
		      lak::platform_event_ptr &&e);

		event()         = default;
		event(event &&) = default;
		event &operator=(event &&) = default;
		~event();
	};

	static_assert(std::is_default_constructible_v<lak::event>,
	              "lak::event must be default constructible");

	bool next_event(lak::event *e);
	bool peek_event(lak::event *e);
}

#endif

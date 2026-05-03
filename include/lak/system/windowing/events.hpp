#ifndef LAK_SYSTEM_WINDOWING_EVENTS_HPP
#define LAK_SYSTEM_WINDOWING_EVENTS_HPP

#include "lak/system/windowing/platform.hpp"

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
		window_hover,
		window_leave,
		window_focus,
		window_no_focus,

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

	enum struct key_code : uint32_t
	{
		none = 0,

		lshift,
		rshift,
		lctrl,
		rctrl,
		lalt,
		ralt,
		lsuper,
		rsuper,

		left,
		right,
		up,
		down,

		del,
		backspace,
		tab,
		home,
		end,
		insert,
		page_up,
		page_down,
		enter,
		escape,

		caps_lock,
		scroll_lock,
		num_lock,

		space,
		apostrophe,
		comma,
		minus,
		plus,
		equal,
		period,
		semicolon,
		slash,
		backslash,
		oem102, // alt backslash
		backtick,
		open_bracket,
		close_bracket,
		// clang-format off
		num_0, num_1, num_2, num_3, num_4, num_5, num_6, num_7, num_8, num_9,
		// clang-format on
		// clang-format off
		let_A, let_B, let_C, let_D, let_E, let_F, let_G, let_H, let_I, let_J,
		let_K, let_L, let_M, let_N, let_O, let_P, let_Q, let_R, let_S, let_T,
		let_U, let_V, let_W, let_X, let_Y, let_Z,
		// clang-format on

		keypad_decimal,
		keypad_divide,
		keypad_multiply,
		keypad_subtract,
		keypad_add,
		keypad_equal,
		keypad_enter,
		// clang-format off
		keypad_0, keypad_1, keypad_2, keypad_3, keypad_4,
		keypad_5, keypad_6, keypad_7, keypad_8, keypad_9,
		// clang-format on

		print_screen,
		pause,
		menu,
		forward,
		back,
		// clang-format off
		f1,  f2,  f3,  f4,  f5,  f6,  f7,  f8,  f9,  f10,
		f11, f12, f13, f14, f15, f16, f17, f18, f19, f20,
		f21, f22, f23, f24,
		// clang-format on
	};

	enum struct mod_key : uint32_t
	{
		none = 0U,

		lshift      = 1U << 0U,
		lctrl       = 1U << 1U,
		lalt        = 1U << 2U,
		lsuper      = 1U << 3U,
		menu        = 1U << 4U,
		caps_lock   = 1U << 5U,
		num_lock    = 1U << 6U,
		scroll_lock = 1U << 7U,

		rshift = lshift << 16U,
		rctrl  = lctrl << 16U,
		ralt   = lalt << 16U,
		rsuper = lsuper << 16U,

		shift = lshift | rshift,
		ctrl  = lctrl | rctrl,
		alt   = lalt | ralt,
		super = lsuper | rsuper, // aka GUI/"windows key"

		// :TODO: meta? hyper?
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

	static inline lak::mod_key operator-(lak::mod_key m)
	{
		return static_cast<lak::mod_key>(UINT32_MAX ^ static_cast<uint32_t>(m));
	}

	struct key_event
	{
		lak::key_code key;
		lak::mod_key mod;
		uint32_t native_scancode;
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

		event()                    = default;
		event(event &&)            = default;
		event &operator=(event &&) = default;
		~event();
	};

	static_assert(std::is_default_constructible_v<lak::event>,
	              "lak::event must be default constructible");

	bool next_event(lak::event *e);
	bool peek_event(lak::event *e);
}

#endif

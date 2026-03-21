#define APP_NAME "template"

#define LAK_BASIC_PROGRAM_IMGUI_IMPL

#define LAK_BASIC_PROGRAM_IMGUI_WINDOW_IMPL

#include <lak/basic_program.inl>

struct my_window : public virtual LAK_BASIC_PROGRAM(window_api)
{
	my_window() : LAK_BASIC_PROGRAM(window_api)()
	{
		//
	}

	virtual void init() override final
	{
		//
	}

	virtual ~my_window()
	{
		//
	}

	virtual void handle_event(lak::event &event) override final
	{
		switch (event.type)
		{
			case lak::event_type::close_window: destroy(); break;
		}
	}

	virtual void loop(uint64_t counter_delta) override final
	{
		//
	}
};

lak::error_code<int> LAK_BASIC_PROGRAM(program_preinit)(lak::span<char *> args)
{
	return lak::ok_t{};
}

lak::weak_ptr<LAK_BASIC_PROGRAM(window_instance<my_window>)> my_window_ptr;

lak::error_code<int> LAK_BASIC_PROGRAM(program_init)()
{
	auto map_str_err = [](lak::u8string err) -> int
	{
		ERROR(err);
		return EXIT_FAILURE;
	};

	RES_TRY_ASSIGN(
	  my_window_ptr =,
	  LAK_BASIC_PROGRAM(create_window<my_window>)().map_err(map_str_err));

	return lak::ok_t{};
}

void LAK_BASIC_PROGRAM(program_handle_event)(lak::event &event)
{
	switch (event.type)
	{
		case lak::event_type::quit_program:
			if (auto wnd = my_window_ptr.get(); wnd) wnd->destroy();
			my_window_ptr.reset();
			break;
	}
}

bool LAK_BASIC_PROGRAM(program_loop)(uint64_t counter_delta)
{
	return !LAK_BASIC_PROGRAM(window_instances)().empty();
}

int LAK_BASIC_PROGRAM(program_quit)()
{
	my_window_ptr.reset();
	return EXIT_SUCCESS;
}

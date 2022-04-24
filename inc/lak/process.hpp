#ifndef LAK_PROCESS_HPP
#define LAK_PROCESS_HPP

#include "lak/file.hpp"
#include "lak/memory.hpp"
#include "lak/result.hpp"
#include "lak/string_view.hpp"

namespace lak
{
	struct process_impl;
	extern template lak::unique_ptr<lak::process_impl>;

	struct process
	{
	private:
		lak::unique_ptr<lak::process_impl> _impl;

		process(lak::unique_ptr<lak::process_impl> &&impl);

	public:
		static lak::result<process> create(
		  const lak::fs::path &app, lak::string_view<char8_t> arguments = {});

		process(process &&) = default;
		process &operator=(process &&) = default;

		~process();

		operator bool() const;

		bool joinable();

		lak::result<int> join();

		void release();
	};
}

#endif

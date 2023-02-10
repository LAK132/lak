#ifndef LAK_PROCESS_HPP
#define LAK_PROCESS_HPP

#include "lak/file.hpp"
#include "lak/memory.hpp"
#include "lak/result.hpp"
#include "lak/string_view.hpp"

namespace lak
{
	struct process_impl;
	extern template struct lak::unique_ptr<lak::process_impl>;

	struct process
	{
	private:
		lak::unique_ptr<lak::process_impl> _impl;

		process(lak::unique_ptr<lak::process_impl> &&impl);

	public:
		static lak::result<process> create(
		  const lak::fs::path &app, lak::string_view<char8_t> arguments = {});

		process(process &&)            = default;
		process &operator=(process &&) = default;

		~process();

		operator bool() const;

		bool joinable();

		lak::result<int> join();

		void release();

		std::ostream *std_in() const;
		std::istream *std_out() const;
		std::istream *std_err() const;

		void close_std_in();
		void close_std_out();
		void close_std_err();

		inline void close_input() { close_std_in(); }
		inline void close_output()
		{
			close_std_out();
			close_std_err();
		}
	};
}

#endif

#ifndef LAK_TOKENISER_HPP
#define LAK_TOKENISER_HPP

#include "lak/array.hpp"
#include "lak/span.hpp"
#include "lak/string.hpp"
#include "lak/string_view.hpp"

#include <ostream>

namespace lak
{
	struct codepoint_position
	{
		// Measured in codepoints not source characters.
		size_t line   = 1;
		size_t column = 1;

		void operator+=(char32_t c);

		template<typename CHAR>
		lak::string_view<CHAR> find(lak::string_view<CHAR> str) const;

		bool operator==(const codepoint_position &other) const;
		bool operator!=(const codepoint_position &other) const;

		inline friend std::ostream &operator<<(std::ostream &strm,
		                                       const lak::codepoint_position &pos)
		{
			return strm << std::dec << "{line: " << pos.line
			            << ", column: " << pos.column << "}";
		}
	};

	struct token_position
	{
		codepoint_position begin = {};
		codepoint_position end   = {};

		bool operator==(const token_position &other) const;
		bool operator!=(const token_position &other) const;

		inline friend std::ostream &operator<<(std::ostream &strm,
		                                       const lak::token_position &pos)
		{
			return strm << std::dec << "{begin: " << pos.begin
			            << ", end: " << pos.end << "}";
		}
	};

	template<typename CHAR>
	struct token
	{
		lak::string_view<CHAR> source;
		lak::token_position position;

		bool operator==(const token &other) const;
		bool operator!=(const token &other) const;

		template<size_t I>
		inline auto &get();

		template<size_t I>
		inline const auto &get() const;

		inline friend std::ostream &operator<<(std::ostream &strm,
		                                       const lak::token<CHAR> &token)
		{
			return strm << std::dec << "{source: " << token.source
			            << ", position: " << token.position << "}";
		}
	};

	template<typename CHAR>
	struct token_buffer
	{
		lak::string_view<CHAR> _source;
		size_t _max_size;

		lak::token<CHAR> _buffer_token;
		std::vector<char32_t> _buffer;

		inline token_buffer(lak::string_view<CHAR> data,
		                    size_t max_size,
		                    const lak::token<CHAR> &start);

		inline lak::u32string_view buffer() const;

		inline lak::string_view<CHAR> source() const;

		inline const lak::token_position &position() const;

		std::optional<lak::token<CHAR>> match(
		  lak::span<const lak::u32string> operators,
		  const lak::token<CHAR> &token);

		void operator++();

		void operator--();
	};

	template<typename CHAR>
	struct tokeniser
	{
		struct iterator
		{
		private:
			tokeniser &_tokeniser;

		public:
			iterator(tokeniser &t) : _tokeniser(t) {}
			auto &&operator++() { return _tokeniser.operator++(); }
			auto &&operator*() { return _tokeniser.operator*(); }
			auto operator->() { return _tokeniser.operator->(); }
			bool operator==(nullptr_t) { return _tokeniser == nullptr; }
			bool operator!=(nullptr_t) { return _tokeniser != nullptr; }
		};

	private:
		lak::string_view<CHAR> _data;
		lak::array<lak::u32string> _operators;
		size_t _longest_operator;

		lak::token<CHAR> _current;
		mutable lak::token<CHAR> _next;

		void internal_pump() noexcept;

	public:
		inline tokeniser(lak::string_view<CHAR> str,
		                 lak::array<lak::u32string> operators) noexcept;

		inline iterator begin() noexcept { return *this; }

		inline nullptr_t end() const noexcept { return nullptr; }

		inline token_position position() const noexcept
		{
			return _current.position;
		}

		inline token_position peek_position() const noexcept
		{
			peek();
			return _next.position;
		}

		inline const lak::token<CHAR> &operator*() const noexcept
		{
			return _current;
		}

		inline const lak::token<CHAR> *operator->() const noexcept
		{
			return &_current;
		}

		tokeniser &operator++() noexcept;

		// Don't tokenise, just peek at the very next character (codepoint).
		lak::token<CHAR> peek_char() const noexcept;

		lak::token<CHAR> read_char() noexcept;

		// Peek at the next token.
		lak::token<CHAR> peek() const noexcept;

		// Scan until codepoint is found (exclusive).
		lak::token<CHAR> until(char32_t codepoint) noexcept;
		// If you're tokenising something like:
		//  R"delimiter(this is a string)delimiter";
		// You can mark `R"` as an operator, then do `until(U'(')` to get
		// "delimiter".
		// This may also be useful for skipping comments:
		//  # this is a comment [newline]
		// Marking `#` as an operator will allow you do to `until(U'\n')` to scan
		// until it reaches the end of the line (or end of the file).

		// Scan until str is found (exclusive).
		lak::token<CHAR> until(u32string_view str) noexcept;
		// Useful for tokenising something like multiline comments:
		//  /*
		//  this is a multiline comment
		//  */
		// Once the operator `/*` is found, you can do `until(U"*/")` to scan until
		// the end of the comment (or end of the file) is found.

		// Scan until codepoint is found (inclusive).
		lak::token<CHAR> skip(char32_t codepoint) noexcept;

		// Scan until str is found (inclusive).
		lak::token<CHAR> skip(u32string_view str) noexcept;

		void reset() noexcept;

		inline bool operator==(nullptr_t) const noexcept
		{
			return _current.source.begin() >= _data.end();
		}

		inline bool operator!=(nullptr_t) const noexcept
		{
			return _current.source.begin() < _data.end();
		}

		operator bool() const noexcept { return _current.source.size() != 0; }
	};

	LAK_EXTERN_TEMPLATE_FOREACH_CHAR(token)
	LAK_EXTERN_TEMPLATE_FOREACH_CHAR(token_buffer)
	LAK_EXTERN_TEMPLATE_FOREACH_CHAR(tokeniser)
	LAK_TYPEDEF_FOREACH_CHAR(token)
	LAK_TYPEDEF_FOREACH_CHAR(token_buffer)
	LAK_TYPEDEF_FOREACH_CHAR(tokeniser)
}

template<typename CHAR>
struct std::tuple_size<lak::token<CHAR>>
{
	static constexpr size_t value = 2;
};

template<size_t I, typename CHAR>
struct std::tuple_element<I, lak::token<CHAR>>
{
	static_assert(I < 2);
	using type = lak::remove_cvref_t<
	  decltype(std::declval<lak::token<CHAR> &>().template get<I>())>;
};

#include "tokeniser.inl"

#endif

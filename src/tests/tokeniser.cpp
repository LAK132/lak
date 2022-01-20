#include "lak/tokeniser.hpp"
#include "lak/string_literals.hpp"
#include "lak/string_utils.hpp"

#include "lak/test.hpp"

BEGIN_TEST(tokeniser)
{
	auto input_string = R"(
struct chunk1 {
	u8 my_value;
};
)"_view;

	lak::array<lak::u32string> operators{
	  U"{",
	  U"}",
	  U";",
	};

	lak::atokeniser tokeniser(input_string, lak::move(operators));

	ASSERT_EQUAL(tokeniser->source, "struct"_view);
	++tokeniser;
	ASSERT_EQUAL(tokeniser->source, "chunk1"_view);
	ASSERT_EQUAL(tokeniser.peek().source, "{"_view);
	++tokeniser;
	ASSERT_EQUAL(tokeniser->source, "{"_view);
	++tokeniser;
	ASSERT_EQUAL(tokeniser->source, "u8"_view);
	++tokeniser;
	ASSERT_EQUAL(tokeniser->source, "my_value"_view);
	++tokeniser;
	ASSERT_EQUAL(tokeniser->source, ";"_view);
	++tokeniser;
	ASSERT_EQUAL(tokeniser->source, "}"_view);
	ASSERT_EQUAL(tokeniser.peek().source, ";"_view);
	++tokeniser;
	ASSERT_EQUAL(tokeniser->source, ";"_view);
	++tokeniser;
	ASSERT_EQUAL(tokeniser->source, ""_view);

	return 0;
}
END_TEST()

BEGIN_TEST(tokeniser2)
{
	// Make sure std::cout has been used before std::wcout so that std::cout
	// actually works under WSL.
	std::cout << LAK_SGR_RESET;

	lak::u8string str = lak::to_u8string(R"(; This is a comment!
(begin
	(define func
		(lambda (x n)
			(begin
				(println "Called func")
				; (println "this should have been commented out")
				(println (list x n))
				(if (zero? n) x (tail (func (* x 2) (- n 1))))
			)
		)
	)
	(println (func 2 10))
	; (exit)
))");

	lak::array<lak::u32string> operators = {U";", U"(", U")", U"\""};

	lak::u8tokeniser tokeniser{lak::string_view{str}, operators};

	lak::u8string result;

	for (const auto &[token, position] : tokeniser)
	{
		if (token == u8";"_view)
		{
			tokeniser.skip(U'\n');
		}
		else if (token == u8"\""_view)
		{
			if (auto str{tokeniser.read_char()}; str.source == u8"\""_view)
			{
				// it was an empty string
				ASSERT_UNREACHABLE();
			}
			else
			{
				auto t{tokeniser.until(U'"')};
				while (t.source.last(1)[0] == u8'\\') t = tokeniser.until(U'"');
				ASSERT_EQUAL(tokeniser.read_char().source, u8"\""_view);
				// :TODO: handle end of input before end of string
				str.source = lak::string_view{str.source.begin(), t.source.end()};
				str.position.end = t.position.end;

				result += u8"\""_view;
				result += str.source;
				result += u8"\" "_view;
			}
		}
		else if (token == u8"("_view)
		{
		}
		else if (token == u8")"_view)
		{
		}
		else
		{
			result += token;
			result += u8" "_view;
		}
	}

	ASSERT_EQUAL(result,
	             u8"begin define func lambda x n begin println "
	             "\"Called func\" println list x n if zero? n x "
	             "tail func * x 2 - n 1 println func 2 10 "_view);

	return 0;
}
END_TEST()

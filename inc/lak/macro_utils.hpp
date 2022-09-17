#undef STRINGIFY_EX
#define STRINGIFY_EX(...) #__VA_ARGS__
#undef STRINGIFY
#define STRINGIFY(...) STRINGIFY_EX(__VA_ARGS__)

#undef TOKEN_CONCAT_EX
#define TOKEN_CONCAT_EX(x, y) x##y
#undef TOKEN_CONCAT
#define TOKEN_CONCAT(x, y) TOKEN_CONCAT_EX(x, y)

#undef LINE_TRACE_STR
#define LINE_TRACE_STR __FILE__ ":" STRINGIFY(__LINE__)

#undef UNIQUIFY
#define UNIQUIFY(x) TOKEN_CONCAT(x, __LINE__)

#undef EMPTY_MACRO
#define EMPTY_MACRO(...)

#undef EXPAND
#define EXPAND(x) x

#undef LAK_UNUSED
#define LAK_UNUSED(x) ((void)x)

#undef LAK_ALL_CVTS
#define LAK_ALL_CVTS(MACRO, ...)                                              \
	EXPAND(MACRO(, __VA_ARGS__))                                                \
	EXPAND(MACRO(const, __VA_ARGS__))                                           \
	EXPAND(MACRO(volatile, __VA_ARGS__))                                        \
	EXPAND(MACRO(const volatile, __VA_ARGS__))

#undef LAK_ESC
#undef LAK_CSI
#undef LAK_SGR_STR
#undef LAK_SGR
#define LAK_ESC          "\x1B"
#define LAK_CSI          LAK_ESC "["
#define LAK_SGR_STR(str) LAK_CSI str "m"
#define LAK_SGR(x)       LAK_SGR_STR(STRINGIFY(x))

#undef LAK_SGR_RESET
#undef LAK_BOLD
#undef LAK_FAINT
#undef LAK_ITALIC
#define LAK_SGR_RESET LAK_SGR(0)
#define LAK_BOLD      LAK_SGR(1)
#define LAK_FAINT     LAK_SGR(2)
#define LAK_ITALIC    LAK_SGR(3)

#undef LAK_RED
#undef LAK_GREEN
#undef LAK_YELLOW
#undef LAK_BLUE
#undef LAK_MAGENTA
#undef LAK_CYAN
#undef LAK_WHITE
#define LAK_RED     LAK_SGR(31)
#define LAK_GREEN   LAK_SGR(32)
#define LAK_YELLOW  LAK_SGR(33)
#define LAK_BLUE    LAK_SGR(34)
#define LAK_MAGENTA LAK_SGR(35)
#define LAK_CYAN    LAK_SGR(36)
#define LAK_WHITE   LAK_SGR(37)
#define LAK_DEFAULT LAK_SGR(39)

#undef LAK_BRIGHT_BLACK
#undef LAK_BRIGHT_RED
#undef LAK_BRIGHT_GREEN
#undef LAK_BRIGHT_YELLOW
#undef LAK_BRIGHT_BLUE
#undef LAK_BRIGHT_MAGENTA
#undef LAK_BRIGHT_CYAN
#undef LAK_BRIGHT_WHITE
#define LAK_BRIGHT_BLACK   LAK_SGR(90)
#define LAK_BRIGHT_RED     LAK_SGR(91)
#define LAK_BRIGHT_GREEN   LAK_SGR(92)
#define LAK_BRIGHT_YELLOW  LAK_SGR(93)
#define LAK_BRIGHT_BLUE    LAK_SGR(94)
#define LAK_BRIGHT_MAGENTA LAK_SGR(95)
#define LAK_BRIGHT_CYAN    LAK_SGR(96)
#define LAK_BRIGHT_WHITE   LAK_SGR(97)

#undef LAK_SGR_FORE
#undef LAK_SGR_FORE_8
#undef LAK_SGR_FORE_24
#undef LAK_SGR_BACK
#undef LAK_SGR_BACK_8
#undef LAK_SGR_BACK_24
#define LAK_SGR_FORE(x)   LAK_SGR_STR("3" STRINGIFY(x))
#define LAK_SGR_FORE_8(x) LAK_SGR_STR("38;5;" STRINGIFY(x))
#define LAK_SGR_FORE_24(r, g, b)                                              \
	LAK_SGR_STR("38;2;" STRINGIFY(r) ";" STRINGIFY(g) ";" STRINGIFY(b))
#define LAK_SGR_BACK(x)   LAK_SGR_STR("4" STRINGIFY(x))
#define LAK_SGR_BACK_8(x) LAK_SGR_STR("48;5;" STRINGIFY(x))
#define LAK_SGR_BACK_24(r, g, b)                                              \
	LAK_SGR_STR("48;2;" STRINGIFY(r) ";" STRINGIFY(g) ";" STRINGIFY(b))

#define LAK_MEM_FN(X)                                                         \
	[this]<typename... T>(T && ...args) { return (X)(lak::forward<T>(args)...); }

#undef do_with
// do_with (var = stmt) { }
#define do_with(...)                                                          \
	if (__VA_ARGS__; false)                                                     \
		;                                                                         \
	else

#undef do_for
// do_for (i < n, l = m) { }
#define do_for(CONDITION, ...) for (__VA_ARGS__; CONDITION; __VA_ARGS__)

#undef for_or
// for_or (pre, condition, post) { }
// else { }
#define for_or(PRE_STATEMENT, CONDITION, ...)                                 \
	if (PRE_STATEMENT; CONDITION)                                               \
		for (bool UNIQUIFY(FIRST_LOOP_) = true;                                   \
		     UNIQUIFY(FIRST_LOOP_) || (CONDITION);                                \
		     (UNIQUIFY(FIRST_LOOP_) = false), (__VA_ARGS__))

#undef range_for_or
// range_for_or (auto& v, range) { }
// else { }
#define range_for_or(VAR, ...)                                                \
	if (auto &&UNIQUIFY(LOOP_RANGE_){__VA_ARGS__};                              \
	    UNIQUIFY(LOOP_RANGE_).begin() != UNIQUIFY(LOOP_RANGE_).end())           \
		for (VAR : UNIQUIFY(LOOP_RANGE_))

#undef while_or
// while_or (condition) { }
// else { }
#define while_or(...)                                                         \
	if (__VA_ARGS__)                                                            \
		for (bool UNIQUIFY(FIRST_LOOP_) = true;                                   \
		     UNIQUIFY(FIRST_LOOP_) || (__VA_ARGS__);                              \
		     UNIQUIFY(FIRST_LOOP_) = false)

#undef if_ref
// if_ref (auto &ref, ptr) { }
#define if_ref(VAR, ...)                                                      \
	if (auto &&UNIQUIFY(PTR_){__VA_ARGS__}; UNIQUIFY(PTR_))                     \
		do_with (VAR{*UNIQUIFY(PTR_)})

#undef if_in
// if_in (value, range, i) { range[i]; }
#define if_in(VALUE, RANGE, INDEX)                                            \
	do_with (auto &&UNIQUIFY(FIND_RANGE_){RANGE})                               \
		if (const size_t INDEX = [](auto &&value, auto &&range) -> size_t         \
		    {                                                                     \
			    size_t result = 0;                                                  \
			    for (auto &v : range)                                               \
				    if (v == value)                                                   \
					    break;                                                          \
				    else                                                              \
					    ++result;                                                       \
			    return result;                                                      \
		    }(VALUE, UNIQUIFY(FIND_RANGE_));                                      \
		    INDEX < UNIQUIFY(FIND_RANGE_).size())

#undef if_not_in
// if_not_in (value, range) { }
#define if_not_in(VALUE, RANGE)                                               \
	do_with (auto &&UNIQUIFY(FIND_RANGE_){RANGE})                               \
		if (const size_t UNIQUIFY(INDEX_) = [](auto &&value,                      \
		                                       auto &&range) -> size_t            \
		    {                                                                     \
			    size_t result = 0;                                                  \
			    for (auto &v : range)                                               \
				    if (v == value)                                                   \
					    break;                                                          \
				    else                                                              \
					    ++result;                                                       \
			    return result;                                                      \
		    }(VALUE, UNIQUIFY(FIND_RANGE_));                                      \
		    UNIQUIFY(INDEX_) >= UNIQUIFY(FIND_RANGE_).size())

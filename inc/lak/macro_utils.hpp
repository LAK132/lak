#undef STRINGIFY_EX
#define STRINGIFY_EX(x) #x
#undef STRINGIFY
#define STRINGIFY(x) STRINGIFY_EX(x)

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
#undef LAK_SGR_RESET
#undef LAK_BOLD
#undef LAK_FAINT
#undef LAK_ITALIC
#undef LAK_RED
#undef LAK_GREEN
#undef LAK_YELLOW
#undef LAK_BRIGHT_BLACK
#undef LAK_BRIGHT_RED
#undef LAK_BRIGHT_GREEN
#undef LAK_BRIGHT_YELLOW
#undef LAK_SGR_FORE
#undef LAK_SGR_FORE_8
#undef LAK_SGR_FORE_24
#undef LAK_SGR_BACK
#undef LAK_SGR_BACK_8
#undef LAK_SGR_BACK_24

#define LAK_ESC           "\x1B"
#define LAK_CSI           LAK_ESC "["
#define LAK_SGR_STR(str)  LAK_CSI str "m"
#define LAK_SGR(x)        LAK_SGR_STR(STRINGIFY(x))
#define LAK_SGR_RESET     LAK_SGR(0)
#define LAK_BOLD          LAK_SGR(1)
#define LAK_FAINT         LAK_SGR(2)
#define LAK_ITALIC        LAK_SGR(3)
#define LAK_RED           LAK_SGR(31)
#define LAK_GREEN         LAK_SGR(32)
#define LAK_YELLOW        LAK_SGR(33)
#define LAK_BRIGHT_BLACK  LAK_SGR(90)
#define LAK_BRIGHT_RED    LAK_SGR(91)
#define LAK_BRIGHT_GREEN  LAK_SGR(92)
#define LAK_BRIGHT_YELLOW LAK_SGR(93)
#define LAK_SGR_FORE(x)   LAK_SGR_STR("3" STRINGIFY(x))
#define LAK_SGR_FORE_8(x) LAK_SGR_STR("38;5;" STRINGIFY(x))
#define LAK_SGR_FORE_24(r, g, b)                                              \
	LAK_SGR_STR("38;2;" STRINGIFY(r) ";" STRINGIFY(g) ";" STRINGIFY(b))
#define LAK_SGR_BACK(x)   LAK_SGR_STR("4" STRINGIFY(x))
#define LAK_SGR_BACK_8(x) LAK_SGR_STR("48;5;" STRINGIFY(x))
#define LAK_SGR_BACK_24(r, g, b)                                              \
	LAK_SGR_STR("48;2;" STRINGIFY(r) ";" STRINGIFY(g) ";" STRINGIFY(b))

#undef for_or
#define for_or(PRE_STATEMENT, CONDITION, ...)                                 \
	if (PRE_STATEMENT; CONDITION)                                               \
		for (bool UNIQUIFY(FIRST_LOOP_) = true;                                   \
		     UNIQUIFY(FIRST_LOOP_) || CONDITION;                                  \
		     (UNIQUIFY(FIRST_LOOP_) = false), (__VA_ARGS__))

#undef range_for_or
#define range_for_or(RANGE, ...)                                              \
	if (auto &&UNIQUIFY(LOOP_RANGE_) = RANGE;                                   \
	    UNIQUIFY(LOOP_RANGE_).begin() != UNIQUIFY(LOOP_RANGE_).end())           \
		for (__VA_ARGS__ : UNIQUIFY(LOOP_RANGE_))

#undef while_or
#define while_or(...)                                                         \
	if (__VA_ARGS__)                                                            \
		for (bool UNIQUIFY(FIRST_LOOP_) = true;                                   \
		     UNIQUIFY(FIRST_LOOP_) || __VA_ARGS__;                                \
		     UNIQUIFY(FIRST_LOOP_) = false)

#undef if_in
#define if_in(VALUE, RANGE, INDEX)                                            \
	auto &&UNIQUIFY(FIND_RANGE_) = RANGE;                                       \
	auto &&UNIQUIFY(FIND_VALUE_) = VALUE;                                       \
	size_t UNIQUIFY(FIND_INDEX_) = 0;                                           \
	for (; UNIQUIFY(FIND_INDEX_) < UNIQUIFY(FIND_RANGE_).size();                \
	     ++UNIQUIFY(FIND_INDEX_))                                               \
		if (UNIQUIFY(FIND_RANGE_)[UNIQUIFY(FIND_INDEX_)] ==                       \
		    UNIQUIFY(FIND_VALUE_))                                                \
			break;                                                                  \
	if (size_t INDEX = UNIQUIFY(FIND_INDEX_);                                   \
	    INDEX < UNIQUIFY(FIND_RANGE_).size())

#undef if_not_in
#define if_not_in(VALUE, RANGE)                                               \
	auto &&UNIQUIFY(FIND_RANGE_) = RANGE;                                       \
	auto &&UNIQUIFY(FIND_VALUE_) = VALUE;                                       \
	size_t UNIQUIFY(FIND_INDEX_) = 0;                                           \
	for (; UNIQUIFY(FIND_INDEX_) < UNIQUIFY(FIND_RANGE_).size();                \
	     ++UNIQUIFY(FIND_INDEX_))                                               \
		if (UNIQUIFY(FIND_RANGE_)[UNIQUIFY(FIND_INDEX_)] ==                       \
		    UNIQUIFY(FIND_VALUE_))                                                \
			break;                                                                  \
	if (UNIQUIFY(FIND_INDEX_) == UNIQUIFY(FIND_RANGE_).size())

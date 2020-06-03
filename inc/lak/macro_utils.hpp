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

// clang-format off
// UNIQUIFY(FIRST_LOOP_) all need to be on the same line
#undef for_or
#define for_or(PRE_STATEMENT, CONDITION, ...)                                 \
  if (PRE_STATEMENT; CONDITION)                                               \
    for (bool UNIQUIFY(FIRST_LOOP_) = true; UNIQUIFY(FIRST_LOOP_) || CONDITION; (UNIQUIFY(FIRST_LOOP_) = false), (__VA_ARGS__))

#undef while_or
#define while_or(...)                                                         \
  if (__VA_ARGS__)                                                            \
    for (bool UNIQUIFY(FIRST_LOOP_) = true; UNIQUIFY(FIRST_LOOP_) || __VA_ARGS__; UNIQUIFY(FIRST_LOOP_) = false)
// clang-format on

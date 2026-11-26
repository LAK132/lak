# examples

## asc

asc.ebnf:
```
ws = {?lak::dsl::one_of_chars<U' ',U'\t'>?};
number = ws,?lak::dsl::parsed_dec_float<double, lak::dsl::char_literal<U'.'>>?;
vector = 3*number;
point = 2*vector;
line = <point>,ws,["\r"],?lak::dsl::eol?;
```

file.asc:
```
  2.917721 -116.203171 422.329102 0.03214 -0.98883 -0.14557
-50.230118 -110.088989 333.049011 0.59323  0.08693 -0.80033
-50.226219 -109.576881 332.800964 0.59368  0.08535 -0.80016
 50.105286  -90.762428 361.968079 0.70657 -0.58198 -0.40257
```

result:
```
lak::array<lak::array<lak::array<double, 3>, 2>>{
	{{  2.917721, -116.203171, 422.329102}, {0.03214, -0.98883, -0.14557}},
	{{-50.230118, -110.088989, 333.049011}, {0.59323,  0.08693, -0.80033}},
	{{-50.226219, -109.576881, 332.800964}, {0.59368,  0.08535, -0.80016}},
	{{ 50.105286,  -90.762428, 361.968079}, {0.70657, -0.58198, -0.40257}},
};
```

# syntax

## comment (`(* *)`)

comments are ignored by the interpreter.

```
(* this is a comment *)

(*
this is also a comment
*)
```

## rule (`=`)

rules take the form *rule name* `=` *rule body* `;`

## string (`""`)

```
rule = "match string";
```

`rule` matches the string `match string`.

## character (`''`)

```
rule = 'H';
```

`rule` matches the `H` character.

### hex literal

```
rule = '\x1234';
```

`rule` matches the UTF-32 character represented by `0x1234`, yields `char32_t(0x1234)`.

the number following `\x` may be any 32-bit unsigned hex integer.

### escape code

```
rule = '\t';
```

`rule` matches the tab character.

available escape codes:

- `\t`: tab
- `\n`: new line
- `\r`: carriage return
- `\'`: the `'` character
- `\"`: the `"` character

## sequence (`,`)

individual matches are combined into a match sequence with `,`

```
rule1 = "meow";
rule2 = '"',rule1,'"';
```

`rule1` matches `meow`, `rule2` matches `"meow"`.

## unordered sequence (`+`)

like a regular squence, but the sub-matches will match (once) in any order.

```
rule = "meow"+"woof";
```

`rule` matches `meowwoof` or `woofmeow`.

## alternation (`|`)

```
rule = "a"|"b"|"meow";
```

`rule` matches `a` or `b` or `meow`.

## capture group (`<>`)

```
rule = "'",<"hello">,"'";
```

`rule` matches `'hello'`, yields `hello`.

## non-capture group (`()`)

non-capture groups can be used to represent sub-sequences.

```
abc  = "a",("b","c");
aab  = 2*"a","b";
abab = 2*("a","b");
```

`abc` matches `abc`.

`aab` matches 2 `a`s followed by a `b` (`aab`).

`abab` matches 2 `ab`s (`abab`).

## repeat

### any (`{}`)

```
rule = {"a"};
```

`rule` matches the empty string, `a`, `aa`, `aaaaaa`, etc.

### at-least (`+`)

```
rule1 = 3+"a";
rule2 = 3+("a",'b');
```

`rule1` matches `a` at least 3 times (`aaa`, `aaaa`, `aaaaaaa`, etc).

`rule2` matches `ab` at least 3 times (`ababab`, `abababab`, `ababababababab`, etc).

### exactly (`*`)

```
rule1 = 3*"a";
rule2 = 3*("a",'b');
```

`rule1` matches `a` exactly 3 times (`aaa`).

`rule2` matches `ab` exactly 3 times (`ababab`).

## optional (`[]`)

```
rule = "hello",['!'];
```

`rule` matches `hello` or `hello!`.

## range (`~`)

```
a2z = 'a'~'z';
A2Z = 'A'~'Z';
```

`a2z` matches a single character between `a` and `z`.

`A2Z` matches a single character between `A` and `Z`.

## match case (`->`)

```
meow = "meow";
woof = "woof";
rule =
	"dog says: " -> woof |
	"cat says: " -> meow;
```

`rule` matches `dog says: woof` or `cat says: meow`.

## except (`-`)

```
rule = ('a'~'z')-'b';
```

`rule` matches a single character between `a` and `z`, except `b`.

## negative lookahead (`-`)

```
rule = -'b';
```

`rule` matches any character except `b`;

## positive lookahead (`+`)

```
rule = +"ab","a";
```

`rule` matches the substring `a` of `ab`.

## special (`??`)

specials are passed through as-is to the interpreter.

### ebnf2cpp

special sequences can be used to access more advanced features of `lak::dsl::`,
with immediate access to `lak/dsl/dsl.hpp` and `lak/dsl/utility.hpp` headers.

```
rule = ?lak::dsl::eol?;
```

`rule` matches end-of-line.

## replace (`^`)

```
rule =
	"true"  ^ ?true? |
	"false" ^ ?false?;
```

`rule` matches `true` or `false`, yielding the special `true` or `false` respectively.

## transform (`$$`)

used at the end of a rule body,
passed as-is to the interpreter for transforming the match result.

### ebnf2cpp

```
rule = 3+"a" $[](lak::u8string_view match) { return match.size(); }$;
```

`rule` matches 3 or more `a`s, yields the length of the match (`aaaa`->`4`, `aaaaaa`->`6`, etc).

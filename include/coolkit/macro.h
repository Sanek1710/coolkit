#pragma once

// common macros
#define PP_EMPTY
#define PP_COMMA ,
#define PP_IGNORE(...) PP_EMPTY

#define PP_IDENTITY(...) __VA_ARGS__
#define PP_EXPAND(...) __VA_ARGS__

#define PP_UNARY_OP(op, x) op x
#define PP_BINARY_OP(op, x, y) x op y

// preprocessor arg operators
#define _PP_STR(x) #x
#define PP_STR(x) _PP_STR(x)

#define _PP_CAT(x, y) x##y
#define PP_CAT(x, y) _PP_CAT(x, y)

#define _PP_PREFIXED(x, ...) x##__VA_ARGS__
#define PP_PREFIXED(x, ...) _PP_PREFIXED(x, __VA_ARGS__)

// pack-unpack macro args
#define PP_PACK(...) (__VA_ARGS__)
#define _PP_UNPACK(...) __VA_ARGS__
#define PP_UNPACK(pack) _PP_UNPACK pack

// bind-apply manipulations
#define PP_PASS(...) __VA_ARGS__
#define PP_BIND(F, ...) F, __VA_ARGS__
#define _PP_APPLY(F, ...) F(__VA_ARGS__)
#define PP_APPLY(...) _PP_APPLY(__VA_ARGS__)
#define PP_FIRST_ARG(x, ...) x

#define PP_IF_0(t, f) f
#define PP_IF_1(t, f) t
#define PP_IF(cond, t, f) PP_CAT(PP_IF_, cond(t, f))

#define PP_NOT(cond) PP_IF(cond, 0, 1)
#define PP_AND(cond1, cond2) PP_IF(cond1, cond2, 0)
#define PP_OR(cond1, cond2) PP_IF(cond1, 1, cond2)
#define PP_XOR(cond1, cond2) PP_IF(cond1, PP_NOT(cond2), cond2)

// text apply pseudo functions
#define PP_RET_1 1,
#define PP_RET_0 0,
#define PP_RET(...) PP_APPLY(PP_FIRST_ARG, PP_PREFIXED(PP_RET_, __VA_ARGS__))

// empty macro arg check
#define PP_CONSUME_PARENS(...) 1
#define PP_RET_PP_CONSUME_PARENS PP_RET_0
#define PP_IS_EMPTY(...)                                \
  PP_AND(PP_NOT(PP_RET(PP_CONSUME_PARENS __VA_ARGS__)), \
         PP_RET(PP_CONSUME_PARENS __VA_ARGS__()))

// apply macro for each arg

#define _PP_EXPAND4(...) PP_EXPAND(PP_EXPAND(PP_EXPAND(PP_EXPAND(__VA_ARGS__))))
#define _PP_EXPAND16(...) \
  _PP_EXPAND4(_PP_EXPAND4(_PP_EXPAND4(_PP_EXPAND4(__VA_ARGS__))))
#define _PP_EXPAND64(...) \
  _PP_EXPAND16(_PP_EXPAND16(_PP_EXPAND16(_PP_EXPAND16(__VA_ARGS__))))
#define _PP_EXPAND256(...) \
  _PP_EXPAND64(_PP_EXPAND64(_PP_EXPAND64(_PP_EXPAND64(__VA_ARGS__))))

#define _PP_FOREACH_REPEAT(cond, sep) \
  PP_IF(cond, PP_UNPACK, PP_IGNORE)   \
  (sep) PP_IF(cond, _PP_FOREACH, PP_IGNORE)
#define _PP_FOREACH(macro, sep, x, ...)                              \
  PP_APPLY(PP_UNPACK(macro), x)                                      \
  _PP_FOREACH_REPEAT PP_PACK(PP_NOT(PP_IS_EMPTY(__VA_ARGS__)), sep)( \
      macro, sep, __VA_ARGS__)

#define PP_FOREACH_SEP(macro, sep, ...)                             \
  PP_IF(PP_NOT(PP_IS_EMPTY(__VA_ARGS__)), _PP_EXPAND256, PP_IGNORE) \
  (_PP_FOREACH(PP_PACK(macro), PP_PACK(sep), __VA_ARGS__))

#define PP_FOREACH(macro, ...) \
  PP_FOREACH_SEP(PP_PASS(macro), PP_EMPTY, __VA_ARGS__)
#define PP_FOREACH_LIST(macro, ...) \
  PP_FOREACH_SEP(PP_PASS(macro), PP_COMMA, __VA_ARGS__)

#define PP_JOIN(sep, ...) PP_FOREACH_SEP(PP_IDENTITY, sep, __VA_ARGS__)

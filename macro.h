#pragma once

#define PP_EMPTY
#define PP_COMMA ,

#define PP_STR_HELPER(x) #x
#define PP_STR(x) PP_STR_HELPER(x)
#define PP_CAT_HELPER(x, y) x##y
#define PP_CAT(x, y) PP_CAT_HELPER(x, y)

#define PP_PACK(args...) args
#define PP_BIND(F, args...) PP_PACK(F, args)
#define PP_APPLY_HELPER(F, args...) F(args)
#define PP_APPLY(F, args...) PP_APPLY_HELPER(F, args)

#define PP_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, \
                 _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26,  \
                 _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38,  \
                 _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50,  \
                 _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62,  \
                 _63, N, ...)                                                 \
  N
#define PP_NARG_HELPER(...) PP_ARG_N(__VA_ARGS__)
#define PP_NARG(args...)                                                       \
  PP_NARG_HELPER(args, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, \
                 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35,   \
                 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20,   \
                 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3,  \
                 2, 1, 0)

#define PP_FOREACH0(F, sep) PP_EMPTY
#define PP_FOREACH1(F, sep, arg) PP_APPLY(F, arg)
#define PP_FOREACH2(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH1(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH3(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH2(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH4(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH3(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH5(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH4(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH6(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH5(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH7(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH6(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH8(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH7(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH9(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH8(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH10(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH9(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH11(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH10(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH12(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH11(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH13(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH12(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH14(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH13(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH15(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH14(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH16(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH15(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH17(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH16(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH18(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH17(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH19(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH18(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH20(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH19(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH21(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH20(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH22(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH21(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH23(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH22(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH24(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH23(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH25(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH24(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH26(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH25(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH27(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH26(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH28(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH27(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH29(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH28(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH30(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH29(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH31(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH30(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH32(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH31(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH33(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH32(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH34(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH33(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH35(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH34(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH36(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH35(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH37(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH36(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH38(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH37(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH39(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH38(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH40(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH39(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH41(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH40(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH42(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH41(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH43(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH42(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH44(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH43(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH45(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH44(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH46(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH45(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH47(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH46(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH48(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH47(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH49(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH48(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH50(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH49(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH51(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH50(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH52(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH51(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH53(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH52(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH54(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH53(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH55(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH54(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH56(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH55(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH57(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH56(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH58(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH57(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH59(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH58(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH60(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH59(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH61(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH60(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH62(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH61(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH63(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH62(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH64(F, sep, arg, args...) \
  PP_APPLY(F, arg) sep PP_FOREACH63(PP_PACK(F), PP_PACK(sep), args)

#define PP_FOREACHN(F, sep, N, args...) \
  PP_CAT(PP_FOREACH, N)(PP_PACK(F), PP_PACK(sep), args)
#define PP_FOREACH_SEP(F, sep, args...) \
  PP_FOREACHN(PP_PACK(F), PP_PACK(sep), PP_NARG(args), args)

#define PP_FOREACH(F, args...) PP_FOREACH_SEP(PP_PACK(F), PP_EMPTY, args)
#define PP_FOREACH_LIST(F, args...) PP_FOREACH_SEP(PP_PACK(F), PP_COMMA, args)

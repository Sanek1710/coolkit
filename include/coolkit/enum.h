#pragma once

#include "macro.h"

template <typename EnumT>
struct Enum;

#define ENUM_STR_CASE(Type, field) \
  case Type::field:                \
    return #field;

#define ENUM(Type, ...)                                                 \
  template <>                                                           \
  struct Enum<Type> {                                                   \
    inline static Type values[]{                                        \
        PP_FOREACH_LIST(PP_BIND(PP_BINARY_OP, ::, Type), __VA_ARGS__)}; \
    static constexpr size_t size = std::size(values);                   \
                                                                        \
    template <typename F>                                               \
    static void foreach (F&& f) {                                       \
      for (auto value : values) f(value);                               \
    }                                                                   \
                                                                        \
    static constexpr std::string_view string(Type value) {              \
      switch (value) {                                                  \
        PP_FOREACH(PP_BIND(ENUM_STR_CASE, Type), __VA_ARGS__);          \
        default:                                                        \
          break;                                                        \
      }                                                                 \
      return #Type "::(unknown)";                                       \
    }                                                                   \
  };

#define DEFINE_ENUM(Type, ...) \
  enum Type { __VA_ARGS__ };   \
  ENUM(Type, __VA_ARGS__)
#define DEFINE_ENUM_STRUCT(Type, ...) \
  enum struct Type { __VA_ARGS__ };   \
  ENUM(Type, __VA_ARGS__)
#define DEFINE_ENUM_CLASS(Type, ...) \
  enum class Type { __VA_ARGS__ };   \
  ENUM(Type, __VA_ARGS__)

#define ENUM_OSTREAM(Type)                                        \
  std::ostream& operator<<(std::ostream& os, const Type& value) { \
    return os << Enum<Type>::string(value);                       \
  }

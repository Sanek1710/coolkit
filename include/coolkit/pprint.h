#pragma once

#define PPRINT_USE_ABI
#define PPRINT_COLORS

#ifdef PPRINT_USE_ABI
#include <cxxabi.h>
#endif

#include <optional>
#include <tuple>
#include <type_traits>

#include "ansi.h"
#include "indentos.h"
#include "macro.h"
#include "memstat.h"

namespace Theme {
#ifdef PPRINT_COLORS
static constexpr auto color_typename = ansi::fg::rgb(0x4EC9B0);
static constexpr auto color_number = ansi::fg::rgb(0xB5CEA8);
static constexpr auto color_string = ansi::fg::rgb(0xCE9178);
static constexpr auto color_constant = ansi::fg::rgb(0x4FC1FF);
static constexpr auto color_variable = ansi::fg::rgb(0x9CDCFE);
static constexpr auto color_memstat = ansi::fg::rgb(0x2D2D2E);
// static constexpr auto color_function = ansi::fg::rgb(0xDCDCAA);
// static constexpr auto color_inactive_function = ansi::fg::rgb(0x8D8D6F);
static constexpr auto color_reset = ansi::fg::deflt;
#else
static constexpr auto color_typename = "";
static constexpr auto color_number = "";
static constexpr auto color_string = "";
static constexpr auto color_constant = "";
static constexpr auto color_variable = "";
static constexpr auto color_memstat = "";
static constexpr auto color_reset = "";
#endif
}  // namespace Theme

// Helper to detect if type has print method
template <typename T, typename = void>
struct has_print_method : std::false_type {};

template <typename T>
struct has_print_method<T, std::void_t<decltype(std::declval<T>().print(
                               std::declval<std::ostream&>()))>>
    : std::true_type {};

template <typename T>
constexpr bool has_print_method_v = has_print_method<T>::value;

// Helper to detect if type has ostream operator
template <typename T, typename = void>
struct has_ostream_operator : std::false_type {};

template <typename T>
struct has_ostream_operator<
    T,
    std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>>
    : std::true_type {};

template <typename T>
constexpr bool has_ostream_operator_v = has_ostream_operator<T>::value;

// Helper to detect string-like types
template <typename T>
struct is_string_like : std::is_constructible<std::string_view, T> {};

template <typename T>
constexpr bool is_string_like_v = is_string_like<T>::value;

// Helper to estimate if a type is "small"
template <typename T, typename = void>
struct is_small_type {
  static constexpr bool value = sizeof(T) < 16;
};

template <typename T>
constexpr bool is_small_type_v = is_small_type<T>::value;

// Helper to get type name
template <typename T>
std::string get_typename() {
  std::string result{typeid(T).name()};
#ifdef PPRINT_USE_ABI
  int status;
  char* buffer = abi::__cxa_demangle(result.c_str(), NULL, NULL, &status);
  result = buffer;
  std::free(buffer);
#endif
  return result;
}

struct PrintContext {
  PrintContext(std::ostream& os) : os(os) {}
  bool colors = true;
  bool multiline = true;
  bool quotes = false;
  bool memstat = true;
  std::ostream& os;
};

// Base printer template
template <typename T, typename = void>
struct Printer {
  static void print(PrintContext ctx, const T& val) {
    if constexpr (has_print_method_v<T>) {
      val.print(ctx.os);
    } else if constexpr (is_string_like_v<T>) {
      if (ctx.colors) ctx.os << Theme::color_string;
      if (ctx.quotes)
        ctx.os << std::quoted(val);
      else
        ctx.os << val;
      if (ctx.colors) ctx.os << Theme::color_reset;
    } else if constexpr (std::is_enum_v<T>) {
      if (ctx.colors) ctx.os << Theme::color_constant;
      ctx.os << val;
      if (ctx.colors) ctx.os << Theme::color_reset;
    } else if constexpr (has_ostream_operator_v<T>) {
      if (std::is_integral_v<T> || std::is_floating_point_v<T>)
        if (ctx.colors) ctx.os << Theme::color_number;
      ctx.os << val;
      if (ctx.colors) ctx.os << Theme::color_reset;
    } else {
      if (ctx.colors) ctx.os << Theme::color_typename;
      ctx.os << get_typename<T>();
      if (ctx.colors) ctx.os << Theme::color_reset;
      ctx.os << "{}";
    }
  }
};

template <typename T, typename = void>
struct is_memstattable : std::true_type {};
template <typename T>
struct is_memstattable<T, std::enable_if_t<is_small_type_v<T>>>
    : std::false_type {};
template <>
struct is_memstattable<std::string> : std::true_type {};
template <typename T>
constexpr bool is_memstattable_v = is_memstattable<T>::value;

template <typename T>
void print_memstat(PrintContext ctx, const T& val) {
  if constexpr (is_memstattable_v<T>) {
    if (ctx.colors) ctx.os << Theme::color_memstat;
    ctx.os << "<" << memstat(val) << ">";
    if (ctx.colors) ctx.os << Theme::color_reset;
  }
}

template <typename T>
void print_impl(PrintContext ctx, const T& val) {
  Printer<T>::print(ctx, val);
  if (ctx.memstat) print_memstat(ctx, val);
}

// Main print functions
template <typename T>
void print(std::ostream& os, const T& val) {
  PrintContext ctx{os};
  print_impl(ctx, val);
}

template <typename T>
void printout(const T& val) {
  PrintContext ctx{std::cout};
  ctx.quotes = true;
  ctx.colors = false;
  print_impl(ctx, val);
  std::cout << "\n";
}

// Main print function
template <typename T>
void printerr(const T& val) {
  PrintContext ctx{std::cerr};
  print_impl(ctx, val);
  std::cerr << "\n";
}

template <typename T>
std::string stringify(const T& val) {
  std::stringstream ss;
  print(ss, val);
  return ss.str();
}

// range print

template <typename T, typename = void>
struct is_range : std::false_type {};

template <typename T>
struct is_range<T, std::void_t<decltype(std::begin(std::declval<T>())),
                               decltype(std::end(std::declval<T>()))>>
    : std::true_type {};

template <typename T>
struct is_range<T, std::enable_if_t<std::is_array_v<T>>> : std::true_type {};

template <typename T>
constexpr bool is_range_v = is_range<T>::value;

// Helper to detect if type is a set-like container
template <typename T, typename = void>
struct has_keys : std::false_type {};

template <typename T>
struct has_keys<T, std::void_t<typename T::key_type, typename T::value_type>>
    : std::true_type {};

template <typename T>
constexpr bool has_keys_v = has_keys<T>::value;

// Helper to detect if type is a map-like container
template <typename T, typename = void>
struct is_map_like : std::false_type {};

template <typename T>
struct is_map_like<T,
                   std::void_t<typename T::key_type, typename T::mapped_type>>
    : std::true_type {};

template <typename T>
constexpr bool is_map_like_v = is_map_like<T>::value;

template <typename T>
struct is_small_type<T, std::enable_if_t<is_string_like_v<T>>>
    : std::true_type {};

struct PunctuatorSet {
  const char* start;
  const char* sep;
  const char* end;
  const char* split = "\n";
};
namespace punct {
static constexpr PunctuatorSet keylist{"{", ", ", "}", "\n"};
static constexpr PunctuatorSet dynlist{"[", ", ", "]", "\n"};
static constexpr PunctuatorSet statlist{"(", ", ", ")", "\n"};
};  // namespace punct

// range printer
template <typename T>
struct Printer<T, std::enable_if_t<is_range_v<T> && !is_string_like_v<T>>> {
  static void print(PrintContext ctx, const T& range) {
    using value_type =
        typename std::iterator_traits<decltype(std::begin(range))>::value_type;
    static constexpr bool is_small = is_small_type_v<value_type>;
    PunctuatorSet punct = has_keys_v<T> ? punct::keylist : punct::dynlist;
    punct.split = is_small ? "" : punct.split;
    if constexpr (is_map_like_v<T>) punct.split = "\n";
    if (!ctx.multiline) punct.split = "";

    ctx.os << punct.start;
    {
      const indentos indent{ctx.os, false};
      auto it = std::begin(range);
      auto end = std::end(range);
      bool first = true;
      for (; it != end; ++it) {
        if (!first) ctx.os << punct.sep;
        ctx.os << punct.split;
        if constexpr (is_map_like_v<T>) {
          ::print_impl(ctx, it->first);
          ctx.os << ": ";
          ::print_impl(ctx, it->second);
        } else {
          ::print_impl(ctx, *it);
        }
        first = false;
      }
    }
    ctx.os << punct.split << punct.end;
  }
};

// pair printer
template <typename T1, typename T2>
struct Printer<std::pair<T1, T2>> {
  static void print(PrintContext ctx, const std::pair<T1, T2>& pair) {
    const bool is_small = is_small_type_v<T1> && is_small_type_v<T2>;
    PunctuatorSet punct = punct::statlist;
    punct.split = is_small ? "" : punct.split;
    if (!ctx.multiline) punct.split = "";

    ctx.os << punct.start;
    {
      const indentos indent{ctx.os, false};
      ctx.os << punct.split;
      ::print_impl(ctx, pair.first);
      ctx.os << punct.sep << punct.split;
      ::print_impl(ctx, pair.second);
    }
    ctx.os << punct.split << punct.end;
  }
};

// tuple printer

template <typename... Types>
void print_tuple(PrintContext ctx, const std::tuple<Types...>& t,
                 PunctuatorSet punct) {
  const bool is_small = (is_small_type<Types>::value && ...);
  punct.split = is_small ? "" : punct.split;
  if (!ctx.multiline) punct.split = "";

  ctx.os << punct.start;
  {
    const indentos indent{ctx.os, false};
    std::apply(
        [&](auto... args) {
          bool first = true;
          ((ctx.os << (first ? "" : punct.sep) << punct.split, first = false,
            ::print_impl(ctx, args)),
           ...);
        },
        t);
  }
  ctx.os << punct.split << punct.end;
}

template <typename... Types>
struct Printer<std::tuple<Types...>> {
  static void print(PrintContext ctx, const std::tuple<Types...>& t) {
    print_tuple(ctx, t, punct::statlist);
  }
};

// optional printer
template <typename T>
struct Printer<std::optional<T>> {
  static void print(PrintContext ctx, const std::optional<T>& opt) {
    if (opt) return ::print_impl(ctx, *opt);
    if (ctx.colors) ctx.os << Theme::color_constant;
    ctx.os << "<nullopt>";
    if (ctx.colors) ctx.os << Theme::color_reset;
  }
};

// struct printer

template <typename FieldT>
struct FieldInfo {
  const char* name;
  const FieldT& value;
  FieldInfo(const char* name, const FieldT& value) : name(name), value(value) {}
};

template <typename FieldT>
struct is_small_type<FieldInfo<FieldT>> : std::false_type {};

template <typename T>
struct is_memstattable<FieldInfo<T>> : std::false_type {};

template <typename... FieldTs>
struct StructInfo {
  const char* tname;
  std::tuple<FieldInfo<FieldTs>...> field_infos;

  constexpr StructInfo(const char* tname, FieldInfo<FieldTs>... fields)
      : tname(tname), field_infos(std::make_tuple(fields...)) {}
};

template <typename... Ts>
struct is_memstattable<StructInfo<Ts...>> : std::false_type {};

template <typename T>
struct Printer<FieldInfo<T>> {
  static void print(PrintContext ctx, const FieldInfo<T>& field_info) {
    ctx.os << ".";
    if (ctx.colors) ctx.os << Theme::color_variable;
    ctx.os << field_info.name;
    if (ctx.colors) ctx.os << Theme::color_reset;
    ctx.os << "= ";
    ::print_impl(ctx, field_info.value);
  }
};

template <typename... FieldTs>
struct Printer<StructInfo<FieldTs...>> {
  static void print(PrintContext ctx,
                    const StructInfo<FieldTs...>& struct_info) {
    if (ctx.colors) ctx.os << Theme::color_typename;
    ctx.os << struct_info.tname;
    if (ctx.colors) ctx.os << Theme::color_reset;
    print_tuple(ctx, struct_info.field_infos, punct::keylist);
  }
};

// convenience macro

#define FIELD_INFO(field) \
  FieldInfo {             \
#field, field         \
  }
#define INLINE_PRINT(Type, fields...)                                         \
  void print(PrintContext ctx) const {                                        \
    const auto info = StructInfo(#Type, PP_FOREACH_LIST(FIELD_INFO, fields)); \
    ::print_impl(ctx, info);                                                  \
  }

#define OBJ_FIELD_INFO(obj, field) \
  FieldInfo {                      \
#field, obj.field              \
  }
#define PRINT_STRUCT(Type, fields...)                                    \
  template <>                                                            \
  struct Printer<Type> {                                                 \
    static void print(PrintContext ctx, const Type& obj) {               \
      const auto info = StructInfo(                                      \
          #Type, PP_FOREACH_LIST(PP_BIND(OBJ_FIELD_INFO, obj), fields)); \
      ::print_impl(ctx, info);                                           \
    }                                                                    \
  };

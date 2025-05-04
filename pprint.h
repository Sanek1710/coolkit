#pragma once

#include <optional>
#include <tuple>
#include <type_traits>

#include "ansi.h"

#define PPRINT_USE_ABI
#ifdef PPRINT_USE_ABI
#include "cxxabi.h"
#endif

#include "indentos.h"



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


struct PrintContext {
  std::ostream& os;
  const char* nl;

  PrintContext(std::ostream& os, const char* nl = "\n") : os(os), nl(nl) {}
};

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

// Base printer template
template <typename T, typename = void>
struct Printer {
  static void print(PrintContext ctx, const T& val) {
    if constexpr (has_print_method_v<T>) {
      val.print(ctx.os);
    } else if (is_string_like_v<T>) {
      ctx.os << ansi::fg::rgb(236, 177, 126);
      ctx.os << "\"" << val << "\"";
      ctx.os << ansi::fg::deflt;
    } else if constexpr (has_ostream_operator_v<T>) {
      if (std::is_integral_v<T> || std::is_floating_point_v<T>)
        ctx.os << ansi::fg::rgb(172, 226, 182);
      ctx.os << val;
      ctx.os << ansi::fg::deflt;
    } else {
      ctx.os << ansi::fg::rgb(36, 142, 110);
      ctx.os << get_typename<T>() << "{}";
      ctx.os << ansi::fg::deflt;
    }
  }
};


// Main print function
template <typename T>
void print(std::ostream& os, const T& val, const char* nl = "\n") {
  PrintContext ctx(os, nl);
  Printer<T>::print(ctx, val);
}



// range print

template <typename T, typename = void>
struct is_range : std::false_type {};

template <typename T>
struct is_range<T, std::void_t<decltype(std::begin(std::declval<T>())),
                               decltype(std::end(std::declval<T>()))>>
    : std::true_type {};

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

template <typename T>
constexpr bool is_small_type_v = is_small_type<T>::value;



struct ListPuctuators {
  const char* start;
  const char* sep;
  const char* end;
};

static constexpr ListPuctuators punct_keylist{"{", ", ", "}"};
static constexpr ListPuctuators punct_dynlist{"[", ", ", "]"};
static constexpr ListPuctuators punct_statlist{"(", ", ", ")"};

// range printer
template <typename T>
struct Printer<T, std::enable_if_t<is_range_v<T> && !is_string_like_v<T>>> {
  static void print(PrintContext ctx, const T& range) {
    using value_type =
        typename std::iterator_traits<decltype(std::begin(range))>::value_type;
    static constexpr bool is_small = is_small_type_v<value_type>;
    static constexpr ListPuctuators punct =
        has_keys_v<T> ? punct_keylist : punct_dynlist;
    if constexpr (!is_map_like_v<T>) ctx.nl = is_small ? "" : ctx.nl;
      
    ctx.os << punct.start;
    {
      const indentos indent{ctx.os, false};
      auto it = std::begin(range);
      auto end = std::end(range);
      bool first = true;
      for (; it != end; ++it) {
        if (!first) ctx.os << punct.sep;
        ctx.os << ctx.nl;
        if constexpr (is_map_like_v<T>) {
          Printer<typename T::key_type>::print(ctx, it->first);
          ctx.os << ": ";
          Printer<typename T::mapped_type>::print(ctx, it->second);
        } else {
          Printer<value_type>::print(ctx, *it);
        }
        first = false;
      }
    }
    ctx.os << ctx.nl << punct.end;
  }
};

// pair printer
template <typename T1, typename T2>
struct Printer<std::pair<T1, T2>> {
  static void print(PrintContext ctx, const std::pair<T1, T2>& pair) {
    const bool is_small = is_small_type_v<T1> && is_small_type_v<T2>;
    static constexpr ListPuctuators punct = punct_statlist;
    ctx.nl = is_small ? "" : ctx.nl;

    ctx.os << punct.start;
    {
      const indentos indent{ctx.os, false};
      ctx.os << ctx.nl;
      Printer<T1>::print(ctx, pair.first);
      ctx.os << punct.sep << ctx.nl;
      Printer<T2>::print(ctx, pair.second);
    }
    ctx.os << ctx.nl << punct.end;
  }
};

// tuple printer

template <typename Tuple, size_t... Is>
void print_tuple(PrintContext ctx, const Tuple& t, std::index_sequence<Is...>,
                 ListPuctuators punct) {
  const bool is_small =
      (is_small_type<std::tuple_element_t<Is, Tuple>>::value && ...);
  ctx.nl = is_small ? "" : ctx.nl;

  ctx.os << punct.start;
  {
    const indentos indent{ctx.os, false};
    ((ctx.os << (Is == 0 ? "" : punct.sep) << ctx.nl,
      Printer<std::tuple_element_t<Is, Tuple>>::print(ctx, std::get<Is>(t))),
     ...);
  }
  ctx.os << ctx.nl << punct.end;
}

template <typename... Types>
struct Printer<std::tuple<Types...>> {
  static void print(PrintContext ctx, const std::tuple<Types...>& t) {
    print_tuple(ctx, t, std::index_sequence_for<Types...>{}, punct_statlist);
  }
};

// optional printer
template <typename T>
struct Printer<std::optional<T>> {
  static void print(PrintContext ctx, const std::optional<T>& opt) {
    if (opt) return Printer<T>::print(ctx, *opt);
    ctx.os << ansi::fg::rgb(109, 183, 229);
    ctx.os << "<nullopt>";
    ctx.os << ansi::fg::deflt;
  }
};


// struct printer

template <typename FieldT>
struct FieldInfo {
  const char* name;
  const FieldT& value;
};

// Deduction guide for FieldInfo
template <typename FieldT>
FieldInfo(const char*, const FieldT&) -> FieldInfo<FieldT>;

// Strings are small if they're short
template <typename FieldT>
struct is_small_type<FieldInfo<FieldT>> : std::false_type {};

template <typename... FieldTs>
struct FieldInfos : std::tuple<FieldInfo<FieldTs>...> {
  // using std::tuple<FieldInfo<FieldTs>...>::tuple;
  constexpr FieldInfos(const std::tuple<FieldInfo<FieldTs>...>& field_infos)
      : std::tuple<FieldInfo<FieldTs>...>(field_infos) {}
};
template <typename... FieldTs>
FieldInfos(const std::tuple<FieldInfo<FieldTs>...>&) -> FieldInfos<FieldTs...>;

template <typename... FieldTs>
struct StructInfo {
  const char* tname;
  std::tuple<FieldInfo<FieldTs>...> field_infos;
  // FieldInfos<FieldTs...> field_infos;

  constexpr StructInfo(const char* tname, FieldInfo<FieldTs>... fields)
      : tname(tname), field_infos(std::make_tuple(fields...)) {}
};

// Helper function to create StructInfo with simpler syntax
template <typename... FieldTs>
constexpr auto make_struct_info(const char* tname,
                                FieldInfo<FieldTs>... fields) {
  return StructInfo<FieldTs...>{tname, fields...};
}

// FieldInfo printer
template <typename T>
struct Printer<FieldInfo<T>> {
  static void print(PrintContext ctx, const FieldInfo<T>& field_info) {
    ctx.os << "."                           //
           << ansi::fg::rgb(163, 223, 233)  //
           << field_info.name               //
           << ansi::fg::deflt               //
           << "= ";
    Printer<T>::print(ctx, field_info.value);
  }
};

// StructInfo printer
template <typename... FieldTs>
struct Printer<StructInfo<FieldTs...>> {
  static void print(PrintContext ctx,
                    const StructInfo<FieldTs...>& struct_info) {
    ctx.os << ansi::fg::rgb(99, 184, 159);
    ctx.os << struct_info.tname;
    ctx.os << ansi::fg::deflt;
    print_tuple(ctx, struct_info.field_infos,
                std::index_sequence_for<FieldTs...>{}, punct_keylist);
  }
};

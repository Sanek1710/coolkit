#include <cstddef>
#include <iostream>
#include <map>
#include <ostream>
#include <set>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

// #define PPRINT_USE_ABI
#ifdef PPRINT_USE_ABI
#include "cxxabi.h"
#endif

// Forward declarations
template <typename T, typename = void>
struct Printer;

// Helper to detect if type has print method
template <typename T, typename = void>
struct has_print_method : std::false_type {};

template <typename T>
struct has_print_method<T, std::void_t<decltype(std::declval<T>().print(
                               std::declval<std::ostream&>()))>>
    : std::true_type {};

// Helper to detect if type has ostream operator
template <typename T, typename = void>
struct has_ostream_operator : std::false_type {};

template <typename T>
struct has_ostream_operator<
    T,
    std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>>
    : std::true_type {};

// Helper to detect if type is a range
template <typename T, typename = void>
struct is_range : std::false_type {};

template <typename T>
struct is_range<T, std::void_t<decltype(std::begin(std::declval<T>())),
                               decltype(std::end(std::declval<T>()))>>
    : std::true_type {};

// Helper to detect if type is a map-like container
template <typename T, typename = void>
struct is_map_like : std::false_type {};

template <typename T>
struct is_map_like<T,
                   std::void_t<typename T::key_type, typename T::mapped_type>>
    : std::true_type {};

// Helper to detect if type is a set-like container
template <typename T, typename = void>
struct is_set_like : std::false_type {};

template <typename T>
struct is_set_like<T, std::void_t<typename T::key_type, typename T::value_type>>
    : std::true_type {};

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

// Field printing helper
template <typename T, typename FT>
void print_field(std::ostream& os, const T& obj, FT T::*field,
                 const char* name) {
  os << name << ": ";
  Printer<FT>::print(os, obj.*field);
  os << ",\n";
}

// Base printer template
template <typename T, typename>
struct Printer {
  static void print(std::ostream& os, const T& val) {
    if constexpr (has_print_method<T>::value) {
      val.print(os);
    } else if constexpr (has_ostream_operator<T>::value) {
      os << val;
    } else {
      os << get_typename<T>() << "{}";
    }
  }
};

// String printer
template <>
struct Printer<std::string> {
  static void print(std::ostream& os, const std::string& str) {
    os << "\"" << str << "\"";
  }
};

template <>
struct Printer<std::string_view> {
  static void print(std::ostream& os, const std::string_view& str) {
    os << "\"" << str << "\"";
  }
};

// C-style string printer
template <>
struct Printer<const char*> {
  static void print(std::ostream& os, const char* str) {
    os << "\"" << str << "\"";
  }
};

template <>
struct Printer<char*> {
  static void print(std::ostream& os, const char* str) {
    os << "\"" << str << "\"";
  }
};

template <size_t N>
struct Printer<const char[N]> {
  static void print(std::ostream& os, const char* str) {
    os << "\"" << str << "\"";
  }
};

template <size_t N>
struct Printer<char[N]> {
  static void print(std::ostream& os, const char* str) {
    os << "\"" << str << "\"";
  }
};

// Main pprint function
template <typename T>
void print(std::ostream& os, const T& val) {
  Printer<T>::print(os, val);
}

// Generic range printer
template <typename T>
struct Printer<T,
               std::enable_if_t<is_range<T>::value && !is_map_like<T>::value &&
                                !is_set_like<T>::value>> {
  static void print(std::ostream& os, const T& range) {
    os << "[";
    auto it = std::begin(range);
    auto end = std::end(range);
    bool first = true;
    for (; it != end; ++it) {
      if (!first) os << ", ";
      Printer<typename std::iterator_traits<decltype(it)>::value_type>::print(
          os, *it);
      first = false;
    }
    os << "]";
  }
};

// Generic map-like printer
template <typename T>
struct Printer<T, std::enable_if_t<is_map_like<T>::value>> {
  static void print(std::ostream& os, const T& map) {
    os << "{";
    auto it = std::begin(map);
    auto end = std::end(map);
    bool first = true;
    for (; it != end; ++it) {
      if (!first) os << ", ";
      Printer<typename T::key_type>::print(os, it->first);
      os << ": ";
      Printer<typename T::mapped_type>::print(os, it->second);
      first = false;
    }
    os << "}";
  }
};

// Generic set-like printer
template <typename T>
struct Printer<
    T, std::enable_if_t<is_set_like<T>::value && !is_map_like<T>::value>> {
  static void print(std::ostream& os, const T& set) {
    os << "{";
    auto it = std::begin(set);
    auto end = std::end(set);
    bool first = true;
    for (; it != end; ++it) {
      if (!first) os << ", ";
      Printer<typename T::value_type>::print(os, *it);
      first = false;
    }
    os << "}";
  }
};

// Pair printer
template <typename T1, typename T2>
struct Printer<std::pair<T1, T2>> {
  static void print(std::ostream& os, const std::pair<T1, T2>& pair) {
    os << "(";
    Printer<T1>::print(os, pair.first);
    os << ", ";
    Printer<T2>::print(os, pair.second);
    os << ")";
  }
};

// Tuple printer helper
template <typename Tuple, size_t... Is>
void print_tuple(std::ostream& os, const Tuple& t, std::index_sequence<Is...>) {
  os << "(";
  ((os << (Is == 0 ? "" : ", "),
    Printer<std::tuple_element_t<Is, Tuple>>::print(os, std::get<Is>(t))),
   ...);
  os << ")";
}

// Tuple printer
template <typename... Types>
struct Printer<std::tuple<Types...>> {
  static void print(std::ostream& os, const std::tuple<Types...>& t) {
    print_tuple(os, t, std::index_sequence_for<Types...>{});
  }
};

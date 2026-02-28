#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

namespace _detail {

struct any_t {
  template <typename T>
  operator T();
};

template <typename T, typename IS, typename = void>
struct is_initable : std::false_type {};

template <typename T, std::size_t... I>
struct is_initable<T, std::index_sequence<I...>,
                   std::void_t<decltype(T{(void(I), any_t{})...})>>
    : std::true_type {};

template <typename T, std::size_t N = 0>
struct count_fields {
  static constexpr bool can_next =
      is_initable<T, std::make_index_sequence<N + 1>>::value;

  static constexpr std::size_t value =
      std::conditional_t<can_next, count_fields<T, N + 1>,
                         std::integral_constant<std::size_t, N>>::value;
};

template <typename T>
struct false_t : std::false_type {};

template <typename T>
struct count_fields<T, 256> {
  static_assert(false_t<T>::value, "too many fields");
  static constexpr std::size_t value = 256;
};

}  // namespace _detail

template <typename T>
inline constexpr std::size_t fields_count_v = _detail::count_fields<T>::value;

#define STRUCT_TO_TUPLE

namespace _detail {

// struct to tuple
template <typename T, std::size_t N = fields_count_v<T>>
struct struct_to_tuple;

// empty structs
template <typename T>
struct struct_to_tuple<T, 0> {
  using type = std::tuple<>;
  static constexpr type convert(T&) { return std::tie(); }
};

// default - error
template <typename T, std::size_t N>
struct struct_to_tuple {
  static_assert(false_t<T>::value,
                "\n\nERROR: fields count limit reached.\n"
                "add REGISTER_STRUCT_TO_TUPLE(N, a1, a2, ... aN) for your N\n");

  using type = std::tuple<>;
  static constexpr type convert(T&) { return std::tie(); }
};

#define REGISTER_STRUCT_TO_TUPLE(N, ...)                \
  template <typename T>                                 \
  struct struct_to_tuple<T, N> {                        \
    static constexpr auto convert(T& obj) {             \
      auto& [__VA_ARGS__] = obj;                        \
      return std::tie(__VA_ARGS__);                     \
    }                                                   \
    using type = decltype(convert(std::declval<T&>())); \
  };

REGISTER_STRUCT_TO_TUPLE(1, a1)
REGISTER_STRUCT_TO_TUPLE(2, a1, a2)
REGISTER_STRUCT_TO_TUPLE(3, a1, a2, a3)
REGISTER_STRUCT_TO_TUPLE(4, a1, a2, a3, a4)
REGISTER_STRUCT_TO_TUPLE(5, a1, a2, a3, a4, a5)
REGISTER_STRUCT_TO_TUPLE(6, a1, a2, a3, a4, a5, a6)
REGISTER_STRUCT_TO_TUPLE(7, a1, a2, a3, a4, a5, a6, a7)
REGISTER_STRUCT_TO_TUPLE(8, a1, a2, a3, a4, a5, a6, a7, a8)
REGISTER_STRUCT_TO_TUPLE(9, a1, a2, a3, a4, a5, a6, a7, a8, a9)
REGISTER_STRUCT_TO_TUPLE(10, a1, a2, a3, a4, a5, a6, a7, a8, a10)

}  // namespace _detail

template <typename T>
using struct_to_tuple_t = typename _detail::struct_to_tuple<T>::type;

template <typename T>
constexpr auto to_tuple(T& obj) {
  return _detail::struct_to_tuple<T>::convert(obj);
}
template <typename T>
constexpr auto to_tuple(const T& obj) {
  return _detail::struct_to_tuple<const T>::convert(obj);
}

#pragma once

#include <cstddef>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <optional>
#include <set>
#include <stack>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Helper to detect if type has memstat method
template <typename T, typename = void>
struct has_memstat_method : std::false_type {};

template <typename T>
struct has_memstat_method<T, std::void_t<decltype(std::declval<T>().memstat())>>
    : std::true_type {};

template <typename T>
constexpr bool has_memstat_method_v = has_memstat_method<T>::value;

template <typename T, typename = void>
struct Memstat {
  static size_t memstat(const T& val) {
    if constexpr (has_memstat_method_v<T>) {
      return val.memstat();
    } else {
      return sizeof(T);
    }
  }
};

// Main function
template <typename T>
size_t memstat(const T& val) {
  return Memstat<T>::memstat(val);
}

// std::string
template <typename C, typename T, typename A>
struct Memstat<std::basic_string<C, T, A>> {
  static size_t memstat(const std::basic_string<C, T, A>& str) {
    static const size_t sso_length = std::basic_string<C, T, A>{}.capacity();
    size_t size = sizeof(std::basic_string<C, T, A>);
    if (str.capacity() > sso_length) size += str.capacity() * sizeof(C);
    return size;
  }
};

// std::vector
template <typename T, typename A>
struct Memstat<std::vector<T, A>> {
  static size_t memstat(const std::vector<T, A>& vec) {
    size_t size = sizeof(std::vector<T, A>);
    size += vec.capacity() * sizeof(T);
    for (const auto& elem : vec) size += Memstat<T>::memstat(elem) - sizeof(T);
    return size;
  }
};

// std::deque
template <typename T, typename A>
struct Memstat<std::deque<T, A>> {
  static size_t memstat(const std::deque<T, A>& deq) {
    size_t size = sizeof(std::deque<T, A>);
    // Deque typically allocates in chunks, estimate based on size
    size += (deq.size() + 1) * sizeof(T);  // +1 for potential partial chunk
    for (const auto& elem : deq) size += Memstat<T>::memstat(elem) - sizeof(T);
    return size;
  }
};

// std::list
template <typename T, typename A>
struct Memstat<std::list<T, A>> {
  static size_t memstat(const std::list<T, A>& lst) {
    size_t size = sizeof(std::list<T, A>);
    for (const auto& elem : lst) {
      size += Memstat<T>::memstat(elem);
      size += sizeof(void*) * 2;  // prev and next pointers
    }
    return size;
  }
};

// std::forward_list
template <typename T, typename A>
struct Memstat<std::forward_list<T, A>> {
  static size_t memstat(const std::forward_list<T, A>& lst) {
    size_t size = sizeof(std::forward_list<T, A>);
    for (const auto& elem : lst) {
      size += Memstat<T>::memstat(elem);
      size += sizeof(void*);  // next pointer
    }
    return size;
  }
};

// std::set
template <typename T, typename C, typename A>
struct Memstat<std::set<T, C, A>> {
  static size_t memstat(const std::set<T, C, A>& set) {
    size_t size = sizeof(std::set<T, C, A>);
    for (const auto& elem : set) {
      size += Memstat<T>::memstat(elem);
      size += sizeof(void*) * 3;  // Left, right, parent pointers
    }
    return size;
  }
};

// std::unordered_set
template <typename T, typename H, typename E, typename A>
struct Memstat<std::unordered_set<T, H, E, A>> {
  static size_t memstat(const std::unordered_set<T, H, E, A>& set) {
    size_t size = sizeof(std::unordered_set<T, H, E, A>);
    for (const auto& elem : set) {
      size += Memstat<T>::memstat(elem);
      size += sizeof(void*);  // Next pointer
    }
    size += set.bucket_count() * sizeof(void*);
    return size;
  }
};

// std::map
template <typename K, typename V, typename C, typename A>
struct Memstat<std::map<K, V, C, A>> {
  static size_t memstat(const std::map<K, V, C, A>& map) {
    size_t size = sizeof(std::map<K, V, C, A>);
    for (const auto& [key, value] : map) {
      size += Memstat<std::pair<const K, V>>::memstat({key, value});
      size += sizeof(void*) * 3;  // Left, right, parent pointers
    }
    return size;
  }
};

// std::unordered_map
template <typename K, typename V, typename H, typename E, typename A>
struct Memstat<std::unordered_map<K, V, H, E, A>> {
  static size_t memstat(const std::unordered_map<K, V, H, E, A>& map) {
    size_t size = sizeof(std::unordered_map<K, V, H, E, A>);
    for (const auto& item : map) {
      size += Memstat<std::pair<const K, V>>::memstat(item);
      size += sizeof(void*);  // Next pointer
    }
    size += map.bucket_count() * sizeof(void*);
    return size;
  }
};

// std::pair
template <typename T1, typename T2>
struct Memstat<std::pair<T1, T2>> {
  static size_t memstat(const std::pair<T1, T2>& pair) {
    size_t size = sizeof(std::pair<T1, T2>);
    size += Memstat<T1>::memstat(pair.first) - sizeof(T1);
    size += Memstat<T2>::memstat(pair.second) - sizeof(T2);
    return size;
  }
};

// std::optional
template <typename T>
struct Memstat<std::optional<T>> {
  static size_t memstat(const std::optional<T>& opt) {
    size_t size = sizeof(std::optional<T>);
    if (opt) size += Memstat<T>::memstat(*opt) - sizeof(T);
    return size;
  }
};

// std::stack (just a container adapter)
template <typename T, typename C>
struct Memstat<std::stack<T, C>> {
  static size_t memstat(const std::stack<T, C>& stack) {
    return Memstat<C>::memstat(stack.c);
  }
};

// Convenience macros for adding memstat to structures
#define MEMSTAT_FIELD(res, field) size += ::memstat(field) - sizeof(field)
#define INLINE_MEMSTAT(Type, fields...)                    \
  size_t memstat() const {                                 \
    size_t size = sizeof(Type);                            \
    PP_FOREACH_LIST(PP_BIND(MEMSTAT_FIELD, size), fields); \
    return size;                                           \
  }

#define OBJ_MEMSTAT_FIELD(res, obj, field) \
  res += ::memstat(obj.field) - sizeof(obj.field)
#define MEMSTAT_STRUCT(Type, fields...)                               \
  template <>                                                         \
  struct Memstat<Type> {                                              \
    static size_t memstat(const Type& obj) {                          \
      size_t size = sizeof(Type);                                     \
      PP_FOREACH_LIST(PP_BIND(OBJ_MEMSTAT_FIELD, size, obj), fields); \
      return size;                                                    \
    }                                                                 \
  };

#pragma once

#include <tuple>

template <typename FieldT>
struct FieldInfo {
  const char* name;
  const FieldT& value;
  FieldInfo(const char* name, const FieldT& value) : name(name), value(value) {}
};

template <typename... FieldTs>
struct StructInfo {
  const char* tname;
  std::tuple<FieldInfo<FieldTs>...> field_infos;

  constexpr StructInfo(const char* tname, FieldInfo<FieldTs>... fields)
      : tname(tname), field_infos(std::make_tuple(fields...)) {}
};

// include macro.h to use these
#define FIELD_INFO(obj, field) \
  FieldInfo {                  \
#field, obj.field          \
  }
#define STRUCT_INFO(obj, Type, ...)                               \
  StructInfo {                                                    \
#Type, PP_FOREACH_LIST(PP_BIND(FIELD_INFO, obj), __VA_ARGS__) \
  }

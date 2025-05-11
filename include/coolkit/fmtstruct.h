#pragma once

#include <cstdint>

#include "fmt/color.h"
#include "indentos.h"

namespace fmt {

struct struct_formatter {
 protected:
  static constexpr auto style_type = fg(color::medium_turquoise);
  static constexpr auto style_field = fg(color::light_blue);
  static constexpr const char* punct_empty = "";

  struct punctuators {
    static constexpr const char* empty = "";
    const char* sep;
    const char* start;
    const char* end;
  };

  static constexpr punctuators flat_punct{", ", "{", "}"};
  static constexpr punctuators block_punct{",\n", "{\n", "\n}"};

  using format_flags = uint32_t;

  format_flags flags;

#define FLAG_VALUE(marker) (1U << ((marker) - 'a'))

  static constexpr format_flags flag_multiline = FLAG_VALUE('n');
  static constexpr format_flags flag_values_only = FLAG_VALUE('v');
  static constexpr format_flags flag_colored = FLAG_VALUE('c');
  static constexpr format_flags flag_indent_os = FLAG_VALUE('t');

 public:
  constexpr auto parse(format_parse_context& ctx) {
    auto i = ctx.begin(), end = ctx.end();
    while (i != end && *i != '}') {
      if (std::islower(*i)) flags |= FLAG_VALUE(*i);
      ++i;
    }
    return i;
  }
#undef FLAG_VALUE

 protected:
  template <typename Context>
  struct FieldFormatter {
    Context& ctx;
    int nfields = 0;
    punctuators punct;
    format_flags flags;

    FieldFormatter(Context& ctx, format_flags flags, punctuators punct)
        : ctx(ctx), flags(flags), punct(punct) {
      format_to(ctx.out(), "{}", punct.start);
      if (flags & flag_indent_os) format_to(ctx.out(), indent::ctrl::push);
    }
    ~FieldFormatter() {
      if (flags & flag_indent_os) format_to(ctx.out(), indent::ctrl::pop);
      format_to(ctx.out(), "{}", punct.end);
    }

    template <typename T>
    FieldFormatter& fieldf(const char* name, const T& value,
                           const char* format = "{}",
                           const char* name_format = ".{}=") {
      if (nfields > 0) format_to(ctx.out(), "{}", punct.sep);
      if (!(flags & flag_values_only)) {
        if (flags & flag_colored) {
          format_to(ctx.out(), style_field, name_format, name);
        } else {
          format_to(ctx.out(), name_format, name);
        }
      }
      format_to(ctx.out(), format, value);
      ++nfields;
      return *this;
    }
  };

  template <typename Context>
  auto structf(const char* tname, Context& ctx) const {
    return structf(tname, ctx, flags,
                   flags & flag_multiline ? block_punct : flat_punct);
  }
  template <typename Context>
  auto structf(const char* tname, Context& ctx, format_flags flags) const {
    return structf(tname, ctx, flags,
                   flags & flag_multiline ? block_punct : flat_punct);
  }
  template <typename Context>
  auto structf(const char* tname, Context& ctx, punctuators punct) const {
    return structf(tname, ctx, flags, punct);
  }
  template <typename Context>
  auto structf(const char* tname, Context& ctx, format_flags flags,
               punctuators punct) const {
    if (flags & flag_colored) {
      format_to(ctx.out(), style_type, "{}", tname);
    } else {
      format_to(ctx.out(), "{}", tname);
    }
    return FieldFormatter(ctx, flags, punct);
  }
};

}  // namespace fmt

#define FMT_FIELDF(obj, field) fieldf(#field, obj.field)
#define FMT_STRUCT_FORMATTER(Type, ...)                                       \
  template <>                                                                 \
  struct fmt::formatter<Type> : struct_formatter {                            \
    template <typename Context>                                               \
    auto format(const Type& obj, Context& ctx) const -> decltype(ctx.out()) { \
      structf(#Type, ctx)                                                     \
          .PP_FOREACH_SEP(PP_BIND(FMT_FIELDF, obj), ., __VA_ARGS__);          \
      return ctx.out();                                                       \
    }                                                                         \
  }

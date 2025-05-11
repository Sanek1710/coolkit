#pragma once

#include <iostream>
#include <string_view>

namespace indent {

struct base_indent_context {
  int last_ch;
  uint32_t level;
  std::string_view indent;

  base_indent_context(int last_ch, uint32_t level, std::string_view indent)
      : last_ch(last_ch), level(level), indent(indent) {}

  void push_indent() { ++level; }
  void pop_indent() {
    if (level) --level;
  }

  int overflow(std::streambuf* streambuf, int ch) {
    if (last_ch == '\n' && ch != '\n') put_indent(streambuf);
    return streambuf->sputc(last_ch = ch);
  }

  void put_indent(std::streambuf* streambuf) const {
    for (uint32_t i = 0; i < level; ++i)
      streambuf->sputn(indent.data(), indent.size());
  }
};

template <typename indent_context_t>
class base_streambuf_wrapper : public std::streambuf {
  std::streambuf* mbase;
  indent_context_t indent_context;

 public:
  explicit base_streambuf_wrapper(std::streambuf* streambuf,
                                  indent_context_t indent_context)
      : mbase(streambuf), indent_context(indent_context) {}
  std::streambuf* base() const { return mbase; }

 protected:
  virtual int overflow(int ch) { return indent_context.overflow(mbase, ch); }
};

template <typename indent_context_t>
class base_ostream {
  std::ostream* os;
  base_streambuf_wrapper<indent_context_t> streambuf;

 public:
  explicit base_ostream(std::ostream& os, bool newline = true,
                        uint32_t level = 0, std::string_view indent = "  ")
      : os(&os),
        streambuf(os.rdbuf(),
                  indent_context_t{newline ? '\n' : '\0', level, indent}) {
    os.rdbuf(&streambuf);
  }
  virtual ~base_ostream() { os->rdbuf(streambuf.base()); }
};

template <typename indent_context_t>
class base_ostream_lifetime : public base_ostream<indent_context_t> {
 public:
  explicit base_ostream_lifetime(std::ostream& os, bool newline = true)
      : base_ostream<indent_context_t>(os, newline, 1, "  ") {}
};

template <typename indent_context_t>
class base_ostream_iterator
    : public std::iterator<std::output_iterator_tag, void, void, void, void> {
 private:
  std::ostream* os;
  indent_context_t indent_context;

 public:
  base_ostream_iterator(std::ostream& os)
      : os(&os), indent_context('\n', 0, "  ") {}
  // base_ostream_iterator(const base_ostream_iterator& other) = default;
  // base_ostream_iterator& operator=(const base_ostream_iterator&) = default;

  base_ostream_iterator& operator=(const char& value) {
    indent_context.overflow(os->rdbuf(), value);
    return *this;
  }

  base_ostream_iterator& operator*() { return *this; }
  base_ostream_iterator& operator++() { return *this; }
  base_ostream_iterator& operator++(int) { return *this; }
};

using indent_context = base_indent_context;
using ostream = base_ostream<indent_context>;
using ostream_lifetime = base_ostream_lifetime<indent_context>;
using ostream_iterator = base_ostream_iterator<indent_context>;

namespace ctrl {

namespace details {

static constexpr char esc = '\033';
static constexpr char push_ctrl = '>';
static constexpr char pop_ctrl = '<';

}  // namespace details

static constexpr char push[3] = {details::esc, details::push_ctrl, '\0'};
static constexpr char pop[3] = {details::esc, details::pop_ctrl, '\0'};

struct indent_context : public base_indent_context {
  bool is_indent = false;

  using base_indent_context::base_indent_context;

  int overflow(std::streambuf* streambuf, int ch) {
    if (is_indent) {
      is_indent = false;
      if (ch == details::push_ctrl) {
        push_indent();
        return ch;
      }
      if (ch == details::pop_ctrl) {
        pop_indent();
        return ch;
      }
      base_indent_context::overflow(streambuf, details::esc);
    }
    if (ch == details::esc) {
      is_indent = true;
      return ch;
    }
    return base_indent_context::overflow(streambuf, ch);
  }
};

using ostream = base_ostream<indent_context>;
using ostream_lifetime = base_ostream_lifetime<indent_context>;
using ostream_iterator = base_ostream_iterator<indent_context>;

}  // namespace ctrl

}  // namespace indent

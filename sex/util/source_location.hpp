#pragma once

#include <cstddef>
#include <string_view>
#include <ostream>

namespace sex::util {

class SourceLocation {
 public:
  SourceLocation() noexcept = default;

  // Should be defined in the same translation unit for consteval to work
  static consteval SourceLocation Current(
    size_t line = __builtin_LINE(),
    size_t column = __builtin_COLUMN(),
    std::string_view function = __builtin_FUNCTION(),
    std::string_view filename = __builtin_FILE()
    ) {
    return {line, column, function, filename};
  }

  [[nodiscard]] constexpr size_t Line() const;

  [[nodiscard]] constexpr size_t Column() const;

  [[nodiscard]] constexpr std::string_view Filename() const;

  [[nodiscard]] constexpr std::string_view Function() const;

 private:
  constexpr SourceLocation(
    size_t line,
    size_t column,
    std::string_view function_name,
    std::string_view filename)
    : line_(line),
      column_(column),
      function_name_(function_name),
      filename_(filename) {
  }

  size_t line_{0};
  size_t column_{0};
  std::string_view function_name_;
  std::string_view filename_;
};

std::ostream& operator<<(std::ostream& out, const SourceLocation& location);

}  // namespace sex::util

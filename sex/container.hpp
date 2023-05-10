#pragma once

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

namespace sex {

class Container {

public:
  Container() = delete;

  Container(const Container &) = delete;

  Container(Container &&) = delete;

  explicit Container(const std::string &name);

  Container(const std::string &name, const fs::path &where);

  void enter() const;

  void remove();

  [[nodiscard]] const fs::path &getPath() const;

  ~Container();

  static const fs::path TmpSbox;
private:
  const fs::path containerPath_;
  bool removed_{false};
};

}  // namespace sex

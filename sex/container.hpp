#pragma once

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

class Container {

 public:
  Container() = delete;

  Container(const Container&) = delete;

  Container(Container&&) = delete;

  explicit Container(const std::string& name);

  Container(
    const std::string& name,
    const fs::path& where);

  void Enter() const;

  [[nodiscard]] const fs::path& GetPath() const;

  ~Container();

  static const fs::path TmpSbox;
 private:
  const fs::path ContainerPath_;
};

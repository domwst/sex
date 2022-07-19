#pragma once

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

class Container {

public:
    Container() = delete;
    Container(const Container&) = delete;
    Container(Container&&) = delete;

    Container(const std::string& name);
    void Enter() const;
    const fs::path& GetPath() const;
    ~Container();

    static const fs::path TmpSbox;
private:
    const fs::path ContainerPath_;
};
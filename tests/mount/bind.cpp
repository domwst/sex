#include <sex/util/mount/bind.hpp>
#include <sex/container.hpp>

#include <boost/ut.hpp>
#include <fcntl.h>
#include "sex/execute.hpp"

using namespace sex;
using namespace boost::ut;

std::string ReadFile(const fs::path& path) {
  std::stringstream ss;
  ss << std::ifstream(path).rdbuf();
  return ss.str();
}

static suite bindMount = [] {
  "bind_directory_ro"_test = [] {
    Container c("bind_directory_ro");

    const fs::path textFile = c.getPath() / "test" / "test.txt";
    fs::create_directory(textFile.parent_path());
    SEX_ASSERT((std::ofstream(textFile) << "Test text here").good());

    auto status = sex::Execute([&c, &textFile] {
      const fs::path otherDir = c.getPath() / "test_2";
      util::DirectoryBindMount mnt(textFile.parent_path(), otherDir);
      mnt.mount().ensure();

      fs::path newTextPath = otherDir / "test.txt";

      SEX_ASSERT(ReadFile(newTextPath) == "Test text here");
      int fd = open(newTextPath.c_str(), O_WRONLY);
      SEX_ASSERT(fd == -1 && errno == EROFS);
    }, sex::ExecuteArgs{}.NewUserNS().NewMountNS()).wait();
    expect(status.isExited() && status.exitCode() == 0);
  };

  "bind_directory_rw"_test = [] {
    Container c("bind_directory_rw");

    const fs::path textFile = c.getPath() / "test" / "test.txt";
    fs::create_directory(textFile.parent_path());
    expect((std::ofstream(textFile) << "Test text here").good());

    auto status = sex::Execute([&c, &textFile] {
      const fs::path otherDir = c.getPath() / "test_2";
      util::DirectoryBindMount mnt(textFile.parent_path(), otherDir, util::DirectoryBindMount::ALLOW_WRITE);
      mnt.mount().ensure();

      SEX_ASSERT((std::ofstream(otherDir / "test.txt") << "Kek").good());
    },sex::ExecuteArgs{}.NewUserNS().NewMountNS()).wait();
    expect(status.isExited() && status.exitCode() == 0);
    expect(that % ReadFile(textFile) == std::string("Kek"));
  };

  "bind_file_ro"_test = [] {
    Container c("bind_file_ro");

    const fs::path textFile = c.getPath() / "test.txt";
    expect((std::ofstream(textFile) << "test").good());

    auto status = sex::Execute([&textFile] {
      const auto otherFile = textFile.parent_path() / "test2.txt";
      util::FileBindMount mnt(textFile, otherFile);
      mnt.mount().ensure();

      SEX_ASSERT(ReadFile(otherFile) == "test");

      int fd = open(otherFile.c_str(), O_WRONLY);
      SEX_ASSERT(fd == -1 && errno == EROFS);
    }, sex::ExecuteArgs{}.NewMountNS().NewUserNS()).wait();

    expect(status.isExited() && status.exitCode() == 0);
  };

  "bind_file_rw"_test = [] {
    Container c("bind_file_rw");

    const fs::path textFile = c.getPath() / "test.txt";
    expect((std::ofstream(textFile) << "test").good());

    auto status = sex::Execute([&textFile] {
      const auto otherFile = textFile.parent_path() / "test2.txt";
      util::FileBindMount mnt(textFile, otherFile, util::FileBindMount::ALLOW_WRITE);
      mnt.mount().ensure();

      SEX_ASSERT((std::ofstream(otherFile) << "lol").good());
    }, sex::ExecuteArgs{}.NewMountNS().NewUserNS()).wait();

    expect(status.isExited() && status.exitCode() == 0);
    expect(that % ReadFile(textFile) == std::string("lol"));
  };
};
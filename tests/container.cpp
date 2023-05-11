#include <sex/container.hpp>
#include <sex/execute.hpp>

#include <boost/ut.hpp>
#include <vector>
#include <fstream>

using namespace sex;
using namespace boost::ut;

namespace {

std::vector<fs::path> listDirectory(const fs::path& dir) {
  std::vector<fs::path> entries;
  for (const auto& entry : fs::directory_iterator(dir)) {
    entries.push_back(entry.path());
  }
  return entries;
}

}

static suite container = [] {
  "basic_container"_test = [] {

    auto path = fs::temp_directory_path() / "sbox" / "test";
    expect(!fs::exists(path));
    Container container("test");
    expect(that % container.getPath() == path);
    expect(fs::is_directory(path));

    expect(std::ofstream(path / "test.txt").good());

    auto status = sex::Execute([&container] {
      container.enter();
      assert(fs::current_path() == fs::path("/"));
      assert(listDirectory("/") == std::vector{fs::path("/test.txt")});
    }, sex::util::ExecuteArgs{}.NewUserNS()).wait();

    expect(that % status.isExited() && that % status.exitCode() == 0);

    expect(fs::is_directory(path));
    container.remove();
    expect(!fs::exists(path));
  };
};
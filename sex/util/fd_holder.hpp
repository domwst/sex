#pragma once

namespace sex {

class FdHolder {
 private:
  constexpr static int NeutralValue = -1;
  int fd_;

 public:
  explicit FdHolder(int fd) noexcept;

  FdHolder() noexcept;

  FdHolder(FdHolder&& other) noexcept;

  FdHolder(const FdHolder&) = delete;

  FdHolder& operator=(const FdHolder&) = delete;

  FdHolder& operator=(FdHolder&& other) noexcept;

  void Swap(FdHolder& other) noexcept;

  [[nodiscard]] int Get() const noexcept;

  void Reset(int new_fd = NeutralValue) noexcept;

  int Release() noexcept;

  ~FdHolder();
};

}

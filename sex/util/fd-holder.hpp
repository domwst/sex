#pragma once

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

    void swap(FdHolder& other) noexcept;
    [[nodiscard]] int get() const noexcept;
    void reset(int new_fd = NeutralValue) noexcept;
    int release() noexcept;

    ~FdHolder();
};

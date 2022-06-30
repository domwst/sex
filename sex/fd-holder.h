#pragma once

class FdHolder {
private:
    constexpr static int NeutralValue = -1;
    int fd_;

public:
    FdHolder(int fd);
    FdHolder();
    FdHolder(FdHolder&& other);
    FdHolder(const FdHolder&) = delete;

    FdHolder& operator=(const FdHolder&) = delete;
    FdHolder& operator=(FdHolder&& other);

    void swap(FdHolder& other);
    int get() const;
    void reset(int new_fd = NeutralValue);
    int release();

    ~FdHolder();
};
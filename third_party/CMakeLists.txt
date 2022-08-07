cmake_minimum_required(VERSION 3.5)

include(FetchContent)

# --------------------------------------------------------------------

# set(FETCHCONTENT_FULLY_DISCONNECTED ON)
# set(FETCHCONTENT_QUIET OFF)

# --------------------------------------------------------------------

FetchContent_Declare(
        ut
        GIT_REPOSITORY https://github.com/boost-ext/ut.git
        GIT_TAG v1.1.9
)
FetchContent_MakeAvailable(ut)


set(FMT_SYSTEM_HEADERS ON)
FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 9.0.0
)
FetchContent_MakeAvailable(fmt)

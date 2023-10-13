include(FetchContent)

FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG master
)
FetchContent_MakeAvailable(fmt)


find_package(Threads REQUIRED) # for pthread

add_executable(omlog main.cpp TextStreamReader.hpp defs.hpp utils.h)

target_link_libraries(omlog PRIVATE fmt::fmt)

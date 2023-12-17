include(FetchContent)

FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG master
)
FetchContent_MakeAvailable(fmt)


find_package(Threads REQUIRED) # for pthread

add_executable(l4h main.cpp TextStreamReader.hpp definitions.hpp utils.h
        Timeline.hpp
        ObserverAdapter.hpp
        LogRecord.hpp)

target_link_libraries(l4h PRIVATE fmt::fmt)

add_subdirectory(tests)
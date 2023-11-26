#define CONFIG_CATCH_MAIN

#include <catch2/catch_test_macros.hpp>

TEST_CASE("creation", "[header]") {

    REQUIRE(2==2);
}
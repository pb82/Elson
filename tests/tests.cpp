#define CATCH_CONFIG_MAIN
#include "./Catch/catch.hpp"
#include "../include/Elson.hpp"

using namespace JSON;

TEST_CASE( "base/types", "Basic JSON Data types" ) {
    Value val = 5;
    REQUIRE(val.is(JSON_NUMBER));
    REQUIRE(val.as<std::string>() == "5");
    
    val = "5";
    REQUIRE(val.is(JSON_STRING));
    REQUIRE(val.as<std::string>() == "5");
}

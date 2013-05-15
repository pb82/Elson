#define CATCH_CONFIG_RUNNER
#include <iostream>
#include "./Catch/catch.hpp"
#include "../include/Elson.hpp"

using namespace JSON;

TEST_CASE( "base/types", "Basic JSON Data types" ) {
    Value val = 5;
    REQUIRE(val.is(JSON_NUMBER));
    REQUIRE(val.as<std::string>().compare("5") == 0);
    
    val = 3.0;
    REQUIRE(val.is(JSON_NUMBER));
    REQUIRE(val.as<std::string>().compare("3") == 0);
    
    val = 1L;
    REQUIRE(val.is(JSON_NUMBER));
    REQUIRE(val.as<std::string>().compare("1") == 0);
    
    val = 7u;
    REQUIRE(val.is(JSON_NUMBER));
    REQUIRE(val.as<std::string>().compare("7") == 0);
    
    val = -7.5;
    REQUIRE(val.is(JSON_NUMBER));
    REQUIRE(val.as<std::string>().compare("-7.5") == 0);
    
    val = -.05;
    REQUIRE(val.is(JSON_NUMBER));
    REQUIRE(val.as<std::string>().compare("-0.05") == 0);

    val = 1E3;
    REQUIRE(val.is(JSON_NUMBER));
    REQUIRE(val.as<std::string>().compare("1000") == 0);
    
    val = -7.5;
    REQUIRE(val.is(JSON_NUMBER));
    REQUIRE(val.as<unsigned int>() == 7);
    
    val = "5";
    REQUIRE(val.is(JSON_STRING));
    REQUIRE(val.as<int>() == 5);
    
    val = "5L";
    REQUIRE(val.is(JSON_STRING));
    REQUIRE(val.as<long>() == 5L);
    
    
    val = "\\";
    REQUIRE(val.is(JSON_STRING));
    REQUIRE(val.as<std::string>().compare("\\") == 0);
    
    val = true;
    REQUIRE(val.is(JSON_BOOL));
    REQUIRE(val.as<bool>());
    REQUIRE(val.as<int>() == 1);
    REQUIRE(val.as<std::string>() == "true");
    
    val = false;
    REQUIRE(val.is(JSON_BOOL));
    REQUIRE(!val.as<bool>());
    REQUIRE(val.as<int>() == 0);
    REQUIRE(val.as<std::string>() == "false");
    
    val = { true, false };
    REQUIRE(val[0].is(JSON_BOOL));
    REQUIRE(val[1].is(JSON_BOOL));
    REQUIRE(val.is(JSON_ARRAY));
    REQUIRE(val[0].as<bool>());
    REQUIRE(!val[1].as<bool>());

    val = { 1, .5, "hello world", true, false, null };
    REQUIRE(val[0].is(JSON_NUMBER));
    REQUIRE(val[1].is(JSON_NUMBER));
    REQUIRE(val[2].is(JSON_STRING));
    REQUIRE(val[3].is(JSON_BOOL));
    REQUIRE(val[4].is(JSON_BOOL));
    REQUIRE(val[5].is(JSON_NULL));
    
    val = { {1,0,0}, {0,1,0},{0,0,1} };
    REQUIRE(val.is(JSON_ARRAY));
    REQUIRE(val.as<Array>().size() == 3);
    REQUIRE(val[0].is(JSON_ARRAY));
    REQUIRE(val[1].is(JSON_ARRAY));
    REQUIRE(val[2].is(JSON_ARRAY));
    REQUIRE(val[0].as<Array>().size() == 3);
    REQUIRE(val[1].as<Array>().size() == 3);
    REQUIRE(val[2].as<Array>().size() == 3);
    REQUIRE(val[0][0].is(JSON_NUMBER));
    REQUIRE(val[0][1].is(JSON_NUMBER));
    REQUIRE(val[0][2].is(JSON_NUMBER));
    REQUIRE(val[1][0].is(JSON_NUMBER));
    REQUIRE(val[1][1].is(JSON_NUMBER));
    REQUIRE(val[1][2].is(JSON_NUMBER));
    REQUIRE(val[2][0].is(JSON_NUMBER));
    REQUIRE(val[2][1].is(JSON_NUMBER));
    REQUIRE(val[2][2].is(JSON_NUMBER));
    
    std::string key = "property";
    val = Object { { "property", "value" } };
    REQUIRE(val.is(JSON_OBJECT));
    REQUIRE(val["property"].is(JSON_STRING));
    REQUIRE(val[key].is(JSON_STRING));
    REQUIRE(val[std::string("property")].is(JSON_STRING));
    
    REQUIRE(val["property"].as<std::string>().compare("value") == 0);
    
    val["property"] = 42;
    REQUIRE(val["property"].is(JSON_NUMBER));
    REQUIRE(val[key].as<unsigned int>() == 42);
}

TEST_CASE( "base/parse", "Basic parsing") {
    Parser p;
    Printer printer;
    Value val;

    REQUIRE_THROWS(p.parse(val, "[]a"));
    REQUIRE_THROWS(p.parse(val, "a[]"));
    

    REQUIRE_NOTHROW(p.parse(val, ""));
    REQUIRE(val.is(JSON_NULL));

    auto numbers = {"0", "1", "1E3", "1e-3", "0.5", "-.05e-07"};
    
    for (auto number: numbers) {
        REQUIRE_NOTHROW(p.parse(val, number));
        REQUIRE(val.is(JSON_NUMBER));
    }
    
    REQUIRE_THROWS(p.parse(val, "-0.5w4"));
    
    std::vector<std::string> strings = {
        "\"\"",
        "\"0\"", 
        "\"hello world\"", 
        "\"äöüȩéáã\""
    };
    
    for (int i = 0; i < strings.size(); i++) {
        REQUIRE_NOTHROW(p.parse(val, strings[i]));
        REQUIRE(printer.print(val).compare(strings[i]) == 0);
    }
    
    std::map<std::string, std::string> escapes = {
        { "\"\\\\\"", "\"\\\"" },
        { "\"\\/\"", "\"/\"" },
        { "\"\\u5022\"", "\"倢\"" }
    };
    
    for (auto pair: escapes) {
        REQUIRE_NOTHROW(p.parse(val, pair.first));
        REQUIRE(printer.print(val).compare(pair.second) == 0);
    }    
    
    REQUIRE_NOTHROW(p.parse(val, "true"));
    REQUIRE(val.is(JSON_BOOL));
    REQUIRE(val.as<bool>());
    REQUIRE(printer.print(val).compare("true") == 0);
    
    REQUIRE_NOTHROW(p.parse(val, "false"));
    REQUIRE(val.is(JSON_BOOL));
    REQUIRE(!val.as<bool>());
    REQUIRE(printer.print(val).compare("false") == 0);
    
    REQUIRE_NOTHROW(p.parse(val, "null"));
    REQUIRE(val.is(JSON_NULL));
    REQUIRE(printer.print(val).compare("null") == 0);
    
    REQUIRE_NOTHROW(p.parse(val, "[]"));
    REQUIRE(val.is(JSON_ARRAY));
    REQUIRE(printer.print(val).compare("[]") == 0);
    REQUIRE(val.as<Array>().size() == 0);
    
    REQUIRE_NOTHROW(p.parse(val, "[ 1, 2,   4   ]"));
    REQUIRE(val.is(JSON_ARRAY));
    REQUIRE(printer.print(val).compare("[1,2,4]") == 0);
    REQUIRE(val.as<Array>().size() == 3);
    
    REQUIRE_THROWS(p.parse(val, "["));
    REQUIRE_THROWS(p.parse(val, "]"));
    REQUIRE_THROWS(p.parse(val, "[["));
    REQUIRE_THROWS(p.parse(val, "]]"));    
    REQUIRE_THROWS(p.parse(val, "[,]"));
    REQUIRE_THROWS(p.parse(val, "[][]"));
    REQUIRE_THROWS(p.parse(val, "[1,]"));
    REQUIRE_THROWS(p.parse(val, "[,1]"));
    REQUIRE_THROWS(p.parse(val, "[1,2"));
    REQUIRE_THROWS(p.parse(val, "1,2]"));
    REQUIRE_THROWS(p.parse(val, "[[],[]"));
    REQUIRE_THROWS(p.parse(val, "[[],]]"));
    REQUIRE_THROWS(p.parse(val, "[[1,2,],[]]"));
    REQUIRE_THROWS(p.parse(val, "[[1,2,] []]"));
    
    REQUIRE_NOTHROW(p.parse(val, "[[]]"));
    REQUIRE_NOTHROW(p.parse(val, "[[[]]]"));
    REQUIRE_NOTHROW(p.parse(val, "[[],[]]"));
    REQUIRE_NOTHROW(p.parse(val, "[[[]],[[]]]"));
    REQUIRE_NOTHROW(p.parse(val, "[[[1]],[[2]]]"));
    REQUIRE_NOTHROW(p.parse(val, "[[[1,null,true,false,\"\"]],[[]]]"));
    REQUIRE(val[0][0][0].is(JSON_NUMBER));
    REQUIRE(val[0][0][1].is(JSON_NULL));
    REQUIRE(val[0][0][2].is(JSON_BOOL));
    REQUIRE(val[0][0][3].is(JSON_BOOL));
    REQUIRE(val[0][0][4].is(JSON_STRING));
    REQUIRE(val[0][0].as<Array>().size() == 5);
    REQUIRE(printer.print(val).compare("[[[1,null,true,false,\"\"]],[[]]]") == 0);
    
    val[0][0].as<Array>().push_back(null);
    REQUIRE(val[0][0].as<Array>().size() == 5);

    val[0][0].asMutable<Array>().push_back(null);
    REQUIRE(val[0][0].as<Array>().size() == 6);
    
    REQUIRE_NOTHROW(p.parse(val, "{}"));
    REQUIRE_THROWS(p.parse(val, "{1}"));
    REQUIRE_THROWS(p.parse(val, "{{}}"));
    REQUIRE_NOTHROW(p.parse(val, "{\"a\": \"b\"}"));
    REQUIRE(val["a"].as<std::string>().compare("b") == 0);
    REQUIRE_THROWS(val.as<double>());
    
    REQUIRE_NOTHROW(p.parse(val, "[{}]"));
    REQUIRE_NOTHROW(p.parse(val, "[{},{}]"));
    REQUIRE_NOTHROW(p.parse(val, "[{},[]]"));
    REQUIRE_NOTHROW(p.parse(val, "[[[{}]]]"));
    REQUIRE_THROWS(p.parse(val, "[[[{}]]"));
    REQUIRE_THROWS(p.parse(val, "[{}]]"));
    REQUIRE_THROWS(p.parse(val, "{[]}"));
}

TEST_CASE( "base/unicode", "Unicode escape handling") {
    Parser p;
    Printer printer;
    Value val;

    std::map<std::string, std::string> escapes = {
        { "\"\\u0041\"", "\"A\"" },
        { "\"\\u0042\"", "\"B\"" },
        { "\"\\u0043\"", "\"C\"" },
        { "\"\\u0044\"", "\"D\"" },
        { "\"\\u0045\"", "\"E\"" },
        { "\"\\u0046\"", "\"F\"" },
        { "\"\\u0047\"", "\"G\"" },
        { "\"\\u0048\"", "\"H\"" },
        { "\"\\u0049\"", "\"I\"" },
        { "\"\\u0041\\u0042\\u0043\"", "\"ABC\"" },
        { "\"\\u0041B\\u0043\"", "\"ABC\"" },
        { "\"A\\u0042\\u0043\"", "\"ABC\"" },
        { "\"\\u0041\\u0042C\"", "\"ABC\"" },
        { "[\"\\u0041\\u0042C\"]", "[\"ABC\"]" },
        { "\"\\u0041\\u004212\"", "\"AB12\"" },
    };
        
    for (auto pair: escapes) {
        REQUIRE_NOTHROW(p.parse(val, pair.first));
        REQUIRE(printer.print(val).compare(pair.second) == 0);
    }    
    
    REQUIRE_THROWS(p.parse(val, "\"\\u0fg\""));
    REQUIRE_THROWS(p.parse(val, "\"\\u00\""));
    REQUIRE_THROWS(p.parse(val, "\"\\u\""));
}

TEST_CASE( "utils/base", "Unicode escape handling") {
    Value val = Object {
        { "a", 1 },
        { "b", 2 },
        { "c", 3}
    };
    
    PropertyList properties = traverse(val);
    REQUIRE(properties.size() == 3);    
    
    PropertyList list = filter(traverse(val), 
        [] (std::string name, Value& value) -> bool {
            return name.compare("b") == 0;
        });
        
    REQUIRE(list.size() == 1);
}

int main (int argc, char* const argv[]) {
     exit(Catch::Main( argc, argv ));
     return 0;
}

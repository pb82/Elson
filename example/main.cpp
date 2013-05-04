#include <iostream>
#include "../include/Elson.hpp"

int main () {
    using namespace JSON;
    Value val;
    Parser parser;
    PrettyPrinter printer;
  
    std::string json_source;
    for (std::string line; std::getline(std::cin, line);) {
        json_source += line;
        // Insert newlines to allow the parser to
        // print the line number of an eventual
        // error
        json_source += '\n';
    }
    
    parser.parse(val, json_source);
    std::cout << val["fathers"][0]["name"].as<std::string>() << std::endl;
}

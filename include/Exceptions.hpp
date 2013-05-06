#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace JSON {
    /*
     * Exception for unexpected end of parse input
     */
    class UnexpectedEndOfInputException : public std::runtime_error {
    public:
        UnexpectedEndOfInputException(int line) 
        : std::runtime_error("") {
            std::stringstream ss;
            ss << "Unexpected end of input in line " << line;
            static_cast<std::runtime_error&>(*this) = 
              std::runtime_error(ss.str());
        }
    };    

    /*
     * Parsing terminated but still data in the stream
     */
    class UnexpectedCharactersException : public std::runtime_error {
    public:
        UnexpectedCharactersException()
        : std::runtime_error("Unexpected characters at end of input") { }
    };    
    
    /*
     * JSON syntax error
     */
    class ParseException : public std::runtime_error {
    public:
        ParseException(int line)
        : std::runtime_error("") {
            std::stringstream ss;
            ss << "Syntax error in line " << line;
            static_cast<std::runtime_error&>(*this) = 
              std::runtime_error(ss.str());
        }
    };
}

#endif // EXCEPTIONS_H

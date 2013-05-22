#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace JSON {
      // Possible JSON types
    enum JsonType {
        JSON_STRING = 0,
        JSON_NUMBER = 1,
        JSON_BOOL =   2,
        JSON_ARRAY =  3,
        JSON_OBJECT = 4,
        JSON_NULL
    };
  
    // Human readable typenames
    static std::map<JsonType, std::string> typenames = {
        {JSON_STRING, "string"},
        {JSON_NUMBER, "number"},
        {JSON_BOOL,   "boolean"},
        {JSON_ARRAY,  "array"},
        {JSON_OBJECT, "object"},
        {JSON_NULL,   "null"}
    };

    /*
     * Occurs when trying to convert a value to a type
     * for which no conversion is known.
     */
    class ConversionException : public std::runtime_error {
        public:
            ConversionException(JsonType from, std::string& to) 
            : std::runtime_error("") {
                std::stringstream ss;
                ss
                    << "No conversion from '"
                    << typenames[from]
                    << "' to '"
                    << to
                    << "' known.";
                    
                static_cast<std::runtime_error&>(*this) = 
                std::runtime_error(ss.str());
            }        
    };
    
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
    
        // Invalid code point in utf8 sequence.
    class InvalidCodePointException : public std::runtime_error {
    public:
        InvalidCodePointException(int codePoint)
        : std::runtime_error("") {
            std::stringstream ss;
            ss << "Invalid code point " << codePoint;
            static_cast<std::runtime_error&>(*this) = 
              std::runtime_error(ss.str());
        }
    };

}

#endif // EXCEPTIONS_H

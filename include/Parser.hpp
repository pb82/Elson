#ifndef PARSER_H
#define PARSER_H

#include <stack>
#include <ctype.h>
#include <stdint.h>

#include "utf8.h"
#include "Value.hpp"

namespace JSON {
    // JSON escape characters that may appear
    // after a reverse solidus
    const char ESC_REVERSE_SOLIUDS  = 92;
    const char ESC_QUOTATION_MARK   = 34;
    const char ESC_SOLIDUS          = 47;
    const char ESC_BACKSPACE        = 98;
    const char ESC_FORMFEED         = 102;
    const char ESC_NEWLINE          = 110;
    const char ESC_CARRET           = 114;
    const char ESC_HORIZONTAL_TAB   = 116;
    const char ESC_UNICODE          = 117;
    
    // Use 32 bit characters for unicode strings
    typedef std::basic_string<int32_t> wideString;

    // Represents a JSON parser
    class Parser {
        public:
            Parser() : parseIndex(0), lineNumber(1) { }

            void parse(Value& object, const std::string& source) throw(std::exception);
            void parse(Value& object, const char * source) throw(std::exception);

        private:
            void reset() {
                lineNumber = 1;
                parseIndex = 0;
                while(!objectStack.empty()) {
                    objectStack.pop();
                }
            }
        
            // Increment the parse index until a non-whitespace character
            // is encountered.
            void clearWhitespace() {
                while (hasNext() && isspace(source.at(parseIndex))) {
                    if (peek() == 10 || peek() == 12 || peek() == 13) {
                        lineNumber++;
                    }
                    parseIndex++;
                }
            }

            // Increase the parse index but ignore the current character
            void consume() {
                parseIndex++;
            }

            // End of Stream reached?
            bool hasNext() const {
                return parseIndex < source.length();
            }
                
            // Return a reference to the current character in the
            // stream and increase the index.
            char& next() throw(UnexpectedEndOfInputException) {
                if (!hasNext()) {
                    // End of Stream already reached?
                    throw UnexpectedEndOfInputException(lineNumber);
                }
                // Increase the line number. This is used to print
                // the position when a parse error occurs.
                if (peek() == '\n') {
                    lineNumber++;
                }

                return source.at(parseIndex++);
            }

            // Return a reference to the current character in the
            // stream without increasing the index.
            char& peek() throw(UnexpectedEndOfInputException) {
                if (!hasNext()) {
                    // End of Stream already reached?
                    throw UnexpectedEndOfInputException(lineNumber);
                }
                return source.at(parseIndex);
            }

            // Return a reference to the top item on the stack.
            // The top item usually holds a reference to the 
            // current array / object at the parse position.
            Value& top() {
                return *objectStack.top();
            }

            // Store a parsed value and return a reference
            // to it
            Value& store(Value&& val) {
                if (top().is(JSON_ARRAY)) {
                    // Current parse position is inside an array:
                    // Append the new item to the end of the array
                    top().push_back(val);
                    return top().asMutable<Array>().back();
                } else if (top().is(JSON_OBJECT)) {
                    // Current parse position is inside an object:
                    // Read the current property and store the new item
                    // under this property inside the current object.
                    top()[currentProperty.str().c_str()] = val;
                    return top()[currentProperty.str().c_str()];
                } else {
                    // Parse position is not inside an array and not
                    // inside an object. Put the new item on the top
                    // of the stack.
                    top() = val;
                    return top();
                }
            }

            // Tests if a character is allowed for numeric expressions.
            bool validNumericChar(char code) {
                return (code >= 48 && code <= 57)
                    || (code == 46)     // '.'
                    || (code == 45)     // '-'
                    || (code == 43)     // '+'
                    || (code == 69)     // 'E'                    
                    || (code == 101);   // 'e'
            }

            // Tests if a character is allowed to introduce a numeric
            // expression.
            bool validNumericStartingChar(char code) {
                return (code >= 48 && code <= 57)
                    || (code == 45);    // '-'
            }

            // Valid digit within an \u2360 unicode escape?
            bool validHexDigit(char code) {
                return (code >= 48 && code <= 57)
                    || (code >= 65 && code <= 70)
                    || (code >= 97 && code <= 102);
            }

            void parseObject()      throw(std::exception);
            void parseProperty()    throw(std::exception);
            void parseValue()       throw(std::exception);
            void parseString()      throw(std::exception);
            void parseArray()       throw(std::exception);
            void parseBoolean()     throw(std::exception);
            void parseNumber()      throw(std::exception);
            void parseNull()        throw(std::exception);
            void escapeChar()       throw(std::exception);
            void readUTF8Escape()   throw(std::exception);
                        
            unsigned int parseIndex;
            unsigned int lineNumber;

            std::string source;
            std::ostringstream currentProperty;
            std::ostringstream currentString;
            
            // Since std::stack<Value&> is not possible use
            // a pointer here
            std::stack<Value *> objectStack;
    };

    /**
     * { ... }
     */
    void Parser::parseObject() throw(std::exception) {
        clearWhitespace();
        if (next() == '{') {
            clearWhitespace();
            
            // Push a new object on the stack
            objectStack.push(&store(Object {}));
            
            if (peek() != '}') {
                // Parse object properties
                while (hasNext()) {
                    parseProperty();
                    clearWhitespace();

                    if (hasNext() && peek() != ',') {
                        break;
                    } else {
                        // Another property to parse. Get rid
                        // of the ',' and go on...
                        consume(); // ','
                    }
                }
            }
            
            // End of properties
            clearWhitespace();
            
            if (peek() == '}') {
                consume(); // '}'
                // Done with parsing the object.
                // Pop it from the stack, since this is no longer
                // the reference object for eventual coming items.
                objectStack.pop();
                return;
            } else {
                // Objects must end with ...}
                throw ParseException(lineNumber);
            }

        } else {
            // Objects have to start with {...
            throw ParseException(lineNumber);
        }
    }

    /**
     * ...:
     */
    void Parser::parseProperty() throw(std::exception) {
        // Reset currentProperty buffer
        currentProperty.str(""); 
        clearWhitespace();
        
        // Properties must start with '"'
        if (peek() == ESC_QUOTATION_MARK) {
            consume(); // '"'
            while (peek() != ESC_QUOTATION_MARK) {
                currentProperty << next();
            }
            
            // Properties must end with '"'
            if (next() != ESC_QUOTATION_MARK) {
                throw ParseException(lineNumber);
            } else {
                clearWhitespace();
                // ...": ...
                // Properties must be in the form of
                // "key": value
                if (peek() != ':') {
                    throw ParseException(lineNumber);
                } else {
                    consume(); // ':'
                    // Parse the value
                    // :... 
                    parseValue();
                }
            }
        } else {
            throw ParseException(lineNumber);
        }
    }

    /**
     * :...
     */
    void Parser::parseValue() throw(std::exception) {
        clearWhitespace();
        
        // Decide the type of the value on the stream
        switch(peek()) {
            case '{':
                parseObject();
                return;
            case '"':
                parseString();
                return;
            case '[':
                parseArray();
                return;
            case 'n':
              parseNull();
              return;
            case 't':
            case 'f':
                parseBoolean();
                return;
            default:
                // If none of the former types matched always try to
                // parse a number.
                if (validNumericStartingChar(peek())) {
                    parseNumber();
                    return;
                } else {
                    throw ParseException(lineNumber);
                }
        }
    }

    /**
     * null
     */
    void Parser::parseNull() throw(std::exception) {
      currentString.str("");
      
      // Read the next four characters and test if they
      // are equal to 'null'
      for (int i=0;i<4;i++) {
          currentString << next();
      }
      
      if (currentString.str().compare("null") == 0) {
          store(Value());
      } else {
        throw ParseException(lineNumber);
      }
    }

    /**
     * numbers
     */
    void Parser::parseNumber() throw(std::exception) {
        currentString.str("");
        while (hasNext() && validNumericChar(peek())) {
            currentString << next();
        }

        store(fromString<double>(currentString.str()));
    }

    /**
     * true | false 
     */
    void Parser::parseBoolean() throw(std::exception) {
        currentString.str("");
        // consume lowercase letters
        while (hasNext() && peek() >= 97 && peek() <= 122) {
            currentString << next();
        }

        bool result;
        if (currentString.str().compare("true") == 0) {
            result = true;
        } else if (currentString.str().compare("false") == 0) {
            result = false;
        } else {
            throw ParseException (lineNumber);
        } 
        
        store(result);
    }

    /**
     * "..."
     */
    void Parser::parseString() throw(std::exception) {
        // Reset string buffer
        currentString.str("");
        consume(); // '"'
        while (peek() != ESC_QUOTATION_MARK) {
            // String contains an escaped character?
            if (peek() == ESC_REVERSE_SOLIUDS) {
                escapeChar();
            } else {
                currentString << next();
            }
        }

        consume(); // '"'
        store(currentString.str());
    }

    /**
     * \...
     */
    void Parser::escapeChar() throw(std::exception) {
      consume(); // REVERSE_SOLIDUS
      // Decide which escape character follows
      switch(peek()) {
          case ESC_BACKSPACE:
              consume();
              currentString << (char) 8;
              break;
          case ESC_HORIZONTAL_TAB:
              consume();
              currentString << (char) 9;
              break;
          case ESC_NEWLINE:
              consume();
              currentString << (char) 10;
              break;
          case ESC_FORMFEED:
              consume();
              currentString << (char) 12;
              break;
          case ESC_CARRET:
              consume();
              currentString << (char) 13;
              break;
          case ESC_QUOTATION_MARK:
          case ESC_REVERSE_SOLIUDS:
          case ESC_SOLIDUS:
              currentString << next();
              break;
          case ESC_UNICODE:
              // a \u occured
              // read the following sequence and insert it's
              // unicode representation into the string.
              readUTF8Escape();            
              break;
          default:
              throw ParseException(lineNumber);
      }
    }

    void Parser::readUTF8Escape() throw(std::exception) {
        consume(); // u
        int codePoint = -1;
        std::string tmp = "    ";
        std::stringstream ss;
        std::string result;
        wideString str32;
        
        for (int index=0;index<4;index++) {
            if (!hasNext() || !validHexDigit(peek())) {
              throw ParseException(lineNumber);
            }
            
            tmp[index] = next();
        }
        
        ss << std::hex << tmp;
        ss >> codePoint;
        str32.push_back(codePoint);
        utf8::utf32to8(str32.begin(), str32.end(), std::back_inserter(result));
        currentString << result;
    }

    /**
     * [...]
     */
    void Parser::parseArray() throw(std::exception) {
        consume(); // '['
                
        objectStack.push(&store(Array {}));

        // Empty array?
        if (peek() == ']') {
            consume(); // ']'
            objectStack.pop();
            return;
        }
                
        while(hasNext()) {                        
            clearWhitespace();

            parseValue();
            clearWhitespace();
            
            if (peek() != ',') {
                break;
            } else {
                consume(); // ','
            }
        }
        
        if (peek() != ']') {
            throw ParseException(lineNumber);
        } else {
            consume(); // ']'
        }
                
        objectStack.pop();
    }

    /**
     * Entry points
     */
    inline void Parser::parse(Value& value, const std::string &source) 
    throw(std::exception) {
        reset();
        if (source.length() > 0) {
            value = null;
            this->source = source;
            objectStack.push(&value);
            parseValue();
            if (parseIndex < (source.length()-1)) {
                throw UnexpectedCharactersException();
            }
        }
    }

    inline void Parser::parse(Value& value, const char *source) 
    throw(std::exception) {
        std::string _source(source);
        parse(value, _source);
    }
}

#endif // PARSER_H

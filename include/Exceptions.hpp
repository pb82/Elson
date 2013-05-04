#include <iostream>
#include <sstream>
#include <exception>

namespace JSON {
    /*
     * Exception for unexpected end of parse input
     */
    class UnexpectedEndOfInputException : public std::exception {
    public:
        UnexpectedEndOfInputException(unsigned int lineNumber)
          : lineNumber(lineNumber) {
        }
        
        virtual const char* what() const throw() {
            std::ostringstream ss;
            ss 
              << "Unexpected end of input in line "
              << lineNumber;
              
            return ss.str().c_str();
        }
    private:
        unsigned int lineNumber;
    };    

    /*
     * Parsing terminated but still data in the stream
     */
    class TrailingCharactersException : public std::exception {
    public:
        virtual const char* what() const throw() {
            std::ostringstream ss;
            ss 
              << "Trailing characters in stream";
              
            return ss.str().c_str();
        }
    };    
    
    /*
     * On error throw an exception of the form:
     * Expected X but found Y
     */
    class ParseException : public std::exception {
        public:
            ParseException(
              const char *expected, 
              const char* found,
              unsigned int lineNumber) : lineNumber(lineNumber) {
              setReason(expected, found);
            }
        
            ParseException(
              const char *expected, 
              char found,
              unsigned int lineNumber) : lineNumber(lineNumber) {
              const char* f = &found;
              setReason(expected, f);
            }

            virtual ~ParseException() throw() { }

            virtual const char *what() const throw() {
                return reason.c_str();
            }
        private:
            unsigned int lineNumber;
        
            void setReason(const char* expected, const char* found) {
                std::ostringstream ss;
                ss
                    << "Expected '"
                    << expected
                    << "' but found '"
                    << found
                    << "' in line "
                    << lineNumber;

                reason = ss.str();
            }
            
            std::string reason;
    };
}

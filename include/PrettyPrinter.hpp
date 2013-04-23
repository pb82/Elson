#ifndef PRETTYPRINTER_H
#define PRETTYPRINTER_H

#include "Printer.hpp"

namespace JSON {
    static unsigned int default_indent = 4;
  
    class PrettyPrinter : public Printer {
    public:
        PrettyPrinter(unsigned int indent=default_indent)
            : indentDepth(indent),
              currentIndent(0) { }
    private:
        void printArray(Value& value, std::ostringstream &out);
        void printObject(Value& value, std::ostringstream &out);
        void printIndent(std::ostringstream& out);

        unsigned int indentDepth;
        unsigned int currentIndent;
    };

    void PrettyPrinter::printIndent(std::ostringstream &out) {
        for (unsigned int i = 0; i < currentIndent; ++i) {
            out << " ";
        }
    }

    void PrettyPrinter::printArray(Value& value, std::ostringstream &out) {
        bool firstItem = true;
        out << "[";
        for (auto item : value.as<Array>()) {
            if (!firstItem) { out << ", "; }
            dispatchType(item, out);
            firstItem = false;
        }
        out << "]";
    }

    void PrettyPrinter::printObject(Value& value, std::ostringstream &out) {
        currentIndent += indentDepth;
        out << "{\n";
        bool firstLine = true;
        for (auto pair : value.as<Object>()) {
            if (!firstLine) { out << ",\n"; }
            printIndent(out);
            out << "\"";
            out << pair.first;
            out << "\"";
            out << ": ";
            dispatchType(pair.second, out);
            firstLine = false;
        }
        currentIndent -= indentDepth;
        out << "\n";
        printIndent(out);
        out << "}";
    }
}

#endif // PRETTYPRINTER_H

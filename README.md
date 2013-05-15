Elson [![Build Status](https://travis-ci.org/pb82/Elson.png?branch=master)](https://travis-ci.org/pb82/Elson)
=====

A C++11 JSON Parser/Serializer that aims to come as close to literal JSON as
possible. Header only. Tested with g++ 4.7.2

--- This is a work in progress. It has not been tested extensively ---

Writing JSON
-------------

```c++
#include <iostream>
#include "Elson.hpp"

int main() {
    using namespace JSON;
    Value val;
    
    val["Name"] = "Homer";
    val["Family"]["Wife"] = "Marge";
    val["Family"]["Kids"] = { "Bart", "Lisa", "Maggie" };
    val["Age"] = 40;

    PrettyPrinter printer;
    std::cout << printer.print(val) << std::endl;

    return 0;
}
```

Since a JSON::Object is just a typedef'd std::map you may also use map's
initializer list constructor and write:

```c++
#include <iostream>
#include "Elson.hpp"

int main() {
    using namespace JSON;
    Value val = Object {
        { "Name",   "Homer" },
        { "Family", Object {
            { "Wife", "Marge" },
            { "Kids", { "Bart", "Lisa", "Maggie" } }
          }
        },
        { "Age", 40 }
    };
    
    PrettyPrinter printer;    
    std::cout << printer.print(val) << std::endl;

    return 0;
}
```

Almost JSON Literals. Nested Arrays may be written as:

```c++

Value val = { {1,0,0}, {0,1,0}, {0,0,1} };

```


Reading JSON
-------------

```c++
#include <iostream>
#include "Elson.hpp"

int main() {
    using namespace JSON;
    Value val;
    Parser p;
    
    p.parse(val, "{\"Answer\": \"42\"}");
    
    double answer = val["Answer"].as<double>();
    
    std::cout << answer << std::endl;

    return 0;
}
```

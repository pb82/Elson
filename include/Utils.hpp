#ifndef UTILS_H
#define UTILS_H

#include <functional>

#include "Value.hpp"

namespace JSON {
    typedef std::pair<std::string, Value *> Property;
    typedef std::vector<Property> PropertyList;
    
    PropertyList traverse(Value& object) {
        PropertyList list;
        if (!object.is(JSON_OBJECT)) {
            return list;
        }
        
        for (auto p: object.asMutable<Object>()) {
            list.push_back(Property {p.first, &p.second});
        }
        
        return list;
    }
    
    PropertyList filter(
        PropertyList&& properties,
        std::function<bool(std::string, Value&)> filterFunction) {
        
        PropertyList list;
        for (auto p: properties) {
            Value& candidate = *p.second;
            if (filterFunction(p.first, candidate)) {
                list.push_back(p);
            }
        }
        
        return list;
    }
}

#endif // UTILS_H

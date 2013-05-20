#ifndef UTILS_H
#define UTILS_H

#include <functional>

#include "Value.hpp"

namespace JSON {
    typedef std::pair<std::string, Value *> Property;
    typedef std::vector<Property> PropertyList;
    
    PropertyList& list(
        PropertyList& properties,
        Value& object) {
        if (!object.is(JSON_OBJECT)) {
            return properties;
        }
        
        for (auto p: object.asMutable<Object>()) {
            properties.push_back(Property {p.first, &p.second});
        }
        
        return properties;
    }

	PropertyList& traverse(
		PropertyList& properties,
		Value& object) {
		if (!object.is(JSON_OBJECT)) {
		    return properties;
		}

        for (auto p: object.asMutable<Object>()) {
            if (p.second.is(JSON_OBJECT)) {
                traverse(properties, p.second);
            } else {
                properties.push_back(Property {p.first, &p.second});
            }
        }
	}

    PropertyList filter(
        PropertyList& properties,
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

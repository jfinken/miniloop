/**
 * Copyright (C) 2022 Osmozis SA - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#ifndef SRC_JSON_JSON_H_
#define SRC_JSON_JSON_H_

#include <algorithm>
#include <string>

struct json_object;
struct lh_entry;

namespace JSON {

class Object {
public:
    static Object fromFile(const std::string& path);
    static Object fromStr(const std::string& str);

    Object() = default;
    Object(const Object& o);
    Object(Object&& o);
    ~Object();

    Object& operator=(const Object& o);

    bool isValid() const;
    explicit operator bool() const { return isValid(); }

    bool isInt() const;
    bool isString() const;
    bool isObject() const;
    bool isArray() const;
    bool isBoolean() const;

    Object operator[](const std::string& arg) const;

    int asInt() const;
    const char* asString() const;
    bool asBoolean() const;

    class iterator : public std::iterator<std::input_iterator_tag, Object> {
        friend Object;

    public:
        iterator& operator++();
        iterator operator++(int)
        {
            iterator retval = *this;
            ++(*this);
            return retval;
        }
        bool operator==(const iterator& other) const;
        bool operator!=(const iterator& other) const { return !(*this == other); }
        Object operator*() const;

        const char* key() const;

    private:
        iterator(Object* container, struct lh_entry* object_entry);
        iterator(Object* container, int array_index);
        Object* container;
        union {
            int array_index;
            struct lh_entry* object_entry;
        };
        static iterator end();
    };
    iterator begin();
    iterator end();

private:
    Object(json_object* o);
    struct json_object* o { nullptr };
};

}

#endif /* SRC_JSON_JSON_H_ */

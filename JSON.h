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

class Object
{
public:
    // Creation/destruction interface
    static bool toFile(const std::string& path, const JSON::Object& o) noexcept;
    static Object fromFile(const std::string& path) noexcept;
    static Object fromStr(const std::string& str) noexcept;

    static Object newObject() noexcept;
    static Object newInt(int) noexcept;
    static Object newBoolean(bool) noexcept;
    static Object newArray(int) noexcept;
    static Object newString(const std::string&) noexcept;

    Object() noexcept = default;
    Object(const Object& o) noexcept;
    Object(Object&& o) noexcept;
    ~Object() noexcept;
    Object& operator=(const Object& o) noexcept;

    // Testing interface
    bool     isValid() const noexcept;
    explicit operator bool() const noexcept { return isValid(); }
    bool     isInt() const noexcept;
    bool     isString() const noexcept;
    bool     isObject() const noexcept;
    bool     isArray() const noexcept;
    bool     isBoolean() const noexcept;

    // Object interface
    Object operator[](const std::string& arg) const noexcept;
    bool   insert(const std::string& key, const Object& val) noexcept;
    bool   erase(const std::string& key) noexcept;

    // Array interface
    bool   append(const Object& val) noexcept;
    bool   erase(int idx) noexcept;
    bool   insert(int idx, const Object& val) noexcept;
    Object operator[](int idx) noexcept;

    // Conversion interface
    int         asInt() const noexcept;
    const char* asString() const noexcept;
    bool        asBoolean() const noexcept;
    std::string toStr() const noexcept;

    // Iterator
    class iterator : public std::iterator<std::input_iterator_tag, Object>
    {
        friend Object;

    public:
        iterator& operator++();
        iterator  operator++(int)
        {
            iterator retval = *this;
            ++(*this);
            return retval;
        }
        bool operator==(const iterator& other) const;
        bool operator!=(const iterator& other) const
        {
            return !(*this == other);
        }
        Object operator*() const;

        const char* key() const;

    private:
        iterator(Object* container, struct lh_entry* object_entry);
        iterator(Object* container, int array_index);
        Object* container;
        union
        {
            int              array_index;
            struct lh_entry* object_entry;
        };
        static iterator end();
    };
    iterator begin() noexcept;
    iterator end() noexcept;

private:
    Object(json_object* o);
    struct json_object* o{ nullptr };
};

}

#endif /* SRC_JSON_JSON_H_ */

/**
 * Copyright (C) 2022 Osmozis SA - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "JSON.h"

#include <json-c/json.h>
#include <json-c/json_util.h>

using namespace JSON;

/**
 * @brief fromFile Create from a JSON file
 * @param path The path of the file
 * @return The created JSON object
 */
Object
Object::fromFile(const std::string& path) noexcept
{
    json_object* o{ json_object_from_file(path.c_str()) };
    Object       ret{ o };

    if (nullptr != o)
        json_object_put(o);

    return ret;
}

/**
 * @brief fromFile Create from a JSON file
 * @param path The path of the file
 * @return The created JSON object
 */
bool
Object::toFile(const std::string& path, const JSON::Object& o) noexcept
{
    if (!o.isValid())
        return false;

    return (-1 !=
            json_object_to_file_ext(path.c_str(), o.o, JSON_C_TO_STRING_PLAIN));
}

/**
 * @brief fromStr Create from a JSON-formatted string
 * @param str The string
 * @return The created JSON object
 */
Object
Object::fromStr(const std::string& str) noexcept
{
    json_object* o{ json_tokener_parse(str.c_str()) };
    Object       ret{ o };

    if (nullptr != o)
        json_object_put(o);

    return ret;
}

/**
 * @brief newObject Create a new JSON object
 * @return The resulting Object
 */
Object
Object::newObject() noexcept
{
    Object ret;

    ret.o = json_object_new_object();
    return ret;
}

/**
 * @brief newInt Create a new JSON integer
 * @param i The input string
 * @return The resulting Object
 */
Object
Object::newInt(int i) noexcept
{
    Object ret;

    ret.o = json_object_new_int(i);
    return ret;
}

/**
 * @brief newBoolean Create a new JSON boolean
 * @param b The input boolean
 * @return The resulting Object
 */
Object
Object::newBoolean(bool b) noexcept
{
    Object ret;

    ret.o = json_object_new_boolean(b);
    return ret;
}

/**
 * @brief newArray Create a new JSON array
 * @param size The size of the array
 * @return The resulting Object
 */
Object
Object::newArray(int size) noexcept
{
    Object ret;

    ret.o = json_object_new_array_ext(size);
    return ret;
}

/**
 * @brief newString Create a new JSON string
 * @param str The input string
 * @return The resulting Object
 */
Object
Object::newString(const std::string& str) noexcept
{
    Object ret;

    ret.o = json_object_new_string(str.c_str());
    return ret;
}

/**
 * @brief Object Copy constructor
 * @param other
 */
Object::Object(const Object& other) noexcept
  : o(other.o)
{
    if (other.isValid())
        json_object_get(o);
}

/**
 * @brief Object Move constructor
 * @param other
 */
Object::Object(Object&& other) noexcept
  : o(other.o)
{
    other.o = nullptr;
}

/**
 * @brief ~Object Destructor
 */
Object::~Object() noexcept
{
    if (isValid())
        json_object_put(o);
}

/**
 * @brief operator= Assignement operator
 * @param other
 * @return The assigned JSON object
 */
Object&
Object::operator=(const Object& other) noexcept
{
    if (isValid())
        json_object_put(o);

    o = other.o;

    if (isValid())
        json_object_get(o);

    return *this;
}

/**
 * @brief isValid Test the validity of the JSON object
 * @return true / false
 */
bool
Object::isValid() const noexcept
{
    return nullptr != o;
}

/**
 * @brief isInt Test if the JSON object contains an integer
 * @return true / false
 */
bool
Object::isInt() const noexcept
{
    return isValid() && json_object_is_type(o, json_type_int);
}

/**
 * @brief isString Test if the JSON object contains a string
 * @return true / false
 */
bool
Object::isString() const noexcept
{
    return isValid() && json_object_is_type(o, json_type_string);
}

/**
 * @brief isObject Test if the JSON object contains an Object
 * @return true / false
 */
bool
Object::isObject() const noexcept
{
    return isValid() && json_object_is_type(o, json_type_object);
}

/**
 * @brief isArray Test if the JSON object contains an array
 * @return true / false
 */
bool
Object::isArray() const noexcept
{
    return isValid() && json_object_is_type(o, json_type_array);
}

/**
 * @brief isBoolean Test if the JSON object contains a boolean
 * @return true / false
 */
bool
Object::isBoolean() const noexcept
{
    return isValid() && json_object_is_type(o, json_type_boolean);
}

/**
 * @brief operator[] Index operator for JSON Object
 * @param arg the key of the JSON object
 * @return The JSON object associated to the required key
 */
Object
Object::operator[](const std::string& arg) const noexcept
{
    if (isObject()) {
        json_object* child;
        if (json_object_object_get_ex(o, arg.c_str(), &child))
            return child;
    }

    return nullptr;
}

/**
 * @brief insert Insert a couple key/value in the JSON Object
 * @param key The key (a string)
 * @param val The value (a JSON object)
 * @return True in case of success, false otherwise
 */
bool
Object::insert(const std::string& key, const Object& val) noexcept
{
    bool ret{ false };

    if (!val.isValid())
        return ret;

    if (!isObject())
        return ret;

    ret = (0 == json_object_object_add(o, key.c_str(), val.o));

    if (ret)
        json_object_get(val.o);

    return ret;
}

/**
 * @brief erase Erase the JSON object at the specified key
 * @param key The key
 * @return true if the key/value pair was successfully removed
 */
bool
Object::erase(const std::string& key) noexcept
{
    bool ret{ false };

    if (!isObject())
        return ret;

    json_object_object_del(o, key.c_str());

    return true;
}

/**
 * @brief append Append to the JSON array
 * @param val The object to append to the array
 * @return true if the JSON object was successfully appened
 */
bool
Object::append(const Object& val) noexcept
{
    bool ret{ false };

    if (!val.isObject())
        return ret;

    if (!isArray())
        return ret;

    ret = (0 == json_object_array_add(o, val.o));

    if (ret)
        json_object_get(val.o);

    return ret;
}

/**
 * @brief erase Erase an element at a specific index of the JSON array
 * @param idx The index
 * @return true if an element was removed at that index
 */
bool
Object::erase(int idx) noexcept
{
    bool ret{ false };

    if (!isArray())
        return ret;

    if (0 > idx || json_object_array_length(o) <= static_cast<size_t>(idx))
        return ret;

    ret = (0 == json_object_array_del_idx(o, static_cast<size_t>(idx), 1));

    return ret;
}

/**
 * @brief insert Insert an element at a specific index in the JSON array
 * @param idx The index
 * @param val The JSON Object to insert
 * @return true if the element was successfully inserted
 */
bool
Object::insert(int idx, const Object& val) noexcept
{
    bool ret{ false };

    if (!isArray())
        return ret;

    if (!val.isValid())
        return ret;

    if (0 > idx)
        return ret;

    ret = (0 == json_object_array_put_idx(o, static_cast<size_t>(idx), val.o));

    if (ret)
        json_object_get(val.o);

    return ret;
}

/**
 * @brief operator[] Access an element at a specific index
 * @param idx The index
 * @return The element (JSON object) - not necessarily valid
 */
Object
Object::operator[](int idx) noexcept
{
    Object ret;

    if (!isArray())
        return ret;

    if (0 > idx)
        return ret;

    ret = Object(json_object_array_get_idx(o, static_cast<size_t>(idx)));

    return ret;
}

/**
 * @brief begin Return an iterator to the first element of the object
 * @return The iterator
 */
Object::iterator
Object::begin() noexcept
{
    if (isObject())
        return iterator(this, json_object_get_object(o)->head);
    if (isArray())
        return iterator(this, 0);

    return end();
}

/**
 * @brief end Return an iterator to the element following the last element of
 * the object
 * @return The iterator
 */
Object::iterator
Object::end() noexcept
{
    return iterator::end();
}

/**
 * @brief Object Private constructor - libJSON-c wrapping
 * @param ptr a json_object (\see
 * https://json-c.github.io/json-c/json-c-0.10/doc/html/structjson__object.html)
 */
Object::Object(json_object* ptr)
  : o(ptr)
{
    if (isValid())
        json_object_get(o);
}

/**
 * @brief asInt Interprets the JSON object as an integer
 * @return The integer
 *
 * @note You might want to check its type (\see isInt()) first
 */
int
Object::asInt() const noexcept
{
    return json_object_get_int(o);
}

/**
 * @brief asString Interprets the JSON object as a c string
 * @return The c string
 *
 * @note You might want to check its type (\see isString()) first
 */
const char*
Object::asString() const noexcept
{
    return isValid() ? json_object_get_string(o) : "";
}

/**
 * @brief asBoolean Interprets the JSON object as a boolean
 * @return The boolean
 *
 * @note You might want to check its type (\see isBoolean()) first
 */
bool
Object::asBoolean() const noexcept
{
    return json_object_get_boolean(o);
}

/**
 * @brief toStr Stringify the object to JSON format
 * @return A string to JSON format
 */
std::string
Object::toStr() const noexcept
{
    std::string ret;

    if (isValid())
        ret = json_object_to_json_string(o);

    return ret;
}

Object::iterator&
Object::iterator::operator++()
{
    if (!operator==(end())) {
        if (container->isObject()) {
            object_entry = object_entry->next;
            if (!object_entry)
                container = nullptr; // end
        } else {
            array_index++;
            if (static_cast<size_t>(array_index) >=
                json_object_array_length(container->o))
                container = nullptr; // end
        }
    }
    return *this;
}

bool
Object::iterator::operator==(const iterator& other) const
{
    if (container != other.container)
        return false;

    if (container == nullptr)
        return true;

    if (container->isObject())
        return object_entry == other.object_entry;
    else
        return array_index == other.array_index;
}

Object
Object::iterator::operator*() const
{
    if (!container)
        return nullptr;

    if (container->isObject())
        return (struct json_object*)object_entry->v;
    else
        return json_object_array_get_idx(container->o, array_index);
}

const char*
Object::iterator::key() const
{
    if (!container || !container->isObject())
        return nullptr;

    return (char*)object_entry->k;
}

Object::iterator
Object::iterator::end()
{
    return iterator(nullptr, 0);
}

Object::iterator::iterator(Object* container, struct lh_entry* object_entry)
  : container(container)
  , object_entry(object_entry)
{}

Object::iterator::iterator(Object* container, int array_index)
  : container(container)
  , array_index(array_index)
{}

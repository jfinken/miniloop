/**
 * Copyright (C) 2022 Osmozis SA - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "JSON.h"

#include <json-c/json.h>
#include <json-c/json_util.h>

using namespace JSON;

Object
Object::fromFile(const std::string& path)
{
    return json_object_from_file(path.c_str());
}

Object
Object::fromStr(const std::string& str)
{
    return json_tokener_parse(str.c_str());
}

Object::Object(const Object& other)
  : o(other.o)
{
    if (other.isValid())
        json_object_get(o);
}

Object::Object(Object&& other)
  : o(other.o)
{
    other.o = nullptr;
}

Object::~Object()
{
    if (isValid())
        json_object_put(o);
}

Object&
JSON::Object::operator=(const Object& other)
{
    if (isValid())
        json_object_put(o);

    o = other.o;

    if (isValid())
        json_object_get(o);

    return *this;
}

bool
Object::isValid() const
{
    return nullptr != o;
}

bool
Object::isInt() const
{
    return isValid() && json_object_is_type(o, json_type_int);
}
bool
Object::isString() const
{
    return isValid() && json_object_is_type(o, json_type_string);
}
bool
Object::isObject() const
{
    return isValid() && json_object_is_type(o, json_type_object);
}
bool
JSON::Object::isArray() const
{
    return isValid() && json_object_is_type(o, json_type_array);
}
bool
JSON::Object::isBoolean() const
{
    return isValid() && json_object_is_type(o, json_type_boolean);
}

Object
Object::operator[](const std::string& arg) const
{
    if (isObject()) {
        json_object* child;
        if (json_object_object_get_ex(o, arg.c_str(), &child))
            return child;
    }

    return nullptr;
}

JSON::Object::iterator
JSON::Object::begin()
{
    if (isObject())
        return iterator(this, json_object_get_object(o)->head);
    if (isArray())
        return iterator(this, 0);

    return end();
}

JSON::Object::iterator
JSON::Object::end()
{
    return iterator::end();
}

JSON::Object::Object(json_object* ptr)
  : o(ptr)
{
    if (isValid())
        json_object_get(o);
}

int
JSON::Object::asInt() const
{
    return json_object_get_int(o);
}

const char*
JSON::Object::asString() const
{
    return isValid() ? json_object_get_string(o) : "";
}

bool
JSON::Object::asBoolean() const
{
    return json_object_get_boolean(o);
}

JSON::Object::iterator&
JSON::Object::iterator::operator++()
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
JSON::Object::iterator::operator==(const iterator& other) const
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

JSON::Object
JSON::Object::iterator::operator*() const
{
    if (!container)
        return nullptr;

    if (container->isObject())
        return (struct json_object*)object_entry->v;
    else
        return json_object_array_get_idx(container->o, array_index);
}

const char*
JSON::Object::iterator::key() const
{
    if (!container || !container->isObject())
        return nullptr;

    return (char*)object_entry->k;
}

JSON::Object::iterator
JSON::Object::iterator::end()
{
    return iterator(nullptr, 0);
}

JSON::Object::iterator::iterator(Object*          container,
                                 struct lh_entry* object_entry)
  : container(container)
  , object_entry(object_entry)
{}

JSON::Object::iterator::iterator(Object* container, int array_index)
  : container(container)
  , array_index(array_index)
{}

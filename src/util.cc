#include "util.h"

std::uint64_t now()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

std::uint64_t now_ns()
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

template<>
bool has<std::string>(const json& j, std::string key)
{
    return j.count(key) && j[key].is_string();
}

template<>
bool has<float>(const json& j, std::string key)
{
    return j.count(key) && j[key].is_number();
}

template<>
bool has<int>(const json& j, std::string key)
{
    return j.count(key) && j[key].is_number_integer();
}

template<>
bool has<unsigned int>(const json& j, std::string key)
{
    return j.count(key) && j[key].is_number_unsigned();
}

template<>
bool has<bool>(const json& j, std::string key)
{
    return j.count(key) && j[key].is_boolean();
}

bool has_array(const json& j, std::string key, int size)
{
    return j.count(key) && j[key].is_array() && (size == -1 || j[key].size() == size);
}
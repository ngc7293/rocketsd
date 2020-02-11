#include "util.h"

std::uint64_t now()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

std::uint64_t now_ns()
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

bool has_string(const json& j, std::string key)
{
    return j.count(key) && j[key].is_string();
}

bool has_float(const json& j, std::string key)
{
    return j.count(key) && j[key].is_number();
}

bool has_int(const json& j, std::string key)
{
    return j.count(key) && j[key].is_number_integer();
}

bool has_uint(const json& j, std::string key)
{
    return j.count(key) && j[key].is_number_unsigned();
}

bool has_array(const json& j, std::string key, int size)
{
    return j.count(key) && j[key].is_array() && (size == -1 || j[key].size() == size);
}
#ifndef UTIL_H_
#define UTIL_H_

#include <chrono>
#include <string>

#include <cstdint>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

/** now
 * Get system clock in milliseconds
 * 
 * @return milliseconds since epoch
 */
std::uint64_t now();

/** now_ns
 * Get system clock in nanoseconds
 * 
 * @return nanoseconds since epoch
 * @note the returned value will always be nanoseconds, but may not have ns precision
 */
std::uint64_t now_ns();

/** has<T>
 * Returns true if JSON contains a key of type <T>
 * 
 * @param j   json to check
 * @param key json key
 * 
 * @return true/false
 */
template<typename T>
bool has(const json& j, std::string key);

/** has_array
 * Checks if JSON contains an array for a given key
 *
 * @param j    json to check
 * @param key  key to look for
 * @param size expected size of array, -1 for any (default)
 *
 * @return true/false
 */
bool has_array(const json& j, std::string key, int size = -1);

#endif
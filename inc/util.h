#ifndef UTIL_H_
#define UTIL_H_

#include <chrono>
#include <cstdint>

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

#endif
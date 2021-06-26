#pragma once

#include <cstring>
#include <limits>
#include <locale>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include "base/inc/logging.h"

inline void
eat_lead_white(std::string &s)
{
    std::string::size_type off = s.find_first_not_of(' ');
    if (off != std::string::npos) {
        std::string::iterator begin = s.begin();
        s.erase(begin, begin + off);
    }
}

inline void
eat_end_white(std::string &s)
{
    std::string::size_type off = s.find_last_not_of(' ');
    if (off != std::string::npos)
        s.erase(s.begin() + off + 1, s.end());
}

inline void
eat_white(std::string &s)
{
    eat_lead_white(s);
    eat_end_white(s);
}

inline std::string
to_lower(const std::string &s)
{
    std::string lower;
    int len = s.size();

    lower.reserve(len);

    for (const auto &c : s)
        lower.push_back(std::tolower(c));

    return lower;
}

// Split the string s into lhs and rhs on the first occurence of the
// character c. Character c is not included in either lhs or rhs. If
// character c is not contained within string s, lsh equals s.
bool
split_first(const std::string &s, std::string &lhs, std::string &rhs, char c);

// Split the string s into lhs and rhs on the last occurence of the
// character c. Character c is not included in either lhs or rhs. If
// character c is not contained within string s, lhs equals s.
bool
split_last(const std::string &s, std::string &lhs, std::string &rhs, char c);

// Tokenize the string <s> splitting on the character <token>, and
// place the result in the string vector <vector>.  If <ign> is true,
// then empty result strings (due to trailing tokens, or consecutive
// tokens) are skipped.
void
tokenize(std::vector<std::string> &vector, const std::string &s,
         char token, bool ign = true);

/**
 * @{
 *
 * @name String to number helper functions for signed and unsigned
 *       integeral type, as well as enums and floating-point types.
 */
template <class T>
typename std::enable_if<std::is_integral<T>::value &&
                        std::is_signed<T>::value, T>::type
__to_number(const std::string &value)
{
    // start big and narrow it down if needed, determine the base dynamically
    long long r = std::stoll(value, nullptr, 0);
    if (r < std::numeric_limits<T>::lowest()
        || r > std::numeric_limits<T>::max()) {
        throw std::out_of_range("Out of range");
    }
    return static_cast<T>(r);
}

template <class T>
typename std::enable_if<std::is_integral<T>::value &&
                        !std::is_signed<T>::value, T>::type
__to_number(const std::string &value)
{
    // start big and narrow it down if needed, determine the base dynamically
    unsigned long long r = std::stoull(value, nullptr, 0);
    if (r > std::numeric_limits<T>::max())
        throw std::out_of_range("Out of range");
    return static_cast<T>(r);
}

template <class T>
typename std::enable_if<std::is_enum<T>::value, T>::type
__to_number(const std::string &value)
{
    auto r = __to_number<typename std::underlying_type<T>::type>(value);
    return static_cast<T>(r);
}

template <class T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type
__to_number(const std::string &value)
{
    // start big and narrow it down if needed
    long double r = std::stold(value);
    if (r < std::numeric_limits<T>::lowest()
        || r > std::numeric_limits<T>::max()) {
        throw std::out_of_range("Out of range");
    }
    return static_cast<T>(r);
}
/** @} */

/**
 * Turn a string representation of a number, either integral or
 * floating point, into an actual number.
 *
 * @param value The string representing the number
 * @param retval The resulting value
 * @return True if the parsing was successful
 */
template <class T>
inline bool
to_number(const std::string &value, T &retval)
{
    try {
        retval = __to_number<T>(value);
        return true;
    } catch (const std::out_of_range&) {
        return false;
    } catch (const std::invalid_argument&) {
        return false;
    } catch (...) {
        panic("Unrecognized exception.\n");
    }
}

/**
 * Turn a string representation of a boolean into a boolean value.
 */
inline bool
to_bool(const std::string &value, bool &retval)
{
    std::string s = to_lower(value);

    if (s == "true") {
        retval = true;
        return true;
    } else if (s == "false") {
        retval = false;
        return true;
    }

    return false;
}

// Put quotes around string arg if it contains spaces.
inline std::string
quote(const std::string &s)
{
    std::string ret;
    bool quote = s.find(' ') != std::string::npos;

    if (quote)
        ret = '"';

    ret += s;

    if (quote)
        ret += '"';

    return ret;
}


/**
 * Return true if 's' starts with the prefix string 'prefix'.
 */
inline bool
startswith(const char *s, const char *prefix)
{
    return (strncmp(s, prefix, strlen(prefix)) == 0);
}


/**
 * Return true if 's' starts with the prefix string 'prefix'.
 */
inline bool
startswith(const std::string &s, const char *prefix)
{
    return (s.compare(0, strlen(prefix), prefix) == 0);
}


/**
 * Return true if 's' starts with the prefix string 'prefix'.
 */
inline bool
startswith(const std::string &s, const std::string &prefix)
{
    return (s.compare(0, prefix.size(), prefix) == 0);
}



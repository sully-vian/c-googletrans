#ifndef URLS_HPP
#define URLS_HPP

#include <string>

namespace urls {
const std::string BASE = "https://translate.google.com";

// as a lambda function that takes the host as parameter
inline std::string TRANSLATE(const std::string& host) {
    return "https://" + host + "/translate_a/single";
}
}  // namespace urls

#endif  // URLS_HPP

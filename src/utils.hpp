#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

class Params {
   public:
    std::string client;
    std::string sl;               // source language
    std::string tl;               // target language
    std::string hl;               // host language
    std::vector<std::string> dt;  // data types array
    std::string ie;               // input encoding
    std::string oe;               // output encoding
    int otf;
    int ssl;
    int tsel;
    std::string tk;  // token
    std::string q;   // query text

    Params(const std::string& client, const std::string& query,
           const std::string& src, const std::string& dest,
           const std::string& token);

    // default constructor
    Params() = default;
};

std::uint32_t rshift(std::uint32_t val, int n);

// JSON parsing functions
nlohmann::json format_json(const std::string& original);

nlohmann::json legacy_format_json(const std::string& original);

#endif  // UTILS_HPP

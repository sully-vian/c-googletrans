#include "utils.hpp"

#include <iostream>
#include <regex>

// Constructor implementation
Params::Params(const std::string& client, const std::string& query,
               const std::string& src, const std::string& dest,
               const std::string& token) {
    this->client = client;
    this->sl = src;
    this->tl = dest;
    this->hl = dest;  // Python sets hl = dest

    // Default dt array from Python
    this->dt = {"at", "bd", "ex", "ld", "md", "qca", "rw", "rm", "ss", "t"};

    this->ie = "UTF-8";
    this->oe = "UTF-8";
    this->otf = 1;
    this->ssl = 0;  // Note: Python uses "ssel", you had "ssl"
    this->tsel = 0;
    this->tk = token;
    this->q = query;
}

// Right shift with padding (like Python's >>>)
std::uint32_t rshift(std::uint32_t val, int n) {
    // Python: return (val % 0x100000000) >> n
    return (val % 0x100000000U) >> n;
}

// Legacy format JSON - simplified for nlohmann::json
nlohmann::json legacy_format_json(const std::string& original) {
    std::string text = original;
    while (text.find(",,") != std::string::npos) {
        text = std::regex_replace(text, std::regex(",,"), ",null,");
    }
    while (text.find("[,") != std::string::npos) {
        text = std::regex_replace(text, std::regex("\\[,"), "[null,");
    }
    try {
        return nlohmann::json::parse(text);
    } catch (const std::exception& e) {
        std::cerr << "Legacy JSON parsing failed: " << e.what() << std::endl;
        return nullptr;
    }
}

nlohmann::json format_json(const std::string& original) {
    try {
        return nlohmann::json::parse(original);
    } catch (const std::exception& e) {
        std::cerr << "Normal JSON parsing failed, trying legacy format..."
                  << std::endl;
        return legacy_format_json(original);
    }
}

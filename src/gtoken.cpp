#include "gtoken.hpp"

#include <cpr/cpr.h>

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>

#include "constants.hpp"
#include "utils.hpp"

TokenAcquirer::TokenAcquirer(const std::string& tkk, const std::string& host) {
    this->tkk = tkk;
    this->host =
        (host.find("http") == std::string::npos) ? "https://" + host : host;

    tkk_patterns = {std::regex(R"(tkk:'([\s\S]+?)')")};
}

int TokenAcquirer::update() {
    // Time-based validation (like Python)
    auto now = std::time(nullptr);
    long current_hour = (long)(now * 1000) / 3600000;

    if (tkk != "0") {
        size_t dot_pos = tkk.find('.');
        if (dot_pos != std::string::npos) {
            long tkk_hour = std::stol(tkk.substr(0, dot_pos));
            if (tkk_hour == current_hour) {
                return 0;  // Still valid
            }
        }
    }

    cpr::Response response =
        cpr::Get(cpr::Url{host},
                 cpr::Header{{"User-Agent", constants::DEFAULT_USER_AGENT}});

    if (response.error) {
        std::cerr << "CPR error: " << response.error.message << std::endl;
        return 2;
    }

    std::string html = response.text;

    // Try each regex pattern
    std::smatch match;
    for (const auto& pattern : tkk_patterns) {
        if (std::regex_search(html, match, pattern)) {
            tkk = match[1].str();
            std::cout << "Found TKK: " << tkk << std::endl;
            return 0;
        }
    }

    // If simple patterns fail, try to find and parse JavaScript code
    std::regex code_pattern(R"(tkk:function\(\)\{[^}]+\})");
    if (std::regex_search(html, match, code_pattern)) {
        std::cout << "Found TKK function, would need JS parsing: "
                  << match[0].str() << std::endl;
        tkk = "0";
        return 0;
    }

    std::cout << "No TKK pattern found in response" << std::endl;
    return 3;
}

uint32_t TokenAcquirer::xr(uint32_t a, const std::string& b) {
    size_t c = 0;
    while (c < b.length() - 2) {
        char d = b[c + 2];
        int d_val = (d >= 'a') ? (d - 87) : (d - '0');
        uint32_t d_result = (b[c + 1] == '+') ? rshift(a, d_val) : (a << d_val);
        a = (b[c] == '+') ? ((a + d_result) & 4294967295U) : (a ^ d_result);
        c += 3;
    }
    return a;
}

std::string TokenAcquirer::acquire(const std::string& text) {
    // Convert text to Unicode code points
    std::vector<int> a;
    for (char32_t c : text) {  // C++11 UTF-32 iteration
        if (c < 0x10000) {
            a.push_back(c);
        } else {
            // Handle surrogate pairs
            a.push_back(((c - 0x10000) / 0x400) + 0xD800);
            a.push_back(((c - 0x10000) % 0x400) + 0xDC00);
        }
    }

    std::string b = (tkk != "0") ? tkk : "";
    size_t dot_pos = b.find('.');
    int b_val =
        (dot_pos != std::string::npos) ? std::stoi(b.substr(0, dot_pos)) : 0;
    int b_val_2 =
        (dot_pos != std::string::npos) ? std::stoi(b.substr(dot_pos + 1)) : 0;

    // Build e array (UTF-8 encoding)
    std::vector<int> e;
    for (size_t g = 0; g < a.size(); g++) {
        int l = a[g];
        if (l < 128) {
            e.push_back(l);
        } else {
            if (l < 2048) {
                e.push_back((l >> 6) | 192);
            } else {
                if ((l & 64512) == 55296 && g + 1 < a.size() &&
                    (a[g + 1] & 64512) == 56320) {
                    g++;
                    l = 65536 + ((l & 1023) << 10) + (a[g] & 1023);
                    e.push_back((l >> 18) | 240);
                    e.push_back(((l >> 12) & 63) | 128);
                } else {
                    e.push_back((l >> 12) | 224);
                }
                e.push_back(((l >> 6) & 63) | 128);
            }
            e.push_back((l & 63) | 128);
        }
    }

    uint32_t a_val = b_val;
    for (int value : e) {
        a_val += value;
        a_val = xr(a_val, "+-a^+6");
    }
    a_val = xr(a_val, "+-3^+b+-f");
    a_val ^= b_val_2;

    if ((int)a_val < 0) {
        a_val = (a_val & 2147483647U) + 2147483648U;
    }
    a_val %= 1000000U;

    return std::to_string(a_val) + "." + std::to_string(a_val ^ b_val);
}

const std::string TokenAcquirer::do_token(const std::string text) {
    update();
    std::string tk = acquire(text);
    return tk;
}

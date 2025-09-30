#ifndef GTOKEN_HPP
#define GTOKEN_HPP

#include <cstdint>
#include <regex>
#include <string>
#include <vector>

class TokenAcquirer {
   private:
    std::string tkk;
    std::string host;

    // Multiple regex patterns to try (matching Python approach)
    std::vector<std::regex> tkk_patterns;

    uint32_t xr(uint32_t a, const std::string& b);

    int update();
    std::string acquire(const std::string& text);

   public:
    TokenAcquirer(const std::string& tkk = "0",
                  const std::string& host = "translate.google.com");

    const std::string do_token(const std::string text);
    const std::string& getTKK() { return tkk; }
    const std::string& getHost() { return host; }
};

#endif  // GTOKEN_HPP

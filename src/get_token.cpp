#include <iostream>
#include <memory>

#include "gtoken.hpp"

int main() {
    const std::string text = "Hallo Welt";

    auto acquirer =
        std::make_unique<TokenAcquirer>("0", "https://translate.google.com");
    const std::string& token = acquirer->do_token(text);
    std::cout << "Token:" << token << std::endl;
    return 0;
}

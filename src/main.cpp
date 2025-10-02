#include <iostream>

#include "client.hpp"

int main() {
    std::string text = "Hallo Welt";

    std::cout << "Acquiring..." << std::endl;
    TokenAcquirer acquirer = TokenAcquirer("0", "https://translate.google.com");
    const std::string& token = acquirer.do_token(text);
    std::cout << "Token:" << token << std::endl;

    std::cout << "Translating..." << std::endl;
    Translator translator = Translator(constants::DEFAULT_SERVICE_URLS,
                                       constants::DEFAULT_USER_AGENT, true);
    std::optional<Translated> translated = translator.translate("Hallo Welt");
    if (!translated.has_value()) {
        std::cerr << "Translation has failed" << std::endl;
    }
    std::string result = translated.value().text;
    std::cout << "Translated to:" << result << std::endl;
}

#include <iostream>

#include "client.hpp"

int main() {
    Translator translator = Translator();
    std::string text = "Hallo Welt";
    std::optional<Translated> translated = translator.translate("Hallo Welt");
    if (!translated.has_value()) {
        std::cerr << "Translation has failed" << std::endl;
    }

    auto result = translated.value().text;
    std::cout << "Translated to:" << result << std::endl;
}

#ifndef MODELS_HPP
#define MODELS_HPP

#include <map>
#include <optional>
#include <string>
class Translated {
   public:
    std::string src;
    std::string dest;
    std::string origin;
    std::string text;
    std::string pronunciation;
    std::optional<std::map<std::string, std::string>> extra_data;

    Translated(std::string& src, std::string& dest, std::string& origin,
               std::string& text, std::string& pronunciation,
               std::optional<std::map<std::string, std::string>> extra_data =
                   std::nullopt)
        : src(src),
          dest(dest),
          origin(origin),
          text(text),
          pronunciation(pronunciation),
          extra_data(extra_data) {}
};

class Detected {
   public:
    std::string lang;
    float confidence;

    Detected(const std::string& lang, float confidence)
        : lang(lang), confidence(confidence) {}
};

#endif  // MODELS_HPP

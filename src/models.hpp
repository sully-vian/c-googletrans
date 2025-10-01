#ifndef MODELS_HPP
#define MODELS_HPP

#include <map>
#include <optional>
#include <string>
class Translated {
   public:
    const std::string src;
    const std::string dest;
    const std::string origin;
    const std::string text;
    const std::string pronunciation;
    const std::optional<const std::map<const std::string, const std::string>>
        extra_data;

    Translated(const std::string& src, const std::string& dest,
               const std::string& origin, const std::string& text,
               const std::string& pronunciation,
               std::optional<std::map<const std::string, const std::string>>
                   extra_data = std::nullopt)
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

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <memory>
#include <optional>
#include <string>

#include "constants.hpp"
#include "gtoken.hpp"
#include "models.hpp"

class Translator {
   public:
    Translator(const std::vector<std::string>& service_urls =
                   constants::DEFAULT_CLIENT_SERVICE_URLS,
               const std::string& user_agent = constants::DEFAULT_USER_AGENT,
               bool raise_exception = constants::DEFAULT_RAISE_EXCEPTION);

    // Synchronous version for symplicity
    std::optional<Translated> translate(const std::string& text,
                                        const std::string& dest = "en",
                                        const std::string& src = "auto");

    std::optional<Detected> detect(const std::string& text);

   private:
    std::vector<std::string> service_urls;
    std::string user_agent;
    bool raise_exception;
    std::unique_ptr<TokenAcquirer> token_acquirer;

    std::string pick_service_url();
    std::map<std::string, std::string> build_params(const std::string& client,
                                                    const std::string& query,
                                                    const std::string& src,
                                                    const std::string& dest,
                                                    const std::string& token);
};

#endif  // CLIENT_HPP

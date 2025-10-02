#include "client.hpp"

#include <cpr/cpr.h>

#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "urls.hpp"
#include "utils.hpp"

Translator::Translator(const std::vector<std::string>& service_urls,
                       const std::string& user_agent, bool raise_exception)
    : service_urls(service_urls),
      user_agent(user_agent),
      raise_exception(raise_exception) {
    // Default to first service URl
    std::string host = service_urls.empty()
                           ? constants::DEFAULT_CLIENT_SERVICE_URLS[0]
                           : service_urls[0];
    token_acquirer = std::make_unique<TokenAcquirer>("0", host);
}

std::string Translator::pick_service_url() {
    if (service_urls.size() == 1) {
        return service_urls[0];
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, service_urls.size() - 1);
    int gen_ind = dis(gen);
    return service_urls[gen_ind];
}

cpr::Parameters Translator::build_params(const std::string& client,
                                         const std::string& query,
                                         const std::string& src,
                                         const std::string& dest,
                                         const std::string& token) {
    Params params(client, query, src, dest, token);
    cpr::Parameters result;
    result.Add({"client", client});
    result.Add({"sl", params.sl});
    result.Add({"tl", params.tl});
    result.Add({"hl", params.hl});
    for (const auto& dt : params.dt) {
        result.Add({"dt", dt});
    }
    result.Add({"ie", params.ie});
    result.Add({"oe", params.oe});
    result.Add({"otf", std::to_string(params.otf)});
    result.Add({"ssel", std::to_string(params.ssl)});
    result.Add({"tsel", std::to_string(params.tsel)});
    result.Add({"tk", params.tk});
    result.Add({"q", params.q});
    return result;
}

std::optional<Translated> Translator::translate(const std::string& text,
                                                const std::string& dest,
                                                const std::string& src) {
    std::string token = token_acquirer->do_token(text);
    cpr::Parameters params = build_params("webapp", text, src, dest, token);
    std::string url = urls::TRANSLATE(pick_service_url());

    cpr::CurlHolder holder;
    std::string req = params.GetContent(holder);
    std::cout << "params:" << std::endl;
    std::cout << req << std::endl;

    url = "https://translate.google.com/translate_a/single";
    std::cout << "url:" << url << std::endl;
    cpr::Response response = cpr::Get(cpr::Url{url}, params,
                                      cpr::Header{{"User-Agent", user_agent}});
    if (response.status_code != cpr::status::HTTP_OK) {
        if (raise_exception) {
            throw std::runtime_error("Unexpected status code:" +
                                     std::to_string(response.status_code));
        }
        return std::nullopt;
    }

    nlohmann::json data;
    data = nlohmann::json::parse(response.text);

    // Parse translation result (simplified, may need to adapt for full
    // compatibility)
    std::string translated;
    std::string pron;
    std::string src_lang = src;
    for (const auto& item : data[0]) {
        if (!item[0].is_null()) {
            translated += item[0].get<std::string>();
        }
    }
    pron = text;
    if (data[0][1].size() > 2) {
        pron = data[0][1][2].get<std::string>();
    }
    if (data.size() > 2) {
        src_lang = data[2].get<std::string>();
    }

    auto extra_data = std::nullopt;

    return Translated(src_lang, dest, text, translated, pron, extra_data);
}

std::optional<Detected> Translator::detect(const std::string& text) {
    std::string token = token_acquirer->do_token(text);
    cpr::Parameters params = build_params("webapp", text, "auto", "en", token);
    std::string url = urls::TRANSLATE(pick_service_url());

    cpr::Response response = cpr::Get(cpr::Url{url}, params,
                                      cpr::Header{{"User-Agent", user_agent}});

    if (response.status_code != cpr::status::HTTP_OK) {
        if (raise_exception) {
            throw std::runtime_error("Unexpected status code:" +
                                     std::to_string(response.status_code));
        }
        return std::nullopt;
    }

    nlohmann::json data;
    data = nlohmann::json::parse(response.text);

    std::string lang;
    float confidence = 0.0f;
    if (data[8][0].size() > 1) {
        lang = data[8][0].get<std::string>();
        confidence = data[8][-2].get<float>();
    } else {
        lang = data[8][0].get<std::string>();
        confidence = data[8][-2][0].get<float>();
    }

    return Detected(lang, confidence);
}

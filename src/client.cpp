#include "client.hpp"

#include <cpr/cpr.h>

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

std::map<std::string, std::string> Translator::build_params(
    const std::string& client, const std::string& query, const std::string& src,
    const std::string& dest, const std::string& token) {
    Params params(client, query, src, dest, token);
    std::map<std::string, std::string> result;
    result["client"] = params.client;
    result["sl"] = params.sl;
    result["tl"] = params.tl;
    result["hl"] = params.hl;
    for (const auto& dt : params.dt) {
        result["dt"] = dt;
    }
    result["ie"] = params.ie;
    result["oe"] = params.oe;
    result["otf"] = std::to_string(params.otf);
    result["ssel"] = std::to_string(params.ssl);
    result["tsel"] = std::to_string(params.tsel);
    result["tk"] = params.tk;
    result["q"] = params.q;
    return result;
}

std::optional<Translated> Translator::translate(const std::string& text,
                                                const std::string& dest,
                                                const std::string& src) {
    std::string token = token_acquirer->do_token(text);
    std::map<std::string, std::string> params =
        build_params("webapp", text, src, dest, token);
    std::string url = urls::TRANSLATE(pick_service_url());

    cpr::Parameters cpr_params;
    for (const auto& kv : params) {
        cpr_params.Add({kv.first, kv.second});
    }

    cpr::Response response = cpr::Get(cpr::Url{url}, cpr_params,
                                      cpr::Header{{"User-Agent", user_agent}});

    if (response.status_code != cpr::status::HTTP_OK) {
        if (raise_exception) {
            throw std::runtime_error("Unexpected status code:" +
                                     std::to_string(response.status_code));
        }
        return std::nullopt;
    }

    nlohmann::json data;
    try {
        data = nlohmann::json::parse(response.text);
    } catch (...) {
        if (raise_exception) {
            throw;
        }
        return std::nullopt;
    }

    // Parse translation result (simplified, may need to adapt for full
    // compatibility)
    std::string translated;
    std::string pron;
    std::string src_lang = src;
    try {
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
    } catch (...) {
    }

    auto extra_data = std::nullopt;

    return Translated(src_lang, dest, text, translated, pron, extra_data);
}

std::optional<Detected> Translator::detect(const std::string& text) {
    auto token = token_acquirer->do_token(text);
    auto params = build_params("webapp", text, "auto", "en", token);
    std::string url = urls::TRANSLATE(pick_service_url());

    cpr::Parameters cpr_params;
    for (const auto& kv : params) {
        cpr_params.Add({kv.first, kv.second});
    }

    cpr::Response response = cpr::Get(cpr::Url{url}, cpr_params,
                                      cpr::Header{{"User-Agent", user_agent}});

    if (response.status_code != cpr::status::HTTP_OK) {
        if (raise_exception) {
            throw std::runtime_error("Unexpected status code:" +
                                     std::to_string(response.status_code));
        }
        return std::nullopt;
    }

    nlohmann::json data;
    try {
        data = nlohmann::json::parse(response.text);
    } catch (...) {
        if (raise_exception) {
            throw;
        }
        return std::nullopt;
    }

    std::string lang;
    float confidence = 0.0f;
    try {
        if (data[8][0].size() > 1) {
            lang = data[8][0].get<std::string>();
            confidence = data[8][-2].get<float>();
        } else {
            lang = data[8][0].get<std::string>();
            confidence = data[8][-2][0].get<float>();
        }
    } catch (...) {
    }

    return Detected(lang, confidence);
}

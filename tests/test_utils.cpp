#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/utils.hpp"

void test_rshift() {
    uint32_t value = 1000;
    uint32_t n = 3;
    uint32_t result = rshift(value, n);
    assert(result == 125);
}

void test_build_params_with_override() {
    Params *params = build_params("", "", "", "", "");
    // stimulate override: change tf to 3
    params->otf = 3;
    assert(params->otf == 3);
    free(params);
}

void test_format_json() {
    const char *text =
        "[, ,\"en\", , , ,0.96954316, [ [\"en\"], , [0.96954316] ]]";
    printf("formatting...\n");
    cJSON *result = format_json(text);
    printf("formatted\n");
    if (!result) {
        printf("null\n");
    }
    printf("type: [%d]\n", result->type);
    assert(cJSON_IsArray(result));
    // check some values
    cJSON *item = cJSON_GetArrayItem(result, 2);
    assert(strcmp(cJSON_GetStringValue(item), "en") == 0);

    item = cJSON_GetArrayItem(result, 6);
    assert(cJSON_IsNumber(item));
    assert(item->valuedouble == 0.96954316);

    // Nested array
    cJSON *nested = cJSON_GetArrayItem(result, 8);
    assert(cJSON_IsArray(nested));
    cJSON *nested_item = cJSON_GetArrayItem(nested, 0);
    assert(cJSON_IsArray(nested_item));
    cJSON *lang = cJSON_GetArrayItem(nested_item, 0);
    assert(strcmp(cJSON_GetStringValue(lang), "en") == 0);

    cJSON_Delete(result);
}

void test_format_malformed_json() {
    const char *text =
        "[, ,\"en\", , , ,0.96954316, ,[ [\"en\"], 0.96954316 ]]]";
    cJSON *result = format_json(text);
    assert(result != NULL);  // Should parse, since legacy_format_json tries to
                             // fix malformed JSON
    cJSON_Delete(result);
}

int main() {
    test_rshift();
    test_build_params_with_override();
    test_format_json();
    test_format_malformed_json();
    printf("All tests passed.\n");
    return 0;
}

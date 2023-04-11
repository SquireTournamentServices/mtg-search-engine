#include "./test_card_txt_field_trie.h"
#include "../testing_h/testing.h"
#include "../src/card_txt_fields_trie.h"
#include <stdlib.h>
#include <string.h>

#define DANDAN "Dandân"
#define DANDAN_F "dndn"

#define DUPLICATE_TEXT "aaaaaaaabbbbbbbbbccccc"
#define DUPLICATE_TEXT_F "bc"

static int test_filter_str()
{
    char *dandan = mse_filter_text(DANDAN);
    lprintf(LOG_INFO, "%s\n", dandan);
    ASSERT(dandan != NULL);
    ASSERT(strcmp(dandan, DANDAN_F) == 0);
    free(dandan);

    char *dup_text = mse_filter_text(DUPLICATE_TEXT);
    ASSERT(dup_text != NULL);
    ASSERT(strcmp(dup_text, DUPLICATE_TEXT_F) == 0);
    free(dup_text);
    return 1;
}

SUB_TEST(test_card_txt_field_trie, {&test_filter_str, "Test filter string"})

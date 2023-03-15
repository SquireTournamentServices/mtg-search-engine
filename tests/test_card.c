#include "./test_card.h"
#include "../testing_h/testing.h"
#include "../src/card.h"
#include <unistd.h>
#include <stdio.h>

static int test_card_parse_json()
{
    return 1;
}

SUB_TEST(test_card, {&test_card_parse_json, "Test parse card from JSON"})

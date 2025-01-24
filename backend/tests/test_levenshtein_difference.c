#include "./test_levenshtein_difference.h"
#include "../mse/levenshtein_difference.h"
#include "../testing_h/testing.h"

int test_levenshtein_difference()
{
    ASSERT(levenshtein_difference("kitten", "sitting") == 3);
    return 1;
}

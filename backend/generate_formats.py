import json
from typing import List
from common import FILE, PREFIX, FILE_NOTICE, read_mtg_json

BASENAME = "mse_formats"
OUTPUT_FILE_H = f"{BASENAME}.h"
OUTPUT_FILE_U = f"{BASENAME}.c"

# Types
FORMAT_ENUM = f"{PREFIX.lower()}_formats_t"
FORMAT_LEGALITIES_ENUM = f"{PREFIX.lower()}_format_legalities_t"
CARD_FORMAT_LEGALITIES_STRUCT = f"{PREFIX.lower()}_card_format_legalities_t"
CARD_FORMAT_LEGALITY_INDEXES_STRUCT = f"{PREFIX.lower()}_format_legality_indexes_t"

# Functions
FORMATS_FROM_JSON = f"{PREFIX.lower()}_card_formats_legalities_t_from_json"
READ_FORMATS_FROM_FILE = f"{PREFIX.lower()}_read_legalities"
WRITE_FORMATS_TO_FILE = f"{PREFIX.lower()}_write_legalities"
GENERATE_CARD_FORMAT_LEGALITY_INDEXES = (
    f"{PREFIX.lower()}_generate_format_legality_indexes"
)
FREE_CARD_FORMAT_LEGALITY_INDEXES = f"{PREFIX.lower()}_free_format_legality_indexes"
STRING_AS_FORMAT_ENUM = f"{PREFIX.lower()}_str_as_{FORMAT_ENUM}"

NOT_LEGAL = "Unplayable"
formats = []
format_legalities = []


def read_json_file():
    print(f"Reading all formats")
    data = read_mtg_json()
    data_j: dict = json.loads(data)

    formats_set = set()
    format_legalities_set = set()
    format_legalities_set.add(NOT_LEGAL)

    for set_k in data_j["data"]:
        set_o: dict = data_j["data"][set_k]

        for card in set_o["cards"]:
            legalities = card["legalities"]
            for format in legalities:
                formats_set.add(format)
                format_legalities_set.add(legalities[format])

    sorted_formats = list(formats_set)
    sorted_formats.sort()

    sorted_format_legalities = list(format_legalities_set)
    sorted_format_legalities.sort()

    [formats.append(f) for f in sorted_formats]
    [format_legalities.append(f) for f in sorted_format_legalities]
    print(f"Found the following formats: {formats}")
    print(f"Found the following legalities: {format_legalities}")


def gen_header() -> None:
    print("Computing magic number for the format part of the system version number")
    concat = "formats:"

    for format in formats:
        concat += format.casefold() + ","

    concat += ";legalities:"
    for legality in format_legalities:
        concat += legality.casefold() + ","

    magic_number = hash(concat)
    magic_number = magic_number ^ (magic_number >> 32)
    magic_number &= (2**21) - 1

    print(f"Magic number is {magic_number}")

    output_h = f"""#pragma once
{FILE_NOTICE}

#include <jansson.h>
#include <stdio.h>
#include "../mse/thread_pool.h"
#include "../mse/avl_tree.h"

#define {PREFIX}_FORMAT_MAGIC_NUMBER ({magic_number}u)

"""

    # FORMAT_ENUM
    output_h += f"typedef enum {FORMAT_ENUM}" + "{\n"
    for format in formats:
        output_h += f"    MSE_FORMAT_{format.upper()},\n"
    output_h += f"    {PREFIX}_FORMAT_END\n" + "}" + f" {FORMAT_ENUM};\n"

    # FORMAT_LEGALITIES_ENUM
    output_h += f"\ntypedef enum {FORMAT_LEGALITIES_ENUM}" + "{\n"
    for legality in format_legalities:
        output_h += (
            f"    {PREFIX}_FORMAT_LEGALITIES_{legality.upper().replace(' ', '')},\n"
        )
    output_h += (
        f"    {PREFIX}_FORMAT_LEGALITIES_END\n"
        + "}"
        + f" {FORMAT_LEGALITIES_ENUM};\n\n"
    )

    # CARD_FORMAT_LEGALITIES_STRUCT
    output_h += f"typedef struct {CARD_FORMAT_LEGALITIES_STRUCT} " + "{\n"
    for format in formats:
        output_h += f"    {PREFIX.lower()}_format_legalities_t {format.lower().replace(' ', '')};\n"
    output_h += "}" + f" {CARD_FORMAT_LEGALITIES_STRUCT};\n\n"

    # FORMAT_INDEXES
    output_h += f"typedef struct {CARD_FORMAT_LEGALITY_INDEXES_STRUCT} " + "{"
    for format in formats:
        output_h += f"\n    /// Index for cards in {format}\n"
        for legality in format_legalities:
            output_h += (
                f"    mse_avl_tree_node_t *{format.lower()}_{legality.lower()}_index;\n"
            )
    output_h += "} " + f"{CARD_FORMAT_LEGALITY_INDEXES_STRUCT};\n\n"

    # Function definitions
    output_h += f"""
int {STRING_AS_FORMAT_ENUM}(const char *str, {FORMAT_ENUM} *ret);
int {PREFIX.lower()}_str_as_{FORMAT_LEGALITIES_ENUM}(const char *str, {FORMAT_LEGALITIES_ENUM} *ret);

const char *{FORMAT_ENUM}_as_str({FORMAT_ENUM} format);
const char *{FORMAT_LEGALITIES_ENUM}_as_str({FORMAT_LEGALITIES_ENUM} format_legality);

int {FORMATS_FROM_JSON}(json_t *json, {CARD_FORMAT_LEGALITIES_STRUCT} *ret);
int {READ_FORMATS_FROM_FILE}(FILE *f, {CARD_FORMAT_LEGALITIES_STRUCT} *ret);
int {WRITE_FORMATS_TO_FILE}(FILE *f, {CARD_FORMAT_LEGALITIES_STRUCT} legalities);

int {GENERATE_CARD_FORMAT_LEGALITY_INDEXES}(mse_avl_tree_node_t *cards,
    {CARD_FORMAT_LEGALITY_INDEXES_STRUCT} *ret,
    mse_thread_pool_t *pool);

void {FREE_CARD_FORMAT_LEGALITY_INDEXES}({CARD_FORMAT_LEGALITY_INDEXES_STRUCT} *indexes);

// Forward declare the types for the generators
struct mse_set_generator_t;
struct mse_search_intermediate_t;
struct mse_all_printings_cards_t;

"""

    for legality in format_legalities:
        output_h += f"""int mse_generate_{legality.lower()}(struct mse_set_generator_t *gen,
    struct mse_search_intermediate_t *res,
    struct mse_all_printings_cards_t *cards);

"""

    with open(OUTPUT_FILE_H, "w") as f:
        f.write(output_h)


def gen_unit() -> None:
    # Strings to enums
    output_unit = f"""{FILE_NOTICE}

#include "{OUTPUT_FILE_H}"
#include <string.h>
#include <semaphore.h>
#include "../testing_h/testing.h"
#include "../mse/io_utils.h"
#include "../mse/card.h"
#include "../mse/generators.h"

int {STRING_AS_FORMAT_ENUM}(const char *str, {FORMAT_ENUM} *ret)
"""
    output_unit += "{\n"

    i = 0
    for format in formats:
        output_unit += "    "
        if i > 0:
            output_unit += "else "
        i += 1

        output_unit += f'if (strcmp(str, "{format.lower()}") == 0)' + " {\n"
        output_unit += f"        *ret = {PREFIX}_FORMAT_{format.upper()};\n"
        output_unit += "        return 1;\n"
        output_unit += "    }\n"

    output_unit += """
    return 0;
}

"""

    output_unit += f"int {PREFIX.lower()}_str_as_{FORMAT_LEGALITIES_ENUM}(const char *str, {FORMAT_LEGALITIES_ENUM} *ret)\n"
    output_unit += """{

    char *tmp = mse_to_lower(str);
    ASSERT(tmp != NULL);
"""

    i = 0
    for legality in format_legalities:
        output_unit += "    "
        if i > 0:
            output_unit += "else "
        i += 1

        output_unit += f'if (strcmp(tmp, "{legality.lower()}") == 0)' + " {\n"
        output_unit += (
            f"        *ret = {PREFIX}_FORMAT_LEGALITIES_{legality.upper()};\n"
        )
        output_unit += """        free(tmp);
        return 1;
     }
"""

    output_unit += (
        f"    *ret = {PREFIX}_FORMAT_LEGALITIES_{NOT_LEGAL.replace(' ', '').upper()};"
    )

    output_unit += """
    free(tmp);
    return 1;
}

"""

    # Enums to strings
    output_unit += f"const char *{FORMAT_ENUM}_as_str({FORMAT_ENUM} format)\n"
    output_unit += """{
    switch(format) {"""

    for format in formats:
        output_unit += f"""
    case {PREFIX}_FORMAT_{format.upper()}:
        return "{format.lower()}";"""

    output_unit += """
    }
    return "Invalid format.";
}

"""

    output_unit += f"const char *{FORMAT_LEGALITIES_ENUM}_as_str({FORMAT_LEGALITIES_ENUM} format_legality)\n"

    output_unit += """{
    switch(format_legality) {"""

    for legality in format_legalities:
        output_unit += f"""
    case {PREFIX}_FORMAT_LEGALITIES_{legality.upper()}:
        return "{legality.lower()}";"""

    output_unit += """
    }
    return "Invalid legality.";
}


"""

    # I/O for formats
    # JSON read
    output_unit += (
        f"int {FORMATS_FROM_JSON}(json_t *json, {CARD_FORMAT_LEGALITIES_STRUCT} *ret)\n"
    )

    output_unit += """{
    memset(ret, 0, sizeof(*ret));

    json_t *legalities= json_object_get(json, "legalities");
    ASSERT(legalities != NULL);
    ASSERT(json_is_object(legalities));

"""

    for format in formats:
        output_unit += (
            f"""    // Read {format} from JSON object
    ret->{format.lower()} = {PREFIX.lower()}_FORMAT_LEGALITIES_{NOT_LEGAL};
    json_t *{format.lower()} = json_object_get(legalities, "{format}");
    if ({format.lower()} != NULL)"""
            + " {"
            + f"""
        ASSERT(json_is_string({format.lower()}));

        const char *legality_str = json_string_value({format.lower()});
        ASSERT({PREFIX.lower()}_str_as_{FORMAT_LEGALITIES_ENUM}(legality_str, &ret->{format.lower()}));
"""
            + "    }\n"
        )

    output_unit += """    return 1;
}

"""

    # File Read
    output_unit += (
        f"int {READ_FORMATS_FROM_FILE}(FILE *f, {CARD_FORMAT_LEGALITIES_STRUCT} *ret)\n"
    )
    output_unit += """{
    int tmp = 0;
"""

    for format in formats:
        output_unit += f"""    // Read {format}
    ASSERT(mse_read_int(f, &tmp));
    ret->{format.lower()} = ({FORMAT_LEGALITIES_ENUM}) tmp;
"""

    output_unit += """    return 1;
}

"""

    # File Write
    output_unit += f"int {WRITE_FORMATS_TO_FILE}(FILE *f, {CARD_FORMAT_LEGALITIES_STRUCT} legalities)\n"
    output_unit += "{\n"

    for format in formats:
        output_unit += f"""    // Write {format}
    ASSERT(mse_write_int(f, (int) legalities.{format.lower()}));
"""

    output_unit += """    return 1;
}

"""

    # Index generation
    output_unit += "typedef struct mse_format_legality_index_generator_state_t {\n"
    output_unit += f"""    {CARD_FORMAT_LEGALITY_INDEXES_STRUCT} *index;
    mse_avl_tree_node_t *card_tree;
    sem_t sem;
    int ret;
"""
    output_unit += "} mse_format_legality_index_generator_state_t;\n\n"

    # The recrusive call to traverse the card tree
    def inner_function_name_for(format: str, legality: str) -> str:
        return f"__mse_generate_index_for_{format.lower()}_{legality.lower()}_r"

    # The worker task for generating the index
    def function_name_for(format: str, legality: str) -> str:
        return f"__mse_generate_index_for_{format.lower()}_{legality.lower()}"

    for format in formats:
        output_unit += f"// Index generators for {format}\n\n"
        for legality in format_legalities:
            # Recursive function
            output_unit += (
                f"static int {inner_function_name_for(format, legality)}(mse_avl_tree_node_t *node, mse_avl_tree_node_t **index) "
                + "{\n"
            )
            # Process current node
            output_unit += "    mse_card_t *card = (mse_card_t *) node->payload;\n"
            output_unit += (
                f"    if (card->format_legalities.{format} == {PREFIX.upper()}_FORMAT_LEGALITIES_{legality.upper()}) "
                + "{\n"
            )
            output_unit += """        mse_avl_tree_node_t *node_copy = mse_init_avl_tree_node(NULL, &mse_avl_cmp_card, node->payload);
            ASSERT(mse_insert_node(index, node_copy));
    }

"""

            # Left node recursive call
            output_unit += "    if (node->l != NULL) {\n"
            output_unit += f"        if (!{inner_function_name_for(format, legality)}(node->l, index)) "
            output_unit += """{
            return 0;
        }
    }

"""
            # Right node recursive call
            output_unit += "    if (node->r != NULL) {\n"
            output_unit += f"        if (!{inner_function_name_for(format, legality)}(node->r, index)) "
            output_unit += """{
             return 0;
        }
    }

"""

            # Base case
            output_unit += "   return 1;\n"
            output_unit += "}\n\n"

            # Wrapper function for task
            output_unit += (
                f"static void {function_name_for(format, legality)}(void *__state, mse_thread_pool_t *pool) "
                + "{\n"
            )
            output_unit += """   mse_format_legality_index_generator_state_t *state = (mse_format_legality_index_generator_state_t*) __state;
"""
            output_unit += f"    if (!{inner_function_name_for(format, legality)}(state->card_tree,\n"
            output_unit += (
                f"        &state->index->{format.lower()}_{legality.lower()}_index))"
            )
            output_unit += """ {
        state->ret = 0;
    }

    sem_post(&(state->sem));
}

"""

    # Index generator (called by mtg_json_indexes.c)
    output_unit += f"int {GENERATE_CARD_FORMAT_LEGALITY_INDEXES}(mse_avl_tree_node_t *cards, {CARD_FORMAT_LEGALITY_INDEXES_STRUCT} *ret, mse_thread_pool_t *pool)\n"
    output_unit += """{
    mse_format_legality_index_generator_state_t state;

    memset(&state, 0, sizeof(state));
    state.card_tree = cards;
    state.index = ret;
    state.ret = 1;

    ASSERT(sem_init(&state.sem, 0, 0) == 0);


    void (*tasks[])(void *, struct mse_thread_pool_t *) = {
"""
    for format in formats:
        output_unit += f"        // Index generators for {format}\n\n"
        for legality in format_legalities:
            output_unit += f"        &{function_name_for(format, legality)},\n"

    output_unit += """    };

    #define TASK_COUNT(T) (sizeof(T) / sizeof(*T))

    // Start the tasks
    size_t len = TASK_COUNT(tasks);

    for (size_t i = 0; i < len; i++) {
        mse_task_t task = {(void *) &state, tasks[i]};
        ASSERT(mse_task_queue_enqueue(&pool->queue, task));
    }

"""

    output_unit += (
        f"    for(size_t i = 0; i < ({len(formats)} * {len(format_legalities)}); i++) "
        + "{\n"
    )
    output_unit += """        int waiting = 1;
        while (waiting) {
            mse_pool_try_consume(pool);
            waiting = sem_trywait(&state.sem) != 0;
        }
    }

"""
    output_unit += """    sem_destroy(&state.sem);
    ASSERT(state.ret);
    return 1;
}

"""

    # Free function
    output_unit += f"void {FREE_CARD_FORMAT_LEGALITY_INDEXES}({CARD_FORMAT_LEGALITY_INDEXES_STRUCT} *indexes) " + "{\n"
    for format in formats:
        output_unit += f"    // Free indexes for {format}\n"
        for legality in format_legalities:
            output_unit += f"    if (indexes->{format.lower()}_{legality.lower()}_index != NULL) " + "{\n"
            output_unit += f"        mse_free_tree(indexes->{format.lower()}_{legality.lower()}_index);\n"
            output_unit += "    }\n\n"

    output_unit += """}

    mse_avl_tree_node_t *node =  NULL;

"""

    # Generators
    for legality in format_legalities:
        output_unit += f"""int mse_generate_{legality.lower()} (mse_set_generator_t *gen,
    mse_search_intermediate_t *res,
    mse_all_printings_cards_t *cards)
"""
        output_unit += "{\n"
        output_unit += f"""    {FORMAT_ENUM} format;
    ASSERT({STRING_AS_FORMAT_ENUM}(gen->argument, &format));
"""

        output_unit += """    switch(format) {
"""

        for format in formats:
            output_unit += f"    case {PREFIX.upper()}_FORMAT_{format.upper()}:\n"
            output_unit += f"         node = cards->indexes.format_legality_index.{format.lower()}_{legality.lower()}_index;\n"
            output_unit += "        break;\n"

        output_unit += """    default:
        lprintf(LOG_ERROR, "%s is not a valid format\\n", gen->argument);
        return 0;
    }

    ASSERT(node != NULL);
    *res = mse_init_search_intermediate_tree(node, 1);

    if (gen->negate) {
        mse_search_intermediate_t tmp;
        ASSERT(mse_set_negate(&tmp, cards, res));
        mse_free_search_intermediate(res);
        *res = tmp;
    }

    return 1;
}

"""

    with open(OUTPUT_FILE_U, "wb") as f:
        f.write(output_unit.encode("utf-8"))


def main() -> None:
    read_json_file()
    gen_header()
    gen_unit()


if __name__ == "__main__":
    main()

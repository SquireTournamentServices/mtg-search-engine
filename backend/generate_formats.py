import json
from typing import List
from common import FILE, PREFIX, FILE_NOTICE, read_mtg_json

BASENAME = "mse_formats"
OUTPUT_FILE_H = f"{BASENAME}.h"
OUTPUT_FILE_U = f"{BASENAME}.c"

FORMAT_ENUM = f"{PREFIX.lower()}_formats_t"
FORMAT_LEGALITIES_ENUM = f"{PREFIX.lower()}_format_legalities_t"
CARD_FORMAT_LEGALITIES_STRUCT = f"{PREFIX.lower()}_card_format_legalities_t"
FORMATS_FROM_JSON = f"{PREFIX.lower()}_card_formats_legalities_t_from_json"

formats = set()
format_legalities = set()
NOT_LEGAL = "Unplayable"


def read_json_file():
    print(f"Reading all formats")
    data = read_mtg_json()
    data_j: dict = json.loads(data)

    for set_k in data_j["data"]:
        set_o: dict = data_j["data"][set_k]

        for card in set_o["cards"]:
            legalities = card["legalities"]
            for format in legalities:
                formats.add(format)
                format_legalities.add(legalities[format])

    format_legalities.add(NOT_LEGAL)
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

#define {PREFIX}_FORMAT_MAGIC_NUMBER ({magic_number}u)

"""

    # FORMAT_ENUM
    output_h += f"typedef enum {FORMAT_ENUM}" + "{\n"
    for format in formats:
        output_h += f"  MSE_FORMAT_{format.upper()},\n"
    output_h += f"  {PREFIX}_FORMAT_END\n" + "}" + f" {FORMAT_ENUM};\n"

    # FORMAT_LEGALITIES_ENUM
    output_h += f"\ntypedef enum {FORMAT_LEGALITIES_ENUM}" + "{\n"
    for legality in format_legalities:
        output_h += f"    {PREFIX}_FORMAT_LEGALITIES_{legality.upper().replace(' ', '')},\n"
    output_h += (
        f"    {PREFIX}_FORMAT_LEGALITIES_END\n"
        + "}"
        + f" {FORMAT_LEGALITIES_ENUM};\n\n"
    )

    # CARD_FORMAT_LEGALITIES_STRUCT
    output_h += f"typedef struct {CARD_FORMAT_LEGALITIES_STRUCT} " + "{\n"
    for format in formats:
        output_h += f"    {PREFIX.lower()}_format_legalities_t {format.lower().replace(' ', '')};\n"
    output_h += "}" + f" {CARD_FORMAT_LEGALITIES_STRUCT};\n"

    output_h += f"""
int {PREFIX.lower()}_str_as_{FORMAT_ENUM}(const char *str, {FORMAT_ENUM} *ret);
int {PREFIX.lower()}_str_as_{FORMAT_LEGALITIES_ENUM}(const char *str, {FORMAT_LEGALITIES_ENUM} *ret);

const char *{FORMAT_ENUM}_as_str({FORMAT_ENUM} format);
const char *{FORMAT_LEGALITIES_ENUM}_as_str({FORMAT_LEGALITIES_ENUM} format_legality);

int {FORMATS_FROM_JSON}(json_t *json, {CARD_FORMAT_LEGALITIES_STRUCT} *ret);
"""

    with open(OUTPUT_FILE_H, "w") as f:
        f.write(output_h)


def gen_unit() -> None:
    # Strings to enums
    output_unit = f"""{FILE_NOTICE}
#include "{OUTPUT_FILE_H}"
#include <string.h>
#include "../testing_h/testing.h"
#include "../mse/io_utils.h"

int {PREFIX.lower()}_str_as_{FORMAT_ENUM}(const char *str, {FORMAT_ENUM} *ret)
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

    char *tmp = strdup(str);
    ASSERT(tmp != NULL);
    mse_to_lower(tmp);
    """

    i = 0
    for legality in format_legalities:
        output_unit += "    "
        if i > 0:
            output_unit += "else "
        i += 1

        output_unit += f'if (strcmp(str, "{legality.lower()}") == 0)' + " {\n"
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

    with open(OUTPUT_FILE_U, "wb") as f:
        f.write(output_unit.encode("utf-8"))


def main() -> None:
    read_json_file()
    gen_header()
    gen_unit()


if __name__ == "__main__":
    main()

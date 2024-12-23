import json
from typing import List
from common import FILE, PREFIX, FILE_NOTICE, read_mtg_json

BASENAME = "mse_formats"
OUTPUT_FILE_H = f"{BASENAME}.h"
OUTPUT_FILE_U = f"{BASENAME}.c"

FORMAT_ENUM = f"{PREFIX.lower()}_formats_t"
FORMAT_LEGALITIES_ENUM = f"{PREFIX.lower()}_format_legalities_t"
CARD_FORMAT_LEGALITIES_STRUCT = f"{PREFIX.lower()}_card_format_legalities_t"

formats = set()
format_legalities = set()


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

    print(f"Found the following formats: {formats}")
    print(f"Found the following legalities: {format_legalities}")


def gen_header() -> None:
    output_h = f"""#pragma once
{FILE_NOTICE}

"""

    # FORMAT_ENUM
    output_h += f"typedef enum {FORMAT_ENUM}" + "{\n"
    for format in formats:
        output_h += f"  MSE_FORMAT_{format.upper()},\n"
    output_h += f"  {PREFIX}_FORMATS_END\n" + "}" + f" {FORMAT_ENUM};\n"

    # FORMAT_LEGALITIES_ENUM
    output_h += f"\ntypedef enum {FORMAT_LEGALITIES_ENUM}" + "{\n"
    for legality in format_legalities:
        output_h += f"  {PREFIX}_FORMAT_LEGALITIES_{legality.upper()},\n"
    output_h += (
        f"  {PREFIX}_FORMAT_LEGALITIES_END\n" + "}" + f" {FORMAT_LEGALITIES_ENUM};\n\n"
    )

    # CARD_FORMAT_LEGALITIES_STRUCT
    output_h += f"typedef struct {CARD_FORMAT_LEGALITIES_STRUCT} " + "{\n"
    for format in formats:
        output_h += f"    {PREFIX.lower()}_format_legalities_t {format.lower().replace(' ', '')};\n"
    output_h += "}" +f" {CARD_FORMAT_LEGALITIES_STRUCT};\n"

    output_h += f"""
int {PREFIX.lower()}_str_as_{FORMAT_ENUM}(char *str, {FORMAT_ENUM} *ret);
int {PREFIX.lower()}_str_as_{FORMAT_LEGALITIES_ENUM}(char *str, {FORMAT_ENUM} *ret);

const char *{PREFIX.lower()}_{FORMAT_LEGALITIES_ENUM}_as_str({FORMAT_ENUM} ret);
const char *{PREFIX.lower()}_{FORMAT_ENUM}_as_str({FORMAT_ENUM} ret);
"""

    with open(OUTPUT_FILE_H, "w") as f:
        f.write(output_h)


def gen_unit() -> None:
    output_unit = f"""{FILE_NOTICE}
#include "{OUTPUT_FILE_H}"
#include <string.h>

int {PREFIX.lower()}_str_as_{FORMAT_ENUM}(char *str, {FORMAT_ENUM} *ret)
"""
    output_unit += "{\n"

    i = 0
    for format in formats:
        output_unit += "    "
        if i > 0:
            output_unit += "else "
        i += 1

        output_unit += f'if (strcmp(str, "{format.lower()}") == 0)' + "{\n"
        output_unit += f"        *ret = {PREFIX}_FORMAT_{format.upper()};\n"
        output_unit += "        return 1;\n"
        output_unit += "    }\n"

    output_unit += """
    return 0;
}
"""

    output_unit += f"int {PREFIX.lower()}_str_as_{FORMAT_LEGALITIES_ENUM}(char *str, {FORMAT_ENUM} *ret)\n"
    output_unit += "{\n"

    i = 0
    for legality in format_legalities:
        output_unit += "    "
        if i > 0:
            output_unit += "else "
        i += 1

        output_unit += f'if (strcmp(str, "{legality.lower()}") == 0)' + "{\n"
        output_unit += (
            f"        *ret = {PREFIX}_FORMAT_LEGALITIES_{legality.upper()};\n"
        )
        output_unit += "        return 1;\n"
        output_unit += "    }\n"

    output_unit += """
    return 0;
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

import os
import sys
import json
from typing import List
from common import FILE, PREFIX, FILE_NOTICE, read_mtg_json

BASENAME = "mse_formats"
OUTPUT_FILE_H = f"{BASENAME}.h"
OUTPUT_FILE_U = f"{BASENAME}.c"

FORMAT_ENUM = f"{PREFIX.lower()}_formats_t"

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

/// This file has a list of all formats
"""

    output_h += f"typedef enum {FORMAT_ENUM}" + "{"

    for format in formats:
        output_h += f"MSE_FORMAT_{format.upper()}, "

    output_h += f"{PREFIX}_FORMATS_END" + "}" + f"{FORMAT_ENUM};"

    output_h += """
int {PREFIX.lower()}_as_{FORMAT_ENUM}(char *str, {FORMAT_ENUM} *ret);
    """

    with open(OUTPUT_FILE_H, "w") as f:
        f.write(output_h)


def gen_unit() -> None:
    output_unit = f"""{FILE_NOTICE}
#include "{OUTPUT_FILE_H}"

#define MSE_UNDERSCORE_POS 26"""

    with open(OUTPUT_FILE_U, "wb") as f:
        f.write(output_unit.encode("utf-8"))


def main() -> None:
    read_json_file()
    gen_header()
    gen_unit()


if __name__ == "__main__":
    main()

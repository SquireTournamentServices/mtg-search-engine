from typing import List
from common import FILE_NOTICE

BASENAME = "mse_char_map"
OUTPUT_FILE_H = f"{BASENAME}.h"
OUTPUT_FILE_U = f"{BASENAME}.c"
CHAR_MAP_DEF = "char mse_filter_char_map[0xFF]"
GET_INDEX_DEF = "long mse_char_map_get_index(char c)"


def gen_header() -> None:
    output_h = f"""#pragma once
{FILE_NOTICE}

/// This file map will turn chars into the filtered output, returning 0 when they are to be ignored
extern {CHAR_MAP_DEF};

/*
     _______           ZZZZ
   / __    \\  /\\/\\ ZZZZ
  /    \\    \\/ uu \\
 / _____|_  > ____/<
 \\________)______))

*/
/// We use the latin alaphabet, arabic digits, (and '_'), if you are not using this alphabet then I am sorry
/// Here is an ascii cat to make you feel better

/// Gets the index for an array that uses the defined chars
#define MSE_ALPHABET_LENGTH (26 + 1 + 10)
{GET_INDEX_DEF};"""
    with open(OUTPUT_FILE_H, "w") as f:
        f.write(output_h)


def __get_entry_val(i: int) -> int:
    s: str = chr(i).lower()
    # Filter out vowels
    if s in ["a", "â", "e", "i", "o", "u", "û"]:
        return 0

    # Spanish people!
    if s in ["ñ"]:
        return ord("n")

    # Germans
    if s in ["ß"]:
        return ord("s")

    # Idk where these letters come from
    if s in ["ý", "ÿ"]:
        return ord("y")

    # French people
    if s in ["ç"]:
        return ord("c")

    # The americans like to use Z instead of S
    if s == "z":
        return ord("s")

    # Silly cards with those silly blank names
    if s == "_":
        return ord("_")

    # Letters and, numbers are good
    if (s.isalpha() and i <= ord("z")) or (i >= ord("0") and i <= ord("9")):
        return ord(s)
    # This letter can probably be ignored
    return 0


def get_entry(i: int) -> str:
    val: int = __get_entry_val(i)
    return f"    /* {i} - {chr(i)} */ {val}"


def gen_unit() -> None:
    output_unit = f"""
#include "{OUTPUT_FILE_H}"

#define MSE_UNDERSCORE_POS 26

{FILE_NOTICE}

{CHAR_MAP_DEF} =
"""
    entries: List[str] = []
    for i in range(0, 0xFF):
        entries.append(get_entry(i))

    output_unit += "{\n" + ",\n".join(entries) + "\n};\n\n"
    output_unit += GET_INDEX_DEF + "\n"
    output_unit += """{
    if (c == '_') {
        return MSE_UNDERSCORE_POS;
    } else if (c >= 'a' && c <= 'z'){
        return c - 'a';
    } else if (c >= '0' && c <= '9') {
        return MSE_UNDERSCORE_POS + c - '0';
    } else {
        return -1;
    }
}"""

    with open(OUTPUT_FILE_U, "wb") as f:
        f.write(output_unit.encode("utf-8"))


def main() -> None:
    gen_header()
    gen_unit()


if __name__ == "__main__":
    main()

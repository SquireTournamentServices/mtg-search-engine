import json
from iso639.exceptions import InvalidLanguageValue
from iso639 import Lang
from typing import List
from common import FILE, PREFIX, FILE_NOTICE, read_mtg_json

OUTPUT_FILE = "mtgjson_initial.h"


def escape(s: str) -> str:
    ret = ""
    for i in range(len(s)):
        ret += s[i]
        # This allows windows to have functional strings
        # - idk why this is even a feature in windows
        if i % 200 == 0:
            ret += "\n"

    # Escape \ to \\, Escape " to \", Fix new lines
    ret = ret.replace("\\", "\\\\").replace('"', '\\"').replace("\n", '"\n"')
    return ret


def get_langs_from_card(card_object: dict) -> List[str]:
    f_data = card_object["foreignData"]
    output = []
    for translation in f_data:
        lang = translation["language"]
        if lang not in output:
            output.append(lang)
    return output


def get_langs_from_set(set_object: dict) -> List[str]:
    cards = set_object["cards"]
    output = []
    for card in cards:
        langs = get_langs_from_card(card)
        for lang in langs:
            if lang not in output:
                output.append(lang)
    return output


def get_first_letter(word: str) -> str:
    for c in word:
        if c.isalnum():
            return c.lower()
    return ""


def get_lang_code(lang_name: str) -> str:
    try:
        return Lang(lang_name).pt1
    except InvalidLanguageValue as e:
        tmp: List[str] = lang_name.split(" ")
        if len(tmp) == 0:
            raise e
        if tmp[0] == "":
            raise e
        suffix = []
        if len(tmp) >= 2:
            suffix = get_first_letter(tmp[-1])
        return get_lang_code(" ".join(tmp[0:-1])) + suffix


def get_languages_from_cards(data: str) -> List[str]:
    print("Getting languages")
    data_j: dict = json.loads(data)
    codes: dict = dict()

    # English is not in foreign data (as it isn't classed as foreign?)
    ENGLISH: str = "English"
    codes[get_lang_code(ENGLISH)] = ENGLISH
    error_langs: List[str] = []

    for set_k in data_j["data"]:
        set_o: dict = data_j["data"][set_k]
        langs: List[str] = get_langs_from_set(set_o)

        for lang_name in langs:
            try:
                lang_code: str = get_lang_code(lang_name)
            except InvalidLanguageValue:
                if lang_name not in error_langs:
                    print(f"Cannot find language {lang_name}, skipping")
                    error_langs.append(lang_name)
                continue
            if lang_code not in codes:
                codes[lang_code] = lang_name

    return codes


OUTPUT_FILE_BASE_NAME: str = "mse_langs"
GET_LANG_NAME_FUNC: str = f"{PREFIX.lower()}_get_lang_name_str"
GET_LANG_CODE_FUNC: str = f"{PREFIX.lower()}_get_lang_code_str"
LANG_CODES_ENUM_T: str = "mse_lang_codes_t"
ERROR_VAR: str = f"{PREFIX}_ERROR_VAR"


def get_const_name(body: str) -> str:
    return f"{PREFIX}_CONST_{body.upper()}"


def generate_output_header(langs: dict) -> str:
    codes: List[str] = [f"{PREFIX}_LANG_{lang.upper()}, " for lang in langs]
    langs_code_enum: str = f"typedef enum {LANG_CODES_ENUM_T}"
    langs_code_enum += (
        " {" + " ".join(codes) + " MSE_LANGS_END} " + f"{LANG_CODES_ENUM_T};"
    )

    langs_enum: str = f"""
/// This enum contains all of the language codes
{langs_code_enum}
    """

    langs_codes_consts: str = ""
    for lang in langs:
        langs_codes_consts += f"extern const char *{get_const_name(lang)};\n"

    langs_names_consts: str = ""
    for lang in langs:
        name: str = f"{lang}_NAME"
        langs_names_consts += f"extern const char *{get_const_name(name)};\n"

    output: str = f"""#pragma once
{FILE_NOTICE}

// Enum definition
{langs_enum}

// Language name strings
{langs_names_consts}

// Language codes strings
{langs_codes_consts}

extern const char *{ERROR_VAR};

/// Gets the language name for a code as a string
const char *{GET_LANG_NAME_FUNC}({LANG_CODES_ENUM_T} code);

/// Gets the language code as a string
const char *{GET_LANG_CODE_FUNC}({LANG_CODES_ENUM_T} code);"""
    return output


def generate_output_unit(langs: dict) -> str:
    lang_name_cases: str = "\n".join(
        [
            f"""    case {PREFIX}_LANG_{lang.upper()}:
        return {get_const_name(f"{lang}_NAME")};"""
            for lang in langs
        ]
    )
    lang_name_cases += f"""
    case MSE_LANGS_END:
        return {ERROR_VAR};"""
    lang_name_body: str = (
        """{
    switch(code) {
    """
        + lang_name_cases
        + """
    }
    return """
        + ERROR_VAR
        + """;
}"""
    )

    lang_code_cases: str = "\n".join(
        [
            f"""    case {PREFIX}_LANG_{lang.upper()}:
        return {get_const_name(f"{lang}")};"""
            for lang in langs
        ]
    )
    lang_code_cases += f"""
    case MSE_LANGS_END:
        return {ERROR_VAR};"""
    lang_code_body: str = (
        """{
    switch(code) {
"""
        + lang_code_cases
        + """
    }
    return """
        + ERROR_VAR
        + """;
}"""
    )

    langs_codes_consts: str = ""
    for lang in langs:
        langs_codes_consts += f'const char *{get_const_name(lang)} = "{lang}";\n'

    langs_names_consts: str = ""
    for lang in langs:
        name: str = f"{lang}_NAME"
        langs_names_consts += f'const char *{get_const_name(name)} = "{langs[lang]}";\n'

    output: str = f"""{FILE_NOTICE}
#include "{OUTPUT_FILE_BASE_NAME}.h"

// Code consts
{langs_codes_consts}

// Name consts
{langs_names_consts}

const char *{ERROR_VAR} = "Error - Language not found";

// Name str getter
const char *{GET_LANG_NAME_FUNC}({LANG_CODES_ENUM_T} code)
{lang_name_body}

// Code str getter
const char *{GET_LANG_CODE_FUNC}({LANG_CODES_ENUM_T} code)
{lang_code_body}"""
    return output


def generate_output(langs: dict) -> None:
    print("Generating output")
    header: str = generate_output_header(langs)
    unit: str = generate_output_unit(langs)

    with open(f"{OUTPUT_FILE_BASE_NAME}.h", "w") as f:
        f.write(header)

    with open(f"{OUTPUT_FILE_BASE_NAME}.c", "w") as f:
        f.write(unit)


if __name__ == "__main__":
    cards: dict = read_mtg_json()
    langs: dict = get_languages_from_cards(cards)
    generate_output(langs)

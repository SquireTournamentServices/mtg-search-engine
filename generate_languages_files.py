import json
import requests
import tempfile
import zipfile
from iso639.exceptions import InvalidLanguageValue
from iso639 import Lang
from typing import List

DATA_URL = "https://mtgjson.com/api/v5/AllPrintings.json.zip"
FILE = "AllPrintings.json"
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


def get_atomic_cards() -> str:
    print("Downloading atomic json as compressed stream")
    resp = requests.get(DATA_URL)

    with tempfile.TemporaryFile("wb+") as tmp_file:
        for chunk in resp.iter_content(chunk_size=512 * 1024):
            if chunk:  # filter out keep-alive new chunks
                tmp_file.write(chunk)

        tmp_file.seek(0)

        print("Decompressing")
        with zipfile.ZipFile(tmp_file, "r") as file:
            file.extract(FILE, "./")

    print("Generating output")
    with open(FILE, "rb") as f:
        data = f.read().decode("UTF-8")
        return data
    raise RuntimeError("Shit is fucked")


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


if __name__ == "__main__":
    cards = get_atomic_cards()
    langs = get_languages_from_cards(cards)
    print(langs)

FILE = "AllPrintings.json"
PREFIX: str = "MSE"
FILE_NOTICE = f"""// THIS FILE IS AUTO GENERATED, SEE {__file__}
// DO NOT MODIFY BY HAND"""


def read_mtg_json():
    print("Parsing MTG json")
    with open(FILE, "rb") as f:
        data = f.read().decode("UTF-8")
        return data

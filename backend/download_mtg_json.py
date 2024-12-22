import json
import requests
import tempfile
import zipfile
from common import FILE

DATA_URL = "https://mtgjson.com/api/v5/AllPrintings.json.zip"


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
    raise RuntimeError("Shit is fucked")


if __name__ == "__main__":
    get_atomic_cards()

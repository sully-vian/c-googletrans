import asyncio
import httpx
import sys
import os

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "py-googletrans"))

from googletrans.gtoken import TokenAcquirer
from googletrans import Translator

text = "Hallo Welt"

async def main():
    async with httpx.AsyncClient(http2=True) as client:
        acquirer = TokenAcquirer(client=client)
        token = await acquirer.do(text)
        print(f"Acquired token: {token}")

    async with Translator() as translator:
        result = await translator.translate(text, dest="fr")
        print(f"Translated: \"{text}\" --> \"{result.text}\"")

if __name__ == "__main__":
    asyncio.run(main())

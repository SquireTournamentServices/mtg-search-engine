import RandomQuery from "./randomQuery";

export default function Home() {
  return (
    <main className="flex min-h-screen flex-col items-center p-24">
      <div className="flex flex-col w-full h-full justify-center p-24 gap-3">
        <h1 className="text-center text-2xl font-bold">
          A FOSS Search Engine For MTG.
        </h1>
        <RandomQuery />
        <p>
          Written in C, with a NextJS frontend with a completely unlocked API.
          The search engine can be embedded into apps, used in the web, and is
          licensed under the unlicence. MTG Json is used as the primary data
          source, another free and open source service.
        </p>
        <p>
          This search engine is not currently feature compliant with Scryfall
          yet, however this on the roadmap along with bindings for other
          programming languages and support for none-english cards.
        </p>
      </div>
    </main>
  );
}

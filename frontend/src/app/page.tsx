import RandomQuery from "./randomQuery";

export default function Home() {
  return (
    <main className="flex flex-col items-center w-full h-full gap-5">
      <div className="flex flex-col justify-center gap-3 bg-white rounded-xl p-5 md:p-20 w-full">
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

      <div className="bg-white rounded-xl gap-5 flex flex-col p-5 md:p-20 w-full">
        <h2 className="text-2xl font-bold">Supported Queries</h2>
        <div className="flex flex-row flex-wrap gap-20">
          <div>
            <h3 className="text-xl font-bold">Query Composition</h3>
            <p>The syntax is designed to mimic that of Scryfall.</p>
            <p>You can use brackets as normal with negation.</p>
            <p>
              The below query gets all mutally exclusive cards in m20 and m19.
            </p>
            <blockquote>
              (set:m19 or set:m20) and !(set:m20 and set:m19)
            </blockquote>
          </div>
          <div>
            <h3 className="text-xl font-bold">Operations</h3>
            <ul>
              <li>and</li>
              <li>or</li>
            </ul>
          </div>
          <div>
            <h3 className="text-xl font-bold">Set Generators</h3>
            Queries are in the format {"<property><operator><value>"}, i.e:
            "set:m20".
          </div>
          <div>
            <h3 className="text-xl font-bold">Operators</h3>
            <ul>
              <li>: (includes)</li>
              <li>= (equals)</li>
              <li>{"<"} (less than)</li>
              <li>{"<="} (less than or equal)</li>
              <li>{">"} (greater than)</li>
              <li>{">="} (greater than or equal)</li>
              <li>!= (not equal)</li>
            </ul>
          </div>
          <div>
            <h3 className="text-xl font-bold">Properties</h3>
            <ul>
              <li>
                name (n) <i>(supports regex)</i>
              </li>
              <li>
                oracle (o) <i>(supports regex)</i>
              </li>
              <li>power (p)</li>
              <li>toughness (t)</li>
              <li>cmc (manacost)</li>
              <li>
                colour (color c) <i>(wubrg)</i>
              </li>
              <li>
                commander <i>(wubrg)</i>
              </li>
              <li>
                set <i>(set code only i.e: m18)</i>
              </li>
              <li>type (types)</li>
            </ul>
          </div>
          <div>
            <h3 className="text-xl font-bold">Regex</h3>
            <p>
              A regex query looks like this:{" "}
              <blockquote>/I am a re(gex)?/</blockquote>
            </p>
          </div>
        </div>
      </div>
    </main>
  );
}

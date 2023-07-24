import SearchBar from "../searchBar";
import Card from "./card";

export default async function SearchResultPage({ params }) {
    const query = decodeURIComponent(params.query);
    const resp = await fetch("http://127.0.0.1:4365/api", {
        method: "POST",
        body: query
    });
    const data = await resp.json();

    return (
        <main className="flex min-h-screen flex-col items-center justify-between p-24">
          <SearchBar query={query}/>
          <div className="flex flex-row flex-wrap gap-3">
          {
            data.cards.map((item) => (
              <Card data={item}/>
            ))
          }
          </div>
        </main>
    );
}
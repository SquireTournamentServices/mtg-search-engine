import SearchBar from "../searchBar";
import Card from "./card";

export default async function SearchResultPage({ params }) {
  const query = decodeURIComponent(params.query);
  const resp = await fetch("http://127.0.0.1:4365/api", {
    method: "POST",
    body: query,
  });
  const data = await resp.json();

  return (
    <main className="flex min-h-screen flex-col items-center justify-between p-24 gap-6">
      <SearchBar query={query} />
      <div className="md:flex md:flex-row md:flex-wrap grid grid-rows-5 gap-3 justify-center">
        {data.cards.map((item) => (
          <Card data={item} />
        ))}
      </div>
    </main>
  );
}

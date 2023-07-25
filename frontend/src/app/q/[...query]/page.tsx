import Card from "./card";

export default async function SearchResultPage({ params }) {
  const query = decodeURIComponent(params.query);
  const resp = await fetch("http://127.0.0.1:4365/api", {
    method: "POST",
    body: query,
  });
  const data = await resp.json();

  return (
    <div className="w-full mx-auto grid grid-cols-1 gap-4 md:grid-cols-2 lg:grid-cols-4">
      {data.cards.map((item) => (
        <Card data={item} />
      ))}
    </div>
  );
}

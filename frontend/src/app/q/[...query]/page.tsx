import Card from "./card";

export default async function SearchResultPage({ params }) {
  const query = decodeURIComponent(params.query);
  const resp = await fetch("http://127.0.0.1:4365/api", {
    method: "POST",
    body: query,
  });
  const data = await resp.json();

  return (
    <div className="md:flex md:flex-row md:flex-wrap grid auto-rows-max gap-3 justify-start">
      {data.cards.map((item) => (
        <Card data={item} />
      ))}
    </div>
  );
}

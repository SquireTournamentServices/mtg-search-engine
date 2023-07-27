import Card from "./card";

export default async function SearchResultPage({
  params,
}: {
  params: {
    query: string;
  };
}) {
  const query = decodeURIComponent(params.query);
  const resp = await fetch("http://127.0.0.1:4365/api", {
    method: "POST",
    body: query,
  });
  const data = await resp.json();
  const results = data.cards_total;
  const page = data.page + 1;
  const page_size = data.page_size;
  const pages = Math.ceil(results / page_size);

  return (
    <div>
      <div className="w-full mx-auto grid grid-cols-1 gap-4 md:grid-cols-2 lg:grid-cols-4">
        {data.cards.map((item: any) => (
          <Card data={item} key={item} />
        ))}
      </div>
      <p className="text-center">
        {results} Results | Page {page} / {page_size}
      </p>
    </div>
  );
}

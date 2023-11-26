import Card from "./card";
import PageChanger from "./pageChanger";

export default async function SearchResultPage({
  params,
}: {
  params: {
    query: string;
    page: number;
  };
}) {
  const query = decodeURIComponent(params.query);
  const resp = await fetch("http://127.0.0.1:4365/api", {
    method: "POST",
    body: query,
    headers: {
      page: (params.page - 1).toString(),
    },
  });
  const data = await resp.json();
  const results = data.cards_total;
  const page = data.page;
  const page_size = data.page_size;
  const pages = Math.ceil(results / page_size);

  return (
    <div className="flex flex-col gap-5 pb-5 pt-5">
      <div className="w-full mx-auto grid grid-cols-1 gap-4 md:grid-cols-2 lg:grid-cols-4">
        {data.cards.map((item: any) => (
          <Card data={item} key={item} />
        ))}
      </div>
      <p className="text-center">
        {results} Results | Page {page} / {pages}
      </p>
      <div className="flex flex-row gap-3 w-full overflow-x-auto py-5">
        {Array(pages)
          .fill(0)
          .slice(Math.max(0, page - 5), page + 1 + 50)
          .map((_, i) => (
            <PageChanger base_url={"/q/" + params.query} page={i + 1} key={i} />
          ))}
      </div>
    </div>
  );
}
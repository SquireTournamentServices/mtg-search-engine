import Card from "./card";
import PageChanger from "../../components/pageChanger";
import searchUrlFor from "../searchUrl";
import { defaultApiUrl } from "../apiDefaultUrl";
import Link from "next/link";

export const dynamic = "force-dynamic";

interface params {
  query: string;
  page?: string;
  sort?: string;
  sort_asc?: string;
}

interface Props {
  searchParams: Promise<params>;
}

export default async function SearchResultPage({
  searchParams: paramsRaw,
}: Readonly<Props>) {
  const searchParams = await paramsRaw;
  const query = decodeURIComponent(
    searchParams.query.replaceAll("â", "a").replaceAll("û", "u"),
  );
  const resp = await fetch(
    (process.env.BACKEND_URL ?? defaultApiUrl) + "/api",
    {
      method: "POST",
      body: query,
      headers: {
        page: (parseInt(searchParams.page ?? "1") - 1).toString(),
        sort: searchParams.sort ?? "0",
        sort_asc: searchParams.sort_asc,
      },
    },
  );

  let data: any;
  if (resp.ok) {
    data = await resp.json();
  } else {
    return <p>There was an error: {await resp.text()}</p>;
  }

  const results = data.cards_total;
  const page = data.page ?? parseInt(searchParams.page, 10) ?? -1;
  const sort = parseInt(searchParams.sort, 10) ?? -1;
  const sort_asc = parseInt(searchParams.sort_asc, 10) ?? -1;
  const page_size = data.page_size;
  const pages = Math.ceil(results / page_size);

  return (
    <div className="flex flex-col gap-5 pb-5 pt-5 justify-between w-full">
      <div className="w-full mx-auto grid grid-cols-1 gap-4 md:grid-cols-2 lg:grid-cols-4">
        {data.cards.map((item: any) => (
          <Link href={`/card/${encodeURIComponent(item.id)}`} key={item.id}>
            <Card data={item} />
          </Link>
        ))}
      </div>
      <div className="flex flex-col">
        <p className="text-center">
          {results <= 0 && (
            <div className="text-2xl">No cards match your query.</div>
          )}
          {results} Results | Page {page + 1} / {pages}
        </p>
        <div className="flex flex-row gap-3 w-full overflow-x-auto py-5">
          {Array(pages)
            .fill(0)
            .map((_, i) => (
              <PageChanger
                base_url={searchUrlFor(
                  searchParams.query,
                  i + 1,
                  sort,
                  sort_asc === 1 ? true : false,
                )}
                page={i + 1}
                key={i + "page-changer"}
              />
            ))}
        </div>
      </div>
    </div>
  );
}

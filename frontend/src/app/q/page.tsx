"use client";
import { useRouter } from "next/router";
import { searchUrlFor } from "../searchBar";
import Card from "./card";
import PageChanger from "./pageChanger";

export default async function SearchResultPage({
  searchParams,
}: {
  searchParams: {
    query: string;
    page?: string;
  };
}) {
  const query = decodeURIComponent(searchParams.query);
  const resp = await fetch(
    process.env.BACKEND_URL ?? "http://127.0.0.1:4365/api",
    {
      method: "POST",
      body: query,
      headers: {
        page: (parseInt(searchParams.page ?? "1") - 1).toString(),
      },
    },
  );

  let data: any;
  if (resp.ok) {
    data = await resp.json();
  } else {
    return <p>Error</p>;
  }
  const results = data.cards_total;
  const page = data.page;
  const page_size = data.page_size;
  const pages = Math.ceil(results / page_size);
  const router = useRouter();
  router.prefetch(searchUrlFor(searchParams.query, page + 1));

  return (
    <div className="flex flex-col gap-5 pb-5 pt-5 justify-between w-full">
      <div className="w-full mx-auto grid grid-cols-1 gap-4 md:grid-cols-2 lg:grid-cols-4">
        {data.cards.map((item: any) => (
          <Card data={item} key={item} />
        ))}
      </div>
      <div className="flex flex-col">
        <p className="text-center">
          {results} Results | Page {page + 1} / {pages}
        </p>
        <div className="flex flex-row gap-3 w-full overflow-x-auto py-5">
          {Array(pages)
            .fill(0)
            .map((_, i) => (
              <PageChanger
                base_url={searchUrlFor(searchParams.query, i + 1)}
                page={i + 1}
                key={i}
                currentPage={(i + 1).toString() == searchParams.page}
              />
            ))}
        </div>
      </div>
    </div>
  );
}

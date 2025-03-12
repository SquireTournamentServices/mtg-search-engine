"use client";
import { Suspense, useEffect, useState } from "react";
import { usePathname, useRouter } from "next/navigation";
import { useSearchParams } from "next/navigation";
import searchUrlFor, {
  SortType,
  sortTypeAsString,
  usableSortTypes,
} from "./searchUrl";

function SearchBar() {
  const searchParams = useSearchParams();
  const path = usePathname();
  const router = useRouter();

  const [query, setQuery] = useState(searchParams.get("query") ?? "");
  const [sort, setSort] = useState(
    searchParams.get("sort") ?? SortType.CardName.toString(),
  );

  useEffect(() => {
    setQuery(searchParams.get("query"));
    setSort(searchParams.get("sort"));
  }, [searchParams]);

  const doSearch = (sortOverride: string = sort) => {
    router.push(
      searchUrlFor(query, 1, Number.parseInt(sortOverride) as SortType, true),
    );
  };

  return (
    <div className="flex flex-row w-full justify-center">
      <form
        className="flex flex-1 justify-center gap-3"
        onSubmit={(e) => {
          doSearch();
          e.preventDefault();
        }}
      >
        <div className="flex flex-row bg-slate-100 rounded-xl px-3 py-1 w-64 lg:w-1/3 gap-3">
          <input
            type="text"
            required
            placeholder="Search"
            value={query}
            onChange={(e) => setQuery(e.target.value)}
            className="bg-slate-100 hover:bg-slate-200 rounded-xl w-full px-2 py-1"
          />
          <button
            type="submit"
            className="hover:bg-slate-200 rounded-xl hover:scale-125"
          >
            🔍
          </button>
        </div>

        <fieldset>
          <select
            id="sort-type"
            className="px-2 py-1 rounded-xl h-full"
            onChange={(e) => {
              const newSort = e.target.value;
              setSort(newSort);

              if (path.includes("/q")) {
                doSearch(newSort);
              }
            }}
            value={sort}
          >
            {usableSortTypes().map((x) => (
              <option key={`sort-type-${x}`} id={`sort-type-${x}`} value={x}>
                {sortTypeAsString(x)}
              </option>
            ))}
          </select>
        </fieldset>
      </form>
    </div>
  );
}

export default function SearchBarWrapped() {
  return (
    <Suspense>
      <SearchBar />
    </Suspense>
  );
}

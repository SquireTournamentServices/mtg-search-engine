"use client";
import { Suspense, useEffect, useState } from "react";
import { useRouter } from "next/navigation";
import { useSearchParams } from "next/navigation";
import searchUrlFor from "./searchUrl";

function SearchBar() {
  const searchParams = useSearchParams();
  const initialQuery = searchParams.get("query") ?? "";
  const [query, setQuery] = useState(initialQuery);
  const router = useRouter();

  useEffect(() => {
    setQuery(initialQuery);
  }, [initialQuery, setQuery]);

  return (
    <div className="flex flex-row w-full justify-center">
      <div className="flex flex-row bg-slate-100 rounded-xl px-3 py-1 w-64 lg:w-1/3">
        <form
          className="flex flex-row w-full justify-between gap-3"
          onSubmit={(e) => {
            router.push(searchUrlFor(query, 1));
            e.preventDefault();
          }}
        >
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
        </form>
      </div>
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

"use client";
import { useState } from "react";
import { useRouter } from "next/navigation";
import { useSearchParams } from "next/navigation";

export function searchUrlFor(query: string, page: number): string {
  return "/q/?query=" + encodeURIComponent(query) + "&page=" + page.toString();
}

export default function SearchBar() {
  const searchParams = useSearchParams();
  const query = searchParams.get("query") ?? "";
  const [name, setName] = useState(query);
  const router = useRouter();

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
            value={name}
            onChange={(e) => setName(e.target.value)}
            className="bg-slate-100 hover:bg-slate-200 rounded-xl w-full px-1"
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

"use client";
import { useState } from "react";

export default function RandomQuery() {
  const [query, setQuery] = useState("");
  const queries: string[] = [
    "c:r and name:goblin",
    "c:u and name:wizard and oracle:/draw( a)|[0-9]cards?/",
    "goblin motivator",
    "set:m20 and c:u",
  ];
  setInterval(() => {
    setQuery(queries[Math.floor(Math.random() * queries.length)]);
  }, 1000);

  return (
    <div className="flex flex-row">
      <h2 className="text-l px-3">
        Powerful search engine, search for anything:
      </h2>
      <p>🔍 {query}</p>
    </div>
  );
}

"use client";
import { useEffect, useState } from "react";

export default function RandomQuery() {
  const [query, setQuery] = useState("set:m18 or set:m19");
  const queries: string[] = [
    "c:r and name:goblin",
    "c:u and name:wizard and oracle:/draw( a)|[0-9]cards?/",
    "goblin motivator",
    "set:m20 and c:u",
    "hydra o:/^x?.*.?\\sx[., ].*$/",
    "i:r -c:r",
    "-name:goblin c:r oracle:goblin",
    "-set:m20 set:m21",
    "(i:r -c:r) or i>rgb",
    'set:m19 and type:goblin and name:"motivator"',
  ];
  useEffect(() => {
    setInterval(() => {
      setQuery(queries[Math.floor(Math.random() * queries.length)]);
    }, 1000);
  }, []);

  return (
    <div className="flex flex-row">
      <h2 className="text-l">Powerful search engine, search for anything:</h2>
      <p className="px-3">🔍 {query}</p>
    </div>
  );
}

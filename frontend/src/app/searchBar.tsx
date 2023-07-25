"use client";
import { useState } from "react";
import Link from 'next/link';

export default function SearchBar(props) {
  const [name, setName] = useState(props.query || "");

  return (
    <div className="flex flex-row">
      <h1 className="px-10"><Link href="/">MTG Search engine</Link></h1>
    <div className="flex flex-row bg-slate-100 rounded-xl px-3 py-1">
      <form
        onSubmit={(e) => {
          window.location.href = "/" + encodeURIComponent(name);
          e.preventDefault();
        }}
      >
        <input
          type="text"
          required
          placeholder="Search"
          value={name}
          onChange={(e) => setName(e.target.value)}
          className="w-max bg-slate-100"
        />
        <button type="submit" className="hover:bg-slate-200 rounded-xl">
          🔍
        </button>
      </form>
    </div>
    </div>
  );
}

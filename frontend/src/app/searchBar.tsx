"use client";
import { useState } from "react";

export default function SearchBar(props) {
  const [name, setName] = useState(props.query || "");

  return (
    <div className="flex flex-row w-full justify-center">
      <div className="flex flex-row bg-slate-100 rounded-xl px-3 py-1 w-1/3">
        <form
          className="flex flex-row w-full justify-between gap-3"
          onSubmit={(e) => {
            window.location.href = "/q/" + encodeURIComponent(name);
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
          <button type="submit" className="hover:bg-slate-200 rounded-xl">
            🔍
          </button>
        </form>
      </div>
    </div>
  );
}

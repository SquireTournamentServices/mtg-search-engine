"use client";
import { useState } from "react";

export default function SearchBar(props) {
  const [name, setName] = useState(props.query || "");

  return (
    <div className="flex flex-row">
      <div className="flex flex-row bg-slate-100 rounded-xl px-3 py-1">
        <form
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
            className="w-max bg-slate-100 rounded-l"
          />
          <button type="submit" className="hover:bg-slate-200 rounded-xl">
            🔍
          </button>
        </form>
      </div>
    </div>
  );
}

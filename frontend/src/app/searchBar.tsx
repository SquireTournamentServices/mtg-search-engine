"use client";
import { useState } from "react";

export default function SearchBar(props) {
  const [name, setName] = useState(props.query || "");

  return (
    <div className="flex flex-row bg-slate-100 rounded-l px-3">
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
        <button type="submit" className="px-5 hover:bg-slate-200 rounded">
          Search
        </button>
      </form>
    </div>
  );
}

import SearchBar from "./searchBar";
import RandomQuery from "./randomQuery";

export default function Home() {
  return (
    <main className="flex min-h-screen flex-col items-center p-24">
      <SearchBar />
      <div className="flex flex-col w-full h-full justify-center p-24">
        <h1 className="text-center text-xl font-bold">
          A FOSS Search Engine For MTG.
        </h1>
        <RandomQuery />
      </div>
    </main>
  );
}

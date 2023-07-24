import SearchEngine from "./searchBar"

export default function Home() {
  return (
    <main className="flex min-h-screen flex-col items-center justify-between p-24">
      <SearchEngine/>
      <h1>A FOSS Search Engine For MTG.</h1>
    </main>
  )
}

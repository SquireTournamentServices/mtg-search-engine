import type { Metadata } from "next";
import { Inter } from "next/font/google";
import "./globals.css";
import SearchBar from "./searchBar";
import Link from "next/link";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import { faGithub } from "@fortawesome/free-solid-svg-icons";

const inter = Inter({ subsets: ["latin"] });

export const metadata: Metadata = {
  title: "MTG Search Engine",
  description: "A FOSS search engine for MTG cards.",
};

export default function RootLayout({
  children,
}: {
  children: React.ReactNode;
}) {
  return (
    <html lang="en">
      <body className={inter.className + " min-h-screen"}>
        <div className="py-5 px-5 flex flex-row justify-between sticky top-0 bg-slate-900">
          <div className="flex flex-row">
            <h1 className="text-white px-3 py-1 grow">
              <Link href="/">Home</Link>
            </h1>
          </div>
          <SearchBar />
          <div className="flex flex-row text-white">
            <Link href="https://github.com/MonarchDevelopment/mtg-search-engine">
              <FontAwesomeIcon icon={faGithub} />
              Github
            </Link>
          </div>
        </div>
        <div className="p-5 justify-center">{children}</div>
      </body>
    </html>
  );
}

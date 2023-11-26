import type { Metadata } from "next";
import { Inter } from "next/font/google";
import "./globals.css";
import SearchBar from "./searchBar";
import Link from "next/link";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import { faGithub } from "@fortawesome/free-brands-svg-icons";
import { faHome } from "@fortawesome/free-solid-svg-icons";

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
        <div className="p-5 flex flex-row justify-between sticky top-0 bg-slate-900 z-50">
          <Link href="/" className="w-8 text-white hover:scale-125">
            <FontAwesomeIcon icon={faHome} />
          </Link>
          <SearchBar />
          <Link
            href="https://github.com/MonarchDevelopment/mtg-search-engine"
            className="w-8 text-white hover:scale-125"
          >
            <FontAwesomeIcon icon={faGithub} />
          </Link>
        </div>
        <div className="bg-slate-100">
          <div className="p-5 justify-center">{children}</div>
        </div>
      </body>
    </html>
  );
}

import type { Metadata } from "next";
import { Inter } from "next/font/google";
import "./globals.css";
import SearchBar from "./searchBar";
import Link from "next/link";

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
      <body className={inter.className}>
        <div className="py-5 px-5 flex flex-row justify-between">
          <div className="flex flex-row">
            <h1 className="hover:bg-slate-300 bg-slate-100 rounded-xl px-3 py-1">
              <Link href="/">Home</Link>
            </h1>
          </div>
          <SearchBar />
          <div />
        </div>
        {children}
      </body>
    </html>
  );
}

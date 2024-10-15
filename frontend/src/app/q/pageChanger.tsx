"use client";

import Link from "next/link";
import { useSearchParams } from "next/navigation";

interface Props {
  base_url: string;
  page: number;
}

export default function PageChanger({ base_url, page }: Readonly<Props>) {
  const searchParams = useSearchParams();
  const pageNumber = searchParams.get("page") ?? '1';
  const currentPage = pageNumber === page.toString();

  return (
    <Link href={base_url + "&page=" + page.toString()}>
      <h1
        className={`p-3 ${
          currentPage ? "bg-red-700" : "bg-blue-500"
        } text-white rounded`}
      >
        {page}
      </h1>
    </Link>
  );
}

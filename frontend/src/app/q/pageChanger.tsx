import Link from "next/link";

interface Props {
  base_url: string;
  page: number;
  currentPage: boolean;
}

export default function PageChanger({ base_url, page, currentPage }) {
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

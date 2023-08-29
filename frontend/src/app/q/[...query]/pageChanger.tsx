import Link from "next/link";

export default function PageChanger(props: { base_url: string; page: number }) {
  return (
    <h1 className="p-3 bg-blue-500 text-white rounded">
      <Link href={props.base_url + "?page=" + props.page}>{props.page}</Link>
    </h1>
  );
}

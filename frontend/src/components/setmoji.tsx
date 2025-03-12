import Image from "next/image";

interface Props {
  code: string;
}

export default function Setmoji({ code }: Readonly<Props>) {
  const img = "/setmoji/sets/" + code.toLowerCase() + ".svg";
  const size = 22;

  return (
    <span>
      <Image
        src={img}
        width={size}
        height={size}
        className="aspect-square inline"
        alt={"Setmoji of " + code}
      />
    </span>
  );
}

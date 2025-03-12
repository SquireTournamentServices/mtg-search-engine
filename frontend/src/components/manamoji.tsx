import Image from "next/image";

interface Props {
  mana_cost: string;
  large?: boolean;
}

export default function Manamoji(props: Readonly<Props>) {
  const img =
    "/manamoji/emojis/mana" +
    props.mana_cost.toLowerCase().replace("/", "") +
    ".png";
  const size = props.large ? 25 : 20;

  return (
    <span>
      <Image
        src={img}
        width={size}
        height={size}
        className="aspect-square inline"
        alt={"Manamoji of " + props.mana_cost}
      />
    </span>
  );
}

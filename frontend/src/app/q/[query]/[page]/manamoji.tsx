import Image from "next/image";

export default function Manamoji(props: { mana_cost: string }) {
  const img =
    "/manamoji/emojis/mana" +
    props.mana_cost.toLowerCase().replace("/", "") +
    ".png";
  const size = 20;

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

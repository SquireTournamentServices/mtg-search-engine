import Image from "next/image";

export default function Manamoji(props) {
  const img = "/manamoji/emojis/mana" + props.mana_cost.toLowerCase() + ".png";
  const size = 20;

  return (
    <Image
      src={img}
      width={size}
      height={size}
      alt={"Manamoji of " + props.mana_cost}
    />
  );
}

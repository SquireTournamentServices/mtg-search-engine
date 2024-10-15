import Manamoji from "./manamoji";
import Oracle from "./oracle";
import Colour from "./colour";

export default function Card(props: {
  data: {
    name: string;
    mana_cost: string;
    colours: number;
    colour_identity: number;
    oracle_text: string;
    power: string;
    toughness: string;
    types: string[];
    sets: string[];
  };
}) {
  const colours = [];
  for (var i = 1; i < 1 << 5; i <<= 1) {
    const c = props.data.colour_identity & i;
    if (c) {
      colours.push(c);
    }
  }

  return (
    <div className="flex flex-col rounded-xl bg-white min-h-[300px] p-5 drop-shadow">
      <div className="flex flex-row gap-3 justify-between">
        <h2 className="font-bold hyphens-auto">{props.data.name}</h2>
        <div className="flex flex-row flex-wrap w-max justify-end">
          {props.data.mana_cost &&
            props.data.mana_cost
              .split("{")
              .slice(1)
              .map((mana) => (
                <Manamoji mana_cost={mana.replace("}", "")} key={mana} />
              ))}
        </div>
      </div>
      {props.data.types && (
        <p className="text-m"> {props.data.types.join(" ")} </p>
      )}
      {props.data.oracle_text && (
        <Oracle oracle_text={props.data.oracle_text} />
      )}
      <div className="grow" />
      <div className="flex flex-row justify-between">
        <div className="flex flex-row gap-1">
          {colours.map((c) => (
            <Colour colour={c} key={c} />
          ))}
        </div>
        <p className="text-right">
          {props.data.power && props.data.toughness
            ? props.data.power + "/" + props.data.toughness
            : ""}
        </p>
      </div>
      <div>Printed in: {props.data.sets.join(", ")}</div>
    </div>
  );
}

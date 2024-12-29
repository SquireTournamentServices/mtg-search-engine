import Manamoji from "./manamoji";
import Oracle from "./oracle";
import Colour from "./colour";
import { useMemo } from "react";

const LEGENDARY = "Legendary";
const MAX_SETS_TO_SHOW = 5;

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

  const sets = useMemo(() => {
    let sets = props.data.sets;
    if (sets.length > MAX_SETS_TO_SHOW) {
      sets = sets.slice(0, MAX_SETS_TO_SHOW);
      sets.push(`(${props.data.sets.length - MAX_SETS_TO_SHOW} more ...)`);
    }

    return sets.map((x) => x.toUpperCase()).join(", ");
  }, [props.data.sets]);

  const isLegendary = useMemo(
    () => props.data.types.includes(LEGENDARY),
    [props.data.types],
  );

  return (
    <div
      className={`flex flex-col rounded-xl bg-white min-h-[300px] p-5 drop-shadow ${isLegendary ? "border-2 border-gray-100" : ""}`}
    >
      <div className="flex flex-row gap-3 justify-between">
        <h2 className="font-bold hyphens-auto">{props.data.name}</h2>
        <div className="flex flex-row flex-wrap w-max justify-end">
          {props.data.mana_cost &&
            props.data.mana_cost
              .split("{")
              .slice(1)
              .map((mana, i) => (
                <Manamoji
                  mana_cost={mana.replace("}", "")}
                  key={`${mana}-${i}-${props.data.name}`}
                />
              ))}
        </div>
      </div>
      {props.data.types && (
        <p className="text-m"> {props.data.types.join(" ")} </p>
      )}
      {props.data.oracle_text && (
        <Oracle oracle_text={props.data.oracle_text} id={props.data.name} />
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
      <div className="text-gray-800">Printed in: {sets}</div>
    </div>
  );
}

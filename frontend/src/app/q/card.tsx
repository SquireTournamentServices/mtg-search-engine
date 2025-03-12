import Manamoji from "../../components/manamoji";
import Oracle from "../../components//oracle";
import Colour from "../../components//colour";
import Setmoji from "../../components//setmoji";
import { useMemo } from "react";
import { type Card, MAX_SETS_TO_SHOW, LEGENDARY } from "../../model/card";

export default function Card(props: { data: Card }) {
  const colours = [];
  for (var i = 1; i < 1 << 5; i <<= 1) {
    const c = props.data.colour_identity & i;
    if (c) {
      colours.push(c);
    }
  }

  const sets = useMemo(() => {
    let sets = props.data.sets.map((x) => (
      <Setmoji code={x} key={`set-${x}-${props.data.id}`} />
    ));
    if (sets.length > MAX_SETS_TO_SHOW) {
      sets = sets.slice(0, MAX_SETS_TO_SHOW);
      sets.push(<p>{props.data.sets.length - MAX_SETS_TO_SHOW} more ...</p>);
    }
    return sets;
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
                  key={`mana-${mana}-${i}-${props.data.id}`}
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
            <Colour colour={c} key={`colours-${c}-${props.data.id}`} />
          ))}
        </div>
        <p className="text-right">
          {props.data.power && props.data.toughness
            ? props.data.power + "/" + props.data.toughness
            : ""}
        </p>
      </div>
      <div className="flex flex-row flex-wrap gap-1">
        <p>Sets:</p> {sets}
      </div>
    </div>
  );
}

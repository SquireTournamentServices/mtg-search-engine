import { isPropertySignature } from "typescript";
import Manamoji from "./manamoji.tsx";
import Oracle from "./oracle.tsx";

export default function Card(props) {
  return (
    <div className="flex flex-col rounded-xl bg-slate-100 w-72 min-h-[300px] p-5">
      <div className="flex flex-row flex-wrap gap-3 justify-between">
        <h2 className="font-bold">{props.data.name}</h2>
        <div className="flex flex-row flex-wrap w-max">
          {props.data.mana_cost &&
            props.data.mana_cost
              .split("{")
              .slice(1)
              .map((mana) => <Manamoji mana_cost={mana.replace("}", "")} />)}
        </div>
      </div>
      {props.data.types && (
        <p className="text-m"> {props.data.types.join(" ")} </p>
      )}
      {props.data.oracle_text && (
        <Oracle oracle_text={props.data.oracle_text} />
      )}
      <div className="grow" />
      <p className="text-right">
        {props.data.power && props.data.toughness
          ? props.data.power + "/" + props.data.toughness
          : ""}
      </p>
    </div>
  );
}

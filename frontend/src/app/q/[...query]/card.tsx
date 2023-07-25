import { isPropertySignature } from "typescript";
import Manamoji from "./manamoji.tsx";

export default function Card(props) {
  return (
    <div className="flex flex-col rounded-xl bg-slate-100 w-72 h-auto p-5">
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
        <p className="text-sm">{props.data.oracle_text}</p>
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

import { isPropertySignature } from "typescript";

export default function Card(props) {
  return (
    <div className="flex flex-col rounded-xl bg-slate-100 w-64 min-h-64 p-5">
      <h2 className="font-bold">
        {props.data.name} {props.data.mana_cost && " - " + props.data.mana_cost}
      </h2>
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

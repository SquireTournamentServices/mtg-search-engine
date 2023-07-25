import { isPropertySignature } from "typescript";

export default function Card(props) {
  return (
    <div className="flex-row w-64 min-h-64">
      <div className="flex-column h-full justify-between rounded-xl bg-slate-100 p-5">
        <h2 className="font-bold">
          {props.data.name}{" "}
          {props.data.mana_cost && " - " + props.data.mana_cost}
        </h2>
        {props.data.types && (
          <p className="text-m"> {props.data.types.join(" ")} </p>
        )}
        {props.data.oracle_text && (
          <p className="text-sm">{props.data.oracle_text}</p>
        )}
        <p className="justify-self-end align-self-end">
          {props.data.power && props.data.power}
          {props.data.toughness && "/" + props.data.toughness}
        </p>
      </div>
    </div>
  );
}

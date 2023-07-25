import Manamoji from "./manamoji.tsx";

export default function Oracle(props) {
  const oracle = props.oracle_text.split("{").slice(1);
  return (
    <span className="text-sm">
      {oracle.map((part) => {
        const mana = part.split("}");
        if (mana.length == 1) {
          return <>{part}</>;
        } else {
          return (
            <>
              <Manamoji mana_cost={mana[0]} />
              {mana.slice(1).join(" ")}
            </>
          );
        }
      })}
    </span>
  );
}

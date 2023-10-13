import Manamoji from "./manamoji";

export default function Oracle(props: { oracle_text: string }) {
  const oracle = props.oracle_text.split("{");
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

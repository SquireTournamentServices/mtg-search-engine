import { Fragment, ReactNode } from "react";
import Manamoji from "./manamoji";

export default function Oracle(props: { oracle_text: string; id: string }) {
  const oracle = props.oracle_text.split("{");
  return (
    <span className="text-sm">
      {oracle.map((part) => {
        const mana = part.split("}");
        if (mana.length == 1) {
          return part
            .split("\n")
            .map(
              (x, i): ReactNode => (
                <Fragment key={`${props.id}-${x}-${i}`}>{x}</Fragment>
              ),
            )
            .reduce(
              (a, b) => (
                <>
                  {a}
                  <br />
                  {b}
                </>
              ),
              <></>,
            );
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

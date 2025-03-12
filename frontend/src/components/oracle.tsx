import { Fragment, ReactNode } from "react";
import Manamoji from "./manamoji";

interface Props {
  oracle_text: string;
  id: string;
  large?: boolean;
}

function withNewLines(id: string, x: string) {
  const parts = x
    .split("\n")
    .map(
      (x, i): ReactNode => (
        <Fragment key={`${id} - ${x} - ${i}`}>{x} </Fragment>
      ),
    );

  if (parts.length > 1) {
    return parts.reduce(
      (a, b) => (
        <>
          {a} <br /> {b}{" "}
        </>
      ),
      <></>,
    );
  } else {
    return parts;
  }
}

export default function Oracle(props: Readonly<Props>) {
  const oracle = props.oracle_text.split("{");
  return (
    <span className={`${props.large ? "text-md" : "text-sm"}`}>
      {oracle.map((part) => {
        const mana = part.split("}");
        if (mana.length == 1) {
          return withNewLines(props.id, part);
        } else {
          return (
            <>
              <Manamoji mana_cost={mana[0]} />
              {mana.slice(1).map((x) => withNewLines(props.id, x))}{" "}
            </>
          );
        }
      })}{" "}
    </span>
  );
}

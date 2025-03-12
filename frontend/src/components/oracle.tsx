import { Fragment, ReactNode } from "react";
import Manamoji from "./manamoji";

interface Props {
  oracle_text: string;
  id: string;
  large?: boolean;
}

export default function Oracle(props: Readonly<Props>) {
  const oracle = props.oracle_text.split("{");
  return (
    <span className={`${props.large ? "text-md" : "text-sm"}`}>
      {oracle.map((part) => {
        const mana = part.split("}");
        if (mana.length == 1) {
          return part
            .split("\n")
            .map(
              (x, i): ReactNode => (
                <Fragment key={`${props.id} - ${x} - ${i}`}>{x} </Fragment>
              ),
            )
            .reduce(
              (a, b) => (
                <>
                  {a} <br /> {b}{" "}
                </>
              ),
              <></>,
            );
        } else {
          return (
            <>
              <Manamoji mana_cost={mana[0]} />
              {mana.slice(1).map((x) =>
                x
                  .split("\n")
                  .map(
                    (x, i): ReactNode => (
                      <Fragment key={`${props.id} - ${x} - ${i}`}>
                        {x}{" "}
                      </Fragment>
                    ),
                  )
                  .reduce(
                    (a, b) => (
                      <>
                        {a} <br /> {b}{" "}
                      </>
                    ),
                    <></>,
                  ),
              )}{" "}
            </>
          );
        }
      })}{" "}
    </span>
  );
}

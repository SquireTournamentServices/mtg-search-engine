import { sortTypes } from "../model/typeline";

interface Props {
  types: string[];
}

export function TypeLine(props: Readonly<Props>) {
  const types = sortTypes(props.types);

  return (
    <p className="text-m">
      {types.ImportantTypes.join(" ")}{" "}
      {types.ImportantTypes.length && types.MinorTypes.length && "-"}{" "}
      {types.MinorTypes.join(" ")}
    </p>
  );
}

import { sortTypes } from "../model/typeline";

interface Props {
  types: string[];
}

export function TypeLine(props: Readonly<Props>) {
  const types = sortTypes(props.types);

  return (
    <p className="text-m">
      {types.ImportantTypes.join(" ")} - {types.MinorTypes.join(" ")}
    </p>
  );
}

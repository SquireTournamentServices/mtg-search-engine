interface Props {
  format: string;
  legality: string;
}

export function Legality(props: Readonly<Props>) {
  const colour = mapLegalityToColour(props.legality);
  return (
    <div className="flex flex-row gap-1 justify-between">
      <p>{props.format}:</p>
      <div className={`${colour} p-1 text-white w-1/2 rounded-md`}>
        {props.legality}
      </div>
    </div>
  );
}

function mapLegalityToColour(legality: string): string {
  switch (legality.toLowerCase()) {
    case "legal":
      return "bg-green-600";
    case "banned":
      return "bg-red-500";
    case "restricted":
      return "bg-orange-500";
    case "unplayable":
      return "bg-gray-500";
    default:
      return "bg-amber-500";
  }
}

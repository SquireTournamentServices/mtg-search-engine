export default function Colour(props: { colour: number }) {
  var colour = "text-";
  switch (props.colour) {
    case 1 << 0:
      colour += "yellow-300";
      break;
    case 1 << 1:
      colour += "blue-700";
      break;
    case 1 << 2:
      colour += "purple-900";
      break;
    case 1 << 3:
      colour += "red-700";
      break;
    case 1 << 4:
      colour += "lime-700";
      break;
    default:
      colour += "black";
      console.log(props.colour, "is not a colour");
      break;
  }

  return (
    <span className={"font-bold text-xl " + colour}>
      ⏺
    </span>
  );
}

export default function Colour(props: { colour: number }) {
  var colourLetter = "";
  var colour = "";
  switch (props.colour) {
    case 1 << 0:
      colourLetter = "W";
      colour = "bg-yellow-300";
      break;
    case 1 << 1:
      colourLetter = "U";
      colour = "bg-blue-700";
      break;
    case 1 << 2:
      colourLetter = "B";
      colour = "bg-purple-900";
      break;
    case 1 << 3:
      colourLetter = "R";
      colour = "bg-red-700";
      break;
    case 1 << 4:
      colourLetter = "G";
      colour = "bg-lime-700";
      break;
    default:
      colour += "black";
      console.log(props.colour, "is not a colour");
      break;
  }

  return (
    <div className={"rounded-full w-[1rem] h-[1rem] " + colour}>
      <span className="absolute opacity-0">{colourLetter}</span>
    </div>
  );
}

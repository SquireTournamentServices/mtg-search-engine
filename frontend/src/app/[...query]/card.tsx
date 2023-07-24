import { isPropertySignature } from "typescript";

export default function Card(props) {
    return (
        <div className="flex-row w-64 min-h-64">
            <div className="flex-column">
                <h2>{props.data.name}</h2>
                { props.data.oracle_text && <p className="text-sm">{props.data.oracle_text}</p> }
            </div>
        </div>
    )
}
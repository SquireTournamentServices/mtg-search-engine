"use client";
import { useEffect } from "react";

interface Props {
  error: Error & { digest?: string };
  reset: () => void;
}

export default function Error({
  error,
  reset,
}: Readonly<Props>) {
  useEffect(() => {
    console.error(error);
  }, [error]);

  return (
    <div className="flex flex-col justify-center items-center">
      <h2 className="font-bold text-center text-2xl text-red-500">
        Cannot complete the search
      </h2>
      <button className="bg-white w-fit p-2 rounded-xl" onClick={() => reset()}>
        Try again
      </button>
    </div>
  );
}

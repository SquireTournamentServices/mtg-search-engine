export enum SortType {
  CardName = 0,
  Cmc = 1,
  Id = 2,
  Power = 3,
  Toughness = 4,
}

export function usableSortTypes(): SortType[] {
  return [
    SortType.CardName,
    SortType.Cmc,
    // This does not seem useful to expose to the end user
    // SortType.Id,
    SortType.Power,
    SortType.Toughness,
  ];
}

export function sortTypeAsString(sort: SortType): string {
  switch (sort) {
    case SortType.CardName:
      return "Name";
    case SortType.Id:
      return "Id";
    case SortType.Cmc:
      return "Mana Value";
    case SortType.Power:
      return "Power";
    case SortType.Toughness:
      return "Toughness";
  }
}

export default function searchUrlFor(
  query: string,
  page: number,
  sort: SortType,
  ascending: boolean,
): string {
  return (
    "/q/?query=" +
    encodeURIComponent(query) +
    "&page=" +
    page.toString() +
    "&sort=" +
    sort +
    "&sort_asc=" +
    (ascending ? "1" : "0")
  );
}

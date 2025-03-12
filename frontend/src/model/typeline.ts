enum superTypes {
  Legendary,
  Legend,
  Basic,
  Ongoing,
  Snow,
  World,
  Elite,
  Host,
  Artifact,
  Battle,
  Creature,
  Kindred,
  Enchantment,
  Land,
  Planeswalker,
  Instant,
  Sorcery,
}

export interface TypeLine {
  // Everything before the "-"
  ImportantTypes: string[];
  // Everything after the "-"
  MinorTypes: string[];
}

export function sortTypes(types: string[]): TypeLine {
  types = types.sort();

  const ret: TypeLine = {
    ImportantTypes : [],
    MinorTypes : [],
  };

  for (const type of types) {
    if (Object.keys(superTypes).map(x => x.toLowerCase()).find(x => x === type.toLowerCase())) {
      ret.ImportantTypes.push(type);
    } else {
      ret.MinorTypes.push(type);
    }
  }

  ret.ImportantTypes.sort((a, b) => {
    const aType = superTypes[a];
    const bType = superTypes[b];
    return aType - bType;
  });

  return ret;
}

export interface Card {
  id: string;
  name: string;
  mana_cost: string;
  colours: number;
  colour_identity: number;
  oracle_text: string;
  power: string;
  toughness: string;
  cmc: number;
  loyalty: number;
  types: string[];
  sets: string[];
  format_legalities: Record<string, string>;
}

export const LEGENDARY = "Legendary";
export const MAX_SETS_TO_SHOW = 10;

export interface Card {
    id: string;
    name: string;
    mana_cost: string;
    colours: number;
    colour_identity: number;
    oracle_text: string;
    power: string;
    toughness: string;
    types: string[];
    sets: string[];
};

export const LEGENDARY = "Legendary";
export const MAX_SETS_TO_SHOW = 10;

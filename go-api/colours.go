package goapi

type Colour int

const (
  ColourWhite Colour = 1 << iota
  ColourBlue
  ColourBlack
  ColourRed
  ColourGreen
)

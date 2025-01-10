package goapi

type SortType int

const (
	SortTypeCardName SortType = iota
	SortTypeCmc
	SortTypeCardId
	SortTypePower
	SortTypeToughness
	// DO NOT USE - This is for iterating over the SortType
	SortTypeEnd
)

type SortDirection int

const (
	SortDirectionDescending SortDirection = 0
	SortDirectionAscending  SortDirection = 1
)

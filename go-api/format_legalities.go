package goapi

type FormatLegality string

const (
	FormatLegality_Legal      FormatLegality = "Legal"
	FormatLegality_Banned     FormatLegality = "Banned"
	FormatLegality_Restricted FormatLegality = "Restricted"
	FormatLegality_Unplayable FormatLegality = "Unplayable"
)

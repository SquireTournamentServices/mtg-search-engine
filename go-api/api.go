package goapi

import (
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"strings"
	"sync"
	"time"
)

// Use `docker compose up backend` in this project to start a server on this URL.
// You can omit the "backend" argument if you want a frontend as well, see ../README.md
const ApiUrlLocal = "http://localhost:4365"

// This URL is to the up to date version that @djpiper28 hosts.
const ApiUrlOfficial = "https://monarch.djpiper28.co.uk/api"

type Request struct {
	SortType      SortType
	SortDirection SortDirection
	// See https://monarch.djpiper28.co.uk for example queries and a syntax overview
	Query string
	// Zero-bound, i.e: page 0 is the "first" page
	Page int
	Url  string
	// The client used to make the request, you can edit this to change things such as Timeouts, Proxies, etc...
	Client http.Client
}

// See https://monarch.djpiper28.co.uk for example queries and a syntax overview
func New(query string) Request {
	return Request{SortType: SortTypeCardName,
		SortDirection: SortDirectionAscending,
		Page:          0,
		Query:         query,
		Url:           ApiUrlOfficial,
		Client: http.Client{
			Timeout: time.Second * 5,
		},
	}
}

type Card struct {
	Id string `json:"id"`
	// Formely known as "CMC (converted mana cost)" is how many "manas" are need to cast a spell.
	// i.e: "{2}{R}{R}" has a mana value of 4
	ManaValue float32 `json:"cmc"`
	// The amount of loyalty counters a Planeswalker starts with
	Loyalty float32 `json:"loyalty"`
	// Mana used to cast the spell, i.e: "{2}{R}{R}"
	ManaCost string `json:"mana_cost"`
	// Colours that the card is i.e: "U"
	Colours Colour `json:"colours"`
	// Also known as "commander colours", i.e: "WUBRG"
	ColourIdentity Colour `json:"colour_identity"`
	// Rules text i.e: "{T}: Target creatures gains haste until end of turn."
	OracleText string `json:"oracle_text"`
	// Card name
	Name string `json:"name"`
	// Note, if non-zero then it can be that it is a special value such as 1+* 1.5, or “π"
	Power float32 `json:"power"`
	// Note, if non-zero then it can be that it is a special value such as 1+* 1.5, or “π"
	Toughness float32 `json:"toughness"`
	// i.e: "creature", "goblin", etc...
	Types []string `json:"types"`
	// i.e: "m21", "sld", etc...
	Sets             []string                  `json:"sets"`
	FormatLegalities map[string]FormatLegality `json:"format_legalities"`
}

// Used for internal testing
func (r Request) LookupCard(id string) (Card, error) {
	resp, err := http.Post(r.Url+"/card_id", "application/json", strings.NewReader(id))
	if resp.StatusCode != http.StatusOK {
		return Card{}, fmt.Errorf("Illegal return value from API: %s(%d)", resp.Status, resp.StatusCode)
	}

	bodyRaw, err := io.ReadAll(resp.Body)
	if err != nil {
		return Card{}, err
	}

	var card Card

	err = json.Unmarshal(bodyRaw, &card)
	if err != nil {
		return Card{}, err
	}

	return card, nil
}

type Response struct {
	CardsTotal int    `json:"cards_total"`
	Page       int    `json:"page"`
	PageSize   int    `json:"page_size"`
	Cards      []Card `json:"cards"`
	// A copy of the original request, used to preserve query context on the .Next() function
	request Request `json:"-"`
}

func (r Request) SearchCard() (Response, error) {
	resp, err := http.Post(r.Url+"/api", "application/json", strings.NewReader(r.Query))
	if err != nil {
		return Response{}, err
	}

	if resp.StatusCode != http.StatusOK {
		return Response{}, fmt.Errorf("Illegal return value from API: %s(%d)", resp.Status, resp.StatusCode)
	}

	bodyRaw, err := io.ReadAll(resp.Body)
	if err != nil {
		return Response{}, err
	}

	var response Response

	err = json.Unmarshal(bodyRaw, &response)
	if err != nil {
		return Response{}, err
	}

	response.request = r
	return response, nil
}

// Attempts to get all of the pages for a given query
func (r Request) SearchCardAllPages() ([]Card, error) {
	resp, err := r.SearchCard()
	if err != nil {
		return nil, err
	}

	var wg sync.WaitGroup
	var lock sync.Mutex
	errors := make([]error, 0)
	cards := make([]Card, 0)

	for {
		nextRequest, err := resp.Next()
		if err != nil {
			break
		}

		wg.Add(1)
		go func(r Request) {
			defer wg.Done()

			resp, err := r.SearchCard()

			lock.Lock()
			defer lock.Unlock()

			if err != nil {
				errors = append(errors, err)
				return
			}

			cards = append(cards, resp.Cards...)
		}(nextRequest)
	}

	wg.Wait()

	if len(errors) > 0 {
		return nil, fmt.Errorf("Cannot get cards: %+v", errors)
	}

	return cards, nil
}

func (r *Response) Next() (Request, error) {
	// Assert that the page size has not been modified
	if r.PageSize <= 0 {
		r.PageSize = 50
	}

	if (r.request.Page+1)*r.PageSize >= r.CardsTotal {
		return Request{}, fmt.Errorf("No more pages for the query '%s'", r.request.Query)
	}

	r.request.Page++
	return r.request, nil
}

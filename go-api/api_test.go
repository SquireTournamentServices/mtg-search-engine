package goapi_test

import (
	"net/url"
	"os"
	"sort"
	"testing"
	"time"

	api "github.com/squiretournamentservices/mtg-search-engine/go-api"
	"github.com/stretchr/testify/assert"
)

func TestSortTypesCanBeIterated(t *testing.T) {
	t.Parallel()

	for i := 0; i < int(api.SortTypeEnd); i++ {

	}
}

func getTestUrl() string {
	if url := os.Getenv("URL"); url != "" {
		return url
	}
	return api.ApiUrlLocal
}

func NewApi(query string) api.Request {
	resp := api.New(query)
	resp.Url = getTestUrl()
	resp.Client.Timeout = time.Second / 5
	return resp
}

func TestUrlValidity(t *testing.T) {
	t.Parallel()

	t.Logf("Url is %s", getTestUrl())

	_, err := url.Parse(getTestUrl())
	assert.NoError(t, err)
}

func TestLocalApiValidity(t *testing.T) {
	t.Parallel()

	_, err := url.Parse(api.ApiUrlLocal)
	assert.NoError(t, err)
}

func TestOfficialApiValidity(t *testing.T) {
	t.Parallel()

	_, err := url.Parse(api.ApiUrlOfficial)
	assert.NoError(t, err)
}

func TestQueryExec(t *testing.T) {
	t.Parallel()

	resp, err := NewApi("goblin motivator").Exec()

	assert.NoError(t, err)
	assert.Equal(t, 0, resp.Page)
	assert.True(t, resp.PageSize > 1)
	assert.True(t, resp.CardsTotal >= 1)
	assert.True(t, len(resp.Cards) >= 1)
}

func TestQueryExecAll(t *testing.T) {
	t.Parallel()

	resp, err := NewApi("legal:commander").ExecAll()

	assert.NoError(t, err)
	assert.True(t, len(resp) > 1000)
}

func TestQueryExecAllDandan(t *testing.T) {
	t.Parallel()

	resp, err := NewApi("dandan").ExecAll()

	assert.NoError(t, err)
	assert.True(t, len(resp) > 1000)
}

func TestInvalidQueryExecReturnsError(t *testing.T) {
	t.Parallel()

	_, err := NewApi("legal:(").Exec()
	assert.Error(t, err)
}

func TestInvalidQueryExecAllReturnsError(t *testing.T) {
	t.Parallel()

	_, err := NewApi("legal:(").ExecAll()
	assert.Error(t, err)
}

func TestSortByCardNameExec(t *testing.T) {
	t.Parallel()

	resp, err := NewApi("legal:commander and type:legendary and type:wizard and commander<u").Exec()
	assert.NoError(t, err)

	CardsCopy := make([]api.Card, len(resp.Cards))
	copy(CardsCopy, resp.Cards)

	sort.SliceStable(CardsCopy, func(i, j int) bool {
		return CardsCopy[i].Name < CardsCopy[j].Name
	})
}

func TestSortByCardNameExecAll(t *testing.T) {
	t.Parallel()

	cards, err := NewApi("legal:commander and type:legendary and type:wizard and commander<u").ExecAll()
	assert.NoError(t, err)

	CardsCopy := make([]api.Card, len(cards))
	copy(CardsCopy, cards)

	sort.SliceStable(CardsCopy, func(i, j int) bool {
		return CardsCopy[i].Name < CardsCopy[j].Name
	})
}

func TestSortByCardNameExecAllDesc(t *testing.T) {
	t.Parallel()

	client := NewApi("legal:commander and type:legendary and type:wizard and commander<u")
	client.SortDirection = api.SortDirectionDescending

	cards, err := client.ExecAll()
	assert.NoError(t, err)

	CardsCopy := make([]api.Card, len(cards))
	copy(CardsCopy, cards)

	sort.SliceStable(CardsCopy, func(i, j int) bool {
		return CardsCopy[i].Name < CardsCopy[j].Name
	})
}

func TestComplexLookup(t *testing.T) {
	t.Parallel()

	cards, err := NewApi("type:legendary and legal:commander and (type:creature or o:/.*may be your commander.*/) and cmc<5").ExecAll()

	assert.NoError(t, err)
	assert.True(t, len(cards) > 5)
}

export default function searchUrlFor(query: string, page: number): string {
  return "/q/?query=" + encodeURIComponent(query) + "&page=" + page.toString();
}

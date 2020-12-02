# mtg-search-engine
This repo will have the AST for the mtg search engine. This repo is for checking if a card matches a query. It will (when I get round to it) have implementations in an array of languages.

## AST for mtg card search lib
```xml
<search> = <expression><search>
<expression> = (<expression><binOp><expression>)
  |(<expression><expression>)
  |(<negation><expression>)
```  
*//`<expression><expression>` is treated as `<expression><operator><expression>` where `<binOp>` is and*
```xml
<binOp> = ((and|&&)|(or|\|\|)|(xor|^)|nand)
<negation> = (not|!)

<expressions> = <comptype><operator><parameter>

<comptype> = (p|t|c|set|collectors|commander|type|legal|banned|is)
<operator> = ((:|=)|<=|>=|<|>)
```
*//not each `<opetator>` is defined for each `<comptype>`*
```xml
<parameter> = (<word>|"<words>"|/<regex>/>
```
*//the definitions for the following are given as regex which means I will end up defining regex with regex but who doesn't like a good recursion*
```xml
<words> = "((\\")|[^"])+"
<regex> = /((\\/)|[^"])+/
<word> = [^:/"]([^ ])+
```

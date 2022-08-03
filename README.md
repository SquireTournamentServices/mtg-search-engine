# mtg-search-engine
This repo will have the AST for the mtg search engine. This repo is for checking if a card matches a query. It will (when I get round to it) have implementations in an array of languages.

## Building
This project is written in C and, targets all platforms, using GTK's glibc for various things.

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

<comptype> = (p|t|c|o|set|collectors|commander|type|legal|banned|is)
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

## comptype operation definitions
*Symbolic fields are defined as {x},{y} or anything that is non-real in the p/t field.*
*Formats are defined as a lower case string that, each implementation may support different formats. Typical formats are: vintage, legacy, commander, modern, pauper, pioneer, standard, penny dreadful and oathbreaker but several other formats exist.*
*Only the o comptype supports regex*
*All cards with symbolic power/toughness should be put after the non-symbolic power/toughness cards.*

|            | : or =                                                                                                                                                                                       | <=                                                                                                                      | >=                                                                                                                         | <                                                                                                           | >                                                                                                             |
|------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------------------------|
| p          | Power is equal to operand<br>Or if power is symbolic<br>Symbolic operands must be equal to the<br>power field not including whitespace,<br>case insensitive                                  | Power is less than or equal to operand<br>Or if the power is symbolic<br>Symbolic operands are not defined              | Power is greater than or equal to operand<br>Or if the power is symbolic<br>Symbolic operands are not defined              | Power is less than the operand<br>Or if the power is symbolic<br>Symbolic operands are not defined          | Power is greater than the operand<br>Or if the power is symbolic<br>Symbolic operands are not defined         |
| t          | Toughness is equal to operand<br>Or if toughness is symbolic                                                                                                                                 | Toughness is less than or equal to operand<br>Or if the power is symbolic<br>Symbolic operands are not defined          | Toughness is less than or equal to the operand<br>Or if the toughness is symbolic<br>Symbolic operands are not defined     | Toughness is less than the operand<br>Or if the toughness is symbolic<br>Symbolic operands are note defined | Toughness is greater than the operand<br>Or if the toughness is symbolic<br>Symbolic operands are not defined |
| c          | The cards colours are exclusively the colours in the<br>operand (each char is a seperate colour)                                                                                             | The cards colours are less than or equal to<br>the colours in the operand (each char is a<br>seperate colour)           | The cards colours are greater than or equal to<br>the colours in the operand (each char is a seperate colour)              | The cards colours are less than the<br>colours in the operand (each char is<br>a seperate colour)           | The cards colours are greater than<br>the colours in the operand (each <br>char is a seperate colour)         |
| set        | Set code or name equals the operand                                                                                                                                                          | Not defined                                                                                                             | Not defined                                                                                                                | Not defined                                                                                                 | Not defined                                                                                                   |
| collectors | The cards collectors number is equal to operand                                                                                                                                              | The cards collectors number is less<br>than or equal to the operand                                                     | The collectors number is greater than or equal to the <br>operand                                                          | The collectors number is less than <br>the operand                                                          | The collectors number is greater <br>than the operand                                                         |
| commander  | The card's colour identity has the operand colour/s in<br>it (each char is a seperate colour)                                                                                                | The card's colour identity has less<br>than or equal to the operand colour/s <br>in it (each char is a seperate colour) | The colour identity of the card is greater than or equal<br>to the colours in the operand (each char is a seperate colour) | The colour identity is less than<br>the colours in the operand (each<br>char is a seperate colour)          | The colour identity is greater than <br>the colours in the operand (each <br>char is a seperate colour)       |
| type       | The card contains the type which is the operand                                                                                                                                              | Not defined                                                                                                             | Not defined                                                                                                                | Not defined                                                                                                 | Not defined                                                                                                   |
| legal      | The card is legal in the format defined by the operand<br>case insensitive                                                                                                                   | Not defined                                                                                                             | Not defined                                                                                                                | Not defined                                                                                                 | Not defined                                                                                                   |
| banned     | The card is banned in the format defined by the operand<br>case insensitive                                                                                                                  | Not defined                                                                                                             | Not defined                                                                                                                | Not defined                                                                                                 | Not defined                                                                                                   |
| is         | The card has a property operand that is true                                                                                                                                                 | Not defined                                                                                                             | Not defined                                                                                                                | Not defined                                                                                                 | Not defined                                                                                                   |
| o          | Supports regex.<br>String operand:<br>The card has oracle text that contains the operand<br>Regex operand:<br>The card contains a susbtring that matches the regex<br>defined by the operand |                                                                                                                         | Not defined                                                                                                                | Not defined                                                                                                 | Not defined                                                                                                   |

## Usage
The AST describes how to create a query for searching cards. The implementations will take a given card and return true or false depending on whether the card matches the query.

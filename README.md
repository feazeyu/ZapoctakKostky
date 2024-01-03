# Kostkový parser
## Specifikace
Projekt byl stvořen jako zápočtový program Markem Chwistkem do předmětu "Programování 1" na Karlově univerzitě v Praze
úkol byl stvořit program, který na vstupu dostane matematický výraz, který používá znaménka +, -, *, (), a d, kde d je znaménko pro kostku
xdy by se dalo přečíst jako hoď x kostkama, které mají y stěn.

## Používání programu

Program je jednoduchá konzolová aplikace, které na vstupu můžete dát vzorec s výše zmíněnými operátory a čísly.
Na výstup vám vypíše pravděpodobnosti jednotlivých výsledků.

## Implementační detaily programu

### Shunting yard algorithm
https://en.wikipedia.org/wiki/Shunting_yard_algorithm

Algoritmus co přepíše vzorec do obrácené polské notace, ze které se dá pěkně vyhodnocovat ve správném pořadí

### Třída DieResult

Cílem programu nebylo najít jeden výsledek, ale všechny možné a jejich pravděpodobnosti.
Proto se na všechna čísla dívá jako na soubor pravěpodobností a s tím i vevnitř počítá.
Například číslo 5 by bylo DieResult, která má pravděpodobnost 100% na výsledek 5, samozřejmě 1d6 pak má 1/6 na každé číslo od 1 do 6

### Interní funkce pro manipulaci DieResultů

Aritmetika zde je jiná než s obyčejnými čísly, proto program zahrnuje funkce pro sčítání, odečítání, násobení a "kombinování" různých DieResults
Rozdíl mezi kombinováním a sčítáním je následující:
Když sečtu dva hody 1d6, dostanu pravděpodobnosti od padnutí 2 do 12, kde 7 má nejvyšší pravděpodobnost
Když je zkombinuju, dostanu pravděpodobnost 2/6 na každé číslo od 1 do 6

### Počítání DieResultů

Matematika pro výpočet pravděpodobností toho, co mi spadne když hodím dvěma různostěnnýma kostkama neexistuje, nebo je mi neznámá, takže pravděpodobnosti
různých výsledků hodů počítám tak, že si najdu všechny možnosti co může padnout a zapamatuju si kolikrát jsem kterou viděl.
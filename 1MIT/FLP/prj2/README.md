# Dokumentace k logickému projektu Prolog

Autor: Hung Do
Login: xdohun00
Akad. rok: 2023/24
Zadání: **Rubikova kostka**

## Použitá metoda
Nástroj na řešení Rubikovy kostky používá algoritmus Iterative Deepening Search
(iterativní prohledávání do hlouby). Kostka je uložena jako list 6 stran v pořadí *Up*,
*Front*, *Right*, *Back*, *Left* a *Down*. 

Funkce `solve_cube` se snaží v každé iteraci vyhledat složenou kostku a svojí "cestu"
si ukládá do dynamického predikátu `pos(+Pos, +Move)`, který přijímá stav kostky a tah,
který byl vybrán. Pokud vybraná větěv nevede k cíli, je stav odstraněn z cesty a
funkce zkouší jiný tah. Po menším testování jsem došel k výsledku, že tvorba a vyhledávání
v seznamu již navštívených stavů je mnohem pomalejší než tupému vyhledávání. Jediné, co
funkce zaruřuje, je přeskakování stavů, které již byly navštíveny v rámci momentální cesty
(zaruři zacyklení v rámci cesty).

## Překlad a rozjetí nástroje
Pro překlad skriptu je možné použít přiložený `Makefile` pomocí příkazu `make`. To vytvoří
binární soubor `./flp23-log`. Nástroj očekává na vstupu stav kostky, kterou má vyřešit.
Kostka musí splňovat následující formát (různá písmena reprezentují různou barvu políček):
```
www
www
www
ggg rrr bbb ooo
ggg rrr bbb ooo
ggg rrr bbb ooo
yyy
yyy
yyy
```

Program se pak může např. rozjet tímto způsobem:
```sh
./flp23-log < tests/scramble_colored.in
```
s výstupem:
```
bog
wwg
rrw
ggo gyy rbr yow
ggo grr wbr yow
oor www oby obb
yyb
yyb
grb

boo
www
rrr
ggg yrw bbr yow
ggg yrw bbr yow
oow ggw bby obb
yyo
yyo
grr

www
www
rrr
ggg yrr bbb oow
ggg yrr bbb oow
oow ggg yrr bbb
yyo
yyo
yyo

www
www
rrr
ggg yrr bbb oow
ggg yrr bbb oow
ggg yrr bbb oow
ooo
yyy
yyy

www
www
www
ggg rrr bbb ooo
ggg rrr bbb ooo
ggg rrr bbb ooo
yyy
yyy
yyy
```

## Přiložené soubory
Ve složce `tests` se nachází celkem 6 vstupních a 6 výstupním souborů. Soubory jsou ve
vztahu `<filename>.in` pro vstupní a `<filename>.out` pro výstupní. Navíc každý test má
2 druhy formátu. `<filename>_colored.*` soubory obsahují kostky, kde každé políčko je 
reprezentovan barvou (*r* je červená, *b* je modrá atd.). `<filename>_numbered` soubory 
obsahují stejné kostky, ale políčka jsou reprezentována čísly (pro dodržení zadání).
`scramble_colored.in` kostka je stejná jako `scramble_numbered.in` kostka.

Seznam souborů:
- `scramble_colored.in` a `scramble_colored.out`
- `scramble5_colored.in` a `scramble5_colored.out`
- `scramble6_colored.in` a `scramble6_colored.out`
- `scramble_numbered.in` a `scramble_numbered.out`
- `scramble5_numbered.in` a `scramble5_numbered.out`
- `scramble6_numbered.in` a `scramble6_numbered.out`

## Rychlost jednotlivých testů
Testováno na školním serveru Merlin.
```
scramble_colored:
real    0m0.391s
user    0m0.384s
sys     0m0.005s

scramble5_colored:
real    0m3.983s
user    0m3.843s
sys     0m0.136s

scramble6_colored:
real    2m13.207s
user    2m10.510s
sys     0m2.690s
```

## Rozšíření
Tento nástroj neobsahuje žádné rozšíření nadrámec zadání.

## Omezení
V zadání je napsáno:

*Kostka složená v posledním kroku  musí být orientována přesně tak,
jak je uvedeno v zadání (tedy horní hrana obsahuje barvu 5 apod.).*

To bohužel tento nástroj neumožňuje, jelikož se snaží najít nejoptimálnější řešení (IDS
algoritmus od hloubky 2, neboli pomalý BFS s menšími nároky na paměť). Pokud bych se snažil
přesunout středy kostek tak, aby jejich středy odpovídaly zadání, víc bych tím "zamíchal"
kostku a kostka by se skládala na více tahů (a stále nemůžu zaručit, že na konci bude
kostka splňovat požadovaný formát). Pokud uživatel chce, aby barva 5 byla nahoře, může si
výslednou kostku celou otočit.

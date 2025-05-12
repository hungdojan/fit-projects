# Hra Life v MPI

Pomocí knihovny MPI implementujte v C++ hru Life. 

### Hra Life  

Hra Life reprezentuje příklad tzv. celulárního automatu. Hrací pole se skládá
z buněk, které se v každém kroku přepínají mezi dvěma stavy: 

- živá (značíme 1)
- mrtvá (značíme 0)

Stavy buněk se v průběhu hry mění pomocí definované sady pravidel. Základní
sada pravidel, kterou budete implementovat v projektu je následující:

- každá živá buňka s méně než dvěma živými sousedy umírá 
- každá živá buňka se dvěma nebo třemi živými sousedy zůstává žít 
- každá živá buňka s více než třemi živými sousedy umírá 
- každá mrtvá buňka s právě třemi živými sousedy ožívá

Např. tato mřížka: 

```
00000000  
00111000  
01110000  
00000000
```

bude mít po třech krocích hry s pravidly viz výše tvar:

```
01000000   
00001000   
01000000   
00001000
```

pro implementaci typu wrap-around a 

```
00010000   
01001000   
01001000   
00100000
```

pro implementaci s pevnými stěnami.

### Implementace  

Program bude implementován v jazyce C++ s použitím MPI. Jako první argument
bude program akceptovat **název souboru** s definicí hracího pole pro hru Life,
jako druhý **počet kroků**, který má hra Life provést. 

Při implementaci zvolte okolí buňky jako tzv. osmi okolí (Moorovo okolí), dle
následujícího obrázku.

Buňky na za okrajem hracího pole ignorujte.

## Výstup

Program na standardní výstup (_stdout_) vypíše stav hracího pole pro hru Life
po provedení zadaného počtu kroků. Ve výpisu budou označeny části mřížky
vypočítané procesory jako:   
ID: <část mřížky>, kde:

- ID: rank procesoru
- <část mřížky>: část mřížky vyřešená procesorem s rankem ID

Tedy např.:

```
0: 00010000  
0: 01001000  
1: 01001000  
1: 00100000
```

### Odevzdání  

Projekt odevzdejte nejpozději **26****.4.2024, 23:59:59** do StudISu. Odevzdává se pouze **dobře okomentovaný** zdrojový soubor **life****.cpp** a testovací skript **test.sh,** který po spuštění přeloží váš program a spustí ho pro vámi definovaný počet procesů s parametry výše. Pokud testovací skript neodevzdáte, **uveďte prosím počet procesů do hlavičky vašeho řešení.** Testovací skript bude akceptovat dva argumenty:

1. jméno souboru s definicí hracího pole pro hru Life.
2. počet iterací, které má algoritmus provést

## Hodnocení projektu

V rámci projektu budu hodnotit zejména:

- zvolený přístup k paralelizaci hry Life
- dodržení zadání (tj. aby program dělal co má dle zadání, a vypisoval co má na stdout)  
- kvalitu zdrojového kódu (komentáře, pojmenování proměnných a konstant, ... )

Při hodnocení budou testovány čtvercové mřížky se sudým počtem řádků a sloupců s minimální velikostí mřížky 4x4.

# Implementace IMS projektu 2022/2023
Autoři: 
- Marek Dohnal (xdohna48)
- Hung Do (xdohun00)

Rok: 2022/2023 

## Překlad a rozjetí programu
Pro přeložení a rozjetí programu jsou zapotřebí tyto věci:
- `gcc`
- `make`
- `ncurses` vývojářské knihovny

Pro editaci počátečního stavu je možné použít přiložený python script využívající
`pygame` knihovny.

### Potřebné knihovny
Pro přeložení projektu je zapotřebí vývojářské knihovny `ncurses`. Instalace je různá
podle operačního systému.

```bash
# Ubuntu
sudo apt-get install libncurses5-dev libncursesw5-dev

# Fedora
sudo dnf install ncurses-devel

# Arch Linux
sudo pacman -S ncurses
```

Pro program `pixel_editor.py` je zapotřebí nainstalovat `pygame` modul.
```bash
pip install pygame
```

### Rozjetí programu
K rozjetí programu je možno použít přiložený `Makefile`. Po přeložení všech tří
programu si uživatel může manuálně vybrat, jakou simulaci zapnout.

```bash
# preklad programu
make

# příklady rozjeti programu
./ims_project1
./ims_project2 -n4
./ims_project3 -n6 -i200

# spusteni pixel_editoru
make pixel_edit
```

**Doporučení**: Zmenšete si font v terminále, abyste viděli celou simulaci.

## Ovládání hlavního programu

```txt
|-----------+-----------------------------|
|  klavesa  |         vyznam              |
|-----------+-----------------------------|
|     +     | zvyseni rychlosti iterace   |
|     -     | snizeni rychlosti iterace   |
|     q     | ukonceni programu           |
|  KEY_UP   | posun obrazovky nahoru      |
| KEY_DOWN  | posun obrazovky dolu        |
| KEY_RIGHT | posun obrazovky doprava     |
| KEY_LEFT  | posun obrazovky doleva      |
|   SPACE   | zastaveni/spusteni iterace  |
|-----------+-----------------------------|

Argumenty:
    -i     Maximalni pocet iteraci
    -n     Nutricni hodnota (pouziti v programu "imp_project2")
```


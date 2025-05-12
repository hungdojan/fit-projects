# Implementace MAC a Length Extension Attack za použití hašovací funkce SHA256

Cílem projektu bylo vytvoření generátoru *MAC* (Message Authentication Code) za použití
vlastní implementace hašovací funkce *SHA256*. Program by dále měl být schopen provést
*Length Extension Attack*, neboli schopnost rozšíření původní zprávy a přepočet MAC bez
znalosti původního klíče.

## Rozhraní projektu
- `args.hpp` a `args.cpp` - načítá a spravuje programové argumenty
- `sha256.hpp` a `sha256.cpp` - definuje rozhraní pro práci s hašovací funkcí SHA256
- `main.cpp` - obsahuje hlavní funkci `main`, ze kterého se program spouští

## Sestavování a spuštění programu
K sestavení programu je možno použít přiložený `Makefile` soubor.

```bash
# build a project
make

# example of running the program
echo -ne "this is a message to hash" |./kry -c 
# e59848009fb8cb51c8a799bb952f1740e93671ea6dd10474da63538d08fc9ab1
```

## Použití
```
Usage:
./kry [-c] [-s -k KEY] [-v -k KEY -m MAC_MESSAGE]
      [-e -n NUM -a TEXT -m HASH_VALUE]

Modes:
  -c  Generate a checksum of the input on STDIN.
  -s  Generate a MAC of the input on STDIN. Requires `-k` flag to be set.
  -v  Validate MAC of STDIN with the given MAC. Requires `-k` and `-m` flags to be set.
  -e  Perform a length extension attack. Requires `-a`, `-m`, and `-n` flags to be set.

Parameter flags:
  -k      A key to encrypt a message and generate a MAC hash.
  -m      MAC hash value.
  -n      The size of the key used during length extension attack.
  -a      Text to append during length extension attack.
```

## Jak program funguje
Při spouštění programu si uživatel vybere, který mód programu chce zapnout. Má na výběr
mezi 4 možnostmi:
- `-c` - výpočet checksum vstupního textu (čteno ze STDIN)
- `-s` - generování MAC vstupního textu; potřeba definovat hodnotu klíče přes `-k`
- `-v` - validace MAC vstupního textu s dodaným kódem; potřeba definovat `-k` a `-m`
- `-e` - provedení *length extension attack*; potřeba definovat `-n`, `-a` a `-m`

Hlavní funkce `main` následně pracuje s `SHA256` rozhraním, které je definováno
v `sha256.cpp` a `sha256.hpp` souborech. Třída `SHA256` přijímá jako parametr vstupní
text. Hodnota hash se pak vypočítá skrze funkci `SHA256::sha256`. Třída dále definuje tři
pomocné a tři nebezpečné (modifikují interní stav třídy a nejsou použité hlavní funkcí
`SHA256::sha256`) funkce:
- `pop_mblock`   - extrakce bloku zprávy zarovnané na 512 bitů
- `compute_hash` - provedení jednoho cyklu výpočtu hash (změna interních registrů)
- `digest`       - vytvoření hašovacího řetězce z interních registrů
- `set_hash_unsafe`      - změna hodnost interního registru
- `sha256_extend_unsafe` - výpočet nového hash po připojení nového textu
- `print_mblocks_unsafe` - výpis nové zprávy po připojení vybraného textu



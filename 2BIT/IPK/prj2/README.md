# Implementace 1. projektu z IPK 2021/2022
Autor: Hung Do (xdohun00)

## O programu
Program `ipk-sniffer` složí pro sledování provoz tzv. paketů v systému. Programu se předají parametry, se kterými se následně
program spustí. `ipk-sniffer` dokáže zobrazit obsahy jednotlivých paketů a rozšifrování jednotlivých částí. Tento projekt
byl testován na operačních systémech Manjaro a Ubuntu.

## Prerekvizity
Pro rozjetí programu `ipk-sniffer` je potřeba knihovna `libpcap`.

## Kompilace

### Pomocí Makefile
Program se dá přeložit (a spustil) pomocí přiloženého `Makefile`. Stačí zadat následující příkazy:
```shell
# pro preklad
make

# pro spusteni 
make run
```
Při spouštění pomocí `Makfile` je potřeba nastavit 2 věci:
- Je potřeba spustit program v superuser módu.
```shell
# jednoduche spusteni
sudo make run

# pro vetsi pohodli pri opetovne spousteni
sudo su  # pro prechod do superuser
make run
```
- Zadané parametry programu se musí explicitně napsat do souboru `Makefile` (a pro jeho opětovné úpravy).
```txt
...  pridat do Makefile
run: $(TARGET)
    ./$< [seznam_parametru]
...
```
### Manuální kompilace
Program se může taky manuálně přeložit na příkazovém řádku. Překlad programu může vypadat následovně:
```shell
gcc *.c -o ipk-sniffer -lpcap -lm
```
Pro překlad není potřeba volit překladač `gcc`, ale jakýkoliv jiný C překladač (např. clang).
## Rozjetí programu
Program je potřeba spouštět v superuser módu. Program přijímá různé parametry: 
```txt
    -i eth0 (právě jedno rozhraní, na kterém se bude poslouchat. Nebude-li tento parametr uveden, či bude-li uvedené jen -i bez hodnoty, vypíše se seznam aktivních rozhraní)
    -p 23 (bude filtrování paketů na daném rozhraní podle portu; nebude-li tento parametr uveden, uvažují se všechny porty; pokud je parametr uveden, může se daný port vyskytnout jak v source, tak v destination části)
    -t nebo --tcp (bude zobrazovat pouze TCP pakety)
    -u nebo --udp (bude zobrazovat pouze UDP pakety)
    --icmp (bude zobrazovat pouze ICMPv4 a ICMPv6 pakety)
    --arp (bude zobrazovat pouze ARP rámce)
    Pokud nebudou konkrétní protokoly specifikovány, uvažují se k tisknutí všechny (tj. veškerý obsah, nehledě na protokol)
    -n 10 (určuje počet paketů, které se mají zobrazit, tj. i "dobu" běhu programu; pokud není uvedeno, uvažujte zobrazení pouze jednoho paketu, tedy jakoby -n 1)
    argumenty mohou být v libovolném pořadí
```
Zdroj textu viz:  `zadani.md`
### Příklady použití
```shell
$ sudo ./ipk-sniffer 
enp2s0
ap0
any
lo
wlp3s0
virbr0
virbr223
bluetooth0
bluetooth-monitor
nflog
nfqueue
dbus-system
dbus-session

$ sudo ./ipk-sniffer -i any
--------------- Frame info ---------------
Arrival time:         2022-04-05 21:07:23.382137 
Frame length:         94 bytes
--------------- Ethernet II --------------
Src MAC:              88:51:fb:fe:9e:0a
Dst MAC:              00:01:00:01:00:06
Type:                 0x0806
------ Address Resolution Protocol -------
Hardware type:        2048
Protocol type:        0x4500
Hardware size:        8
Protocol size:        78
Opcode:               18870
Sender MAC address:   00:00:80:11:24:67
Sender IP address:    147.229.208.183
Target MAC address:   93:e5:d3:ff:00:89
Target IP address:    0.137.0.58
-------------- Data Dump -----------------
0x0000   00 01 00 01 00 06 88 51  fb fe 9e 0a 08 06 08 00   .......Q ........
0x0010   45 00 00 4e 49 b6 00 00  80 11 24 67 93 e5 d0 b7   E..NI... ..$g....
0x0020   93 e5 d3 ff 00 89 00 89  00 3a e8 31 de 39 01 10   ........ .:.1.9..
0x0030   00 01 00 00 00 00 00 00  20 45 43 46 43 46 48 44   ........  ECFCFHD
0x0040   4a 45 44 44 44 44 41 44  46 45 43 44 45 44 47 44   JEDDDDAD FECDEDGD
0x0050   41 44 44 44 44 44 46 41  41 00 00 20 00 01         ADDDDDFA A.. ..
```

## Použité zdroje
- [How to code a Packet Sniffer in C with Libpcap on Linux](https://www.binarytides.com/packet-sniffer-code-c-libpcap-linux-sockets/)
- [Man page of PCAP](https://www.tcpdump.org/manpages/pcap.3pcap.html)
- [Man page of INET](https://man7.org/linux/man-pages/man3/inet.3.html)
- [Man page of getopt_long](https://linux.die.net/man/3/getopt_long)

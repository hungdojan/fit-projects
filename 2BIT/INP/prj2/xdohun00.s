; Vernamova sifra na architekture DLX
; Hung Do xdohun00
; xdohun00-r11-r18-r21-r23-r28-r0

        .data 0x04          ; zacatek data segmentu v pameti
login:  .asciiz "xdohun00"  ; <-- nahradte vasim loginem
cipher: .space 9 ; sem ukladejte sifrovane znaky (za posledni nezapomente dat 0)

        .align 2            ; dale zarovnavej na ctverice (2^2) bajtu
laddr:  .word login         ; 4B adresa vstupniho textu (pro vypis)
caddr:  .word cipher        ; 4B adresa sifrovaneho retezce (pro vypis)

        .text 0x40          ; adresa zacatku programu v pameti
        .global main        ; 

main:
        add  r21, r0, r0        ; iteracni registr
loop:
        lb r18, login(r21)

if_lt_a:            ; if (r18 < 'a')
        slti r11, r18, 97       ; nacteny znak < (int)'a'
        bnez r11, end_loop
        nop
        nop
if_gt_z:            ; if (r18 > 'z')
        sgti r11, r18, 122      ; nacteny znak > (int)'z'
        bnez r11, end_loop
        nop
        nop

        ; zjistovani modula
        andi r11, r21, 1
        bnez r11, if_odd
        nop
        nop
if_even:
        addi r28, r0, 4        ; d = 4
        ; addi r11, r0, 1
        ; lb r28, login(r11)  ; ulozeni prvniho klice
        ; subi r28, r28, 96   ; odecteni ('a'+1), abychom ziskali offset
        add r23, r18, r28   ; vystupni znak = nacteny znak + klic

        sgti r11, r23, 122      ; je vetsi nez 'z'
        beqz r11, if_ngt_range  ; pokud ne, tak se preskakuje
        nop
        nop

        subi r23, r23, 26        ; odecte ('z'-'a'+1)
    if_ngt_range:
        j end_module
if_odd:
        addi r28, r0, 15      ; o = 15
        ; addi r11, r0, 2
        ; lb r28, login(r11)  ; ulozeni druheho klice
        ; subi r28, r28, 96   ; prevod na hodnotu <1;26>, ziskani offsetu
        sub r23, r18, r28   ; vystupni znak = nacteny znak - klic

        slti r11, r23, 97       ; je vetsi nez 'a'
        beqz r11, end_module    ; pokud ne, tak se preskoci
        nop
        nop

        addi r23, r23, 26
end_module:
    
        sb cipher(r21), r23     ; nahraje do vystupu znak ve r23
        addi r21, r21, 1        ; zvyseni iterace
        j loop
end_loop:
        sb cipher(r21), r0      ; znak ukonceni retezce

end:    addi r14, r0, caddr ; <-- pro vypis sifry nahradte laddr adresou caddr
        trap 5  ; vypis textoveho retezce (jeho adresa se ocekava v r14)
        trap 0  ; ukonceni simulace

#!/usr/bin/env python3
"""
brief: Hlavni skript resici projekt z ISS, VUT FIT 2021/2022

file:   main.py
date:   07.01.2022
author: Hung Do
"""

"""
Hlavni cast zdrojoveho kodu byla presunuta do python noteboooku
Tento skript nemusi byt up-to-date
"""

import os
import numpy as np
from scipy.io import wavfile
from scipy.linalg import dft
from scipy.signal import spectrogram, lfilter, freqz, tf2zpk, find_peaks,\
                         buttord, butter
import matplotlib.pyplot as plt

lof_frames = []
spect = []
cos_freq = []
cos_indices = []

# vyber typu filteru
filters = ['z', 'spec', 'bandstop']
iof_filt = 0


def get_audio_path(filename: str) -> str:
    """
    Vygeneruje cestu ke zdroji zvuku

    Parameters:

    filename: jmeno souboru
    """
    src_dir = os.path.abspath(os.path.join(os.path.join(__file__, os.pardir)))
    parent_dir = os.path.abspath(os.path.join(src_dir, os.pardir))
    return os.path.abspath(os.path.join(parent_dir, 'audio/', filename))


def task1():
    """Prvni ukol"""

    # delka signalu
    time_len = data.size / fs

    # vypis informaci
    print(f"Pocet vzorku: {data.size}")
    print(f"Delka signalu: {time_len}")
    print(f"Max: {data.max()}")
    print(f"Min: {data.min()}")

    # vykresleni grafu
    time = np.arange(data.size) / fs
    plt.figure(figsize=(12, 6))
    plt.plot(time, data)

    # pojmenovani os
    plt.gca().set_xlabel('$t[s]$')
    plt.gca().set_title('Zvukovy signal')
    plt.tight_layout()


def task2():
    """Druhy ukol"""
    global data

    # vypocet stredni hodnoty
    mean = sum(data) / data.size

    # odecteni stredni hodnoty
    data = data - mean

    # normalizace signalu
    data = data / 2**15

    # vytvoreni ramcu o velikosti 1024 vzorku s presahem 512 vzorku
    i = 0
    while True:
        # zarovnani posledniho ramce a ukonceni cyklu
        if i + 1024 > data.size:
            lof_frames.append(data[i:data.size - 1] + np.zeros(data.size-i-1))
            break
        lof_frames.append(data[i:i + 1024])
        i += 512

    # vytvoreni noveho grafu
    time = np.arange(1024) / fs
    fig, ax = plt.subplots(figsize=(12, 6))
    ax.plot(time, lof_frames[0])

    # pojmenovani os
    ax.set_xlabel('$t[s]$')
    ax.set_title('Zvukovy signal')
    plt.tight_layout()


def task3():
    """Treti ukol"""
    global spect
    index_start = 20
    frame = np.array(lof_frames[index_start])
    N = len(frame)
    # vypocet dft pomoci iterace
    # for k in range(N):
    #     base = np.exp([-2j * np.pi / N * k * n
    #                    for n in range(N)])
    #     spect.append(abs(np.matmul(frame, base)))

    # vypocet dft pomoci matice
    m = dft(N)
    spect = abs(m @ frame)

    # kontrola s fft
    # ft = np.fft.fft(frame)
    # print(np.allclose(abs(ft), spect))

    # vytvoreni grafu
    fig, ax = plt.subplots()
    ax.plot(np.arange(N/2) * fs / N, spect[0:len(spect)//2])

    # pojmenovani os
    ax.set_xlabel('$f[Hz]$')
    ax.set_title('Segment signalu')
    ax.grid(alpha=0.5, linestyle='--')
    plt.tight_layout()

    # priprava pro task6
    spect = abs(m @ np.array(lof_frames[0]))


def task4():
    """Ctvrty ukol"""
    f, t, sgr = spectrogram(data, fs, nperseg=1024, noverlap=512)
    sgr_log = 10 * np.log10(sgr+1e-20)
    plt.figure(figsize=(12, 6))
    plt.pcolormesh(t, f, sgr_log)
    plt.gca().set_title('Spektrogram originálního signálu')
    plt.gca().set_xlabel('Čas [s]')
    plt.gca().set_ylabel('Frekvence [Hz]')
    cbar = plt.colorbar()
    cbar.set_label('Spektralní hustota výkonu [dB]', rotation=270, labelpad=15)

    plt.tight_layout()


def task5():
    """Paty ukol"""
    global cos_freq, cos_indices
    # indexy rusivych frekvenci v signalu
    peaks, _ = find_peaks(spect[0:len(spect)//2], distance=10)
    cos_indices = list(filter(lambda x: spect[x] > 1, peaks))

    # ulozeni nalezenych frekvenci
    # cos_freq = [(i+1) * 671.875 for i in range(4)]
    cos_freq = [i * fs / 1024 for i in cos_indices]


def task6():
    """Sesty ukol"""

    # hlasitost
    volume = 1

    # vytvoreni pole hodnot x-sove osy
    nof_samples = np.arange(data.size) / fs

    # vytvoreni souctu 4 kosin
    output_cos = sum([volume * np.cos(2 * np.pi * f * nof_samples)
                      for f in cos_freq])

    # normovani a prevod na 16bit
    output_cos = output_cos / max(abs(output_cos))
    output_cos = np.iinfo(np.int16).max * output_cos

    # TODO: vygenerovat spektrogram
    f, t, sgr = spectrogram(output_cos, fs, nperseg=1024, noverlap=512)
    sgr_log = 10 * np.log10(sgr+1e-20)
    plt.figure(figsize=(12, 6))
    plt.pcolormesh(t, f, sgr_log)
    plt.gca().set_title('Spektrogram rušivého signálu')
    plt.gca().set_xlabel('Čas [s]')
    plt.gca().set_ylabel('Frekvence [Hz]')
    cbar = plt.colorbar()
    cbar.set_label('Spektralní hustota výkonu [dB]', rotation=270, labelpad=15)

    plt.tight_layout()

    # generovani souboru
    wavfile.write(get_audio_path('4cos.wav'), fs, output_cos.astype(np.int16))


def task7(filter_type: str):
    """Sedmy ukol"""
    b_a = None
    # filter v z-rovine
    if filter_type == 'z':
        # vypocet jednotlivych omega_k ze vzorce 2 * pi * f_k / Fs
        omega_k = list(map(lambda f: 2 * np.pi * f / fs, cos_freq))

        # vypocet n_k ze vzorce n_k = e^(j*omega_k)
        n_k = [np.exp(1j * omega) for omega in omega_k]

        # pridani komplexne sdruzenych nulovych bodu a hledani
        # koeficientu filtru
        n_k = n_k + [np.conj(n) for n in n_k]
        b_a = np.poly(n_k), [1, *np.zeros(len(n_k))]

    # filter ze spektra
    elif filter_type == 'spec':
        # vypln frekvencni charakteristiky jednickami krome 
        # indexu s rusivymi frekvencemi
        H = list(np.ones(513))
        for i in cos_indices:
            H[i-2] = 0
            H[i-1] = 0
            H[i] = 0
            H[i+1] = 0
            H[i+2] = 0

        # pripojeni druhe poloviny frekvencni charakteristiky
        H = H + list(np.flip(H[1:512], 0))

        # provedena inverzni FFT a posun prvniho indedu doprostred
        # navrat jen realne casti
        # print(np.fft.ifft(H))
        ret_val = np.real(np.fft.fftshift(np.fft.ifft(H)))
        b_a = ret_val, [1, *np.zeros(len(ret_val)-1)]

    elif filter_type == 'bandstop':
        b_a = []

        # Nyquistova frekvence
        nyq = fs / 2

        # generovani pasmovych zadrzi
        for f in cos_freq:
            # propustne pasmo
            wp = np.array([f - 30, f + 30])
            # zaverne pasmo
            ws = np.array([wp[0] - 50, wp[1] + 50])
            # vypocet koeficientu
            N, Wn = buttord(wp / nyq, ws / nyq, 3, 40)
            b, a = butter(N, Wn, 'bandstop')
            print(b, a)
            # impulsni odezva filtru
            nof_imp = 32
            imp = [1, *np.zeros(nof_imp-1)]
            h = lfilter(b, a, imp)
            
            # vykresleni grafu
            plt.figure(figsize=(10, 5))
            plt.stem(np.arange(nof_imp), h)
            plt.gca().set_title('Impulsní odezva filtru')
            b_a.append((b, a))

    else:
        print('Unknown type!')
        exit(1)

    # TODO: Uvest koeficienty filtru a zobrazeni impulsni odezvy

    return b_a


def task8(filter_type: str, b_a: any):
    """Osmy ukol"""
    if filter_type != 'bandstop':
        b, a = b_a
        z, p, _ = tf2zpk(b, a)

        plt.figure(figsize=(10, 10))
        # jednotkova kruznice
        ang = np.linspace(0, 2*np.pi, 100)
        plt.plot(np.cos(ang), np.sin(ang))

        # nuly, poly
        plt.scatter(np.real(z), np.imag(z), marker='o', facecolors='none',
                    edgecolors='r', label='nuly')
        plt.scatter(np.real(p), np.imag(p), marker='x', color='g',
                    label='póly')

        plt.gca().set_xlabel('Realná složka $\mathbb{R}\{$z$\}$')
        plt.gca().set_ylabel('Imaginarní složka $\mathbb{I}\{$z$\}$')
        plt.gca().set_aspect('equal')

        plt.grid(alpha=0.5, linestyle='--')
        plt.legend(loc='upper left')

        plt.tight_layout()
    elif isinstance(b_a, list):
        lof_z = []
        lof_p = []
        for b, a in b_a:
            z, p, _ = tf2zpk(b, a)
            lof_z.append(z)
            lof_p.append(p)

        # jednotkova kruznice
        plt.figure(figsize=(10, 10))
        ang = np.linspace(0, 2*np.pi, 100)
        plt.plot(np.cos(ang), np.sin(ang))

        # nuly, poly
        plt.scatter(np.real(lof_z), np.imag(lof_z), marker='o',
                    facecolors='none', edgecolors='r', label='nuly')
        plt.scatter(np.real(lof_p), np.imag(lof_p), marker='x',
                    color='g', label='póly')

        plt.gca().set_xlabel('Realná složka $\mathbb{R}\{$z$\}$')
        plt.gca().set_ylabel('Imaginarní složka $\mathbb{I}\{$z$\}$')
        plt.gca().set_aspect('equal')

        plt.grid(alpha=0.5, linestyle='--')
        plt.legend(loc='upper left')

        plt.tight_layout()
    else:
        print('Nastala chyba v osmem ukolu')
        exit(1)


def task9(filter_type: str, b_a: any):
    """Devaty ukol"""
    def get_iof_axis(i: int):
        return i // 2, (i % 2) * 2

    if filter_type != 'bandstop':
        b, a = b_a
        # frekvencni charakteristika
        w, H = freqz(b, a)
        fig, ax = plt.subplots(1, 2, figsize=(12, 4))
        ax[0].plot(w / 2 / np.pi * fs, np.abs(H))
        ax[0].set_xlabel('Frekvence [Hz]')
        ax[0].set_title('Modul frekvenční charakteristiky $|H(e^{j\omega})|$')

        ax[1].plot(w / 2 / np.pi * fs, np.angle(H))
        ax[1].set_xlabel('Frekvence [Hz]')
        ax[1].set_title('Argument frekvenční charakteristiky' \
                        '$\mathrm{arg}\ H(e^{j\omega})$')

        for ax1 in ax:
            ax1.grid(alpha=0.5, linestyle='--')

        plt.tight_layout()
    elif isinstance(b_a, list):
        _, ax = plt.subplots(2, 4, figsize=(8, 3))
        big_h = 1

        for i in range(len(b_a)):
            b, a = b_a[i]

            # frekvencni charakteristika
            w, H = freqz(b, a)
            big_h = H * big_h

            # vykresleni jednotlivych pasmovych zadrzi
            y, x = get_iof_axis(i)
            ax[y, x].plot(w / 2 / np.pi * fs, np.abs(H))
            ax[y, x].set_xlabel('Frekvence [Hz]')
            ax[y, x].set_title('Modul frekvenční charakteristiky' \
                               '$|H_%d(e^{j\omega})|$' % (i+1))
            ax[y, x].grid(alpha=0.5, linestyle='--')

            ax[y, x+1].plot(w / 2 / np.pi * fs, np.angle(H))
            ax[y, x+1].set_xlabel('Frekvence [Hz]')
            ax[y, x+1].set_title('Argument frekvenční charakteristiky' \
                                 '$\mathrm{arg}\ H_%d(e^{j\omega})$' % (i+1))
            ax[y, x+1].grid(alpha=0.5, linestyle='--')
            plt.tight_layout()

    else:
        print('Nastala chyba v devatem ukolu')
        exit(1)


def task10(filter_type: str, b_a: any):
    """Desaty ukol"""
    # filtrace puvodniho signalu
    if filter_type != 'bandstop':
        b, a = b_a
        filtered = lfilter(b, a, data)
        pass
    elif type(b_a) == list:
        filtered = data
        for b, a in b_a:
            filtered = lfilter(b, a, filtered)
    else:
        print('Nastala chyba v desatem ukolu')
        exit(1)
    # vygenerovany spektrogram
    f, t, sfgr = spectrogram(filtered, fs, nperseg=1024, noverlap=512)
    sfgr_log = 10 * np.log10(sfgr+1e-20)

    plt.figure(figsize=(15, 5))
    plt.pcolormesh(t, f, sfgr_log)
    plt.gca().set_title('Spektrogram vyfiltrovaného signálu')
    plt.gca().set_xlabel('Čas [s]')
    plt.gca().set_ylabel('Frekvence [Hz]')
    cbar = plt.colorbar()
    cbar.set_label('Spektralní hustota výkonu [dB]', rotation=270, labelpad=15)

    # normalizace a prevod na 16bit
    filtered = filtered / max(abs(filtered))
    filtered = np.iinfo(np.int16).max * filtered

    plt.tight_layout()
    # generovani zvuku
    wavfile.write(get_audio_path(f'clean_{filter_type}.wav'), fs,
                  filtered.astype(np.int16))


# absoluti cesta ke zdroji zvuku
# spojeni _parent_dir_ a audio/xlogin00.wav
PATH_TO_AUDIO = get_audio_path('xdohun00.wav')
fs, data = wavfile.read(PATH_TO_AUDIO)
duration = data.size / fs
task1()
task2()
task3()
task4()
task5()
task6()
b_a = task7(filters[iof_filt])
task8(filters[iof_filt], b_a)
task9(filters[iof_filt], b_a)
task10(filters[iof_filt], b_a)
# plt.show()

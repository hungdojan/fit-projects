#!/usr/bin/env python3.9
# coding=utf-8

from matplotlib import pyplot as plt
import pandas as pd
import seaborn as sns
import numpy as np
import zipfile

# muzete pridat libovolnou zakladni knihovnu ci knihovnu predstavenou na prednaskach
# dalsi knihovny pak na dotaz

# Ukol 1: nacteni dat ze ZIP souboru
def load_data(filename : str) -> pd.DataFrame:
    # tyto konstanty nemente, pomuzou vam pri nacitani
    headers = ["p1", "p36", "p37", "p2a", "weekday(p2a)", "p2b", "p6", "p7", "p8", "p9", "p10", "p11", "p12", "p13a",
                "p13b", "p13c", "p14", "p15", "p16", "p17", "p18", "p19", "p20", "p21", "p22", "p23", "p24", "p27", "p28",
                "p34", "p35", "p39", "p44", "p45a", "p47", "p48a", "p49", "p50a", "p50b", "p51", "p52", "p53", "p55a",
                "p57", "p58", "a", "b", "d", "e", "f", "g", "h", "i", "j", "k", "l", "n", "o", "p", "q", "r", "s", "t", "p5a"]

    #def get_dataframe(filename: str, verbose: bool = False) -> pd.DataFrame:
    regions = {
        "PHA": "00",
        "STC": "01",
        "JHC": "02",
        "PLK": "03",
        "ULK": "04",
        "HKK": "05",
        "JHM": "06",
        "MSK": "07",
        "OLK": "14",
        "ZLK": "15",
        "VYS": "16",
        "PAK": "17",
        "LBK": "18",
        "KVK": "19",
    }

    # extract keys and values for later use
    region_keys = list(regions.keys())
    region_values = list(regions.values())

    dfs = []
    with zipfile.ZipFile(filename, mode='r') as z:
        # iterate through years
        for year_zip in z.namelist():
            with zipfile.ZipFile(z.open(year_zip)) as inner_zip:
                # iterate through regions
                for csv_file in inner_zip.namelist():
                    # filter out unwanted files
                    file_name = csv_file.rsplit('.')[0]
                    if file_name not in region_values:
                        continue

                    df = pd.read_csv(inner_zip.open(csv_file), encoding='cp1250',
                                     sep=';', low_memory=False, names=headers)
                    # set region name
                    df['region'] = region_keys[region_values.index(file_name)]
                    dfs.append(df)
    df = pd.concat(dfs, ignore_index=True)
    return df


# Ukol 2: zpracovani dat
def parse_data(df : pd.DataFrame, verbose : bool = False) -> pd.DataFrame:
    df2 = df.copy(deep=True)
    df2 = df2.drop_duplicates(subset='p1')

    # convert to category type
    df2 = df2.astype({ label: 'category'
                for label in ["p36", "weekday(p2a)", "p6", "p7", "p8", "p9", "p10", "p11", "p12", "p15", "p16", "p17", "p18",
                    "p19", "p20", "p21", "p22", "p23", "p24", "p27", "p28", "p35", "p39", "p44", "p45a", "p48a",
                    "p49", "p50a", "p50b", "p51", "p52", "p55a", "p57", "p58", "p5a", "k", 'l', "p", "q", "r", "t"]})
    df2['date'] = pd.to_datetime(df['p2a'], format='%Y-%m-%d')
    df2['d'] = pd.to_numeric(df2['d'].str.replace(',', '.'), errors='coerce').astype('float64')
    df2['e'] = pd.to_numeric(df2['e'].str.replace(',', '.'), errors='coerce').astype('float64')

    if verbose:
        print(f'orig_size={df.memory_usage(deep=True).sum() / 1_000_000:.1f} MB')
        print(f'new_size={df2.memory_usage(deep=True).sum() / 1_000_000:.1f} MB')
    return df2


# Ukol 3: počty nehod v jednotlivých regionech podle viditelnosti
def plot_visibility(df: pd.DataFrame, fig_location: str | None = None,
                    show_figure: bool = False):
    df2 = df.copy(deep=True)
    df2 = df2[['p19', 'region']]
    df2 = df2.replace({'p19': {
        1: 'DEN - nezhoršená',
        2: 'DEN - zhoršená',
        3: 'DEN - zhoršená',
        4: 'NOC - nezhoršená',
        5: 'NOC - zhoršená',
        6: 'NOC - nezhoršená',
        7: 'NOC - zhoršená'
    }})
    df2 = df2.loc[(df2['region'] == 'ULK') | (df2['region'] == 'ZLK')
                | (df2['region'] == 'MSK') | (df2['region'] == 'JHC')]
    df2['count'] = 1
    df2 = df2.groupby(['p19', 'region']).count()
    df2 = df2.reset_index()

    sns.set_style('darkgrid')
    g = sns.catplot(x='region', y='count', col='p19', data=df2, kind='bar', palette='muted',
                    col_wrap=2, errorbar=None, sharex="False", aspect=1.3, height=3.2)
    # plot configuration and label naming
    g.fig.suptitle('Počet nehod v jednotlivých krajích dle viditelnosti')
    g.set_axis_labels('', 'Počet nehod')
    for ax in g.axes.flat:
        ax.set_xlabel('Kraj')
    g.set_titles('Viditelnost: {col_name}')

    # set values on top of the bars
    # this code was written by user aamir23
    # and was taken from: https://stackoverflow.com/a/64867495
    for ax in g.axes.ravel():
        for c in ax.containers:
            labels = [f'{(v.get_height() / 1000):.1f}K' for v in c]
            ax.bar_label(c, labels=labels, label_type='edge')
        ax.margins(y=0.2)

    g.tight_layout()
    if show_figure:
        plt.show()
    if fig_location:
        g.fig.savefig(fig_location)


# Ukol4: druh srážky jedoucích vozidel
def plot_direction(df: pd.DataFrame, fig_location: str | None = None,
                   show_figure: bool = False):
    df2 = df.copy(deep=True)
    df2 = df2[['p7', 'date', 'region']]
    df2['count'] = 1
    # selecting region
    df2 = df2.loc[(df2['region'] == 'ULK') | (df2['region'] == 'ZLK')
                | (df2['region'] == 'MSK') | (df2['region'] == 'JHC')]
    # setup p7 column
    df2['p7'] = df2['p7'].cat.remove_categories(0)
    df2 = df2.replace({'p7': {
        1: 'čelní',
        2: 'boční',
        3: 'boční',
        4: 'zezadu',
    }})

    df2['date'] = df2['date'].dt.month
    df2 = df2.groupby(['region', 'date', 'p7'], as_index=False).count()
    df2 = df2.reset_index()

    # graph parameters
    graph_params = {
        'data': df2,
        'x': 'date',
        'y': 'count',
        'hue': 'p7',
        'col': 'region',
        'kind': 'bar',
        'palette': 'muted',
        'hue_order': ['boční', 'zezadu', 'čelní'],
        'errorbar': None,
        'col_wrap': 2,
    }
    sns.set_style('darkgrid')
    g = sns.catplot(**graph_params, sharex="False", sharey="False")
    g._legend.set_title('Druh srážky')
    for ax in g.axes.flat:
        ax.set_xlabel('Měsíc')
        ax.set_ylabel('Počet nehod')
    g.tight_layout()

    if show_figure:
        plt.show()
    if fig_location:
        g.fig.savefig(fig_location)


# Ukol 5: Následky v čase
def plot_consequences(df: pd.DataFrame, fig_location: str | None = None,
                    show_figure: bool = False):
    df2 = df.copy(deep=True)
    df2 = df2[['p13a', 'p13b', 'p13c', 'region', 'date']]
    df2 = df2.loc[(df2['region'] == 'PLK') | (df2['region'] == 'STC')
                | (df2['region'] == 'JHM') | (df2['region'] == 'JHC')]
    df2.loc[df2['p13a'] != 0, ['p13a', 'p13b', 'p13c']] = [1, 0, 0]
    df2.loc[df2['p13b'] != 0, ['p13b', 'p13c']] = [1, 0]
    df2.loc[df2['p13c'] != 0, 'p13c'] = 1
    df2 = df2.rename(columns={
        'p13a': 'Usmrcení',
        'p13b': 'Těžké zranění',
        'p13c': 'Lehké zranění'
    })
    df2 = df2.melt(['date', 'region'], var_name='Následky', value_name='Pocet')

    df2 = df2.pivot_table(index='date', columns=['region', 'Následky'], values='Pocet', aggfunc=np.sum)
    df2 = df2.resample('MS').sum()
    df2 = df2.stack(level='region')
    df2 = df2.stack(level='Následky')
    df2 = df2.reset_index()
    df2 = df2.rename(columns={0: 'count'})

    graph_params = {
        'data': df2,
        'x': 'date',
        'y': 'count',
        'hue': 'Následky',
        'col': 'region',
        'aspect': 1.7,
        'height': 3,
        'kind': 'line',
        'col_wrap': 2,
        'palette': 'muted'
    }

    g = sns.relplot(**graph_params, facet_kws=dict(sharex=False))
    g.set_axis_labels('', 'Počet nehod').set_titles('Kraj: {col_name}')

    # setup each axis
    for ax in g.axes:
        xticks = ax.get_xticks()
        ax.set_xticks(xticks)
        ax.set_ylim(0, )
        ax.set_xlim(np.array(['2016-01-01', '2022-01-01'], dtype='datetime64[D]'))
        ax.set_xticklabels([pd.to_datetime(tm, unit='d').strftime(r'%m/%y')
                           for tm in xticks])
        ax.set_xlabel('')
        ax.set_ylabel('Počet nehod')
    g.tight_layout()

    if show_figure:
        plt.show()
    if fig_location:
        g.fig.savefig(fig_location)

if __name__ == "__main__":
    # zde je ukazka pouziti, tuto cast muzete modifikovat podle libosti
    # skript nebude pri testovani pousten primo, ale budou volany konkreni
    # funkce.
    df = load_data("data/data.zip")
    df2 = parse_data(df, True)
    plot_visibility(df2, "01_visibility.png", show_figure=True)
    plot_direction(df2, "02_direction.png", show_figure=True)
    plot_consequences(df2, "03_consequences.png", show_figure=True)


# Poznamka:
# pro to, abyste se vyhnuli castemu nacitani muzete vyuzit napr
# VS Code a oznaceni jako bunky (radek #%%% )
# Pak muzete data jednou nacist a dale ladit jednotlive funkce
# Pripadne si muzete vysledny dataframe ulozit nekam na disk (pro ladici
# ucely) a nacitat jej naparsovany z disku

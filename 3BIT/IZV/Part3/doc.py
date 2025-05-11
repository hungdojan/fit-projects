#!/usr/bin/env python
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns
import textwrap

# meaning of values
causes = {
    501: 'jízda po nesprávné straně vozovky, vjetí do protisměru',
    502: 'vyhýbání bez dostatečného bočního odstupu (vůle)',
    503: 'nedodržení bezpečné vzdálenosti za vozidlem',
    504: 'nesprávné otáčení nebo couvání',
    505: 'chyby při udání směru jízdy',
    506: 'bezohledná, agresivní, neohleduplná jízda',
    507: 'náhlé bezdůvodné snížení rychlosti jízdy, zabrždění nebo zastavení',
    508: 'řidič se nevěnoval řízení vozidla',
    509: 'samovolné rozjetí nezajištěného vozidla',
    510: 'vjetí na nezpevněnou komunikaci',
    511: 'nezvládnutí řízení vozidla',
    512: 'jízda jednosměrnou ulicí'
}


def cause_of_accident(df: pd.DataFrame):
    """Print the amount of accidents sorted by the cause.

    Args:
        df (pd.DataFrame): Dataframe of accidents.
    """
    # sort by causes of accident (that are known)
    df2 = df.copy(deep=True)
    df2['p12'] = df2['p12'] // 100 * 100
    df2 = df2.groupby(['p12']) \
            .agg({'p1': 'count'}) \
            .rename(columns={'p1': 'count'}) \
            .reset_index()
    # add total row
    total = np.sum(df2['count'])

    # get values for output print
    nof_drivers_fault = df2.loc[df2['p12'] == 500]['count'].tolist()[0]
    drivers_fault_perc = nof_drivers_fault / total * 100

    print('Nehody zavinene nespravny zpusobem jizdy: {}/{} ({:.2f} %)'\
        .format(nof_drivers_fault, total, drivers_fault_perc))
    print()


def incorrect_way_of_driving(df: pd.DataFrame):
    """Display caused of accident in the category "incorrect way of driving".

    Args:
        df (pd.DataFrame): Dataframe of accidents.
    """
    # display number of accidents for each category and sort them
    df3 = df.copy(deep=True)
    df3 = df3.loc[df3['p12'].isin(np.arange(501, 513))]
    # number of incorrect way of driving
    nof_drivers_fault = len(df3.index)

    #
    sorted_df = df3.groupby(['p12']) \
                .agg({'p1': 'count'}) \
                .rename(columns={'p1': 'count'}) \
                .reset_index() \
                .sort_values('count', ascending=False) \
                .replace(causes)

    # filter out top 5
    top_six_causes = sorted_df.head(6)['p12'].tolist()
    print('Sest nejcastejsich duvodu nehody:')
    for i, cause in enumerate(top_six_causes):
        count = sorted_df['count'].tolist()[i]
        perc = count / nof_drivers_fault * 100
        print('    {:<55} {} ({:.2f} %)'.format(cause, count, perc))
    print()


def accidents_per_regions(df: pd.DataFrame):
    """Display number of accidents in regions per category.

    Args:
        df (pd.DataFrame): Dataframe of accidents.
    """
    df2 = df.copy(deep=True)
    df2 = df2.loc[df2['p12'].isin(np.arange(501, 513))]
    df2 = df2.groupby(['region', 'p12']).agg({'p1': 'count'})
    regions = df2.pivot_table(index='region', columns='p12', values='p1')
    print(regions)
    print()


def vehical_damage(df: pd.DataFrame):
    """Display average vehicle damage per category.

    Args:
        df (pd.DataFrame): Datafram of accidents.
    """
    df5 = df.copy(deep=True)
    df5 = df5.loc[df5['p12'].isin(np.arange(501, 513))]
    df5 = df5.groupby(['p12']) \
            .agg({'p53': np.mean}) \
            .reset_index() \
            .sort_values('p53', ascending=False) \
            .replace(causes)
    top_five_damage = df5.head(5)['p12'].tolist()
    top_five_damage

    print('Prumerne poskozeni vozidla podle priciny nehody:')
    for i, cause in enumerate(top_five_damage):
        mean_damage = df5['p53'].tolist()[i] * 100
        print('    {:<55} {:.2f} Kc'.format(cause, mean_damage))
    print()


def plot_accident_injuries(df: pd.DataFrame):
    """Plot graph of injuries and fatalities per category.

    Args:
        df (pd.DataFrame): Dataframe of accidents.
    """

    df3 = df.copy(deep=True)
    df3 = df3.loc[df3['p12'].isin(np.arange(501, 513))]
    injuries_total_df = df3.groupby(['p12']).agg({
        'p13a': np.sum,
        'p13b': np.sum,
        'p13c': np.sum
    }).reset_index().sort_values('p13a', ascending=False)
    top_six_fatalities = injuries_total_df.head(6)['p12'].tolist()
    # get top 6 most fatal causes of accident
    injuries_total_df = injuries_total_df.loc[injuries_total_df['p12']
                                            .isin(top_six_fatalities)] \
                                        .replace(causes)

    # plot graph
    sns.set_style('darkgrid')
    melted_df = injuries_total_df.melt(
        id_vars=['p12'],
        var_name='zraneni',
        value_name='value'
    ).replace({
        'p13a': 'Úmrtí',
        'p13b': 'Těžká zranění',
        'p13c': 'Lehká zranění'
    })

    plot_params = dict(
        data=melted_df,
        col='zraneni',
        y='p12',
        x='value',
        kind='bar',
        sharex=False,
        palette='pastel',
        orient='h',
        col_wrap=1,
        order=injuries_total_df['p12'].tolist(),
        aspect=1.7,
        height=4
    )
    g = sns.catplot(**plot_params)
    g.set_titles('{col_name}')
    # text wrapping was taken from stack overflow questoin
    # https://stackoverflow.com/a/59315998
    for ax in g.axes.flat:
        ax.set_xlabel('Počet')
        ax.set_ylabel('Hlavní příčina')
        ax.set_yticklabels(textwrap.fill(x.get_text(), 20) for x in ax.get_yticklabels())

    # axis label edit
    for ax in g.axes.ravel():
        for c in ax.containers:
            labels = [f'{int(v.get_width())}' for v in c]
            ax.bar_label(c, labels=labels, label_type='center')
    g.tight_layout()
    g.fig.savefig('fig.png')
    plt.show()

# load data
if __name__ == '__main__':
    df = pd.read_pickle('accidents.pkl.gz')
    cause_of_accident(df)
    incorrect_way_of_driving(df)
    accidents_per_regions(df)
    vehical_damage(df)
    plot_accident_injuries(df)

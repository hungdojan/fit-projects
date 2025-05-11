#!/usr/bin/python3.10
# coding=utf-8
import pandas as pd
import geopandas
import matplotlib.pyplot as plt
import contextily
import sklearn.cluster
import numpy as np
from mpl_toolkits.axes_grid1 import make_axes_locatable


def make_geo(df: pd.DataFrame) -> geopandas.GeoDataFrame:
    """Convert dataframe to geopandas.GeoDataFrame with correct CRS.

    Args:
        df (pd.DataFrame): Dataframe of accidents.

    Returns:
        geopandas.GeoDataFrame: Converted geopandas dataframe.
    """
    df2 = df.copy(deep=True)
    # filter out unknown coordinates
    df2 = df2.dropna(subset=['d', 'e'])
    gdf = geopandas.GeoDataFrame(
            data=df2,
            geometry=geopandas.points_from_xy(df2['d'], df2['e']),
            crs='EPSG:5514'
    )
    return gdf


def plot_geo(gdf: geopandas.GeoDataFrame, fig_location: str | None = None,
             show_figure: bool = False):
    """ Plot map of accidents in a region.

    Only accidents where at least one of the attendee was under the influence
    of drugs or alcohol and happened in years 2018-2021.

    Args:
        gdf (geopandas.GeoDataFrame): Geo dataframe of accidents.
        fig_location (str, optional): File name to store plot to.
                                      Defaults to None.
        show_figure (bool, optional): Show the plot. Defaults to False.
    """
    gdf2 = gdf.copy(deep=True)
    gdf2 = gdf2.loc[(gdf2['region'] == 'ULK') & (gdf2['p11'] >= 3)]
    gdf2['p2a'] = pd.to_datetime(gdf2['p2a'], format='%Y-%m-%d')
    fig, ax = plt.subplots(2, 2, figsize=(12, 8))

    # edit each subplot individually
    for i, a in enumerate(ax.flatten()):
        gdf2[gdf2['p2a'].dt.year == 2018 + i].plot(ax=a, markersize=5,
                                                   color='red', alpha=0.9)
        contextily.add_basemap(ax=a, crs=gdf.crs.to_string(),
                               source=contextily.providers.Stamen.TonerLite)
        a.set_title(f'ULK kraj ({2018 + i})')
        a.axis('off')

    fig.tight_layout()
    if fig_location:
        fig.savefig(fig_location)
    if show_figure:
        plt.show()


def plot_cluster(gdf: geopandas.GeoDataFrame, fig_location: str | None = None,
                 show_figure: bool = False):
    """Plot map of clustered accidents in a region.

    Only selected types of road are plotted.

    Args:
        gdf (geopandas.GeoDataFrame): Geo dataframe of accidents.
        fig_location (str, optional): File name to store plot to.
                                      Defaults to None.
        show_figure (bool, optional): Show the plot. Defaults to False.
    """
    gdf2 = gdf.copy(deep=True)
    gdf2 = gdf2.loc[(gdf2['region'] == 'ULK') &
                    (1 <= gdf2['p36']) & (gdf2['p36'] <= 3)]

    fig, ax = plt.subplots(1, 1, figsize=(8, 8))

    # cluster accidents
    coords = np.dstack([gdf2.geometry.x, gdf2.geometry.y]).reshape(-1, 2)
    kmeans = sklearn.cluster.KMeans(n_clusters=20, n_init='auto',
                                    random_state=0).fit(coords)
    gdf2['cluster'] = kmeans.labels_
    gdf2 = gdf2.dissolve(by='cluster', aggfunc={'p1': 'count'}
                        ).rename(columns=dict(p1='count'))

    divider = make_axes_locatable(ax)
    cax = divider.append_axes('bottom', size='5%', pad=0.1)

    # plot graph
    legend_kwds = {
        'label': 'Počet nehod v úseku',
        'orientation': 'horizontal'
    }
    gdf2.plot(ax=ax, markersize=5, column='count', legend=True, cax=cax,
                   vmin=0, legend_kwds=legend_kwds)
    contextily.add_basemap(ax=ax, crs=gdf.crs.to_string(),
                           source=contextily.providers.Stamen.TonerLite)

    ax.set_title('Nehody v ULK kraji na silnicích 1., 2. a 3. třídy')
    ax.axis('off')

    fig.tight_layout()
    if fig_location:
        fig.savefig(fig_location)
    if show_figure:
        plt.show()

if __name__ == "__main__":
    gdf = make_geo(pd.read_pickle("data/accidents.pkl.gz"))
    plot_geo(gdf, "geo1.png", True)
    plot_cluster(gdf, "geo2.png", True)

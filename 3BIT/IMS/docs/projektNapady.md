# Mozna temata k zpracovani do IMS:

## prenos rezistence vuci antibiotikum mezi bakteriema
## rozvoj mest (ekologie)
-  **Pocatecni hypoteza/co chceme zjistit:** 
-  **Nalezene studie:** 

## rozvoj populace rostlin (ekologie)
-  **Pocatecni hypoteza/co chceme zjistit:** 
-  **Nalezene studie:** 
    - The concept of discrete description of plant ontogenesis and      cellular automata models of plant populations

        https://www-sciencedirect-com.ezproxy.lib.vutbr.cz/science/article/pii/S0304380003002436#FIG1

    - A Solution to the Biodiversity Paradox by Logical Deterministic   Cellular Automata
        https://link.springer.com/article/10.1007/s10441-015-9257-9#Sec2

    - Cellular automata models for vegetation dynamics
        https://www.sciencedirect.com/science/article/pii/S0304380097002020

        4.1 Soutez dvou druhu rostlin

## soutez stromu a travy v savane (ekologie)
-  **Pocatecni hypoteza/co chceme zjistit** 
-  **Co je potreba**
    - **Cas simulace:**
    - **Tranzitivni funkce:**
    - **Stochasticka data destove vody:**
-  **Nalezene studie**
    - Tree-grass competition in space and time: Insights from a simple cellular automata model based on ecohydrological dynamics
    https://agupubs.onlinelibrary.wiley.com/doi/full/10.1029/2001WR000768
        - dynamicka koexistence stromu a travy na zaklade deste
        - pocet stromu zavisi na desti
        - dynamical water stress function is taken as the basis to describe the death and establishment chances of trees and grasses in a simple spatially explicit model
        - grid of 100 by 100 cells
        - size equal to the canopy of a mature tree (25m2)
        - cell types: (unoccupied, grass, tree, tree seedling)
        - Dest je rovnomerne rozprostren
        - Strom pracuje s 24-okolim (2 bunky siroke moorovo okoli)
        - Ty funkce tranzitivni jsou fakt darecek, nechapu je
        - chances of death for trees, tree seedlings and grasses are defined by the value of dynamic stress minus a threshold value
        - Je potreba prijit na charakteristiku vstupu deste
        - Je potreba vyresit tu tranzitivni fci
    - Tree-grass coexistence in savannas revisited - insights from an     examination of assumptions and mechanisms invoked in existing models
    
        https://onlinelibrary.wiley.com/doi/epdf/10.1111/j.1461-0248.2004.00596.x
## rozptyl hub po 2D plose (biologie)
-  **Pocatecni hypoteza/co chceme zjistit** 
-  **Co je potreba**
    - **Cas simulace:**
    - **Tranzitivni funkce:**
    - **Stochasticka data destove vody:**
-  **Nalezene studie**
    - _Cellular automata simulations of fungal growth on solid substrates_
        https://www.sciencedirect.com/science/article/pii/073497509390031H
        - Grown on a solid substrate (cereal grains)
        - Growth rate is largely determined by the availability of nutrients on a growth medium
        - CA view: "If I am unoccupied, have nutrients, and have
            fungal hyphae near me, I will (perhaps) become colonized."
        - Each cell will have, in its inial state a certain amount of nutrients available
        - 320 x 200 lattice
        - Each simulation is started from a single spore placed in the centre
        - If colony covers the entire screen, colony mass density is calculated: `colony_density = number_of_occupied_cells / number_of_all_cells`
        - Models presented by the study:
          - **Model 1a**
            - If a site is in the neughbourhood of the current cell is occupied, then the probability of the currect cell being occupied is defined as follows: `1 neighbor, 12.5%; 2 neighbors, 25%; 3 neighbors, 50%: 4-8 neighbors, 0%`
          - **Variations implemented by other models**
            - An unoccupied site is aged as its neighbours accumulate. This is a representation of nutrient depletion 
            - Experimentation with growth directions - not clockwise or anti-clockwise! Instead for example: N, SW, E, NW, S, NE, W, SE. Site can only be occupied if an occupied neighbour is present in that direction.
            - Adding multiple states to occupied cells. These transitions are dependent on the number of neighbours a cell has.
    - _Nutritional influence on fungal colony growth and biomass distribution in response to toxic metals_
    https://academic.oup.com/femsle/article/204/2/311/471993
        - as the amount of available carbon source (glucose) increased, the apparent toxicity of the metals decreased, suggesting that, the more nutrients a cell has available, the more likely it is to divide in that spot again
    - _Generic model of morphological changes in growing colonies of fungi_ https://academic.oup.com/femsle/article/204/2/311/471993
      - Growth is happening from a line in a single direction, not from a point.
      - In nutrientrich conditions, the colonies showed a homogeneous andsmooth growth front. In contrast, growth in glucose-poor conditions resulted in colonies with rougher surfaces.

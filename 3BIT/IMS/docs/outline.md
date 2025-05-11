# Outline for the simulation study (subject of fungi)
## General points
- Is the published thesis valid? What are the limitations of the model, how did our stduy improve/reflect this?
- **Initial hypothesis**
  - As nurtrients are depleted, the fungal colony grows sparser, but with the same "diameter". We can observe fractal growth
  - How complex is our model?
  - What was included in our model to reflect the inital hypothesis? What was excluded?
  - What are the results of our implementation?
  - How was reality discretized into a cellular automaton?

## Outline
- Intro
  - Context of the problem, aims of the study
- Description of our approach: equations, transitional functions, theory
- Description of implementation
- Results
## Model properties
### canvas
  - size 50x50 (?)
### cell
  - occupied
  - unoccupied
  - amount of nutrients (?)
### transition fucntions
#### existing transitions taken from
https://www.sciencedirect.com/science/article/abs/pii/073497509390031H?fr=RR-2&ref=pdf_download&rr=772b8fb22997b37d
- **Model 1**
  - Simplest. If a site in the neighbourhood is occupied, then the current cell with become occupied with a given probability:
  - **Variant A**
    - 1 neighbour: 12.5%
    - 2 neighbours: 25%
    - 3 neighbours: 50%
    - 4-8 neighbours: 0%
  - **Variant B (inverted A)**
    - 1 neighbour: 50%
    - 2 neighbours: 25%
    - 3 neighbours: 12.5%
    - 4-8 neighbours: 0%
  -  **Variant C**
     - 1 neighbour: 12.5%
     - 2-8 neighbours: 0%
- **Model 2**
  - Probabilities of growth are as specified by the C variant
  - Unoccupied sites cycle through 4 stages, after which they become inactive, with the following probability dependent on the number of neighbours for each stage:
    - 1 neighbour: 50%
    - 2 neighbours: 75%
    - 3-8 neighbours: 100%
- **Model 3**
  - Adds growth directions, wouldn't bother with this yet
- **Model 4**
  - Adds stages to occupied sites, colours could be used here. Neighbour dependent stage movement defined by the following probabilities:
    - 0-2 neighbours 0%
    - 3-8 neighbours 12.5%

#### my trial at something new
- **Modifications to Model 2:**
  - User will be able to pick a number of states an empty cell will have to cycle through in order to become uninhabitable.
  - Stages in an array. Exponential increase in probability of a cell becoming uninhabitable
  - From that a probability of cells dividing is inferred. 
  - Add a counter of cells that are alive, and a counter of generations.
  - Higher nutritional argument should reflect close packed growth -> more aggressive division
- **Completely new model**
  - high nutrients -> more compact growth
  - low nutrients -> less compact growth
  - nutrients are 
    - if a cell is unoccupied, look for neighbours:
    - if a neighbour does not have nutrientes available, do not divide
    - n...number of neighbours (1-8)
    - c...concentration of nutrients (1-90)
    - p = n*(c)/8
  - if a cell is occupied deplete nutrients

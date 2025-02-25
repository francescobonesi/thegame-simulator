# TheGame simulation

## Multiplayer version

### Init 

Open `mpconfig.txt` and modify the constants

```
CARD_MAX_NUMBER 100
REVERSE_MOVE_DIFF 10
CARD_IN_HANDS 6
NUM_CARDS_TO_PLAY 2
NUMBER_OF_ROWS 4
NUMBER_OF_PLAYERS 3
NUM_SIMULATIONS 100
```

### Compile and Run

Then, run simulation

```
g++ -o the_game main.cpp helper_functions.cpp player_strategies.cpp game_logic.cpp
./the_game
```

Output example:

```
3 Players: 
A win rate: 0.1 %
3 Players: 
B win rate: 0 %
3 Players: 
C win rate: 0 %
3 Players: 
D win rate: 0 %
3 Players: 
E win rate: 0 %
3 Players: 
F win rate: 0 %
3 Players: 
G win rate: 0 %
3 Players: 
H win rate: 0 %
3 Players: 
I win rate: 0 %
```


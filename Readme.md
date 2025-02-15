# TheGame simulation

## Init 

Open `thegame.cpp` and modify the constants at the beginning

```
# regular game settings
const int CARD_MAX_NUMBER = 100;
const int REVERSE_MOVE_DIFF = 10;
const int CARD_IN_HANDS = 8;
const int NUM_CARDS_TO_PLAY = 2;
const int NUMBER_OF_ROWS = 4;
# number of simulations (shuffles)
const int NUM_SIMULATIONS = 100;
```

## Compile and Run

Then, run simulation

```
g++ thegame.cpp -o thegame
./thegame
```

Output is `game_results.json`

## Results analysis

Convert it to CSV 

```
python3 game_json_to_csv.py
```

Expected output

```
Successfully converted 'game_results.json' to 'game_results.csv'
```


Then run analysis script

```
python3 analysis.py
```

Output similar to

```
Overall Statistics:

Total Games Played: 900
Overall Win Rate: 0.56%

Strategy-wise Statistics:

               Games Played  Win Rate  Win Rate Std Dev
strategy_name                                          
A                       100      0.02          0.140705
B                       100      0.00          0.000000
C                       100      0.00          0.000000
D                       100      0.00          0.000000
E                       100      0.02          0.140705
F                       100      0.00          0.000000
G                       100      0.00          0.000000
H                       100      0.01          0.100000
I                       100      0.00          0.000000

<and continues>
```




## Tip

For more accurate statistics, set `NUM_SIMULATION` to be greater or equal than 100000 (a hundred thousand).
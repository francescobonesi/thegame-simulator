import pandas as pd
from collections import defaultdict
from itertools import combinations

def analyze_game_results(csv_file):
    """
    Analyzes game results from a CSV file for "The Game".

    Args:
        csv_file (str): The path to the CSV file containing the game results.

    Returns:
        dict: A dictionary containing the analysis results.
    """

    df = pd.read_csv(csv_file)

    # 1. Wins per strategy and per number of players
    wins_by_strategy_players = df.groupby(['NumPlayers', 'Strategy'])['Win'].apply(
        lambda x: (x == True).sum()
    ).reset_index()

    # 2. Average turns before losing (only for lost games)
    lost_games = df[df['Win'] == False]
    avg_turns_lost = lost_games.groupby(['NumPlayers', 'Strategy'])['Turns'].mean().reset_index()

    # 3. Average deck size when losing
    avg_deck_size_lost = lost_games.groupby(['NumPlayers', 'Strategy'])['DeckSize'].mean().reset_index()

    # Get unique Strategy
    strategies = set(df['Strategy'].unique())
    # Get unique NumPlayers
    num_players = set(df['NumPlayers'].unique())


    all_strategies_combinations = []
    for i in range(1, len(strategies) + 1):  # Iterate through combination lengths
        for comb in combinations(strategies, i):
            all_strategies_combinations.append(list(sorted(comb)))  # Sort and add as tuple
    
    shuffle_strategy_map = defaultdict(dict)
    for n in num_players:
        shuffle_strategy_map[n] = defaultdict(list)

    for _, row in df[df['Win'] == True].iterrows():
        n = row['NumPlayers']
        shuffle_id = row['ShuffleID']
        strategy = row['Strategy']
        shuffle_strategy_map[n][shuffle_id].append(strategy)
    
    shuffle_map = dict(shuffle_strategy_map)
    for k in shuffle_map:
        shuffle_map[k] = dict(shuffle_map[k])

    combination_counts = defaultdict(dict)
    for n in num_players:
        combination_counts[n] = defaultdict(int)

    for k in shuffle_map:
        for strategies in shuffle_map[k].values():
            # Sort the strategies and convert to a tuple for consistent counting
            sorted_strategies = tuple(sorted(strategies))
            combination_counts[k][sorted_strategies] += 1

    combination_wins_count = dict(combination_counts)
    for k in combination_wins_count:
        combination_wins_count[k] = dict(combination_wins_count[k])
    

    return {
        'wins_per_strategy_and_players': wins_by_strategy_players,
        'average_turns_before_losing': avg_turns_lost,
        'average_deck_size_when_losing': avg_deck_size_lost,
        'combination_wins_count': combination_wins_count
    }



# Example usage:
results = analyze_game_results('out/game_results.csv')

print("Wins per Strategy and Number of Players:")
print(results['wins_per_strategy_and_players'])
print("\nAverage Turns Before Losing (for lost games):")
print(results['average_turns_before_losing'])
print("\nAverage Deck Size When Losing:")
print(results['average_deck_size_when_losing'])
print("\nWin Combination Counts (Number of Players, Number of Strategies): Count")
print(results['combination_wins_count'])
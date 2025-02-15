import pandas as pd
import numpy as np
import re

def parse_rows(row_str):
    """Parses the playing_rows string into a list of lists of integers."""
    if pd.isna(row_str) or row_str.strip() == "[]":
        return [[] for _ in range(4)]

    try:
        row_str = row_str.strip("[]")
        rows = row_str.split(" 100 ")
        if len(rows) == 1:
            rows = row_str.split(" 1 ")
            if len(rows) == 4:
                 return [[int(x) for x in row.split()] for row in rows]
            else:
                rows = row_str.split(" ")
                output_rows = [[1]]
                start = 1
                for i in range(1, len(rows)):
                    if (rows[i] == '100'):
                        output_rows[start-1].append(100)
                        output_rows.append([])
                        start+=1
                    else:
                        output_rows[start-1].append(int(rows[i]))
                while(len(output_rows) < 4):
                    output_rows.append([100])
                return output_rows

        assert len(rows) <= 4
        for i in range(len(rows)):
             if i == 0:
                rows[i] = [int(x) for x in rows[i].split()]
                if 1 not in rows[i]:
                    rows[i].insert(0,1)
             elif i == len(rows)-1:
                 rows[i] = [int(x) for x in rows[i].split()]
                 if 100 not in rows[i]:
                    rows[i].insert(0,100)
             else:
                rows[i] = [100] + [int(x) for x in rows[i].split()] +[1]

        while len(rows) < 4:
            rows.append([100])
        return rows

    except (ValueError, IndexError) as e:
        print(f"Error parsing row string: {row_str}. Error: {e}")
        return [[] for _ in range(4)]

def parse_hand(hand_str):
    """Parses the hand string into a list of integers."""
    if pd.isna(hand_str) or hand_str.strip() == "[]":
        return []
    try:
        return [int(x) for x in hand_str.strip("[]").split()]
    except ValueError as e:
        print(f"Error parsing hand string: {hand_str}. Error: {e}")
        return []

def analyze_strategy_comparison(df, strategy1, strategy2):
    """Compares two strategies based on win counts, considering only shuffles won by at least one."""

    # Filter for the two strategies
    df_filtered = df[df['strategy_name'].isin([strategy1, strategy2])]

    # Further filter: Only keep rows where at least one of the strategies won.  THIS IS KEY.
    df_filtered = df_filtered[df_filtered['win'] == 1]


    # Pivot the table
    pivot_df = df_filtered.pivot_table(index='shuffle_id', columns='strategy_name', values='win', aggfunc='max')
    pivot_df = pivot_df.fillna(0)

    if strategy1 not in pivot_df.columns or strategy2 not in pivot_df.columns:
        # print(f"Skipping comparison: '{strategy1}' vs. '{strategy2}' (one or both have no wins).") #optional message
        return  # Skip if one strategy never wins

    wins_strategy1 = (pivot_df[strategy1] == 1) & (pivot_df[strategy2] == 0)
    wins_strategy2 = (pivot_df[strategy1] == 0) & (pivot_df[strategy2] == 1)
    wins_both = (pivot_df[strategy1] == 1) & (pivot_df[strategy2] == 1)
    wins_neither = (pivot_df[strategy1] == 0) & (pivot_df[strategy2] == 0)  # Should be 0 now, but good to keep

    print(f"Comparison of Strategies (considering only shuffles won by at least one): {strategy1} vs. {strategy2}")
    print(f"Wins by {strategy1} only: {wins_strategy1.sum()}")
    print(f"Wins by {strategy2} only: {wins_strategy2.sum()}")
    print(f"Wins by both {strategy1} and {strategy2}: {wins_both.sum()}")
    print(f"Wins by neither (should be 0): {wins_neither.sum()}") #this should be 0

def analyze_the_game_results(csv_file):
    """Analyzes The Game results from a CSV file."""

    try:
        df = pd.read_csv(csv_file)
    except FileNotFoundError:
        print(f"Error: File not found: {csv_file}")
        return
    except pd.errors.ParserError:
        print(f"Error: Parsing error in CSV file: {csv_file}.  Check for correct formatting.")
        return
    except Exception as e:
        print(f"An unexpected error occurred: {e}")
        return

    required_columns = ['shuffle_id', 'strategy_name', 'win', 'turns', 'deck_size', 'final_playing_rows', 'final_hand']
    if not all(col in df.columns for col in required_columns):
        print(f"Error: CSV file must contain the following columns: {required_columns}")
        return

    df['final_playing_rows'] = df['final_playing_rows'].apply(parse_rows)
    df['final_hand'] = df['final_hand'].apply(parse_hand)

    print("Overall Statistics:\n")
    print(f"Total Games Played: {len(df)}")
    print(f"Overall Win Rate: {df['win'].mean() * 100:.2f}%")

    print("\nStrategy-wise Statistics:\n")
    strategy_stats = df.groupby('strategy_name')['win'].agg(['count', 'mean', 'std'])
    strategy_stats.rename(columns={'count': 'Games Played', 'mean': 'Win Rate', 'std': 'Win Rate Std Dev'}, inplace=True)
    print(strategy_stats.fillna(0))

    print("\nAverage Turns for wins:\n")
    avg_turns_for_wins = df[df['win'] == 1].groupby('strategy_name')['turns'].agg(['mean', 'std']).fillna(0)
    print(avg_turns_for_wins)

    print("\nAverage Turns for losses:\n")
    avg_turns_for_losses = df[df['win'] == 0].groupby('strategy_name')['turns'].agg(['mean', 'std']).fillna(0)
    print(avg_turns_for_losses)

    print("\nAverage deck size for wins:\n")
    avg_deck_win = df[df['win']==1].groupby('strategy_name')['deck_size'].agg(['mean', 'std']).fillna(0)
    print(avg_deck_win)

    print("\nAverage deck size for losses:\n")
    avg_deck_lost = df[df['win']==0].groupby('strategy_name')['deck_size'].agg(['mean', 'std']).fillna(0)
    print(avg_deck_lost)

    print("\nTurns Distribution (Lost):")
    turns_dist_losts = pd.crosstab(df[df['win'] == 0]['strategy_name'], df[df['win'] == 0]['turns'])
    print(turns_dist_losts)

    print("\nPercentage of Lost Games by Turns:")
    turns_percentage_losts = turns_dist_losts.div(turns_dist_losts.sum(axis=1), axis=0) * 100
    print(turns_percentage_losts)

    df['hand_size'] = df['final_hand'].apply(len)
    print("\nAverage Hand Size at End of Game (by Strategy, for Losses):")
    avg_hand_size_loss = df[df['win'] == 0].groupby('strategy_name')['hand_size'].agg(['mean','std']).fillna(0)
    print(avg_hand_size_loss)
    
    print("\nChecking consistency of suffle_id. Number of distinct shuffle_id:")
    print(df['shuffle_id'].nunique())

    # --- Strategy Comparison ---
    print("\n--- Strategy Comparison ---")
    strategies = df['strategy_name'].unique()
    for i in range(len(strategies)):
      for j in range(i + 1, len(strategies)):
        analyze_strategy_comparison(df.copy(), strategies[i], strategies[j])

# Example usage:
analyze_the_game_results("game_results.csv")
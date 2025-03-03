import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

def analyze_game_results(csv_file):
    """Analyzes the game results CSV and provides statistics and visualizations,
       divided by the number of players.
    """

    try:
        df = pd.read_csv(csv_file)
    except FileNotFoundError:
        print(f"Error: CSV file '{csv_file}' not found.")
        return
    except pd.errors.EmptyDataError:
        print(f"Error: CSV file '{csv_file}' is empty.")
        return
    except Exception as e:
        print(f"An error occurred while reading the CSV: {e}")
        return

    # --- Data Cleaning and Preprocessing ---
    for col in ['NumPlayers', 'Turns', 'DeckSize']:
        df[col] = pd.to_numeric(df[col], errors='coerce').fillna(0).astype(int)

    # --- Analysis, grouped by NumPlayers ---
    for num_players in sorted(df['NumPlayers'].unique()):
        print(f"\n--- Analysis for {num_players} Player(s) ---")
        df_subset = df[df['NumPlayers'] == num_players]

        # --- Win Counts ---
        print("\nWin Counts by Strategy:")
        print(df_subset.groupby('Strategy')['Win'].value_counts().unstack(fill_value=0))

        # --- Win Rate Analysis ---
        win_rate_by_strategy = df_subset.groupby('Strategy')['Win'].value_counts(normalize=True).unstack(fill_value=0)
        print("\nWin Rate by Strategy:\n", win_rate_by_strategy)


        # --- Turns Analysis (Losses Only) ---
        avg_turns_loss = df_subset[df_subset['Win'] == False]['Turns'].mean()
        print("\nAverage Turns (Losses Only):", avg_turns_loss if not pd.isna(avg_turns_loss) else "No losses")

        #Average Turns (Losses Only) By Strategy
        print("\nAverage Turns (Losses Only) by Strategy")
        print(df_subset[df_subset['Win'] == False].groupby('Strategy')['Turns'].mean())


        # # --- Visualizations ---

        # plt.figure(figsize=(10, 6))
        # sns.countplot(data=df_subset, x='Strategy', hue='Win')
        # plt.title(f'Win/Loss Count by Strategy ({num_players} Players)')
        # plt.show()

        # # Only plot if there are losses
        # if not df_subset[df_subset['Win'] == False].empty:
        #     plt.figure(figsize=(10, 6))
        #     sns.barplot(data=df_subset[df_subset['Win'] == False], x='Strategy', y='Turns', ci=None)
        #     plt.title(f'Average Turns for Losses by Strategy ({num_players} Players)')
        #     plt.show()

        #     plt.figure(figsize=(10, 6))
        #     sns.boxplot(data=df_subset[df_subset['Win'] == False], x='Strategy', y='Turns')
        #     plt.title(f'Distribution of Turns for Losses by Strategy ({num_players} Players)')
        #     plt.show()

        # plt.figure(figsize=(10, 6))
        # sns.histplot(data=df_subset, x='Turns', bins=20, kde=True)
        # plt.title(f'Distribution of Turns ({num_players} Players)')
        # plt.show()

        
    # --- Overall Win Counts (Across all player counts) ---
    print("\n--- Overall Win Counts by Number of Players and Strategy ---")
    print(df.groupby(['NumPlayers', 'Strategy'])['Win'].value_counts().unstack(fill_value=0))


# --- Main Execution ---
if __name__ == "__main__":
    csv_file_path = "out/game_results.csv"
    analyze_game_results(csv_file_path)
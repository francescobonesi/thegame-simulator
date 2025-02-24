import json
import pandas as pd
import re

def json_to_csv(json_file, csv_file):
    """
    Converts a JSON file containing game results to a CSV file,
    handling lists within the data correctly.
    """
    try:
        with open(json_file, 'r') as f:
            data = json.load(f)
    except FileNotFoundError:
        print(f"Error: File not found: {json_file}")
        return
    except json.JSONDecodeError:
        print(f"Error: Invalid JSON format in file: {json_file}")
        return
    except Exception as e:
        print(f"An unexpected error occurred reading JSON: {e}")
        return

    if 'game_results' not in data:
        print("Error: JSON data does not contain a 'game_results' key.")
        return

    game_results = data['game_results']

    # Convert lists to comma-separated strings, handling edge cases
    for game in game_results:
        # Playing Rows
        if isinstance(game['final_playing_rows'], list):
            row_strings = []
            for row in game['final_playing_rows']:
                if isinstance(row, list):
                    # Convert each inner list to a comma-separated string
                    row_strings.append(" ".join(str(x) for x in row))
                else:
                    # Handle cases where an inner element isn't a list (shouldn't happen, but be safe)
                    row_strings.append(str(row))
            # Join the row strings with "], [" to match the desired format
            game['final_playing_rows'] = "[" + " ".join(row_strings) + "]"
        else:
             game['final_playing_rows'] = str(game['final_playing_rows']) #handle unexpected type

        # Final Hand
        if isinstance(game['final_hand'], list):
            game['final_hand'] = "[" + " ".join(str(x) for x in game['final_hand']) + "]"
        else:
            game['final_hand'] = str(game['final_hand']) #handle unexpected type.
        game['win'] = int(game['win'])

    try:
        df = pd.DataFrame(game_results)
        df.to_csv(csv_file, index=False)  # Use index=False to avoid writing row numbers
        print(f"Successfully converted '{json_file}' to '{csv_file}'")
    except Exception as e:
        print(f"An unexpected error occurred writing CSV: {e}")
        return

# Example Usage
json_file = 'game_results.json'
csv_file = 'game_results.csv'
json_to_csv(json_file, csv_file)
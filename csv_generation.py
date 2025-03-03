import csv
import re

def parse_game_data(game_text):
    """Parses a single game's results from the text format.

    Args:
        game_text: A string containing the game results.

    Returns:
        A dictionary containing the parsed game data, or None if parsing fails.
    """
    game_data = {}

    match = re.search(r"Number of Players: (\d+)", game_text)
    if match:
        game_data['NumPlayers'] = int(match.group(1))
    else:
        return None

    match = re.search(r"Shuffle ID: ([\d_]+)", game_text)
    if match:
        game_data['ShuffleID'] = match.group(1)

    match = re.search(r"Strategy: (\w+)", game_text)
    if match:
        game_data['Strategy'] = match.group(1)

    match = re.search(r"Win: (true|false)", game_text)
    if match:
        game_data['Win'] = match.group(1).lower() == 'true'

    match = re.search(r"Turns: (\d+)", game_text)
    if match:
        game_data['Turns'] = int(match.group(1))

    match = re.search(r"Deck Size: (\d+)", game_text)
    if match:
        game_data['DeckSize'] = int(match.group(1))

    rows_match = re.search(r"Final Playing Rows:\s*(.*?)\s*Final Hands:", game_text, re.DOTALL)
    if rows_match:
        rows_text = rows_match.group(1)
        rows = {}
        for i, row_type in enumerate(['Ascending1', 'Ascending2', 'Descending1', 'Descending2']):
            row_match = re.search(rf"\s*{row_type.replace('1', '').replace('2', '')}:\s*(.+)", rows_text)
            if row_match:
                rows[row_type] = ' '.join(row_match.group(1).split())
            else:
                rows[row_type] = ''
        game_data.update(rows)

    hands_match = re.search(r"Final Hands:\s*(.*)", game_text, re.DOTALL)
    if hands_match:
        hands_text = hands_match.group(1)
        # Find all player hands
        player_matches = re.findall(r"Player (\d+): (.*?)(?:\n\s*Player \d+:|$)", hands_text, re.DOTALL)
        hands = {}
        for player_num, hand_text in player_matches:
            hands[f'Player{player_num}'] = ' '.join(hand_text.split())
        
        # Normalize player hands: Ensure all possible player keys exist
        for i in range(1, 6):  # Check for players 1 to 5
            player_key = f'Player{i}'
            if player_key not in hands:
                hands[player_key] = ''  # Add missing player with an empty hand
        game_data.update(hands)

    return game_data



def process_all_games(input_files, output_csv):
    """Processes game results from multiple files and writes them to a CSV.

    Args:
        input_files: A list of paths to the input text files.
        output_csv: The path to the output CSV file.
    """
    all_game_data = []

    for input_file in input_files:
        try:
            with open(input_file, 'r') as f:
                input_text = f.read()
                game_texts = re.split(r"(?m)^\s*Game Results:", input_text)
                game_texts = [game.strip() for game in game_texts if game.strip()]

                for game_text in game_texts:
                    game_data = parse_game_data(game_text)
                    if game_data:
                        all_game_data.append(game_data)
        except FileNotFoundError:
            print(f"Error: File not found: {input_file}")
            #  Don't exit; continue processing other files
        except Exception as e:
            print(f"An error occurred processing {input_file}: {e}")
            # Don't exit; continue processing other files

    if not all_game_data:
        print("No game data parsed. Check your input files.")
        return

    fieldnames = set()
    for game in all_game_data:
        fieldnames.update(game.keys())
    prioritized_fields = ['NumPlayers', 'ShuffleID', 'Strategy', 'Win', 'Turns', 'DeckSize',
                         'Ascending1', 'Ascending2', 'Descending1', 'Descending2']
    remaining_fields = sorted(list(fieldnames - set(prioritized_fields)))
    fieldnames = prioritized_fields + remaining_fields

    with open(output_csv, 'w', newline='') as csvfile:
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(all_game_data)

    print(f"CSV file '{output_csv}' created successfully.")

# --- Main Execution ---
if __name__ == "__main__":
    input_files = ["out/game_results.out"]
    output_csv_file = "out/game_results.csv"
    process_all_games(input_files, output_csv_file)
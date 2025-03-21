#include "helper_functions.h"
#include "player_strategies.h"
#include "game_logic.h"

#include <iostream>
#include <fstream> // std::ifstream
#include <sstream> // std::istringstream
#include <string>
#include <vector>
#include <map>       // std::map
#include <ctime>     // std::time, std::srand
#include <algorithm> // std::find

// Constants (declared and initialized here)
int CARD_MAX_NUMBER;   // Maximum card value
int REVERSE_MOVE_DIFF; // Difference for a reverse-10 move
int CARD_IN_HANDS;     // Number of cards each player holds
int NUM_CARDS_TO_PLAY; // Number of cards to play per turn
int NUMBER_OF_ROWS;    // Number of playing rows
int NUMBER_OF_PLAYERS; // Number of players in the game
int NUM_SIMULATIONS;   // Number of games to simulate
int GOOD_MOVE_WINDOW;  // Internal for good moves

/**
 * @brief Main function to simulate and analyze the card game.
 *
 * This function reads configuration parameters from a file, sets up the game,
 * runs multiple simulations with different strategies, and outputs the results.
 *
 * @return 0 if the program executes successfully.
 */
int main(int argc, char** argv) // Corrected argv declaration
{
    std::string config_filename = "mpconfig.txt"; // Default config file name

    // Parse command-line arguments
    for (int i = 1; i < argc; ++i)
    {
        if (std::string(argv[i]) == "--config")
        { // Construct string with char*
            if (i + 1 < argc)
            {
                config_filename = argv[i + 1];
                i++;
            }
            else
            {
                std::cerr << "Error: Missing configuration file name after --config\n";
                return 1;
            }
        }
    }

    std::ifstream config_file(config_filename); // Open the configuration file

    std::string line; // Store each line read from the file

    // Read each line from the configuration file
    while (std::getline(config_file, line))
    {
        std::istringstream iss(line); // Create a stringstream from the line
        std::string variable_name;    // Store the variable name
        int variable_value;           // Store the variable value

        // Attempt to extract the variable name and value from the line
        if (iss >> variable_name >> variable_value)
        {
            // Assign the read value to the corresponding global variable
            if (variable_name == "CARD_MAX_NUMBER")
            {
                CARD_MAX_NUMBER = variable_value;
            }
            else if (variable_name == "REVERSE_MOVE_DIFF")
            {
                REVERSE_MOVE_DIFF = variable_value;
            }
            else if (variable_name == "CARD_IN_HANDS")
            {
                CARD_IN_HANDS = variable_value;
            }
            else if (variable_name == "NUM_CARDS_TO_PLAY")
            {
                NUM_CARDS_TO_PLAY = variable_value;
            }
            else if (variable_name == "NUMBER_OF_ROWS")
            {
                NUMBER_OF_ROWS = variable_value;
            }
            else if (variable_name == "NUMBER_OF_PLAYERS")
            {
                NUMBER_OF_PLAYERS = variable_value;
            }
            else if (variable_name == "NUM_SIMULATIONS")
            {
                NUM_SIMULATIONS = variable_value;
            }
            else if(variable_name == "GOOD_MOVE_WINDOW")
            {
                GOOD_MOVE_WINDOW = variable_value;
            }
            // Output the read variable and its value to the console
            std::cout << variable_name << ": " << variable_value << std::endl;
        }
    }
    config_file.close(); // Close the configuration file

    // --- 2. Setup Random Number Generator and Game Parameters ---
    std::srand(std::time(nullptr));              // Seed the random number generator
    int num_games_to_simulate = NUM_SIMULATIONS; // Number of games to simulate

    // --- 3. Define Player Strategies ---
    // Create a map to associate strategy names with their function pointers.
    //  IMPORTANT: The function pointer type now includes Communication and player_id.
    std::map<std::string, std::pair<int, int> (*)(const std::vector<int> &, const std::vector<std::vector<int>> &, const std::vector<Communication>&, int)> strategies;
    strategies["A1"] = [](const std::vector<int>& hand, const std::vector<std::vector<int>>& playing_rows, const std::vector<Communication>& comms, int player_id) {
        return get_player_move_A1(hand, playing_rows, comms, player_id);
    }; // Strategy A: Closest Card
    strategies["A2"] = [](const std::vector<int>& hand, const std::vector<std::vector<int>>& playing_rows, const std::vector<Communication>& comms, int player_id) {
        return get_player_move_A2(hand, playing_rows, comms, player_id);
    }; // Strategy A: Closest Card

    strategies["E1"] = [](const std::vector<int>& hand, const std::vector<std::vector<int>>& playing_rows, const std::vector<Communication>& comms, int player_id) {
        return get_player_move_E1(hand, playing_rows, comms, player_id);
    }; // Strategy E: Combination of C and A
    strategies["E2"] = [](const std::vector<int>& hand, const std::vector<std::vector<int>>& playing_rows, const std::vector<Communication>& comms, int player_id) {
        return get_player_move_E2(hand, playing_rows, comms, player_id);
    }; // Strategy E: Combination of C and A

    strategies["H1"] = [](const std::vector<int>& hand, const std::vector<std::vector<int>>& playing_rows, const std::vector<Communication>& comms, int player_id) {
        return get_player_move_H1(hand, playing_rows, comms, player_id);
    }; // Strategy H: Panic Mode
    strategies["H2"] = [](const std::vector<int>& hand, const std::vector<std::vector<int>>& playing_rows, const std::vector<Communication>& comms, int player_id) {
        return get_player_move_H2(hand, playing_rows, comms, player_id);
    }; // Strategy H: Panic Mode

    // strategies["A"] = get_player_move_A; // Strategy A: Closest Card
    // strategies["B"] = get_player_move_B; // Strategy B: Closest Card (No Reverse)
    // strategies["C"] = get_player_move_C; // Strategy C: Maximize Future Playability
    // strategies["D"] = get_player_move_D; // Strategy D: Prioritize Ascending Rows
    // strategies["E"] = get_player_move_E; // Strategy E: Combination of C and A
    // strategies["F"] = get_player_move_F; // Strategy F: Maximize Minimum Gap
    // strategies["G"] = get_player_move_G; // Strategy G: Weighted Combination of A, C, and F
    // strategies["H"] = get_player_move_H; // Strategy H: Panic Mode
    // strategies["I"] = get_player_move_I; // Strategy I: Minimize Blocking 1 and 100

    // --- 4. Structure to Store Game Results ---
    // Define a struct to hold the results of each simulated game
    struct GameResult
    {
        int num_players;                                  // Number of players in the game
        std::string shuffle_id;                           // Unique ID for the shuffled deck
        std::string strategy_name;                        // Name of the strategy used
        bool win;                                         // True if the strategy won, false otherwise
        int turns;                                        // Number of turns taken in the game
        std::vector<std::vector<int>> final_playing_rows; // Final state of the playing rows
        std::vector<std::vector<int>> final_hand;         // Final hands of the players
        int deck_size;                                    // Size of the deck at the end of the game (or 0 if won)
    };
    std::vector<GameResult> game_results; // Vector to store the results of all games

    // --- 5. Data Structures for Win Rates and Turn Counts ---
    // Create maps to store win counts and total turns for each strategy
    std::map<std::string, int> win_counts;
    std::map<std::string, int> total_turns;

    // Initialize win counts and total turns to 0 for each strategy
    for (auto const &[key, val] : strategies)
    {
        win_counts[key] = 0;
        total_turns[key] = 0;
    }

    int num_players = NUMBER_OF_PLAYERS; // Get the number of players from the config

    // --- 6. Simulate Games and Store Results ---
    std::vector<int> initial_deck = create_deck(); // Create the initial shuffled deck

    // Simulate multiple games
    for (int game = 0; game < num_games_to_simulate; ++game)
    {
        std::vector<int> game_deck = initial_deck;            // Copy the initial deck for this game
        shuffle(game_deck);                                   // Shuffle the deck
        std::string shuffle_id = generate_deck_id(game_deck); // Generate a unique ID

        // Iterate through each strategy
        for (auto const &[strategy_name, strategy_func] : strategies)
        {
            int turns = 0;                                    // Reset turn counter for each strategy
            std::vector<std::vector<int>> final_playing_rows; // Store final rows
            std::vector<std::vector<int>> final_hand;         // Store final hands
            // Simulate the game with the current strategy
            bool won = simulate_game_multiplayer(strategy_func, num_players, game_deck, turns, final_playing_rows, final_hand);

            // Store the results of the game
            GameResult result;
            result.num_players = num_players;
            result.shuffle_id = shuffle_id;
            result.strategy_name = strategy_name;
            result.win = won;
            result.turns = turns;
            result.final_playing_rows = final_playing_rows;
            result.final_hand = final_hand;
            result.deck_size = -1; // IMPROVE THIS
            game_results.push_back(result); // Add the result to the list of game results

            // If the game was won, update win counts and total turns
            if (won)
            {
                win_counts[strategy_name]++;
                total_turns[strategy_name] += turns;
            }
        }
        // Output progress to the console
        std::cout << "Completed simulation of game " << game << "\n";
    }

    // --- 7. Output Game Results ---
    // Print detailed results for each game
    for (const auto &result : game_results)
    {
        std::cout << "Game Results:\n";
        std::cout << "  Number of Players: " << result.num_players << "\n";
        std::cout << "  Shuffle ID: " << result.shuffle_id << "\n";
        std::cout << "  Strategy: " << result.strategy_name << "\n";
        std::cout << "  Win: " << (result.win ? "true" : "false") << "\n";
        std::cout << "  Turns: " << result.turns << "\n";
        std::cout << "  Deck Size: " << result.deck_size << "\n";

        // Print the final playing rows
        std::cout << "  Final Playing Rows:\n";
        for (int i = 0; i < NUMBER_OF_ROWS; ++i)
        {
            std::cout << "    " << (i < NUMBER_OF_ROWS / 2 ? "Ascending: " : "Descending: ");
            for (int card : result.final_playing_rows[i])
            {
                std::cout << card << " ";
            }
            std::cout << "\n";
        }

        // Print the final hands of players
        std::cout << "  Final Hands:\n";
        for (int i = 0; i < result.final_hand.size(); ++i)
        {
            std::cout << "    Player " << i + 1 << ": ";
            for (int card : result.final_hand[i])
            {
                std::cout << card << " ";
            }
            std::cout << "\n";
        }

        std::cout << std::endl;
    }

    // --- 8. Output Overall Win Rates ---
    // Calculate and print the win rate for each strategy
    for (auto const &[strategy_name, win_count] : win_counts)
    {
        double win_rate = (static_cast<double>(win_count) / num_games_to_simulate) * 100;
        double average_turns = (win_count > 0) ? static_cast<double>(total_turns[strategy_name]) / win_count : 0.0;

        std::cout << num_players << " Players: \n";
        std::cout << strategy_name << " win rate: " << win_rate << " %\n";
    }

    return 0; // Indicate successful execution
}

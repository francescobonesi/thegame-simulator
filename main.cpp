#include "helper_functions.h"
#include "player_strategies.h"
#include "game_logic.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <algorithm>

// Constants init var
int CARD_MAX_NUMBER;
int REVERSE_MOVE_DIFF;
int CARD_IN_HANDS;
int NUM_CARDS_TO_PLAY;
int NUMBER_OF_ROWS;
int NUMBER_OF_PLAYERS;
int NUM_SIMULATIONS;

int main()
{

    // Read variables from config file
    std::ifstream config_file("mpconfig.txt");
    std::string line;
    while (std::getline(config_file, line))
    {
        std::istringstream iss(line);
        std::string variable_name;
        int variable_value;
        if (iss >> variable_name >> variable_value)
        {
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
            std::cout << variable_name << ": " << variable_value << std::endl;
        }
    }
    config_file.close();

    std::srand(std::time(nullptr));
    int num_games_to_simulate = NUM_SIMULATIONS;

    std::map<std::string, std::pair<int, int> (*)(const std::vector<int> &, const std::vector<std::vector<int>> &)> strategies;
    strategies["A"] = get_player_move_A;
    strategies["B"] = get_player_move_B;
    strategies["C"] = get_player_move_C;
    strategies["D"] = get_player_move_D;
    strategies["E"] = get_player_move_E;
    strategies["F"] = get_player_move_F;
    strategies["G"] = get_player_move_G;
    strategies["H"] = get_player_move_H;
    strategies["I"] = get_player_move_I;

    struct GameResult
    {
        int num_players;
        std::string shuffle_id;
        std::string strategy_name;
        bool win;
        int turns;
        std::vector<std::vector<int>> final_playing_rows;
        std::vector<std::vector<int>> final_hand;
        int deck_size;
    };
    std::vector<GameResult> game_results;

    std::map<std::string, int> win_counts;
    std::map<std::string, int> total_turns;

    for (auto const &[key, val] : strategies)
    {
        win_counts[key] = 0;
        total_turns[key] = 0;
    }

    int num_players = NUMBER_OF_PLAYERS;

    std::vector<int> initial_deck = create_deck();

    for (int game = 0; game < num_games_to_simulate; ++game)
    {
        std::vector<int> game_deck = initial_deck;
        shuffle(game_deck);
        std::string shuffle_id = generate_deck_id(game_deck);

        for (auto const &[strategy_name, strategy_func] : strategies)
        {
            int turns = 0;
            std::vector<std::vector<int>> final_playing_rows;
            std::vector<std::vector<int>> final_hand;
            bool won = simulate_game_multiplayer(strategy_func, num_players, game_deck, turns, final_playing_rows, final_hand);

            GameResult result;
            result.num_players = num_players;
            result.shuffle_id = shuffle_id;
            result.strategy_name = strategy_name;
            result.win = won;
            result.turns = turns;
            result.final_playing_rows = final_playing_rows;
            result.final_hand = final_hand;
            result.deck_size = 0;
            if (!won)
            {
                std::vector<int> temp_deck = game_deck;
                for (int i = 0; i < CARD_IN_HANDS; ++i)
                    temp_deck.pop_back();
                for (auto &row : final_playing_rows)
                {
                    for (int card : row)
                    {
                        if (card > 1 && card < CARD_MAX_NUMBER)
                        {
                            auto it = std::find(temp_deck.begin(), temp_deck.end(), card);
                            if (it != temp_deck.end())
                            {
                                temp_deck.erase(it);
                            }
                        }
                    }
                }
                result.deck_size = temp_deck.size();
            }
            game_results.push_back(result);

            if (won)
            {
                win_counts[strategy_name]++;
                total_turns[strategy_name] += turns;
            }
        }
        std::cout << "Completed simulation of game " << game << "\n";
    }

    // Print game results
    for (const auto &result : game_results)
    {
        std::cout << "Game Results:\n";
        std::cout << "  Number of Players: " << result.num_players << "\n";
        std::cout << "  Shuffle ID: " << result.shuffle_id << "\n";
        std::cout << "  Strategy: " << result.strategy_name << "\n";
        std::cout << "  Win: " << (result.win ? "true" : "false") << "\n";
        std::cout << "  Turns: " << result.turns << "\n";
        std::cout << "  Deck Size: " << result.deck_size << "\n";

        // Print final playing rows
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

        // Print final hands of players
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

    // --- Console Output ---
    for (auto const &[strategy_name, win_count] : win_counts)
    {
        double win_rate = (static_cast<double>(win_count) / num_games_to_simulate) * 100;
        double average_turns = (win_count > 0) ? static_cast<double>(total_turns[strategy_name]) / win_count : 0.0;

        std::cout << num_players << " Players: \n";
        std::cout << strategy_name << " win rate: " << win_rate << " %\n";
    }

    return 0;
}
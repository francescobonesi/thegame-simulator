#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <vector>
#include <utility>
#include <string>

// Add forward declaration of Communication here
struct Communication;
struct Player; // Forward declare Player

bool check_win_condition_multiplayer(const std::vector<Player> &players, int deck_size);
bool simulate_game_multiplayer(std::pair<int, int> (*get_player_move)(const std::vector<int> &, const std::vector<std::vector<int>> &, const std::vector<Communication>&, int), int num_players, const std::vector<int> &initial_deck, int &turns_taken, std::vector<std::vector<int>> &final_playing_rows, std::vector<std::vector<int>> &final_hand);
std::string generate_deck_id(const std::vector<int> &deck);

#endif 
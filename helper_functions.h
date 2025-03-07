#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#include <vector>

struct Communication;


void shuffle(std::vector<int> &deck);
std::vector<int> create_deck();
std::vector<int> deal_cards(std::vector<int> &deck, int num_cards);
void display_game_state(const std::vector<std::vector<int>> &playing_rows, const std::vector<int> &hand, int deck_size);
enum class ValidMove {
    YES,
    REVERSE_MOVE,
    NO
};
ValidMove is_valid_move(int card, int row_top, bool is_ascending, bool reverse_move_allowed = true);
void make_move(int card, int row_index, std::vector<std::vector<int>> &playing_rows);
#endif
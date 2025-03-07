#ifndef PLAYER_STRATEGIES_H
#define PLAYER_STRATEGIES_H

#include <vector>

struct Communication {
    int player_id;
    int row_index;
    enum CommType {
        TARGET_ROW,
        GOOD_CARD,
        BAD_CARD,
        REVERSE_TRICK,
        ONLY_ONE_CARD
    } type;
    int relative_value; // -3 = very good, -2 = good, -1 = slightly good,
                        //  0 = neutral,
                        // +1 = slightly bad, +2 = bad, +3 = very bad
};

std::pair<int, int> get_player_move_A(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows, const std::vector<Communication>& communications, int player_id);
// std::pair<int, int> get_player_move_B(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows);
// std::pair<int, int> get_player_move_C(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows);
// std::pair<int, int> get_player_move_D(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows);
// std::pair<int, int> get_player_move_E(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows);
// std::pair<int, int> get_player_move_F(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows);
// std::pair<int, int> get_player_move_G(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows);
// std::pair<int, int> get_player_move_H(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows);
// std::pair<int, int> get_player_move_I(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows);

#endif
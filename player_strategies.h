#ifndef PLAYER_STRATEGIES_H
#define PLAYER_STRATEGIES_H

#include <vector>

std::pair<int, int> get_player_move_A(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows);
std::pair<int, int> get_player_move_B(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows);
std::pair<int, int> get_player_move_C(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows);
std::pair<int, int> get_player_move_D(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows);
std::pair<int, int> get_player_move_E(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows);
std::pair<int, int> get_player_move_F(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows);
std::pair<int, int> get_player_move_G(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows);
std::pair<int, int> get_player_move_H(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows);
std::pair<int, int> get_player_move_I(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows);

#endif
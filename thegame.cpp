#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <map>
#include <cmath> // For std::abs
#include <numeric> // For std::iota and std::accumulate
#include <sstream> // For std::stringstream
#include <fstream> // For file output

// Constants
const int CARD_MAX_NUMBER = 100;
const int REVERSE_MOVE_DIFF = 10;
const int CARD_IN_HANDS = 8;
const int NUM_CARDS_TO_PLAY = 2;
const int NUMBER_OF_ROWS = 4;
const int NUM_SIMULATIONS = 100;

enum class ValidMove {
    YES,
    REVERSE_MOVE,
    NO
};

// Helper Functions
void shuffle(std::vector<int>& deck) {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(deck.begin(), deck.end(), g);
}

std::vector<int> create_deck() {
    std::vector<int> deck;
    for (int i = 2; i < CARD_MAX_NUMBER; ++i) {
        deck.push_back(i);
    }
    shuffle(deck);
    return deck;
}

std::vector<int> deal_cards(std::vector<int>& deck, int num_cards = CARD_IN_HANDS) {
    std::vector<int> hand;
    for (int i = 0; i < num_cards; ++i) {
        hand.push_back(deck.back());
        deck.pop_back();
    }
    return hand;
}

void display_game_state(const std::vector<std::vector<int>>& playing_rows, const std::vector<int>& hand, int deck_size) {
    for (int i = 0; i < NUMBER_OF_ROWS; ++i) {
        std::cout << (i < NUMBER_OF_ROWS / 2? "Ascending: ": "Descending: ");
        for (int card: playing_rows[i]) {
            std::cout << card << " ";
        }
        std::cout << "->\n";
    }
    std::cout << "Your hand: ";
    for (int card: hand) {
        std::cout << card << " ";
    }
    std::cout << "\nDeck size: " << deck_size << "\n";
}

ValidMove is_valid_move(int card, int row_top, bool is_ascending, bool reverse_move_allowed = true) {
    if (is_ascending) {
        if (card == row_top - REVERSE_MOVE_DIFF && reverse_move_allowed) {
            return ValidMove::REVERSE_MOVE;
        } else if (card > row_top) {
            return ValidMove::YES;
        } else {
            return ValidMove::NO;
        }
    } else {
        if (card == row_top + REVERSE_MOVE_DIFF && reverse_move_allowed) {
            return ValidMove::REVERSE_MOVE;
        } else if (card < row_top) {
            return ValidMove::YES;
        } else {
            return ValidMove::NO;
        }
    }
}

void make_move(int card, int row_index, std::vector<std::vector<int>>& playing_rows) {
    playing_rows[row_index].push_back(card);
}

// Player Move Functions
std::pair<int, int> get_player_move_A(const std::vector<int>& hand, const std::vector<std::vector<int>>& playing_rows) {
    int best_card = -1;
    int best_row = -1;
    int min_diff = CARD_MAX_NUMBER * 2;

    for (int i = 0; i < hand.size(); ++i) {
        for (int j = 0; j < NUMBER_OF_ROWS; ++j) {
            ValidMove valid_move = is_valid_move(hand[i], playing_rows[j].back(), j < NUMBER_OF_ROWS / 2);
            if (valid_move!= ValidMove::NO) {
                int diff = (valid_move == ValidMove::REVERSE_MOVE)? -1: std::abs(hand[i] - playing_rows[j].back());
                if (diff < min_diff) {
                    min_diff = diff;
                    best_card = hand[i];
                    best_row = j;
                }
            }
        }
    }
    return {best_card, best_row};
}

//... (get_player_move_B and get_player_move_C - similar structure)

std::pair<int, int> get_player_move_B(const std::vector<int>& hand, const std::vector<std::vector<int>>& playing_rows) {
    int best_card = -1;
    int best_row = -1;
    int min_diff = CARD_MAX_NUMBER * 2;

    for (int i = 0; i < hand.size(); ++i) {
        for (int j = 0; j < NUMBER_OF_ROWS; ++j) {
            ValidMove valid_move = is_valid_move(hand[i], playing_rows[j].back(), j < NUMBER_OF_ROWS / 2, false);
            if (valid_move!= ValidMove::NO) {
                int diff = std::abs(hand[i] - playing_rows[j].back());
                if (diff < min_diff) {
                    min_diff = diff;
                    best_card = hand[i];
                    best_row = j;
                }
            }
        }
    }
    return {best_card, best_row};
}

std::pair<int, int> get_player_move_C(const std::vector<int>& hand, const std::vector<std::vector<int>>& playing_rows) {
    int best_card = -1;
    int best_row = -1;
    int max_playable_after = -1;

    for (int i = 0; i < hand.size(); ++i) {
        for (int j = 0; j < NUMBER_OF_ROWS; ++j) {
            if (is_valid_move(hand[i], playing_rows[j].back(), j < NUMBER_OF_ROWS / 2) != ValidMove::NO) {
                // Simulate the move (create copies)
                std::vector<std::vector<int>> temp_rows = playing_rows;
                temp_rows[j].push_back(hand[i]);

                int playable_after = 0;
                for (int k = 0; k < hand.size(); ++k) {
                    if (hand[k]!= hand[i]) {
                        for (int l = 0; l < NUMBER_OF_ROWS; ++l) {
                            if (is_valid_move(hand[k], temp_rows[l].back(), l < NUMBER_OF_ROWS / 2) != ValidMove::NO) {
                                playable_after++;
                                break;
                            }
                        }
                    }
                }

                if (playable_after > max_playable_after) {
                    max_playable_after = playable_after;
                    best_card = hand[i];
                    best_row = j;
                }
            }
        }
    }
    return {best_card, best_row};
}

// Strategy D:  Prioritize ascending rows, and minimize the maximum card in hand.
std::pair<int, int> get_player_move_D(const std::vector<int>& hand, const std::vector<std::vector<int>>& playing_rows) {
    int best_card = -1;
    int best_row = -1;
    int min_max_card = CARD_MAX_NUMBER + 1;  // Initialize with a value larger than any possible card.

    for (int i = 0; i < hand.size(); ++i) {
        for (int j = 0; j < NUMBER_OF_ROWS; ++j) {
            // Prioritize ascending rows (first half of playing_rows).
            if (j < NUMBER_OF_ROWS / 2 && is_valid_move(hand[i], playing_rows[j].back(), true) != ValidMove::NO) {
                // Simulate the move and find max card in the remaining hand.
                std::vector<int> temp_hand = hand;
                temp_hand.erase(temp_hand.begin() + i);  // Erase by index, not value
                
                int max_card = 0;
                if (!temp_hand.empty()) {
                   max_card = *std::max_element(temp_hand.begin(), temp_hand.end());
                }
                

                if (max_card < min_max_card) {
                    min_max_card = max_card;
                    best_card = hand[i];
                    best_row = j;
                }
            }
        }
    }

    // If no move found in ascending rows, try descending rows using strategy A (closest card, with reverse).
    if (best_card == -1) {
        return get_player_move_A(hand, playing_rows);
    }

    return {best_card, best_row};
}

// Strategy E: Combination of C and A.  Consider playability, but if tied, use closest card.
std::pair<int, int> get_player_move_E(const std::vector<int>& hand, const std::vector<std::vector<int>>& playing_rows) {
    int best_card = -1;
    int best_row = -1;
    int max_playable_after = -1;
    int min_diff = CARD_MAX_NUMBER * 2; // For tie-breaking

    for (int i = 0; i < hand.size(); ++i) {
        for (int j = 0; j < NUMBER_OF_ROWS; ++j) {
            if (is_valid_move(hand[i], playing_rows[j].back(), j < NUMBER_OF_ROWS / 2) != ValidMove::NO) {
                std::vector<std::vector<int>> temp_rows = playing_rows;
                temp_rows[j].push_back(hand[i]);

                int playable_after = 0;
                for (int k = 0; k < hand.size(); ++k) {
                     if (static_cast<unsigned long long>(i) != k) {
                        for (int l = 0; l < NUMBER_OF_ROWS; ++l) {
                            if (is_valid_move(hand[k], temp_rows[l].back(), l < NUMBER_OF_ROWS / 2) != ValidMove::NO) {
                                playable_after++;
                                break;
                            }
                        }
                    }
                }

                int diff = std::abs(hand[i] - playing_rows[j].back());
                if (is_valid_move(hand[i], playing_rows[j].back(), j < NUMBER_OF_ROWS/2) == ValidMove::REVERSE_MOVE) diff = -1;

                // Tie-breaker logic: If playable_after is the same, choose the smaller diff.
                if (playable_after > max_playable_after) {
                    max_playable_after = playable_after;
                    min_diff = diff;
                    best_card = hand[i];
                    best_row = j;
                } else if (playable_after == max_playable_after && diff < min_diff) {
                    min_diff = diff;
                    best_card = hand[i];
                    best_row = j;
                }
            }
        }
    }
    return {best_card, best_row};
}


// Strategy F:  Avoid filling rows too quickly.  Try to keep gaps large.
std::pair<int, int> get_player_move_F(const std::vector<int>& hand, const std::vector<std::vector<int>>& playing_rows) {
    int best_card = -1;
    int best_row = -1;
    int max_min_gap = -1; // Maximize the *minimum* gap.

    for (int i = 0; i < hand.size(); ++i) {
        for (int j = 0; j < NUMBER_OF_ROWS; ++j) {
            if (is_valid_move(hand[i], playing_rows[j].back(), j < NUMBER_OF_ROWS / 2) != ValidMove::NO) {
                std::vector<std::vector<int>> temp_rows = playing_rows;
                temp_rows[j].push_back(hand[i]);

                // Calculate the minimum gap in *all* rows after the move.
                int min_gap = CARD_MAX_NUMBER * 2;
                for (int row = 0; row < NUMBER_OF_ROWS; ++row) {
                    int current_card = temp_rows[row].back();
                    for(int next_card: hand){
                        if(next_card != hand[i]){
                            ValidMove vm = is_valid_move(next_card, current_card, row < NUMBER_OF_ROWS/2);
                            if(vm != ValidMove::NO){
                                int gap;
                                if(vm == ValidMove::REVERSE_MOVE){
                                    gap = -1;
                                } else {
                                    gap = std::abs(next_card - current_card);
                                }
                                min_gap = std::min(min_gap, gap);
                            }
                        }
                    }

                }
                if (min_gap > max_min_gap) {
                    max_min_gap = min_gap;
                    best_card = hand[i];
                    best_row = j;
                }

            }
        }
    }
    return {best_card, best_row};
}

// Strategy G:  Weighted Combination of A, C, and F
std::pair<int, int> get_player_move_G(const std::vector<int>& hand, const std::vector<std::vector<int>>& playing_rows) {
    const double weight_A = 0.2;  // Weight for Strategy A (closeness)
    const double weight_C = 0.5;  // Weight for Strategy C (future playability)
    const double weight_F = 0.3;  // Weight for Strategy F (gap maximization)

    int best_card = -1;
    int best_row = -1;
    double best_score = -1e9; // Initialize with a very low score (negative infinity, practically)

    for (int i = 0; i < hand.size(); ++i) {
        for (int j = 0; j < NUMBER_OF_ROWS; ++j) {
            if (is_valid_move(hand[i], playing_rows[j].back(), j < NUMBER_OF_ROWS / 2) != ValidMove::NO) {
                double score_A = 0.0, score_C = 0.0, score_F = 0.0;

                // --- Score for Strategy A (Closeness) ---
                int diff = std::abs(hand[i] - playing_rows[j].back());
                // Treat reverse-10 moves as a very small difference (better than any other move)
                if (is_valid_move(hand[i], playing_rows[j].back(), j < NUMBER_OF_ROWS / 2) == ValidMove::REVERSE_MOVE) {
                    diff = -1;
                }
                // Invert the difference so that smaller differences get higher scores.
                // A difference of -1 (reverse-10) gets the highest possible score.
                score_A = (diff == -1) ? CARD_MAX_NUMBER : static_cast<double>(CARD_MAX_NUMBER - diff);

                // --- Score for Strategy C (Future Playability) ---
                std::vector<std::vector<int>> temp_rows = playing_rows; // Copy playing_rows
                temp_rows[j].push_back(hand[i]); // Simulate the move

                int playable_after = 0;
                for (int k = 0; k < hand.size(); ++k) {
                    if (static_cast<unsigned long long>(i) != k) { // Don't check the card we just "played"
                        for (int l = 0; l < NUMBER_OF_ROWS; ++l) {
                            if (is_valid_move(hand[k], temp_rows[l].back(), l < NUMBER_OF_ROWS / 2) != ValidMove::NO) {
                                playable_after++;
                                break; // Only count a card once per row
                            }
                        }
                    }
                }
                score_C = static_cast<double>(playable_after);

                // --- Score for Strategy F (Gap Maximization) ---
                std::vector<std::vector<int>> temp_rows2 = playing_rows; // Another copy for F
                temp_rows2[j].push_back(hand[i]);

                // Calculate the minimum gap in *all* rows after the simulated move.
                int min_gap = CARD_MAX_NUMBER * 2; // Initialize with a large gap
                for (int row = 0; row < NUMBER_OF_ROWS; ++row) {
                    int current_card = temp_rows2[row].back();
                    for (int next_card : hand) {
                        if (next_card != hand[i]) { // Don't compare the played card with itself
                            ValidMove vm = is_valid_move(next_card, current_card, row < NUMBER_OF_ROWS / 2);
                            if (vm != ValidMove::NO) {
                                int gap;
                                if (vm == ValidMove::REVERSE_MOVE) {
                                    gap = -1; // Treat reverse-10 as a minimal gap
                                } else {
                                    gap = std::abs(next_card - current_card);
                                }
                                min_gap = std::min(min_gap, gap); // Find the *smallest* gap
                            }
                        }
                    }
                }
                score_F = static_cast<double>(min_gap); // Larger minimum gap is better


                // --- Combine Scores using Weights ---
                double total_score = weight_A * score_A + weight_C * score_C + weight_F * score_F;

                // Update best move if this move has a higher combined score
                if (total_score > best_score) {
                    best_score = total_score;
                    best_card = hand[i];
                    best_row = j;
                }
            }
        }
    }
    return {best_card, best_row};
}

// Strategy H:  "Panic Mode" - If few moves left, play largest/smallest possible card
std::pair<int, int> get_player_move_H(const std::vector<int>& hand, const std::vector<std::vector<int>>& playing_rows) {
    int total_valid_moves = 0;
    for (int card : hand) {
        for (int j = 0; j < NUMBER_OF_ROWS; ++j) {
            if (is_valid_move(card, playing_rows[j].back(), j < NUMBER_OF_ROWS / 2) != ValidMove::NO) {
                total_valid_moves++;
            }
        }
    }

    // If very few moves are left, try to force a play, even if it's not ideal.
    if (total_valid_moves <= 2) {
        int best_card = -1;
        int best_row = -1;

        // Try to play the *largest* possible card on an ascending row,
        // or the *smallest* possible card on a descending row.
        for (int i = 0; i < hand.size(); ++i) {
            for (int j = 0; j < NUMBER_OF_ROWS; ++j) {
                if (is_valid_move(hand[i], playing_rows[j].back(), j < NUMBER_OF_ROWS / 2) != ValidMove::NO) {
                    if (j < NUMBER_OF_ROWS / 2) { // Ascending
                        if (best_card == -1 || hand[i] > best_card) {
                            best_card = hand[i];
                            best_row = j;
                        }
                    } else { // Descending
                        if (best_card == -1 || hand[i] < best_card) {
                            best_card = hand[i];
                            best_row = j;
                        }
                    }
                }
            }
        }
        if(best_card != -1) return {best_card, best_row}; // Return if a move was forced
    }

    // Otherwise, default to Strategy E (a good general-purpose strategy).
    return get_player_move_E(hand, playing_rows);
}

// Strategy I: Minimize cards blocking 1 and 100 at start.
std::pair<int, int> get_player_move_I(const std::vector<int>& hand, const std::vector<std::vector<int>>& playing_rows){
     // Check how many cards on hand blocks 1 and 100.
    int cards_blocking_top_rows = 0;
    int cards_blocking_bottom_rows = 0;
    for (int card : hand) {
        // Check how many cards are blocking the ascending rows.
        if(card < hand[0]){
            cards_blocking_top_rows += 1;
        }
        // Check how many cards are blocking the descending rows.
        if(card > hand[0]){
            cards_blocking_bottom_rows += 1;
        }
    }
    //If we are blocking more the top, use get_player_move_D, else use a new strategy for descending rows.
    if(cards_blocking_top_rows >= cards_blocking_bottom_rows){
        return get_player_move_D(hand, playing_rows);
    } else {
      //Create strategy that focus on playing descending cards:

        int best_card = -1;
        int best_row = -1;
        int max_max_card = 0;  // Initialize with a value lower than any possible card.

        for (int i = 0; i < hand.size(); ++i) {
            for (int j = 0; j < NUMBER_OF_ROWS; ++j) {
                // Prioritize descending rows (first half of playing_rows).
                if (j >= NUMBER_OF_ROWS / 2 && is_valid_move(hand[i], playing_rows[j].back(), false) != ValidMove::NO) {
                    // Simulate the move and find max card in the remaining hand.
                    std::vector<int> temp_hand = hand;
                    temp_hand.erase(temp_hand.begin() + i);

                    int min_card = CARD_MAX_NUMBER;
                    if (!temp_hand.empty()) {
                    min_card = *std::min_element(temp_hand.begin(), temp_hand.end());
                    }
                    

                    if (min_card > max_max_card) {
                        max_max_card = min_card;
                        best_card = hand[i];
                        best_row = j;
                    }
                }
            }
        }
        if (best_card == -1) {
            return get_player_move_A(hand, playing_rows); // Fallback
        }

        return {best_card, best_row};

    }
}


bool check_win_condition(const std::vector<int>& hand, int deck_size) {
    return hand.empty() && deck_size == 0;
}

bool check_lose_condition(const std::vector<int>& hand, const std::vector<std::vector<int>>& playing_rows, int deck_size) {
    if (deck_size > 0 ||!hand.empty()) {
        for (int card: hand) {
            for (int i = 0; i < NUMBER_OF_ROWS; ++i) {
                if (is_valid_move(card, playing_rows[i].back(), i < NUMBER_OF_ROWS / 2)!= ValidMove::NO) {
                    return false; // At least one valid move exists
                }
            }
        }
        return true; // No valid moves
    }
    return false; // Game can continue
}

// Function to generate a unique ID for a shuffled deck
std::string generate_deck_id(const std::vector<int>& deck) {
    std::stringstream ss;
    for (int card : deck) {
        ss << card << "_"; // Use a delimiter to separate card values
    }
    return ss.str();
}
bool simulate_game(std::pair<int, int> (*get_player_move)(const std::vector<int>&, const std::vector<std::vector<int>>&), const std::vector<int>& initial_deck, int& turns_taken, std::vector<std::vector<int>>& final_playing_rows, std::vector<int>& final_hand) {
    std::vector<int> deck = initial_deck; // Use the pre-shuffled deck
    std::vector<int> hand = deal_cards(deck);
    int deck_size = deck.size();

    std::vector<std::vector<int>> playing_rows(NUMBER_OF_ROWS);
    for (int i = 0; i < NUMBER_OF_ROWS; ++i) {
        playing_rows[i].push_back(i < NUMBER_OF_ROWS / 2 ? 1 : CARD_MAX_NUMBER);
    }

    int turns = 0;
    while (!check_win_condition(hand, deck_size) && !check_lose_condition(hand, playing_rows, deck_size)) {
        int num_cards_to_play = (hand.size() == CARD_IN_HANDS) ? NUM_CARDS_TO_PLAY : 1;
        std::pair<int, int> move;

        for (int k = 0; k < num_cards_to_play; ++k) {
            move = get_player_move(hand, playing_rows);
            int card_to_play = move.first;
            int row_index = move.second;

            if (card_to_play != -1) {
                make_move(card_to_play, row_index, playing_rows);
                hand.erase(std::remove(hand.begin(), hand.end(), card_to_play), hand.end());
                turns++; // Increment turns for each card played

                if (k == num_cards_to_play - 1) {
                    for (int _ = 0; _ < num_cards_to_play; ++_) {
                        if (deck_size > 0) {
                            hand.push_back(deck.back());
                            deck.pop_back();
                            deck_size--;
                        }
                    }
                }
            } else {
                break;  // No valid move for this card
            }
        }
          if (move.first == -1) { //If no valid move
              break;
          }
    }
    turns_taken = turns;
    final_playing_rows = playing_rows; // Store the final state
    final_hand = hand; // Store the final hand
    return check_win_condition(hand, deck_size);
}

int main() {
    std::srand(std::time(nullptr));
    int num_games_to_simulate = NUM_SIMULATIONS;

    std::map<std::string, std::pair<int, int> (*)(const std::vector<int>&, const std::vector<std::vector<int>>&)> strategies;
    strategies["A"] = get_player_move_A;
    strategies["B"] = get_player_move_B;
    strategies["C"] = get_player_move_C;
    strategies["D"] = get_player_move_D;
    strategies["E"] = get_player_move_E;
    strategies["F"] = get_player_move_F;
    strategies["G"] = get_player_move_G;
    strategies["H"] = get_player_move_H;
    strategies["I"] = get_player_move_I;

    struct GameResult {
        std::string shuffle_id;
        std::string strategy_name;
        bool win;
        int turns;
        std::vector<std::vector<int>> final_playing_rows;
        std::vector<int> final_hand;
        int deck_size;
    };
    std::vector<GameResult> game_results;

    std::map<std::string, int> win_counts;
    std::map<std::string, int> total_turns;

    for(auto const& [key, val] : strategies) {
        win_counts[key] = 0;
        total_turns[key] = 0;
    }

    std::vector<int> initial_deck = create_deck();

    for (int game = 0; game < num_games_to_simulate; ++game) {
        std::vector<int> game_deck = initial_deck;
        shuffle(game_deck);
        std::string shuffle_id = generate_deck_id(game_deck);

        for (auto const& [strategy_name, strategy_func] : strategies) {
            int turns = 0;
            std::vector<std::vector<int>> final_playing_rows;
            std::vector<int> final_hand;
            bool won = simulate_game(strategy_func, game_deck, turns, final_playing_rows, final_hand);

            GameResult result;
            result.shuffle_id = shuffle_id;
            result.strategy_name = strategy_name;
            result.win = won;
            result.turns = turns;
            result.final_playing_rows = final_playing_rows;
            result.final_hand = final_hand;
            result.deck_size = 0;
                if (!won) {
                    std::vector<int> temp_deck = game_deck;
                    for(int i =0; i < CARD_IN_HANDS; ++i) temp_deck.pop_back();
                    for(auto& row: final_playing_rows){
                        for(int card: row){
                            if(card > 1 && card < CARD_MAX_NUMBER){
                                auto it = std::find(temp_deck.begin(), temp_deck.end(), card);
                                if (it != temp_deck.end()) {
                                        temp_deck.erase(it);
                                    }

                            }
                        }
                    }
                  result.deck_size = temp_deck.size();
                }
            game_results.push_back(result);

            if (won) {
                win_counts[strategy_name]++;
                total_turns[strategy_name] += turns;
            }
        }
    }

    // --- JSON Output ---
    std::ofstream json_file("game_results.json");
    if (json_file.is_open()) {
        json_file << "{\n";
        json_file << "  \"overall_stats\": {\n";

        // Overall win rates and average turns
        bool first_stat = true;
        for (auto const& [strategy_name, win_count] : win_counts) {
            double win_rate = (static_cast<double>(win_count) / num_games_to_simulate) * 100;
            double average_turns = (win_count > 0) ? static_cast<double>(total_turns[strategy_name]) / win_count : 0.0;

            if (!first_stat) {
                json_file << ",\n";
            }
            first_stat = false;

            json_file << "    \"" << strategy_name << "\": {\n";
            json_file << "      \"win_rate\": " << win_rate << ",\n";
            json_file << "      \"average_turns\": " << average_turns << "\n";
            json_file << "    }";
        }
        json_file << "\n  },\n";

        // Individual game results
        json_file << "  \"game_results\": [\n";
        bool first_game = true;
        for (const GameResult& result : game_results) {
            if (!first_game) {
                json_file << ",\n";
            }
            first_game = false;

            json_file << "    {\n";
            json_file << "      \"shuffle_id\": \"" << result.shuffle_id << "\",\n";
            json_file << "      \"strategy_name\": \"" << result.strategy_name << "\",\n";
            json_file << "      \"win\": " << (result.win ? "true" : "false") << ",\n";
            json_file << "      \"turns\": " << result.turns << ",\n";
            json_file << "      \"deck_size\": " << result.deck_size << ",\n"; // Output deck_size

            // Final playing rows
            json_file << "      \"final_playing_rows\": [\n";
            for (size_t i = 0; i < result.final_playing_rows.size(); ++i) {
                json_file << "        [";
                for (size_t j = 0; j < result.final_playing_rows[i].size(); ++j) {
                    json_file << result.final_playing_rows[i][j];
                    if (j < result.final_playing_rows[i].size() - 1) {
                        json_file << ", ";
                    }
                }
                json_file << "]";
                if (i < result.final_playing_rows.size() - 1) {
                    json_file << ",\n";
                } else {
                    json_file << "\n";
                }
            }
            json_file << "      ],\n";

            // Final hand
            json_file << "      \"final_hand\": [";
            for (size_t i = 0; i < result.final_hand.size(); ++i) {
                json_file << result.final_hand[i];
                if (i < result.final_hand.size() - 1) {
                    json_file << ", ";
                }
            }
            json_file << "]\n";
            json_file << "    }";
        }
        json_file << "\n  ]\n";
        json_file << "}\n";
        json_file.close();
        std::cout << "Game results written to game_results.json\n";
    } else {
        std::cerr << "Unable to open file for writing.\n";
    }

    return 0;
}

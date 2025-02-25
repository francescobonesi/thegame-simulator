#include "player_strategies.h"
#include "helper_functions.h"
#include <cmath>
#include <algorithm>

// Constants
extern int CARD_MAX_NUMBER;
extern int REVERSE_MOVE_DIFF;
extern int NUMBER_OF_ROWS;


std::pair<int, int> get_player_move_A(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows)
{
    int best_card = -1;
    int best_row = -1;
    int min_diff = CARD_MAX_NUMBER * 2;

    for (int i = 0; i < hand.size(); ++i)
    {
        for (int j = 0; j < NUMBER_OF_ROWS; ++j)
        {
            ValidMove valid_move = is_valid_move(hand[i], playing_rows[j].back(), j < NUMBER_OF_ROWS / 2);
            if (valid_move != ValidMove::NO)
            {
                int diff = (valid_move == ValidMove::REVERSE_MOVE) ? -1 : std::abs(hand[i] - playing_rows[j].back());
                if (diff < min_diff)
                {
                    min_diff = diff;
                    best_card = hand[i];
                    best_row = j;
                }
            }
        }
    }
    return {best_card, best_row};
}

std::pair<int, int> get_player_move_B(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows)
{
    int best_card = -1;
    int best_row = -1;
    int min_diff = CARD_MAX_NUMBER * 2;

    for (int i = 0; i < hand.size(); ++i)
    {
        for (int j = 0; j < NUMBER_OF_ROWS; ++j)
        {
            ValidMove valid_move = is_valid_move(hand[i], playing_rows[j].back(), j < NUMBER_OF_ROWS / 2, false);
            if (valid_move != ValidMove::NO)
            {
                int diff = std::abs(hand[i] - playing_rows[j].back());
                if (diff < min_diff)
                {
                    min_diff = diff;
                    best_card = hand[i];
                    best_row = j;
                }
            }
        }
    }
    return {best_card, best_row};
}

std::pair<int, int> get_player_move_C(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows)
{
    int best_card = -1;
    int best_row = -1;
    int max_playable_after = -1;

    for (int i = 0; i < hand.size(); ++i)
    {
        for (int j = 0; j < NUMBER_OF_ROWS; ++j)
        {
            if (is_valid_move(hand[i], playing_rows[j].back(), j < NUMBER_OF_ROWS / 2) != ValidMove::NO)
            {
                // Simulate the move (create copies)
                std::vector<std::vector<int>> temp_rows = playing_rows;
                temp_rows[j].push_back(hand[i]);

                int playable_after = 0;
                for (int k = 0; k < hand.size(); ++k)
                {
                    if (hand[k] != hand[i])
                    {
                        for (int l = 0; l < NUMBER_OF_ROWS; ++l)
                        {
                            if (is_valid_move(hand[k], temp_rows[l].back(), l < NUMBER_OF_ROWS / 2) != ValidMove::NO)
                            {
                                playable_after++;
                                break;
                            }
                        }
                    }
                }

                if (playable_after > max_playable_after)
                {
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
std::pair<int, int> get_player_move_D(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows)
{
    int best_card = -1;
    int best_row = -1;
    int min_max_card = CARD_MAX_NUMBER + 1; // Initialize with a value larger than any possible card.

    for (int i = 0; i < hand.size(); ++i)
    {
        for (int j = 0; j < NUMBER_OF_ROWS; ++j)
        {
            // Prioritize ascending rows (first half of playing_rows).
            if (j < NUMBER_OF_ROWS / 2 && is_valid_move(hand[i], playing_rows[j].back(), true) != ValidMove::NO)
            {
                // Simulate the move and find max card in the remaining hand.
                std::vector<int> temp_hand = hand;
                temp_hand.erase(temp_hand.begin() + i); // Erase by index, not value

                int max_card = 0;
                if (!temp_hand.empty())
                {
                    max_card = *std::max_element(temp_hand.begin(), temp_hand.end());
                }

                if (max_card < min_max_card)
                {
                    min_max_card = max_card;
                    best_card = hand[i];
                    best_row = j;
                }
            }
        }
    }

    // If no move found in ascending rows, try descending rows using strategy A (closest card, with reverse).
    if (best_card == -1)
    {
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

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <map>
#include <cmath>   
#include <numeric> 
#include <sstream> 
#include <fstream> 

// Constants init var
int CARD_MAX_NUMBER;
int REVERSE_MOVE_DIFF;
int CARD_IN_HANDS;
int NUM_CARDS_TO_PLAY;
int NUMBER_OF_ROWS;
int NUMBER_OF_PLAYERS;
int NUM_SIMULATIONS;

enum class ValidMove
{
    YES,
    REVERSE_MOVE,
    NO
};

// Helper Functions
void shuffle(std::vector<int> &deck)
{
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(deck.begin(), deck.end(), g);
}

std::vector<int> create_deck()
{
    std::vector<int> deck;
    for (int i = 2; i < CARD_MAX_NUMBER; ++i)
    {
        deck.push_back(i);
    }
    shuffle(deck);

    // Print the deck
    // std::cout << "Created deck: ";
    // for (int card: deck) {
    //     std::cout << card << " ";
    // }
    // std::cout << std::endl;

    return deck;
}

std::vector<int> deal_cards(std::vector<int> &deck, int num_cards = CARD_IN_HANDS)
{
    std::vector<int> hand;
    for (int i = 0; i < num_cards && !deck.empty(); ++i)
    {
        hand.push_back(deck.back());
        deck.pop_back();
    }
    return hand;
}

void display_game_state(const std::vector<std::vector<int>> &playing_rows, const std::vector<int> &hand, int deck_size)
{
    for (int i = 0; i < NUMBER_OF_ROWS; ++i)
    {
        std::cout << (i < NUMBER_OF_ROWS / 2 ? "Ascending: " : "Descending: ");
        for (int card : playing_rows[i])
        {
            std::cout << card << " ";
        }
        std::cout << "->\n";
    }
    std::cout << "Your hand: ";
    for (int card : hand)
    {
        std::cout << card << " ";
    }
    std::cout << "\nDeck size: " << deck_size << "\n";
}

ValidMove is_valid_move(int card, int row_top, bool is_ascending, bool reverse_move_allowed = true)
{
    if (is_ascending)
    {
        if (card == row_top - REVERSE_MOVE_DIFF && reverse_move_allowed)
        {
            return ValidMove::REVERSE_MOVE;
        }
        else if (card > row_top)
        {
            return ValidMove::YES;
        }
        else
        {
            return ValidMove::NO;
        }
    }
    else
    {
        if (card == row_top + REVERSE_MOVE_DIFF && reverse_move_allowed)
        {
            return ValidMove::REVERSE_MOVE;
        }
        else if (card < row_top)
        {
            return ValidMove::YES;
        }
        else
        {
            return ValidMove::NO;
        }
    }
}

void make_move(int card, int row_index, std::vector<std::vector<int>> &playing_rows)
{
    playing_rows[row_index].push_back(card);
}

// Player Move Functions
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

// Player struct
struct Player
{
    std::vector<int> hand;
    bool active = true; // To track if the player is still in the game
};

bool check_win_condition_multiplayer(const std::vector<Player> &players, int deck_size)
{
    for (const auto &player : players)
    {
        if (!player.hand.empty() || deck_size > 0)
        {
            return false;
        }
    }
    return true;
}

bool simulate_game_multiplayer(std::pair<int, int> (*get_player_move)(const std::vector<int>&, const std::vector<std::vector<int>>&), int num_players, const std::vector<int> &initial_deck, int &turns_taken)
{
    std::vector<int> deck = initial_deck;
    std::vector<Player> players(num_players);

    // Deal cards to players
    for (auto &player : players){
        player.hand = deal_cards(deck);
    }

    int deck_size = deck.size(); // Correct deck_size after dealing

    std::vector<std::vector<int>> playing_rows(NUMBER_OF_ROWS);
    for (int i = 0; i < NUMBER_OF_ROWS; ++i)
    {
        playing_rows[i].push_back(i < NUMBER_OF_ROWS / 2 ? 1 : CARD_MAX_NUMBER);
    }

    // Shuffle player order
    std::vector<int> player_order(num_players);
    std::iota(player_order.begin(), player_order.end(), 0);
    shuffle(player_order);

    int current_player_index = 0;
    int turns = 0;

    std::vector<int> drawn_cards; // To store the cards drawn in each turn
    std::vector<int> played_cards; // To store the cards played in each turn

    while (true)
    {
        Player &current_player = players[player_order[current_player_index]];
        if (!current_player.active)
        { // Skip inactive players
            current_player_index = (current_player_index + 1) % num_players;
            continue;
        }

        int num_cards_to_play = std::min(NUM_CARDS_TO_PLAY, (deck_size > 0) ? NUM_CARDS_TO_PLAY : 1);
        bool valid_turn = true; // Flag to check if the player can make a valid move
        played_cards.clear(); // Clear the played cards from the previous turn
        drawn_cards.clear(); // Clear the drawn cards from the previous turn

        for (int k = 0; k < num_cards_to_play; ++k)
        {
            auto move = get_player_move(current_player.hand, playing_rows);
            int card_to_play = move.first;
            int row_index = move.second;

            if (card_to_play != -1)
            {
                make_move(card_to_play, row_index, playing_rows);
                played_cards.push_back(card_to_play); // Store the played card
                current_player.hand.erase(std::remove(current_player.hand.begin(), current_player.hand.end(), card_to_play), current_player.hand.end());
                turns++;
            }
            else
            {
                // Cannot complete the required number of moves
                valid_turn = false;
            }
        }

        if (!valid_turn)
        {
            break; // Game lost
        }

        // Print played cards, deck cards, and drawn cards after each turn
        // std::cout << "Played cards: ";
        // for (int card: played_cards) {
        //     std::cout << card << " ";
        // }
        // std::cout << std::endl;

        // Print deck cards after each turn
        // std::cout << "Deck cards: ";
        // for (int card: deck) {
        //     std::cout << card << " ";
        // }
        // std::cout << std::endl;

        // Replenish hand (only if the deck is not empty)
        while (current_player.hand.size() < CARD_IN_HANDS && deck_size > 0)
        { // Add this check
            current_player.hand.push_back(deck.back());
            drawn_cards.push_back(deck.back()); // Store the drawn card
            deck.pop_back();
            deck_size--;
        }

        // std::cout << "Drawn cards: ";
        // for (int card: drawn_cards) {
        //     std::cout << card << " ";
        // }
        // std::cout << std::endl;

        // Print deck cards after each turn
        // std::cout << "Deck cards: ";
        // for (int card: deck) {
        //     std::cout << card << " ";
        // }
        // std::cout << std::endl;

        // Check if the player has no cards left and deactivate
        if (current_player.hand.empty() && deck_size == 0)
        {
            current_player.active = false;
        }

        current_player_index = (current_player_index + 1) % num_players;

        // Display game state after each player's turn
        // std::cout << "---- Player " << player_order[current_player_index] + 1 << " Turn ----\n"; // Assuming players are numbered from 1
        // display_game_state(playing_rows, current_player.hand, deck_size);

        // Check win condition
        bool all_players_done = true;
        for (const auto &player : players)
        {
            if (player.active)
            {
                all_players_done = false;
                break;
            }
        }
        if (all_players_done)
        {
            break; // Game won
        }
    }

    turns_taken = turns;
    return check_win_condition_multiplayer(players, deck_size);
}

// Function to generate a unique ID for a shuffled deck
std::string generate_deck_id(const std::vector<int> &deck)
{
    std::stringstream ss;
    for (int card : deck)
    {
        ss << card << "_"; // Use a delimiter to separate card values
    }
    return ss.str();
}

int main()
{

    // Read variables from config file
    std::ifstream config_file("mpconfig.txt");
    std::string line;
    while (std::getline(config_file, line)) {
        std::istringstream iss(line);
        std::string variable_name;
        int variable_value;
        if (iss >> variable_name >> variable_value) {
            if (variable_name == "CARD_MAX_NUMBER") {
                CARD_MAX_NUMBER = variable_value;
            } else if (variable_name == "REVERSE_MOVE_DIFF") {
                REVERSE_MOVE_DIFF = variable_value;
            } else if (variable_name == "CARD_IN_HANDS") {
                CARD_IN_HANDS = variable_value;
            } else if (variable_name == "NUM_CARDS_TO_PLAY") {
                NUM_CARDS_TO_PLAY = variable_value;
            } else if (variable_name == "NUMBER_OF_ROWS") {
                NUMBER_OF_ROWS = variable_value;
            } else if (variable_name == "NUMBER_OF_PLAYERS") {
                NUMBER_OF_PLAYERS = variable_value;
            } else if (variable_name == "NUM_SIMULATIONS") {
                NUM_SIMULATIONS = variable_value;
            }
            std::cout << variable_name << ": " << variable_value << std::endl;
        }
        
    }
    config_file.close();

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

    struct GameResult
    {
        std::string shuffle_id;
        int num_players;
        bool win;
        int turns;
    };
    std::vector<GameResult> game_results;

    std::map<std::string, int> win_counts;
    std::map<std::string, int> total_turns;

    for(auto const& [key, val] : strategies) {
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

        for (auto const& [strategy_name, strategy_func] : strategies) {
            int turns = 0;
            std::vector<Player> players(num_players); // Initialize players here
            // Clear the player hands before each game
            for (auto &player : players)
            {
                player.hand.clear();
            }

            bool won = simulate_game_multiplayer(strategy_func, num_players, game_deck, turns);
            
            GameResult result;
            result.shuffle_id = shuffle_id;
            result.num_players = num_players;
            result.win = won;
            result.turns = turns;
            game_results.push_back(result);

            if (won)
            {
                win_counts[strategy_name]++;
                total_turns[strategy_name] += turns;
            }
        }
        std::cout << "Completed simulation of game " << game << "\n";
    }


    // --- Console Output ---
    for (auto const& [strategy_name, win_count] : win_counts){
        double win_rate = (static_cast<double>(win_count) / num_games_to_simulate) * 100;
        double average_turns = (win_count > 0) ? static_cast<double>(total_turns[strategy_name]) / win_count : 0.0;

        std::cout << num_players << " Players: \n";
        std::cout << strategy_name << " win rate: " << win_rate << " %\n";
    }
    

    return 0;
}
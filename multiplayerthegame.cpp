#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <map>
#include <cmath>   // For std::abs
#include <numeric> // For std::iota and std::accumulate
#include <sstream> // For std::stringstream
#include <fstream> // For file output

// Constants
int CARD_MAX_NUMBER = 100;
int REVERSE_MOVE_DIFF = 10;
int CARD_IN_HANDS = 8;
int NUM_CARDS_TO_PLAY = 2;
int NUMBER_OF_ROWS = 4;
int NUMBER_OF_PLAYERS = 1;

int NUM_SIMULATIONS = 10000;

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
    std::cout << "Created deck: ";
    for (int card: deck) {
        std::cout << card << " ";
    }
    std::cout << std::endl;

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

bool simulate_game_multiplayer(int num_players, const std::vector<int> &initial_deck, int &turns_taken)
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
            auto move = get_player_move_A(current_player.hand, playing_rows);
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
        std::cout << "Played cards: ";
        for (int card: played_cards) {
            std::cout << card << " ";
        }
        std::cout << std::endl;

        // Print deck cards after each turn
        std::cout << "Deck cards: ";
        for (int card: deck) {
            std::cout << card << " ";
        }
        std::cout << std::endl;

        // Replenish hand (only if the deck is not empty)
        while (current_player.hand.size() < CARD_IN_HANDS && deck_size > 0)
        { // Add this check
            current_player.hand.push_back(deck.back());
            drawn_cards.push_back(deck.back()); // Store the drawn card
            deck.pop_back();
            deck_size--;
        }

        std::cout << "Drawn cards: ";
        for (int card: drawn_cards) {
            std::cout << card << " ";
        }
        std::cout << std::endl;

        // Print deck cards after each turn
        std::cout << "Deck cards: ";
        for (int card: deck) {
            std::cout << card << " ";
        }
        std::cout << std::endl;

        // Check if the player has no cards left and deactivate
        if (current_player.hand.empty() && deck_size == 0)
        {
            current_player.active = false;
        }

        current_player_index = (current_player_index + 1) % num_players;

        // Display game state after each player's turn
        std::cout << "---- Player " << player_order[current_player_index] + 1 << " Turn ----\n"; // Assuming players are numbered from 1
        display_game_state(playing_rows, current_player.hand, deck_size);

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

    struct GameResult
    {
        std::string shuffle_id;
        int num_players;
        bool win;
        int turns;
    };
    std::vector<GameResult> game_results;

    int win_counts = 0;
    int num_players = NUMBER_OF_PLAYERS;

    
    for (int game = 0; game < num_games_to_simulate; ++game)
    {
        std::vector<int> game_deck = create_deck();
        shuffle(game_deck);
        std::string shuffle_id = generate_deck_id(game_deck);

        int turns = 0;
        std::vector<Player> players(num_players); // Initialize players here

        // Clear the player hands before each game
        for (auto &player : players)
        {
            player.hand.clear();
        }

        bool won = simulate_game_multiplayer(num_players, game_deck, turns);

        GameResult result;
        result.shuffle_id = shuffle_id;
        result.num_players = num_players;
        result.win = won;
        result.turns = turns;
        game_results.push_back(result);

        if (won)
        {
            win_counts++;
        }
    }
    

    // --- Console Output ---
    std::cout << "Overall Stats:\n";
    double win_rate = (static_cast<double>(win_counts) / num_games_to_simulate) * 100;

    std::cout << "  " << num_players << "P: {\n";
    std::cout << "    win_rate: " << win_rate << ",\n";
    std::cout << "  }\n";

    return 0;
}
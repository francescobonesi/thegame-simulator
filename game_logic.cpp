#include "game_logic.h"
 #include "helper_functions.h"
 #include "player_strategies.h"

 #include <iostream>
 #include <numeric>  // std::iota
 #include <sstream>  // std::stringstream
 #include <algorithm> // std::remove, std::find
 #include <iomanip> // For std::hex and std::setw
 #include <cstring>

 // Constants (declared in main.cpp, defined extern here)
 extern int CARD_MAX_NUMBER;  // Maximum value a card can have
 extern int REVERSE_MOVE_DIFF; // Difference needed for a reverse-10 move
 extern int CARD_IN_HANDS;   // Number of cards each player starts with
 extern int NUM_CARDS_TO_PLAY; // Number of cards each player plays per turn
 extern int NUMBER_OF_ROWS;  // Number of rows in the playing area

 // Player struct
 struct Player
 {
  std::vector<int> hand;  // The cards the player currently holds
  bool active = true; // Flag to indicate if the player is still in the game
 };

 /**
  * @brief Checks if the game has been won in a multiplayer context.
  *
  * The game is won when all players have empty hands and the deck is also empty.
  *
  * @param players A vector containing all Player structs.
  * @param deck_size The current size of the deck.
  * @return True if the game is won, false otherwise.
  */
 bool check_win_condition_multiplayer(const std::vector<Player> &players, int deck_size)
 {
  // Iterate through each player
  for (const auto &player : players)
  {
   // If any player has cards in hand OR the deck is not empty, the game is not won
   if (!player.hand.empty() || deck_size > 0)
   {
    return false;
   }
  }
  // If all players have empty hands and the deck is empty, the game is won
  return true;
 }

 /**
  * @brief Simulates a single game in a multiplayer setting.
  *
  * This function manages the game flow, dealing cards, handling player turns,
  * checking win conditions, and storing the final game state.
  *
  * @param get_player_move A function pointer to the chosen player strategy.
  * @param num_players The number of players in the game.
  * @param initial_deck The initial shuffled deck of cards.
  * @param turns_taken (Output) The total number of turns taken in the game.
  * @param final_playing_rows (Output) The final state of the playing rows.
  * @param final_hand (Output) The final hands of all players.
  * @return True if the game was won, false otherwise.
  */
 bool simulate_game_multiplayer(std::pair<int, int> (*get_player_move)(const std::vector<int> &, const std::vector<std::vector<int>> &, const std::vector<Communication>&, int), int num_players, const std::vector<int> &initial_deck, int &turns_taken, std::vector<std::vector<int>> &final_playing_rows, std::vector<std::vector<int>> &final_hand)
{
    std::vector<int> deck = initial_deck;
    std::vector<Player> players(num_players);

    for (auto &player : players)
    {
        player.hand = deal_cards(deck, CARD_IN_HANDS);
    }

    int deck_size = deck.size();

    std::vector<std::vector<int>> playing_rows(NUMBER_OF_ROWS);
    for (int i = 0; i < NUMBER_OF_ROWS; ++i)
    {
        playing_rows[i].push_back(i < NUMBER_OF_ROWS / 2 ? 1 : CARD_MAX_NUMBER);
    }

    std::vector<int> player_order(num_players);
    std::iota(player_order.begin(), player_order.end(), 0);
    shuffle(player_order);

    int current_player_index = 0;
    int turns = 0;

    std::vector<Communication> communications;

    while (true)
    {
        Player current_player = players[player_order[current_player_index]];
        if (!current_player.active)
        {
            current_player_index = (current_player_index + 1) % num_players;
            continue;
        }

        // --- Communication Phase ---
        communications.clear();
        for (int p_idx = 0; p_idx < num_players; ++p_idx)
        {
            if (players[p_idx].active) {
                for (int card : players[p_idx].hand) {
                    for (int r_idx = 0; r_idx < NUMBER_OF_ROWS; ++r_idx) {
                        bool is_ascending = r_idx < NUMBER_OF_ROWS / 2;
                        ValidMove vm = is_valid_move(card, playing_rows[r_idx].back(), is_ascending);
                        if (vm == ValidMove::REVERSE_MOVE)
                        {
                            communications.push_back({p_idx, r_idx, Communication::REVERSE_TRICK, 0});
                        }
                        else if (vm == ValidMove::EXCELLENT){
                            communications.push_back({p_idx, r_idx, Communication::GOOD_CARD, 0});
                        }
                    }
                }
            }
        }

        // --- Action Phase ---
        // Display the game state *before* each player's turn
        std::cout << "---- Player " << player_order[current_player_index] + 1 << " Before Turn ----\n";
        display_game_state(playing_rows, current_player.hand, deck_size);

        int num_cards_to_play_this_turn = (deck_size > 0) ? NUM_CARDS_TO_PLAY : 1;

        bool valid_turn = true;
        std::vector<int> played_cards;
        std::vector<int> drawn_cards; // Cards drawn *this* turn

        for (int k = 0; k < num_cards_to_play_this_turn; ++k)
        {
			// Create a COPY of the hand for the strategy function.  CRITICAL FIX!
            std::vector<int> hand_copy = current_player.hand;
            auto move = get_player_move(hand_copy, playing_rows, communications, player_order[current_player_index]); //Pass the copy
            int card_index = move.first;
            int row_index = move.second;

            if (card_index != -1) {
                int card_to_play = current_player.hand[card_index]; // Use ORIGINAL hand here
                make_move(card_to_play, row_index, playing_rows);
                played_cards.push_back(card_to_play);

                // Remove the card by index *immediately* (STILL CORRECT)
                current_player.hand.erase(current_player.hand.begin() + card_index);
                turns++; // Increment *after* playing (but before drawing)
            }
            else
            {
                valid_turn = false;
            }
        }


        // --- Replenish Hand (AT THE END OF THE TURN) ---
        while (current_player.hand.size() < CARD_IN_HANDS && deck_size > 0) {
            current_player.hand.push_back(deck.back());
            drawn_cards.push_back(deck.back()); // Track drawn cards
            deck.pop_back();
            deck_size--;
        }

        if (!valid_turn)
        {
            break;
        }

        // --- Output for Debugging/Visualization (After playing AND drawing) ---
        std::cout << "---- Player " << player_order[current_player_index] + 1 << " After Turn ----\n";
        display_game_state(playing_rows, current_player.hand, deck_size); // Show correct hand
        std::cout << "Played cards: ";
        for (int card : played_cards) {
            std::cout << card << " ";
        }
        std::cout << std::endl;
        std::cout << "Deck cards: ";
        for (int card : deck) {
            std::cout << card << " ";
        }
        std::cout << std::endl;
        std::cout << "Drawn cards: ";
        for (int card : drawn_cards) {
            std::cout << card << " ";
        }
        std::cout << std::endl;
        // --- End Output ---



        if (current_player.hand.empty() && deck_size == 0)
        {
            current_player.active = false;
        }

        players[player_order[current_player_index]] = current_player;

        current_player_index = (current_player_index + 1) % num_players;

        // Check for game over (all players inactive)
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
            break;
        }
    }

    for (const auto &player : players)
    {
        final_hand.push_back(player.hand);
    }

    turns_taken = turns;
    final_playing_rows = playing_rows;
    return check_win_condition_multiplayer(players, deck_size);
}
 
 // Base64 encoding functions (simplified, you might want to use a library)
namespace base64 {

    static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    std::string encode(const std::string& input) {
        std::string encoded;
        int i = 0;
        unsigned char char_array_3[3];
        unsigned char char_array_4[4];

        for (unsigned char c : input) {
            char_array_3[i++] = c;
            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for (i = 0; (i < 4); i++)
                    encoded += base64_chars[char_array_4[i]];
                i = 0;
            }
        }

        if (i) {
            for (int j = i; j < 3; j++)
                char_array_3[j] = '\0';

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (int j = 0; (j < i + 1); j++)
                encoded += base64_chars[char_array_4[j]];

            while ((i++ < 3))
                encoded += '=';
        }

        return encoded;
    }

} // namespace base64

// MD5 implementation (no external library)
namespace md5 {

    std::string encode(const std::string& input) {
        unsigned char message[input.size()];
        for(size_t i = 0; i < input.size(); ++i) {
            message[i] = static_cast<unsigned char>(input[i]);
        }

        unsigned int h0 = 0x67452301;
        unsigned int h1 = 0xEFCDAB89;
        unsigned int h2 = 0x98BADCFE;
        unsigned int h3 = 0x10325476;

        unsigned long long bit_length = input.size() * 8;
        unsigned char padded_message[input.size() + 64]; // max pad

        memcpy(padded_message, message, input.size());

        padded_message[input.size()] = 0x80;

        int padding_size = (56 - (input.size() + 1) % 64 + 64) % 64;
        memset(padded_message + input.size() + 1, 0, padding_size);

        for (int i = 0; i < 8; i++) {
            padded_message[input.size() + 1 + padding_size + i] = (bit_length >> (i * 8)) & 0xFF;
        }

        unsigned int w[16];
        for (int i = 0; i < input.size() + padding_size + 9; i+=64) {
            for (int j = 0; j < 16; j++) {
                w[j] = (padded_message[i+j*4]) | (padded_message[i+j*4+1] << 8) | (padded_message[i+j*4+2] << 16) | (padded_message[i+j*4+3] << 24);
            }
            unsigned int a = h0;
            unsigned int b = h1;
            unsigned int c = h2;
            unsigned int d = h3;

            unsigned int f, g;
            unsigned int k[] = {
                0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
                0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
                0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
                0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821
            };
            unsigned int s[] = {
                7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
                4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
            };

            for (int j = 0; j < 64; j++) {
                if (j < 16) {
                    f = (b & c) | ((~b) & d);
                    g = j;
                } else if (j < 32) {
                    f = (d & b) | ((~d) & c);
                    g = (5 * j + 1) % 16;
                } else if (j < 48) {
                    f = b ^ c ^ d;
                    g = (3 * j + 5) % 16;
                } else {
                    f = c ^ (b | (~d));
                    g = (7 * j) % 16;
                }
                unsigned int temp = d;
                d = c;
                c = b;
                b = b + ((a + f + k[j] + w[g]) << s[j]) | ((a + f + k[j] + w[g]) >> (32 - s[j]));
                a = temp;
            }
            h0 += a;
            h1 += b;
            h2 += c;
            h3 += d;
        }

        std::stringstream hex_ss;
        hex_ss << std::hex << std::setfill('0') << std::setw(8) << h0
               << std::hex << std::setfill('0') << std::setw(8) << h1
               << std::hex << std::setfill('0') << std::setw(8) << h2
               << std::hex << std::setfill('0') << std::setw(8) << h3;

        return hex_ss.str();
    }
} // namespace md5

// Function to generate a unique ID for a shuffled deck, remove trailing underscore, and base64 encode
std::string generate_deck_id(const std::vector<int> &deck) {
    std::stringstream ss;
    for (int card : deck) {
        ss << card << "_";
    }
    std::string deck_str = ss.str();

    // Remove the trailing underscore
    if (!deck_str.empty()) {
        deck_str.pop_back();
    }

    // Base64 encode the resulting string
    return base64::encode(deck_str);
}
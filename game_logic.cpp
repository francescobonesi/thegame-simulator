#include "game_logic.h"
 #include "helper_functions.h"
 #include "player_strategies.h"

 #include <iostream>
 #include <numeric>  // std::iota
 #include <sstream>  // std::stringstream
 #include <algorithm> // std::remove, std::find

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
                        if (is_valid_move(card, playing_rows[r_idx].back(), is_ascending) == ValidMove::REVERSE_MOVE)
                        {
                            communications.push_back({p_idx, r_idx, Communication::REVERSE_TRICK, 0});
                        }
                    }
                }

                int valid_moves_count = 0;
                for(int card_in_hand : players[p_idx].hand)
                {
                    for(int j=0; j<playing_rows.size(); ++j)
                    {
                        if (is_valid_move(card_in_hand, playing_rows[j].back(), j < NUMBER_OF_ROWS/2) != ValidMove::NO)
                        {
                            valid_moves_count++;
                            break;
                        }
                    }
                }

                if (NUM_CARDS_TO_PLAY == 2 && valid_moves_count <= 1)
                {
                    communications.push_back({p_idx, -1, Communication::ONLY_ONE_CARD, 0});
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
                communications.push_back({player_order[current_player_index], row_index, Communication::TARGET_ROW, 0});
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
 
 /**
  * @brief Generates a unique ID string for a shuffled deck of cards.
  *
  * This function creates a string representation of the deck, with each
  * card value separated by an underscore. This ID can be used to identify
  * a specific deck configuration.
  *
  * @param deck The shuffled deck of cards.
  * @return A unique ID string for the deck.
  */
 std::string generate_deck_id(const std::vector<int> &deck)
 {
  std::stringstream ss; // Create a stringstream to build the ID string
  // Iterate through each card in the deck
  for (int card : deck)
  {
   ss << card << "_"; // Add the card value and an underscore to the stringstream
  }
  return ss.str(); // Return the final ID string
 }
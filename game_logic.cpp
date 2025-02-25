#include "game_logic.h"
 #include "helper_functions.h"
 #include "player_strategies.h"

 #include <iostream>
 #include <numeric>
 #include <sstream>
 #include <algorithm>

 // Constants
 extern int CARD_MAX_NUMBER;
 extern int REVERSE_MOVE_DIFF;
 extern int CARD_IN_HANDS;
 extern int NUM_CARDS_TO_PLAY;
 extern int NUMBER_OF_ROWS;

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

 bool simulate_game_multiplayer(std::pair<int, int> (*get_player_move)(const std::vector<int> &, const std::vector<std::vector<int>> &), int num_players, const std::vector<int> &initial_deck, int &turns_taken, std::vector<std::vector<int>> &final_playing_rows, std::vector<std::vector<int>> &final_hand)
 {
     std::vector<int> deck = initial_deck;
     std::vector<Player> players(num_players);

     // Deal cards to players
     for (auto &player : players)
     {
         player.hand = deal_cards(deck, CARD_IN_HANDS);
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

         int num_cards_to_play_this_turn = std::min(NUM_CARDS_TO_PLAY, (deck_size > 0) ? NUM_CARDS_TO_PLAY : 1);
         bool valid_turn = true; // Flag to check if the player can make a valid move
         played_cards.clear(); // Clear the played cards from the previous turn
         drawn_cards.clear(); // Clear the drawn cards from the previous turn

         for (int k = 0; k < num_cards_to_play_this_turn; ++k)
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
         std::cout << "Played cards: ";
         for (int card : played_cards)
         {
             std::cout << card << " ";
         }
         std::cout << std::endl;

         // Print deck cards after each turn
         std::cout << "Deck cards: ";
         for (int card : deck)
         {
             std::cout << card << " ";
         }
         std::cout << std::endl;

         // Replenish hand (only if the deck is not empty)
         while (current_player.hand.size() < CARD_IN_HANDS && deck_size > 0)
         {
             current_player.hand.push_back(deck.back());
             drawn_cards.push_back(deck.back()); // Store the drawn card
             deck.pop_back();
             deck_size--;
         }

         // Print drawn cards after each turn
         std::cout << "Drawn cards: ";
         for (int card : drawn_cards)
         {
             std::cout << card << " ";
         }
         std::cout << std::endl;

         // Print deck cards after each turn
         std::cout << "Deck cards: ";
         for (int card : deck)
         {
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

     // Store the final hands of all players
     for (const auto& player : players) {
         final_hand.push_back(player.hand);
     }

     turns_taken = turns;
     final_playing_rows = playing_rows; // Store the final state
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
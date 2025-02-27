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
 bool simulate_game_multiplayer(std::pair<int, int> (*get_player_move)(const std::vector<int> &, const std::vector<std::vector<int>> &), int num_players, const std::vector<int> &initial_deck, int &turns_taken, std::vector<std::vector<int>> &final_playing_rows, std::vector<std::vector<int>> &final_hand)
 {
  std::vector<int> deck = initial_deck; // Create a copy of the initial deck
  std::vector<Player> players(num_players); // Create a vector to hold Player structs

  // Deal cards to each player
  for (auto &player : players)
  {
   player.hand = deal_cards(deck, CARD_IN_HANDS);
  }

  int deck_size = deck.size(); // Get the current deck size AFTER dealing

  // Initialize the playing rows
  std::vector<std::vector<int>> playing_rows(NUMBER_OF_ROWS);
  for (int i = 0; i < NUMBER_OF_ROWS; ++i)
  {
   // Ascending rows start with 1, descending rows start with CARD_MAX_NUMBER
   playing_rows[i].push_back(i < NUMBER_OF_ROWS / 2 ? 1 : CARD_MAX_NUMBER);
  }

  // Determine the order of players for this game
  std::vector<int> player_order(num_players);
  std::iota(player_order.begin(), player_order.end(), 0); // Fill with 0, 1, 2, ...
  shuffle(player_order); // Randomize the player order

  int current_player_index = 0; // Index of the player whose turn it is
  int turns = 0;      // Counter for the number of turns taken

  std::vector<int> drawn_cards; // Cards drawn this turn (for output)
  std::vector<int> played_cards; // Cards played this turn (for output)

  // Main game loop
  while (true)
  {
   Player &current_player = players[player_order[current_player_index]]; // Get the current player
   if (!current_player.active)
   { // Skip this player's turn if they are inactive (out of cards)
    current_player_index = (current_player_index + 1) % num_players; // Move to the next player
    continue;
   }

   // Determine how many cards the player should play this turn
   int num_cards_to_play_this_turn = std::min(NUM_CARDS_TO_PLAY, (deck_size > 0) ? NUM_CARDS_TO_PLAY : 1);
   bool valid_turn = true; // Flag to check if the player made all required moves
   played_cards.clear(); // Clear played cards from the previous turn
   drawn_cards.clear(); // Clear drawn cards from the previous turn

   // Player plays their cards
   for (int k = 0; k < num_cards_to_play_this_turn; ++k)
   {
    auto move = get_player_move(current_player.hand, playing_rows); // Get the player's move
    int card_to_play = move.first;  // The card the player chose to play
    int row_index = move.second;  // The row the player chose to play on

    // If the player chose a valid move (card_to_play is not -1)
    if (card_to_play != -1)
    {
     make_move(card_to_play, row_index, playing_rows); // Play the card
     played_cards.push_back(card_to_play); // Store the played card for output
     // Remove the played card from the player's hand
     current_player.hand.erase(std::remove(current_player.hand.begin(), current_player.hand.end(), card_to_play), current_player.hand.end());
     turns++; // Increment the turn counter
    }
    else
    {
     // Player couldn't make all required moves, they lose
     valid_turn = false;
    }
   }

   // If the player couldn't make all their moves, the game is over
   if (!valid_turn)
   {
    break; // Exit the game loop
   }

   // --- Output for Debugging/Visualization ---
   std::cout << "Played cards: ";
   for (int card : played_cards)
   {
    std::cout << card << " ";
   }
   std::cout << std::endl;

   std::cout << "Deck cards: ";
   for (int card : deck)
   {
    std::cout << card << " ";
   }
   std::cout << std::endl;
   // --- End Output ---

   // Replenish the player's hand from the deck (if cards remain)
   while (current_player.hand.size() < CARD_IN_HANDS && deck_size > 0)
   {
    current_player.hand.push_back(deck.back()); // Draw a card
    drawn_cards.push_back(deck.back()); // Store drawn card for output
    deck.pop_back();        // Remove the card from the deck
    deck_size--;         // Update deck size
   }

   // --- Output for Debugging/Visualization ---
   std::cout << "Drawn cards: ";
   for (int card : drawn_cards)
   {
    std::cout << card << " ";
   }
   std::cout << std::endl;

   std::cout << "Deck cards: ";
   for (int card : deck)
   {
    std::cout << card << " ";
   }
   std::cout << std::endl;
   // --- End Output ---

   // If the player has no cards left and the deck is empty, they are inactive
   if (current_player.hand.empty() && deck_size == 0)
   {
    current_player.active = false;
   }

   current_player_index = (current_player_index + 1) % num_players; // Move to the next player

   // Display the game state after each player's turn
   std::cout << "---- Player " << player_order[current_player_index] + 1 << " Turn ----\n";
   display_game_state(playing_rows, current_player.hand, deck_size);

   // Check if all players are inactive (game over)
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
    break; // Exit the game loop
   }
  }

  // Store the final hands of all players
  for (const auto &player : players)
  {
   final_hand.push_back(player.hand);
  }

  turns_taken = turns;       // Store the total number of turns taken
  final_playing_rows = playing_rows; // Store the final state of the playing rows
  return check_win_condition_multiplayer(players, deck_size); // Return whether the game was won
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
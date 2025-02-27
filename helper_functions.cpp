#include "helper_functions.h"
 #include <random>
 #include <iostream>
 #include <algorithm>

 // Constants (declared in main.cpp, defined extern here)
 extern int CARD_MAX_NUMBER; // Maximum value of a card in the deck
 extern int REVERSE_MOVE_DIFF; // Difference required for a reverse move
 extern int CARD_IN_HANDS; // Number of cards each player holds
 extern int NUM_CARDS_TO_PLAY; // Number of cards a player must play each turn
 extern int NUMBER_OF_ROWS; // Number of rows in the playing area

 /**
  * @brief Shuffles the order of elements within a vector.
  *
  * This function uses a Fisher-Yates shuffle algorithm to randomize the order
  * of elements in the given vector.
  *
  * @param deck The vector to be shuffled.  Passed by reference to modify the original.
  */
 void shuffle(std::vector<int> &deck)
 {
  // Obtain a seed for the random number engine from a hardware source (if available)
  std::random_device rd;
  // Use Mersenne Twister engine for generating pseudo-random numbers
  std::mt19937 g(rd());
  // Shuffle the elements of the deck using the random engine
  std::shuffle(deck.begin(), deck.end(), g);
 }

 /**
  * @brief Creates a standard deck of cards for the game.
  *
  * The deck contains cards with values ranging from 2 up to (but not including)
  * CARD_MAX_NUMBER. The cards are then shuffled.
  *
  * @return A vector representing the created and shuffled deck.
  */
 std::vector<int> create_deck()
 {
  std::vector<int> deck;
  // Populate the deck with card values
  for (int i = 2; i < CARD_MAX_NUMBER; ++i)
  {
   deck.push_back(i);
  }
  // Shuffle the newly created deck
  shuffle(deck);

  return deck;
 }

 /**
  * @brief Deals a specified number of cards from the deck to a player's hand.
  *
  * Cards are taken from the back of the deck (LIFO) and added to the hand.
  * If the deck has fewer than num_cards, all remaining cards are dealt.
  *
  * @param deck The deck of cards to deal from. Passed by reference to modify the original.
  * @param num_cards The number of cards to deal.
  * @return A vector representing the player's hand after dealing.
  */
 std::vector<int> deal_cards(std::vector<int> &deck, int num_cards)
 {
  std::vector<int> hand;
  // Deal cards from the back of the deck until num_cards is reached or the deck is empty
  for (int i = 0; i < num_cards && !deck.empty(); ++i)
  {
   hand.push_back(deck.back()); // Add the card to the hand
   deck.pop_back();        // Remove the card from the deck
  }
  return hand;
 }

 /**
  * @brief Displays the current state of the game in the console.
  *
  * This includes the current cards in each playing row (ascending and descending),
  * the cards in the player's hand, and the number of cards remaining in the deck.
  *
  * @param playing_rows A 2D vector representing the playing rows.
  * @param hand The player's current hand of cards.
  * @param deck_size The number of cards remaining in the deck.
  */
 void display_game_state(const std::vector<std::vector<int>> &playing_rows, const std::vector<int> &hand, int deck_size)
 {
  // Display the cards in each playing row
  for (int i = 0; i < NUMBER_OF_ROWS; ++i)
  {
   std::cout << (i < NUMBER_OF_ROWS / 2 ? "Ascending: " : "Descending: "); // Indicate if the row is ascending or descending
   for (int card : playing_rows[i])
   {
    std::cout << card << " "; // Print each card in the row
   }
   std::cout << "->\n";
  }
  // Display the cards in the player's hand
  std::cout << "Your hand: ";
  for (int card : hand)
  {
   std::cout << card << " "; // Print each card in the hand
  }
  std::cout << "\nDeck size: " << deck_size << "\n"; // Display the number of cards in the deck
 }

 /**
  * @brief Checks if a move is valid according to the game rules.
  *
  * A move is valid if:
  * - For ascending rows: the card is greater than the top card of the row,
  * OR the card is equal to the top card minus REVERSE_MOVE_DIFF (reverse move, if allowed).
  * - For descending rows: the card is less than the top card of the row,
  * OR the card is equal to the top card plus REVERSE_MOVE_DIFF (reverse move, if allowed).
  *
  * @param card The card being played.
  * @param row_top The value of the top card in the row.
  * @param is_ascending True if the row is ascending, false if descending.
  * @param reverse_move_allowed True if reverse moves are allowed, false otherwise.
  * @return ValidMove enum indicating if the move is valid, invalid, or a reverse move.
  */
 ValidMove is_valid_move(int card, int row_top, bool is_ascending, bool reverse_move_allowed)
 {
  if (is_ascending)
  {
   // Ascending row logic
   if (card == row_top - REVERSE_MOVE_DIFF && reverse_move_allowed)
   {
    return ValidMove::REVERSE_MOVE; // Reverse move is valid
   }
   else if (card > row_top)
   {
    return ValidMove::YES; // Card is greater than the top card
   }
   else
   {
    return ValidMove::NO; // Card is not valid
   }
  }
  else
  {
   // Descending row logic
   if (card == row_top + REVERSE_MOVE_DIFF && reverse_move_allowed)
   {
    return ValidMove::REVERSE_MOVE; // Reverse move is valid
   }
   else if (card < row_top)
   {
    return ValidMove::YES; // Card is less than the top card
   }
   else
   {
    return ValidMove::NO; // Card is not valid
   }
  }
 }

 /**
  * @brief Makes a move by adding a card to the specified row.
  *
  * The card is added to the back of the vector representing the row.
  *
  * @param card The card to be added to the row.
  * @param row_index The index of the row to add the card to.
  * @param playing_rows A 2D vector representing the playing rows. Passed by reference to modify the original.
  */
 void make_move(int card, int row_index, std::vector<std::vector<int>> &playing_rows)
 {
  playing_rows[row_index].push_back(card); // Add the card to the specified row
 }
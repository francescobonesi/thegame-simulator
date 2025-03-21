#include "player_strategies.h"
#include "helper_functions.h"
#include <cmath>
#include <algorithm>

// Constants (defined in main.cpp, declared extern here)
extern int CARD_MAX_NUMBER;   // Maximum value of a card
extern int REVERSE_MOVE_DIFF; // Difference needed for a reverse move
extern int NUMBER_OF_ROWS;    // Number of rows in the playing area
extern int GOOD_MOVE_WINDOW;  // interval for good moves

/**
 * @brief Strategy A: Plays the card closest in value to the top card of a row. Communication among players.
 *
 * This strategy considers both ascending and descending rows and allows reverse moves.
 * It chooses the card that minimizes the absolute difference with the row's top card.
 *
 * @param hand The player's current hand of cards.
 * @param playing_rows The current state of the playing rows.
 * @return A pair containing the best card to play and the row index, or {-1, -1} if no valid move.
 */
std::pair<int, int> get_player_move_A1(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows, const std::vector<Communication> &communications, int player_id)
{
    int best_card_index = -1; // Store the *index* of the best card
    int best_card = -1;
    int best_row = -1;
    int min_diff = std::numeric_limits<int>::max(); // Use numeric_limits for max value

    // --- Observation Phase ---
    std::vector<int> claimed_rows;
    for (const auto &comm : communications)
    {
        if (comm.player_id != player_id)
        {                                           // Don't react to your own communication
            claimed_rows.push_back(comm.row_index); // GOOD MOVE or REVERSE MOVE from another player
        }
    }

    // --- Decision-Making Phase (with Communication) ---
    for (int i = 0; i < hand.size(); ++i)
    {
        for (int j = 0; j < NUMBER_OF_ROWS; ++j)
        {
            ValidMove valid_move = is_valid_move(hand[i], playing_rows[j].back(), j < NUMBER_OF_ROWS / 2);
            if (valid_move != ValidMove::NO)
            {
                int diff = (valid_move == ValidMove::REVERSE_MOVE) ? -1 : std::abs(hand[i] - playing_rows[j].back());

                // Check if the row is claimed.  If it is, and our move is "bad", increase the diff
                // to make it less likely to be chosen.
                bool row_is_claimed = std::find(claimed_rows.begin(), claimed_rows.end(), j) != claimed_rows.end();
                if (row_is_claimed && diff > GOOD_MOVE_WINDOW)
                {                                               // Consider it a less good move if diff > GOOD_MOVE_WINDOW
                    diff = diff*100; 
                }

                if (diff < min_diff)
                {
                    min_diff = diff;
                    best_card = hand[i];
                    best_card_index = i;
                    best_row = j;
                }
            }
        }
    }

    return {best_card_index, best_row};
}


/**
 * @brief Strategy A2: Plays the card closest in value to the top card of a row. NO COMMUNICATION among players.
 *
 * This strategy considers both ascending and descending rows and allows reverse moves.
 * It chooses the card that minimizes the absolute difference with the row's top card.
 *
 * @param hand The player's current hand of cards.
 * @param playing_rows The current state of the playing rows.
 * @return A pair containing the best card to play and the row index, or {-1, -1} if no valid move.
 */
std::pair<int, int> get_player_move_A2(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows, const std::vector<Communication> &communications, int player_id)
{
    int best_card_index = -1; // Store the *index* of the best card
    int best_card = -1;
    int best_row = -1;
    int min_diff = std::numeric_limits<int>::max(); // Use numeric_limits for max value

    // --- Decision-Making Phase (with Communication) ---
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
                    best_card_index = i;
                    best_row = j;
                }
            }
        }
    }

    return {best_card_index, best_row};
}


// TODO implement strategies E and H (check that E and H are the correct ones before) with and without communication



 /**
  * @brief Strategy E: Combination of Strategy C and Strategy A.
  *
  * This strategy considers future playability (like Strategy C) but uses
  * the closest card (like Strategy A) as a tie-breaker.
  *
  * @param hand The player's current hand of cards.
  * @param playing_rows The current state of the playing rows.
  * @return A pair containing the best card to play and the row index, or {-1, -1} if no valid move.
  */
 std::pair<int, int> get_player_move_E1(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows, const std::vector<Communication>& communications, int player_id)
 {
  int best_card = -1; // Initialize the best card to -1 (no card selected yet)
  int best_row = -1;  // Initialize the best row to -1 (no row selected yet)
  int max_playable_after = -1; // Initialize the maximum playable cards after to -1
  int min_diff = CARD_MAX_NUMBER * 2; // Initialize the minimum difference to a large value

  // Iterate through each card in the player's hand
  for (int i = 0; i < hand.size(); ++i)
  {
   // Iterate through each row in the playing area
   for (int j = 0; j < NUMBER_OF_ROWS; ++j)
   {
    // Check if the current card can be played on the current row
    if (is_valid_move(hand[i], playing_rows[j].back(), j < NUMBER_OF_ROWS / 2) != ValidMove::NO)
    {
     // Simulate the move (create copies of playing rows)
     std::vector<std::vector<int>> temp_rows = playing_rows;
     temp_rows[j].push_back(hand[i]);

     int playable_after = 0; // Initialize the count of playable cards after the move
     // Iterate through the remaining cards in the hand
     for (int k = 0; k < hand.size(); ++k)
     {
      // Skip the card that was just "played"
      if (static_cast<unsigned long long>(i) != k)
      {
       // Iterate through each row to check playability
       for (int l = 0; l < NUMBER_OF_ROWS; ++l)
       {
        // If the remaining card is playable on any row
        if (is_valid_move(hand[k], temp_rows[l].back(), l < NUMBER_OF_ROWS / 2) != ValidMove::NO)
        {
         // Increment the count of playable cards and break the inner loop
         playable_after++;
         break;
        }
       }
      }
     }

     // Calculate the difference between the card and the row's top card
     int diff = std::abs(hand[i] - playing_rows[j].back());
     if (is_valid_move(hand[i], playing_rows[j].back(), j < NUMBER_OF_ROWS / 2) == ValidMove::REVERSE_MOVE)
      diff = -1;

     // Tie-breaker logic: If playable_after is the same, choose the smaller diff
     if (playable_after > max_playable_after)
     {
      // Update the maximum playable cards, best card, best row, and minimum difference
      max_playable_after = playable_after;
      min_diff = diff;
      best_card = hand[i];
      best_row = j;
     }
     else if (playable_after == max_playable_after && diff < min_diff)
     {
      // Update the minimum difference, best card, and best row
      min_diff = diff;
      best_card = hand[i];
      best_row = j;
     }
    }
   }
  }
  // Return the best card and row as a pair
  return {best_card, best_row};
 }



  /**
  * @brief Strategy E: Combination of Strategy C and Strategy A.
  *
  * This strategy considers future playability (like Strategy C) but uses
  * the closest card (like Strategy A) as a tie-breaker.
  *
  * @param hand The player's current hand of cards.
  * @param playing_rows The current state of the playing rows.
  * @return A pair containing the best card to play and the row index, or {-1, -1} if no valid move.
  */
 std::pair<int, int> get_player_move_E2(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows, const std::vector<Communication>& communications, int player_id)
 {
  int best_card = -1; // Initialize the best card to -1 (no card selected yet)
  int best_row = -1;  // Initialize the best row to -1 (no row selected yet)
  int max_playable_after = -1; // Initialize the maximum playable cards after to -1
  int min_diff = CARD_MAX_NUMBER * 2; // Initialize the minimum difference to a large value

  // Iterate through each card in the player's hand
  for (int i = 0; i < hand.size(); ++i)
  {
   // Iterate through each row in the playing area
   for (int j = 0; j < NUMBER_OF_ROWS; ++j)
   {
    // Check if the current card can be played on the current row
    if (is_valid_move(hand[i], playing_rows[j].back(), j < NUMBER_OF_ROWS / 2) != ValidMove::NO)
    {
     // Simulate the move (create copies of playing rows)
     std::vector<std::vector<int>> temp_rows = playing_rows;
     temp_rows[j].push_back(hand[i]);

     int playable_after = 0; // Initialize the count of playable cards after the move
     // Iterate through the remaining cards in the hand
     for (int k = 0; k < hand.size(); ++k)
     {
      // Skip the card that was just "played"
      if (static_cast<unsigned long long>(i) != k)
      {
       // Iterate through each row to check playability
       for (int l = 0; l < NUMBER_OF_ROWS; ++l)
       {
        // If the remaining card is playable on any row
        if (is_valid_move(hand[k], temp_rows[l].back(), l < NUMBER_OF_ROWS / 2) != ValidMove::NO)
        {
         // Increment the count of playable cards and break the inner loop
         playable_after++;
         break;
        }
       }
      }
     }

     // Calculate the difference between the card and the row's top card
     int diff = std::abs(hand[i] - playing_rows[j].back());
     if (is_valid_move(hand[i], playing_rows[j].back(), j < NUMBER_OF_ROWS / 2) == ValidMove::REVERSE_MOVE)
      diff = -1;

     // Tie-breaker logic: If playable_after is the same, choose the smaller diff
     if (playable_after > max_playable_after)
     {
      // Update the maximum playable cards, best card, best row, and minimum difference
      max_playable_after = playable_after;
      min_diff = diff;
      best_card = hand[i];
      best_row = j;
     }
     else if (playable_after == max_playable_after && diff < min_diff)
     {
      // Update the minimum difference, best card, and best row
      min_diff = diff;
      best_card = hand[i];
      best_row = j;
     }
    }
   }
  }
  // Return the best card and row as a pair
  return {best_card, best_row};
 }


 /**
  * @brief Strategy H: "Panic Mode" - If few moves are left, play the largest/smallest possible card.
  *
  * This strategy counts the total number of valid moves. If there are very few valid moves left,
  * it tries to force a play by playing the largest possible card on an ascending row or the
  * smallest possible card on a descending row. Otherwise, it defaults to Strategy E.
  *
  * @param hand The player's current hand of cards.
  * @param playing_rows The current state of the playing rows.
  * @return A pair containing the best card to play and
* the row index, or {-1, -1} if no valid move.
  */
 std::pair<int, int> get_player_move_H1(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows, const std::vector<Communication>& communications, int player_id)
 {
  int total_valid_moves = 0; // Initialize the count of valid moves
  // Iterate through each card in the hand
  for (int card : hand)
  {
   // Iterate through each row in the playing area
   for (int j = 0; j < NUMBER_OF_ROWS; ++j)
   {
    // If the card can be played on the current row
    if (is_valid_move(card, playing_rows[j].back(), j < NUMBER_OF_ROWS / 2) != ValidMove::NO)
    {
     // Increment the count of valid moves
     total_valid_moves++;
    }
   }
  }

  // If there are very few valid moves left (2 or less)
  if (total_valid_moves <= 2)
  {
   int best_card = -1; // Initialize the best card to -1 (no card selected yet)
   int best_row = -1;  // Initialize the best row to -1 (no row selected yet)

   // Try to play the largest possible card on an ascending row, or the smallest on a descending row
   for (int i = 0; i < hand.size(); ++i)
   {
    for (int j = 0; j < NUMBER_OF_ROWS; ++j)
    {
     // If the card can be played on the current row
     if (is_valid_move(hand[i], playing_rows[j].back(), j < NUMBER_OF_ROWS / 2) != ValidMove::NO)
     {
      // If it's an ascending row
      if (j < NUMBER_OF_ROWS / 2)
      {
       // Choose the largest card
       if (best_card == -1 || hand[i] > best_card)
       {
        best_card = hand[i];
        best_row = j;
       }
      }
      else
      { // If it's a descending row
       // Choose the smallest card
       if (best_card == -1 || hand[i] < best_card)
       {
        best_card = hand[i];
        best_row = j;
       }
      }
     }
    }
   }
   // If a move was forced, return it
   if (best_card != -1)
    return {best_card, best_row};
  }

  // Otherwise, default to Strategy E (a good general-purpose strategy)
  return get_player_move_E1(hand, playing_rows, communications, player_id);
 }




/**
  * @brief Strategy H: "Panic Mode" - If few moves are left, play the largest/smallest possible card.
  *
  * This strategy counts the total number of valid moves. If there are very few valid moves left,
  * it tries to force a play by playing the largest possible card on an ascending row or the
  * smallest possible card on a descending row. Otherwise, it defaults to Strategy E.
  *
  * @param hand The player's current hand of cards.
  * @param playing_rows The current state of the playing rows.
  * @return A pair containing the best card to play and
* the row index, or {-1, -1} if no valid move.
  */
 std::pair<int, int> get_player_move_H2(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows, const std::vector<Communication>& communications, int player_id)
 {
  int total_valid_moves = 0; // Initialize the count of valid moves
  // Iterate through each card in the hand
  for (int card : hand)
  {
   // Iterate through each row in the playing area
   for (int j = 0; j < NUMBER_OF_ROWS; ++j)
   {
    // If the card can be played on the current row
    if (is_valid_move(card, playing_rows[j].back(), j < NUMBER_OF_ROWS / 2) != ValidMove::NO)
    {
     // Increment the count of valid moves
     total_valid_moves++;
    }
   }
  }

  // If there are very few valid moves left (2 or less)
  if (total_valid_moves <= 2)
  {
   int best_card = -1; // Initialize the best card to -1 (no card selected yet)
   int best_row = -1;  // Initialize the best row to -1 (no row selected yet)

   // Try to play the largest possible card on an ascending row, or the smallest on a descending row
   for (int i = 0; i < hand.size(); ++i)
   {
    for (int j = 0; j < NUMBER_OF_ROWS; ++j)
    {
     // If the card can be played on the current row
     if (is_valid_move(hand[i], playing_rows[j].back(), j < NUMBER_OF_ROWS / 2) != ValidMove::NO)
     {
      // If it's an ascending row
      if (j < NUMBER_OF_ROWS / 2)
      {
       // Choose the largest card
       if (best_card == -1 || hand[i] > best_card)
       {
        best_card = hand[i];
        best_row = j;
       }
      }
      else
      { // If it's a descending row
       // Choose the smallest card
       if (best_card == -1 || hand[i] < best_card)
       {
        best_card = hand[i];
        best_row = j;
       }
      }
     }
    }
   }
   // If a move was forced, return it
   if (best_card != -1)
    return {best_card, best_row};
  }

  // Otherwise, default to Strategy E (a good general-purpose strategy)
  return get_player_move_E2(hand, playing_rows, communications, player_id);
 }



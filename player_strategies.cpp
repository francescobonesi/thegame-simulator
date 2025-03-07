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

//  /**
//   * @brief Strategy B: Plays the card closest in value to the top card of a row, without reverse moves.
//   *
//   * This strategy is similar to Strategy A but does not consider reverse moves.
//   * It chooses the card that minimizes the absolute difference with the row's top card.
//   *
//   * @param hand The player's current hand of cards.
//   * @param playing_rows The current state of the playing rows.
//   * @return A pair containing the best card to play and the row index, or {-1, -1} if no valid move.
//   */
//  std::pair<int, int> get_player_move_B(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows)
//  {
//   int best_card = -1; // Initialize the best card to -1 (no card selected yet)
//   int best_row = -1;  // Initialize the best row to -1 (no row selected yet)
//   int min_diff = CARD_MAX_NUMBER * 2; // Initialize the minimum difference to a large value

//   // Iterate through each card in the player's hand
//   for (int i = 0; i < hand.size(); ++i)
//   {
//    // Iterate through each row in the playing area
//    for (int j = 0; j < NUMBER_OF_ROWS; ++j)
//    {
//     // Check if the current card can be played on the current row (reverse moves disabled)
//     ValidMove valid_move = is_valid_move(hand[i], playing_rows[j].back(), j < NUMBER_OF_ROWS / 2, false);
//     if (valid_move != ValidMove::NO)
//     {
//      // Calculate the absolute difference between the card and the row's top card
//      int diff = std::abs(hand[i] - playing_rows[j].back());
//      // If this difference is smaller than the current minimum difference
//      if (diff < min_diff)
//      {
//       // Update the minimum difference, best card, and best row
//       min_diff = diff;
//       best_card = hand[i];
//       best_row = j;
//      }
//     }
//    }
//   }
//   // Return the best card and row as a pair
//   return {best_card, best_row};
//  }

//  /**
//   * @brief Strategy C: Plays the card that maximizes future playability.
//   *
//   * This strategy simulates playing each card and counts how many cards in the hand
//   * would still be playable after the move. It chooses the card that results in the
//   * highest number of future playable cards.
//   *
//   * @param hand The player's current hand of cards.
//   * @param playing_rows The current state of the playing rows.
//   * @return A pair containing the best card to play and the row index, or {-1, -1} if no valid move.
//   */
//  std::pair<int, int> get_player_move_C(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows)
//  {
//   int best_card = -1; // Initialize the best card to -1 (no card selected yet)
//   int best_row = -1;  // Initialize the best row to -1 (no row selected yet)
//   int max_playable_after = -1; // Initialize the maximum playable cards after to -1

//   // Iterate through each card in the player's hand
//   for (int i = 0; i < hand.size(); ++i)
//   {
//    // Iterate through each row in the playing area
//    for (int j = 0; j < NUMBER_OF_ROWS; ++j)
//    {
//     // Check if the current card can be played on the current row
//     if (is_valid_move(hand[i], playing_rows[j].back(), j < NUMBER_OF_ROWS / 2) != ValidMove::NO)
//     {
//      // Simulate the move (create copies of playing rows)
//      std::vector<std::vector<int>> temp_rows = playing_rows;
//      temp_rows[j].push_back(hand[i]);

//      int playable_after = 0; // Initialize the count of playable cards after the move
//      // Iterate through the remaining cards in the hand
//      for (int k = 0; k < hand.size(); ++k)
//      {
//       // Skip the card that was just "played"
//       if (hand[k] != hand[i])
//       {
//        // Iterate through each row to check playability
//        for (int l = 0; l < NUMBER_OF_ROWS; ++l)
//        {
//         // If the remaining card is playable on any row
//         if (is_valid_move(hand[k], temp_rows[l].back(), l < NUMBER_OF_ROWS / 2) != ValidMove::NO)
//         {
//          // Increment the count of playable cards and break the inner loop
//          playable_after++;
//          break;
//         }
//        }
//       }
//      }

//      // If this move results in more playable cards than the current maximum
//      if (playable_after > max_playable_after)
//      {
//       // Update the maximum playable cards, best card, and best row
//       max_playable_after = playable_after;
//       best_card = hand[i];
//       best_row = j;
//      }
//     }
//    }
//   }
//   // Return the best card and row as a pair
//   return {best_card, best_row};
//  }

//  /**
//   * @brief Strategy D: Prioritizes ascending rows and minimizes the maximum card left in hand.
//   *
//   * This strategy first tries to play on ascending rows
// * and aims to minimize the maximum card left in hand after the move.
//   * If no valid move is found on ascending rows, it falls back to Strategy A.
//   *
//   * @param hand The player's current hand of cards.
//   * @param playing_rows The current state of the playing rows.
//   * @return A pair containing the best card to play and the row index, or {-1, -1} if no valid move.
//   */
//  std::pair<int, int> get_player_move_D(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows)
//  {
//   int best_card = -1; // Initialize the best card to -1 (no card selected yet)
//   int best_row = -1;  // Initialize the best row to -1 (no row selected yet)
//   int min_max_card = CARD_MAX_NUMBER + 1; // Initialize the minimum maximum card to a large value

//   // Iterate through each card in the player's hand
//   for (int i = 0; i < hand.size(); ++i)
//   {
//    // Iterate through the ascending rows (first half of playing_rows)
//    for (int j = 0; j < NUMBER_OF_ROWS / 2; ++j)
//    {
//     // Check if the current card can be played on the current row
//     if (is_valid_move(hand[i], playing_rows[j].back(), true) != ValidMove::NO)
//     {
//      // Simulate the move and find the maximum card in the remaining hand
//      std::vector<int> temp_hand = hand;
//      temp_hand.erase(temp_hand.begin() + i); // Erase the played card from the temporary hand

//      int max_card = 0; // Initialize the maximum card in the remaining hand
//      // If the temporary hand is not empty
//      if (!temp_hand.empty())
//      {
//       // Find the maximum card in the remaining hand
//       max_card = *std::max_element(temp_hand.begin(), temp_hand.end());
//      }

//      // If the maximum card in the remaining hand is smaller than the current minimum maximum card
//      if (max_card < min_max_card)
//      {
//       // Update the minimum maximum card, best card, and best row
//       min_max_card = max_card;
//       best_card = hand[i];
//       best_row = j;
//      }
//     }
//    }
//   }

//   // If no move was found on the ascending rows
//   if (best_card == -1)
//   {
//    // Fall back to Strategy A (closest card, with reverse)
//    return get_player_move_A(hand, playing_rows);
//   }

//   // Return the best card and row as a pair
//   return {best_card, best_row};
//  }

//  /**
//   * @brief Strategy E: Combination of Strategy C and Strategy A.
//   *
//   * This strategy considers future playability (like Strategy C) but uses
//   * the closest card (like Strategy A) as a tie-breaker.
//   *
//   * @param hand The player's current hand of cards.
//   * @param playing_rows The current state of the playing rows.
//   * @return A pair containing the best card to play and the row index, or {-1, -1} if no valid move.
//   */
//  std::pair<int, int> get_player_move_E(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows)
//  {
//   int best_card = -1; // Initialize the best card to -1 (no card selected yet)
//   int best_row = -1;  // Initialize the best row to -1 (no row selected yet)
//   int max_playable_after = -1; // Initialize the maximum playable cards after to -1
//   int min_diff = CARD_MAX_NUMBER * 2; // Initialize the minimum difference to a large value

//   // Iterate through each card in the player's hand
//   for (int i = 0; i < hand.size(); ++i)
//   {
//    // Iterate through each row in the playing area
//    for (int j = 0; j < NUMBER_OF_ROWS; ++j)
//    {
//     // Check if the current card can be played on the current row
//     if (is_valid_move(hand[i], playing_rows[j].back(), j < NUMBER_OF_ROWS / 2) != ValidMove::NO)
//     {
//      // Simulate the move (create copies of playing rows)
//      std::vector<std::vector<int>> temp_rows = playing_rows;
//      temp_rows[j].push_back(hand[i]);

//      int playable_after = 0; // Initialize the count of playable cards after the move
//      // Iterate through the remaining cards in the hand
//      for (int k = 0; k < hand.size(); ++k)
//      {
//       // Skip the card that was just "played"
//       if (static_cast<unsigned long long>(i) != k)
//       {
//        // Iterate through each row to check playability
//        for (int l = 0; l < NUMBER_OF_ROWS; ++l)
//        {
//         // If the remaining card is playable on any row
//         if (is_valid_move(hand[k], temp_rows[l].back(), l < NUMBER_OF_ROWS / 2) != ValidMove::NO)
//         {
//          // Increment the count of playable cards and break the inner loop
//          playable_after++;
//          break;
//         }
//        }
//       }
//      }

//      // Calculate the difference between the card and the row's top card
//      int diff = std::abs(hand[i] - playing_rows[j].back());
//      if (is_valid_move(hand[i], playing_rows[j].back(), j < NUMBER_OF_ROWS / 2) == ValidMove::REVERSE_MOVE)
//       diff = -1;

//      // Tie-breaker logic: If playable_after is the same, choose the smaller diff
//      if (playable_after > max_playable_after)
//      {
//       // Update the maximum playable cards, best card, best row, and minimum difference
//       max_playable_after = playable_after;
//       min_diff = diff;
//       best_card = hand[i];
//       best_row = j;
//      }
//      else if (playable_after == max_playable_after && diff < min_diff)
//      {
//       // Update the minimum difference, best card, and best row
//       min_diff = diff;
//       best_card = hand[i];
//       best_row = j;
//      }
//     }
//    }
//   }
//   // Return the best card and row as a pair
//   return {best_card, best_row};
//  }

//  /**
//   * @brief Strategy F: Avoids filling rows too quickly by maximizing the minimum gap.
//   *
//   * This strategy tries to keep the gaps between playable cards as large as possible.
//   * It simulates playing each card and calculates the minimum gap in all rows after the move.
//   * The card that results in the largest minimum gap is chosen.
//   *
//   * @param hand The player's current hand of cards.
//   * @param playing_rows The current state of the playing rows.
//   * @return A pair containing the best card to play and the row index, or {-1, -1} if no valid move.
//   */
//  std::pair<int, int> get_player_move_F(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows)
//  {
//   int best_card = -1; // Initialize the best card to -1 (no card selected yet)
//   int best_row = -1;  // Initialize the best row to -1 (no row selected yet)
//   int max_min_gap = -1; // Initialize the maximum minimum gap to -1

//   // Iterate through each card in the player's hand
//   for (int i = 0; i < hand.size(); ++i)
//   {
//    // Iterate through each row in the playing area
//    for (int j = 0; j < NUMBER_OF_ROWS; ++j)
//    {
//     // Check if the current card can be played on the current row
//     if (is_valid_move(hand[i], playing_rows[j].back(), j < NUMBER_OF_ROWS / 2) != ValidMove::NO)
//     {
//      // Simulate the move (create copies of playing rows)
//      std::vector<std::vector<int>> temp_rows = playing_rows;
//      temp_rows[j].push_back(hand[i]);

//      // Calculate the minimum gap in *all* rows after the move
//      int min_gap = CARD_MAX_NUMBER * 2; // Initialize the minimum gap to a large value
//      // Iterate through each row
//      for (int row = 0; row < NUMBER_OF_ROWS; ++row)
//      {
//       int current_card = temp_rows[row].back(); // Get the top card of the current row
//       // Iterate through each card in the hand (except the one being played)
//       for (int next_card : hand)
//       {
//        if (next_card != hand[i])
//        {
//         // Check if the next card can be played on the current row
//         ValidMove vm = is_valid_move(next_card, current_card, row < NUMBER_OF_ROWS / 2);
//         if (vm != ValidMove::NO)
//         {
//          int gap;
//          // If it's a reverse move, set the gap to -1
//          if (vm == ValidMove::REVERSE_MOVE)
//          {
//           gap = -1;
//          }
//          else
//          {
//           // Otherwise, calculate the absolute difference between the cards
//           gap = std::abs(next_card - current_card);
//          }
//          // Update the minimum gap if the current gap is smaller
//          min_gap = std::min(min_gap, gap);
//         }
//        }
//       }
//      }

//      // If the minimum gap for this move is larger than the current maximum minimum gap
//      if (min_gap > max_min_gap)
//      {
//       // Update the maximum minimum gap, best card, and best row
//       max_min_gap = min_gap;
//       best_card = hand[i];
//       best_row = j;
//      }
//     }
//    }
//   }
//   // Return the best card and row as a pair
//   return {best_card, best_row};
//  }

//  /**
//   * @brief Strategy G: Weighted combination of Strategies A, C, and F.
//   *
//   * This strategy combines the scores from Strategies A (closeness), C (future playability),
//   * and F (gap maximization) using weighted factors. The card with the highest combined
//   * score is chosen.
//   *
//   * @param hand The player's current hand of cards.
//   * @param playing_rows The current state of the playing rows.
//   * @return A pair containing the best card to play and the row index, or {-1, -1} if no valid move.
//   */
//  std::pair<int, int> get_player_move_G(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows)
//  {
//   const double weight_A = 0.2; // Weight for Strategy A (closeness)
//   const double weight_C = 0.5; // Weight for Strategy C (future playability)
//   const double weight_F = 0.3; // Weight for Strategy F (gap maximization)

//   int best_card = -1; // Initialize the best card to -1 (no card selected yet)
//   int best_row = -1;  // Initialize the best row to -1 (no row selected yet)
//   double best_score = -1e9; // Initialize the best score to a very low value

//   // Iterate through each card in the player's hand
//   for (int i = 0; i < hand.size(); ++i)
//   {
//    // Iterate through each row in the playing area
//    for (int j = 0; j < NUMBER_OF_ROWS; ++j)
//    {
//     // Check if the current card can be played on the current row
//     if (is_valid_move(hand[i], playing_rows[j].back(), j < NUMBER_OF_ROWS / 2) != ValidMove::NO)
//     {
//      double score_A = 0.0, score_C = 0.0, score_F = 0.0; // Initialize scores for each strategy

//      // --- Score for Strategy A (Closeness) ---
//      int diff = std::abs(hand[i] - playing_rows[j].back());
//      // Treat reverse-10 moves as a very small difference
//      if (is_valid_move(hand[i], playing_rows[j].back(), j < NUMBER_OF_ROWS / 2) == ValidMove::REVERSE_MOVE)
//      {
//       diff = -1;
//      }
//      // Invert the difference so that smaller differences get higher scores
//      score_A = (diff == -1) ? CARD_MAX_NUMBER : static_cast<double>(CARD_MAX_NUMBER - diff);

//      // --- Score for Strategy C (Future Playability) ---
//      std::vector<std::vector<int>> temp_rows = playing_rows; // Copy playing_rows
//      temp_rows[j].push_back(hand[i]); // Simulate the move

//      int playable_after = 0; // Initialize the count of playable cards after the move
//      // Iterate through the remaining cards in the hand
//      for (int k = 0; k < hand.size(); ++k)
//      {
//       // Skip the card that was just "played"
//       if (static_cast<unsigned long long>(i) != k)
//       {
//        // Iterate through each row to check playability
//        for (int l = 0; l < NUMBER_OF_ROWS; ++l)
//        {
//         // If the remaining card is playable on any row
//         if (is_valid_move(hand[k], temp_rows[l].back(), l < NUMBER_OF_ROWS / 2) != ValidMove::NO)
//         {
//          // Increment the count of playable cards and break the inner loop
//          playable_after++;
//          break;
//         }
//        }
//       }
//      }
//      // Calculate the score for Strategy C based on the number of playable cards after the move
//      score_C = static_cast<double>(playable_after);

//      // --- Score for Strategy F (Gap Maximization) ---
//      std::vector<std::vector<int>> temp_rows2 = playing_rows; // Another copy for F
//      temp_rows2[j].push_back(hand[i]);

//      // Calculate the minimum gap in *all* rows after the simulated move.
//      int min_gap = CARD_MAX_NUMBER * 2; // Initialize with a large gap
//      // Iterate through each row
//      for (int row = 0; row < NUMBER_OF_ROWS; ++row)
//      {
//       int current_card = temp_rows2[row].back(); // Get the top card of the current row
//       // Iterate through each card in the hand (except the one being played)
//       for (int next_card : hand)
//       {
//        if (next_card != hand[i])
//        { // Don't compare the played card with itself
//         ValidMove vm = is_valid_move(next_card, current_card, row < NUMBER_OF_ROWS / 2);
//         if (vm != ValidMove::NO)
//         {
//          int gap;
//          if (vm == ValidMove::REVERSE_MOVE)
//          {
//           gap = -1; // Treat reverse-10 as a minimal gap
//          }
//          else
//          {
//           gap = std::abs(next_card - current_card);
//          }
//          min_gap = std::min(min_gap, gap); // Find the *smallest* gap
//         }
//        }
//       }
//      }
//      // Calculate the score for Strategy F based on the minimum gap
//      score_F = static_cast<double>(min_gap);

//      // --- Combine Scores using Weights ---
//      double total_score = weight_A * score_A + weight_C * score_C + weight_F * score_F;

//      // Update best move if this move has a higher combined score
//      if (total_score > best_score)
//      {
//       best_score = total_score;
//       best_card = hand[i];
//       best_row = j;
//      }
//     }
//    }
//   }
//   // Return the best card and row as a pair
//   return {best_card, best_row};
//  }

//  /**
//   * @brief Strategy H: "Panic Mode" - If few moves are left, play the largest/smallest possible card.
//   *
//   * This strategy counts the total number of valid moves. If there are very few valid moves left,
//   * it tries to force a play by playing the largest possible card on an ascending row or the
//   * smallest possible card on a descending row. Otherwise, it defaults to Strategy E.
//   *
//   * @param hand The player's current hand of cards.
//   * @param playing_rows The current state of the playing rows.
//   * @return A pair containing the best card to play and
// * the row index, or {-1, -1} if no valid move.
//   */
//  std::pair<int, int> get_player_move_H(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows)
//  {
//   int total_valid_moves = 0; // Initialize the count of valid moves
//   // Iterate through each card in the hand
//   for (int card : hand)
//   {
//    // Iterate through each row in the playing area
//    for (int j = 0; j < NUMBER_OF_ROWS; ++j)
//    {
//     // If the card can be played on the current row
//     if (is_valid_move(card, playing_rows[j].back(), j < NUMBER_OF_ROWS / 2) != ValidMove::NO)
//     {
//      // Increment the count of valid moves
//      total_valid_moves++;
//     }
//    }
//   }

//   // If there are very few valid moves left (2 or less)
//   if (total_valid_moves <= 2)
//   {
//    int best_card = -1; // Initialize the best card to -1 (no card selected yet)
//    int best_row = -1;  // Initialize the best row to -1 (no row selected yet)

//    // Try to play the largest possible card on an ascending row, or the smallest on a descending row
//    for (int i = 0; i < hand.size(); ++i)
//    {
//     for (int j = 0; j < NUMBER_OF_ROWS; ++j)
//     {
//      // If the card can be played on the current row
//      if (is_valid_move(hand[i], playing_rows[j].back(), j < NUMBER_OF_ROWS / 2) != ValidMove::NO)
//      {
//       // If it's an ascending row
//       if (j < NUMBER_OF_ROWS / 2)
//       {
//        // Choose the largest card
//        if (best_card == -1 || hand[i] > best_card)
//        {
//         best_card = hand[i];
//         best_row = j;
//        }
//       }
//       else
//       { // If it's a descending row
//        // Choose the smallest card
//        if (best_card == -1 || hand[i] < best_card)
//        {
//         best_card = hand[i];
//         best_row = j;
//        }
//       }
//      }
//     }
//    }
//    // If a move was forced, return it
//    if (best_card != -1)
//     return {best_card, best_row};
//   }

//   // Otherwise, default to Strategy E (a good general-purpose strategy)
//   return get_player_move_E(hand, playing_rows);
//  }

//  /**
//   * @brief Strategy I: Minimizes cards blocking 1 and 100 at the start of the game.
//   *
//   * This strategy counts how many cards in the hand block the initial ascending and descending rows.
//   * If more cards block the ascending rows, it uses Strategy D (prioritize ascending rows).
//   * Otherwise, it uses a new strategy that prioritizes descending rows and aims to maximize
//   * the minimum card left in hand after the move.
//   *
//   * @param hand The player's current hand of cards.
//   * @param playing_rows The current state of the playing rows.
//   * @return A pair containing the best card to play and the row index, or {-1, -1} if no valid move.
//   */
//  std::pair<int, int> get_player_move_I(const std::vector<int> &hand, const std::vector<std::vector<int>> &playing_rows)
//  {
//   // Check how many cards on hand block 1 and 100.
//   int cards_blocking_top_rows = 0;    // Initialize the count of cards blocking ascending rows
//   int cards_blocking_bottom_rows = 0; // Initialize the count of cards blocking descending rows
//   // Iterate through each card in the hand
//   for (int card : hand)
//   {
//    // If the card is smaller than the first card in the hand (blocks ascending rows)
//    if (card < hand[0])
//    {
//     // Increment the count of cards blocking ascending rows
//     cards_blocking_top_rows += 1;
//    }
//    // If the card is larger than the first card in the hand (blocks descending rows)
//    if (card > hand[0])
//    {
//     // Increment the count of cards blocking descending rows
//     cards_blocking_bottom_rows += 1;
//    }
//   }
//   // If more cards block the ascending rows
//   if (cards_blocking_top_rows >= cards_blocking_bottom_rows)
//   {
//    // Use Strategy D (prioritize ascending rows)
//    return get_player_move_D(hand, playing_rows);
//   }
//   else
//   {
//    // Create a strategy that focuses on playing descending cards:
//    int best_card = -1; // Initialize the best card to -1 (no card selected yet)
//    int best_row = -1;  // Initialize the best row to -1 (no row selected yet)
//    int max_max_card = 0; // Initialize the maximum maximum card to 0

//    // Iterate through each card in the player's hand
//    for (int i = 0; i < hand.size(); ++i)
//    {
//     // Iterate through the descending rows (second half of playing_rows)
//     for (int j = NUMBER_OF_ROWS / 2; j < NUMBER_OF_ROWS; ++j)
//     {
//      // Check if the current card can be played on the current row (descending)
//      if (is_valid_move(hand[i], playing_rows[j].back(), false) != ValidMove::NO)
//      {
//       // Simulate the move and find the minimum card in the remaining hand
//       std::vector<int> temp_hand = hand;
//       temp_hand.erase(temp_hand.begin() + i); // Erase the played card from the temporary hand

//       int min_card = CARD_MAX_NUMBER; // Initialize the minimum card in the remaining hand
//       // If the temporary hand is not empty
//       if (!temp_hand.empty())
//       {
//        // Find the minimum card in the remaining hand
//        min_card = *std::min_element(temp_hand.begin(), temp_hand.end());
//       }

//       // If the minimum card in the remaining hand is larger than the current maximum maximum card
//       if (min_card > max_max_card)
//       {
//        // Update the maximum maximum card, best card, and best row
//        max_max_card = min_card;
//        best_card = hand[i];
//        best_row = j;
//       }
//      }
//     }
//    }
//    // If no move was found on the descending rows
//    if (best_card == -1)
//    {
//     // Fall back to Strategy A (closest card, with reverse)
//     return get_player_move_A(hand, playing_rows);
//    }

//    // Return the best card and row as a pair
//    return {best_card, best_row};
//   }
//  }

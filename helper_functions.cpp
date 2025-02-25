#include "helper_functions.h"
#include <random>
#include <iostream>
#include <algorithm>

// Constants
extern int CARD_MAX_NUMBER;
extern int REVERSE_MOVE_DIFF;
extern int CARD_IN_HANDS;
extern int NUM_CARDS_TO_PLAY;
extern int NUMBER_OF_ROWS;

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

    return deck;
}

std::vector<int> deal_cards(std::vector<int> &deck, int num_cards)
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

ValidMove is_valid_move(int card, int row_top, bool is_ascending, bool reverse_move_allowed)
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
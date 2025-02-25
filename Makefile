the_game: main.cpp helper_functions.cpp player_strategies.cpp game_logic.cpp
	g++ -o the_game main.cpp helper_functions.cpp player_strategies.cpp game_logic.cpp

clean:
	rm -f the_game
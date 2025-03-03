rm -rf out
mkdir -p out

make clean
make

./the_game --config 1p_config.txt > out/1p_simulation.out
./the_game --config 2p_config.txt > out/2p_simulation.out
./the_game --config 3p_config.txt > out/3p_simulation.out
./the_game --config 4p_config.txt > out/4p_simulation.out
./the_game --config 5p_config.txt > out/5p_simulation.out

sed -n '/Game Results/,$p' out/1p_simulation.out > out/1p_game_results.out
sed -n '/Game Results/,$p' out/2p_simulation.out > out/2p_game_results.out
sed -n '/Game Results/,$p' out/3p_simulation.out > out/3p_game_results.out
sed -n '/Game Results/,$p' out/4p_simulation.out > out/4p_game_results.out
sed -n '/Game Results/,$p' out/5p_simulation.out > out/5p_game_results.out

sed -n '/1 Player/,$p' out/1p_simulation.out >> out/statistics.out
sed -n '/2 Player/,$p' out/2p_simulation.out >> out/statistics.out
sed -n '/3 Player/,$p' out/3p_simulation.out >> out/statistics.out
sed -n '/4 Player/,$p' out/4p_simulation.out >> out/statistics.out
sed -n '/5 Player/,$p' out/5p_simulation.out >> out/statistics.out

sed  '/1 Players/Q' out/1p_game_results.out >> out/game_results.out
sed  '/2 Players/Q' out/2p_game_results.out >> out/game_results.out
sed  '/3 Players/Q' out/3p_game_results.out >> out/game_results.out
sed  '/4 Players/Q' out/4p_game_results.out >> out/game_results.out
sed  '/5 Players/Q' out/5p_game_results.out >> out/game_results.out

python3 csv_generation.py
python3 game_analysis.py
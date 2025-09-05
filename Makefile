parallel:
	rm -f *.o main
	mpic++ -std=c++20  -fcommon -Wl,--allow-multiple-definition main.cpp UF.cpp -o main

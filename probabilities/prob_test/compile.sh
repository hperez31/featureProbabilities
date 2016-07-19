# compile commmand to validate samples renault ..
g++ -O3 -std=c++0x -o test test.c ../prob/bdd_probability.c -lgmp -lbdd -I ../../../buddy-2.4/src -I ../prob/ 


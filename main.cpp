#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdint>
#include <cstdio>
#include <sstream>
#include <unistd.h>
#include <stdio.h>

#include "Predictor.h"

using namespace std;

int main()
{
    clock_t begin = clock(), end;
    vector<int> sample = {1,-25,2,3,6,5,-7,75,12,369,-852,5};
    vector<int> sampleSecond = {5,7,8,3,1,5};
    //int sample1[6] = {1,1,2,3,6,5};
    Statistics a;
//    Predictor b = Predictor("../example.txt");
//    b.printFormula();

    //char cwd[1024];
    //getcwd(cwd, sizeof(cwd));
    //printf("Current working dir: %s\n", cwd);

    Predictor b = Predictor("../test_examples/example_ubuntu.txt");
    //Predictor b = Predictor("../test_examples/example.txt");
    //Predictor b = Predictor("C:\\Users\\nevidjen\\Desktop\\Master\\knfV1\\flat50-904.cnf");
    //cout << "Formula: " << endl;
    //b.printFormula();

//    a = Predictor::computeStatistics(sample);
//    Predictor::printVector(sample);
//    Predictor::printStatistic(a);

    //cout << Predictor::computeCorelation(sample, sampleSecond) << endl;

    end = clock();
    std::cout << "Whole program took: " << static_cast<double>(end - begin) / CLOCKS_PER_SEC << "s" << std::endl;
    return 0;
}

#ifndef PREDICTOR_H
#define PREDICTOR_H

#include <cstdlib>
#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <ctime>
#include <exception>
#include <sstream>
#include <cstdio>
#include <thread>

using namespace std;

//broj niti koji rade na racunu
#define NUM_OF_THREADS 8

//Prosek, minimum, maksimum, koeficijent varijacije, entropija, nakrivljenost i tezina repova
struct Statistics
{
    int minimum;
    int maximum;
    double average;
    double variationCoeficient;
    double entropy;
    double skew;
    double kurtosis;

    Statistics() : minimum(0),
        maximum(0),
        average(0.0),
        variationCoeficient(0.0),
        entropy(0.0),
        skew(0.0),
        kurtosis(0.0) {}

    bool operator=(Statistics rhs)
    {
        try
        {
            average = rhs.average;
            minimum = rhs.minimum;
            maximum = rhs.maximum;
            variationCoeficient = rhs.variationCoeficient;
            entropy = rhs.entropy;
            skew = rhs.skew;
            kurtosis = rhs.kurtosis;

            return true;
        }
        catch (const std::exception&)
        {
            return false;
        }
    }
};

class Predictor
{

public:
    Predictor();
    Predictor(string path);
    ~Predictor();
    static Statistics computeStatistics(vector<int>& sample);
    static Statistics computeStatistics(int* sample, unsigned size);
    static double computeCorelation(int* array1, int sizeArray1, int* array2, int sizeArray2);
    static double computeCorelation(vector<int> array1, vector<int> array2);
    static void printStatistic(Statistics stat);
    static void printArray(int* sample, int length);
    static void printArrayOfArray(int** sample, int lengthArray, int lengthSubarray);
    static void printVectorOfVectors(vector<vector<int> > sample);
    static void printVector(vector<int> sample);
    static void allocateMemory(void** pointer, size_t sizeOf, size_t members);
    static void reallocateMemory(void** pointer, size_t sizeOf, size_t members);
    static int computeNonZeroAppearance(vector<int> array);
    static int computeNonZeroAppearance(int* array, unsigned sizeArray);
    void printFormula();

private:
    unsigned clauses = 0, variables = 0;

    void parseLine(const char* line, bool* p_line, size_t* clause_parsed);
    void parse(string path);
    void parse();
    void initializeStatistics();
    void freeMemory(void** pointer);
    void freeObjectMemory();
    void countNumOfAppearenceNegativeOrPositive(unsigned i, unsigned j,
                                         int begin, int step,
                                         int** clause_num_appear_positive,
                                         int** clause_num_appear_negative);

    //whole formula in cnf
    //vector<vector<int> > formula;
    int** formula = NULL;
    unsigned* formulaLength = NULL;

    //Statistike duzina klauza u kojima se promenljiva pojavljuje, duzina niza je broj promenljivih
    Statistics* clauseLengthStatistics = NULL;
    //Statistike duzina klauza u kojima se promenljiva pojavljuje kao pozitivna, duzina niza je broj promenljivih
    Statistics* clauseLengthStatisticsP = NULL;
    //Statistike duzina klauza u kojima se pojavljuje kao negativna, duzina niza je broj promenljivih
    Statistics* clauseLengthStatisticsN = NULL;

    //Statistike broja klauza u kojima se data promenljiva pojavljuje sa drugim promenljivim, duzina niza je broj promenljivih
    Statistics* clauseNumberStatistics = NULL;
    //Statistike broja klauza u kojima se data promenljiva pojavljuje sa drugim promenljivim kao pozitivna, duzina niza je broj promenljivih
    Statistics* clauseNumberStatisticsP = NULL;
    //Statistike broja klauza u kojima se data promenljiva pojavljuje sa drugim promenljivim kao negativna, duzina niza je broj promenljivih
    Statistics* clauseNumberStatisticsN = NULL;

    //Korelacija izmedju polariteta promenljive i duzine klauza u kojima se promenljiva pojavljuje, duzina niza je broj promenljivih
    double* corelations = NULL;

    //Udeo binarnih klauza u kojima se promenljiva pojavljuje medju svim klauzama u kojima se promenljiva pojavljuje, duzina niza je broj promenljivih
    double* binaryPercentage = NULL;

    //Udeo Hornovih klauza u kojima se promenljiva pojavljuje medju svim klauzama u kojima se promenljiva pojavljuje, duzina niza je broj promenljivih
    double* hornPercentage = NULL;

    //Udeo Hornovih klauza kojima je promenljiva u zakljucku medju svim Hornovim klauzama u kojima se promenljiva pojavljuje
    double* hornPercentageImplicate = NULL;
    int hornPercentageImplicateLength = 0;

    //Udeo pozitivnih pojavljivanja promenljive medju svim pojavljivanjima promenljive, duzina niza je broj promenljivih
    double* positiveAppearPercentage = NULL;

    //Udeo klauza u kojima se promenljiva pojavljuje medju svim klauzama, duzina niza je broj promenljivih
    double* appearPercentage = NULL;

    //Udeo promenljivih koje se javljaju sa datom promenljivom u bar jednoj klauzi, duzina niza je broj promenljivih
    double* withVarAppearPercentage = NULL;

    //Udeo pozitivnih literala u klauzama u kojima se promenljiva pojavljuje, duzina niza je broj promenljivih
    double* positiveLiteralsAppearPercentage = NULL;
};

#endif //PREDICTOR_H

#include "Predictor.h"

Predictor::Predictor()
{
    cout << "using empty one" << endl;
    parse();
    if(formula != NULL && formulaLength != NULL)
        initializeStatistics();
}

Predictor::Predictor(string path)
{
    cout << "using file" << endl;
    parse(path);

    if(formula != NULL && formulaLength != NULL)
        initializeStatistics();
}

Predictor::~Predictor()
{
    freeObjectMemory();
}

void Predictor::parseLine(const char* line, bool *p_line, size_t* clause_parsed)
{
    char commentString[1000];
    //fiksirana velicina duzine klauza:
    //ako je kraca, radice sporije (realokacija)!
    unsigned clause_length = 100;

    //ako nismo procitali "p cnf" cekamo da je procitamo, ili eventualno komentar
    if (!(*p_line))
    {
        char p, cnf[5];

        if (sscanf(line, "%c%3s%d%d", &p, cnf, &variables, &clauses) == 4)
        {
            if (p != 'p')
                throw logic_error("Error while reading p line!");
            cout << p << "; " << cnf << "; " << variables << "; " << clauses << endl;
            *p_line = true;
            //alocate memory for CNF formula
            //formula = vector<vector<int> >(clauses, vector<int>(clause_length));

            allocateMemory((void**)&formulaLength, sizeof(int), clauses);
            allocateMemory((void**)&formula, sizeof(int*), clauses);
            for (unsigned i = 0; i < clauses; i++)
            {
                allocateMemory((void**)&(formula[i]), sizeof(int), clause_length);
            }
        }
        else if ((sscanf(line, "%c%s", &p, commentString) == 2) ||
                 (sscanf(line, "%c", &p) == 1))
        {
            if (p != 'c')
                throw logic_error("Wrong format of comment!");
            cout << "Comment read (1)" << endl;
        }
        else
            throw logic_error("Wrong format of input file!");
    }
    //ako smo procitali "p cnf" citamo klauzu, ili eventualno komentar
    else
    {
        int parsed_num;
        char c;

        if (sscanf(line, "%d", &parsed_num) == 1)
        {
            if (parsed_num == 0)
            {
                (*clause_parsed)++;
                //ako procitamo neku klauzu a nismo je do sada uocili
                (*clause_parsed) > clauses ? throw logic_error("Too many clauses read!"): true;
            }
            else
            {
                //ako krenemo da citamo neku novu klauzu
                if((*clause_parsed) >= clauses)
                    throw logic_error("Too many clauses read!");

                char* pointer = (char*)line;
                int num;

                while (true)
                {
                    num = strtol(pointer, &pointer, 10);

                    if (num == 0 && num != LONG_MAX && num != LONG_MIN && *pointer == '\0')
                    {
                        (*clause_parsed)++;
                        break;
                    }
                    else
                    {
                        static_cast<unsigned>(abs(num)) > variables ?
                                    throw logic_error("Error while parsing variable: "+ std::to_string(num)) : true;

                        formula[*clause_parsed][formulaLength[*clause_parsed]] = num;
                        formulaLength[*clause_parsed]++;

                        if (static_cast<unsigned>(formulaLength[*clause_parsed]) >= clause_length)
                        {
                            clause_length *= 2;
                            reallocateMemory((void**)(&(formula[*clause_parsed])), sizeof(int), clause_length);
                        }
                    }
                }

                if(num != 0)
                {
                    throw logic_error("Wrong clause input!");
                }

            }
        }
        else if ((sscanf(line, "%c%s", &c, commentString) == 2) ||
                 (sscanf(line, "%c", &c) == 1))
        {
            if (c != 'c')
                throw logic_error("Wrong format of comment!");
            cout << "Comment read (2)" << endl;
        }
        else
            throw logic_error("Wrong format of input file!");
    }
}

void Predictor::parse()
{
    string line;
    bool p_line = false;
    size_t clause_parsed = 0;

    try
    {
        while (getline(cin, line))
        {
            //if empty than exit
            if (line.compare("") == 0)
                break;

            parseLine(line.c_str(), &p_line, &clause_parsed);
        }

        if(clause_parsed != clauses)
            throw logic_error("Not enough clauses parsed. Maybe missed zero to end clause?");
    }
    catch (const std::exception& a)
    {
        freeObjectMemory();
        string format_rules = "c\n"
            "c start with comments\n"
            "c\n"
            "c\n"
            "p cnf 5 3\n"
            "1 -5 4 0\n"
            "-1 5 3 4 0\n"
            "-3 -4 0\n";
        cout << a.what() << " Example: " << endl;
        cout << format_rules << endl;
    }
}

void Predictor::parse(string path)
{
    clock_t begin, end;
    begin = clock();

    char temp[10000];
    string line(temp);
    bool p_line = false;
    size_t clause_parsed = 0;

    ifstream file(path);
    try
    {
        while (getline(file, line))
        {
            //if empty than exit
            if (line.compare("") == 0)
                break;

            parseLine(line.c_str(), &p_line, &clause_parsed);
        }

        if(clause_parsed != clauses)
            throw logic_error("Not enough clauses parsed. Maybe missed zero to end clause?");
    }
    catch (const std::exception& a)
    {
        freeObjectMemory();

        string format_rules = "c\n"
            "c start with comments\n"
            "c\n"
            "c\n"
            "p cnf 5 3\n"
            "1 -5 4 0\n"
            "-1 5 3 4 0\n"
            "-3 -4 0\n";
        cout << a.what() << " Example: " << endl;
        cout << format_rules << endl;
    }

    end = clock();
    cout << "Parsing: " << static_cast<double>(end - begin) / CLOCKS_PER_SEC << "s" << endl;
}

void Predictor::initializeStatistics()
{
    clock_t begin = clock(), end;
    int** clause_num_appear;
    int** clause_num_appear_positive;
    int** clause_num_appear_negative;
    int* horn_clauses_implicate;
    int* horn_clauses;
    int* binary_clauses;

    //Duzine klauza u kojima se promenljiva pojavljuje, pozitivna i negativna
    vector<vector<int> > length_appear(variables);
    vector<vector<int> > length_appear_positive(variables);
    vector<vector<int> > length_appear_negative(variables);

    //Broj klauza u kojima se data promenljiva pojavljuje sa drugim promenljivim, pozitivna, negativna
    allocateMemory((void**)&clause_num_appear, sizeof(int*), variables);
    allocateMemory((void**)&clause_num_appear_positive, sizeof(int*), variables);
    allocateMemory((void**)&clause_num_appear_negative, sizeof(int*), variables);

    for (unsigned l = 0; l < variables; l++)
    {
        allocateMemory((void**)(&clause_num_appear_positive[l]), sizeof(int), variables);
        allocateMemory((void**)(&clause_num_appear_negative[l]), sizeof(int), variables);
        allocateMemory((void**)(&clause_num_appear[l]), sizeof(int), variables);
    }

    //Korelacija izmedju polariteta promenljive i duzine klauza u kojima se promenljiva pojavljuje
    vector<vector<int> > polarity(variables);

    //Udeo binarnih i hornovih klauza
    allocateMemory((void**)&binary_clauses, sizeof(int), variables);
    allocateMemory((void**)&horn_clauses, sizeof(int), variables);

    //Udeo Hornovih klauza kojima je promenljiva u zakljucku u odnosu na sve Hornove
    allocateMemory((void**)&horn_clauses_implicate, sizeof(int), variables);

    //allocation memory for statistics
    allocateMemory((void**)&binaryPercentage, sizeof(double), variables);
    allocateMemory((void**)&hornPercentage, sizeof(double), variables);
    allocateMemory((void**)&hornPercentageImplicate, sizeof(double), variables);
    allocateMemory((void**)&positiveAppearPercentage, sizeof(double), variables);
    allocateMemory((void**)&appearPercentage, sizeof(double), variables);
    allocateMemory((void**)&withVarAppearPercentage, sizeof(double), variables);
    allocateMemory((void**)&positiveLiteralsAppearPercentage, sizeof(double), variables);
    allocateMemory((void**)&corelations, sizeof(double), variables);
    allocateMemory((void**)&clauseLengthStatistics, sizeof(Statistics), variables);
    allocateMemory((void**)&clauseLengthStatisticsP, sizeof(Statistics), variables);
    allocateMemory((void**)&clauseLengthStatisticsN, sizeof(Statistics), variables);
    allocateMemory((void**)&clauseNumberStatistics, sizeof(Statistics), variables);
    allocateMemory((void**)&clauseNumberStatisticsP, sizeof(Statistics), variables);
    allocateMemory((void**)&clauseNumberStatisticsN, sizeof(Statistics), variables);
    //end allocating

    //collect data for statistics
    for (unsigned i = 0; i < clauses; i++)
    {
        int numOfPositiveAppereance = 0, positiveVariable = 0;

        for (int j = 0; j < formulaLength[i]; j++)
        {
            //check if it's horn clause
            if (formula[i][j] > 0)
            {
                numOfPositiveAppereance++;
                positiveVariable = formula[i][j];
            }

            //time consuming part - statistics second part
            for (int k = j+1; k < formulaLength[i]; k++)
            {
                if (formula[i][j] > 0)
                {
                    clause_num_appear_positive[abs(formula[i][j])-1][abs(formula[i][k])-1]++;
                    if(formula[i][k] > 0)
                        clause_num_appear_positive[abs(formula[i][k])-1][abs(formula[i][j])-1]++;
                    else
                        clause_num_appear_negative[abs(formula[i][k])-1][abs(formula[i][j])-1]++;
                }
                else
                {
                    clause_num_appear_negative[abs(formula[i][j])-1][abs(formula[i][k])-1]++;
                    if (formula[i][k] < 0)
                        clause_num_appear_negative[abs(formula[i][k])-1][abs(formula[i][j])-1]++;
                    else
                        clause_num_appear_positive[abs(formula[i][k])-1][abs(formula[i][j])-1]++;
                }
            }


            length_appear[static_cast<unsigned>(abs(formula[i][j]))-1].push_back(formulaLength[i]);
            if (formula[i][j] > 0)
            {
                length_appear_positive[static_cast<unsigned>(abs(formula[i][j]))-1].push_back(formulaLength[i]);
                polarity[static_cast<unsigned>(abs(formula[i][j]))-1].push_back(1);
            }
            else
            {
                length_appear_negative[static_cast<unsigned>(abs(formula[i][j]))-1].push_back(formulaLength[i]);
                polarity[static_cast<unsigned>(abs(formula[i][j]))-1].push_back(0);
            }
        }

        //binary
        if (formulaLength[i] == 2)
        {
            for (int k = 0; k < formulaLength[i]; k++)
            {
                binary_clauses[abs(formula[i][k]) - 1]++;
            }
        }

        //horn
        if (numOfPositiveAppereance == 1 && formulaLength[i] > 1)
        {
            for (int k = 0; k < formulaLength[i]; k++)
            {
                horn_clauses[abs(formula[i][k])-1]++;
            }
            //horn percentage where variable is only one positive
            horn_clauses_implicate[abs(positiveVariable)-1]++;
        }
    }

    //clause num appear = appear positive + appear negative
    for (unsigned i = 0; i < variables; i++)
    {
        for (unsigned j = 0; j < variables; j++)
        {
            clause_num_appear[i][j] = clause_num_appear_negative[i][j] + clause_num_appear_positive[i][j];
        }
    }

    end = clock();
    std::cout << "Collecting data, horn and binary: " << static_cast<double>(end - begin) / CLOCKS_PER_SEC << "s" << endl;
    begin = clock();

    //calculate corelation
    for (unsigned i = 0; i < variables; i++)
    {
        corelations[i] = computeCorelation(length_appear[i], polarity[i]);
    }

    end = clock();
    cout << "Corelation: " << static_cast<double>(end - begin) / CLOCKS_PER_SEC << "s" << endl;
    begin = clock();

    //calculate statistics
    for (unsigned l = 0; l < variables; l++)
    {
        hornPercentage[l] = (static_cast<double>(horn_clauses[l]) / length_appear[l].size());
        binaryPercentage[l] = (static_cast<double>(binary_clauses[l]) / length_appear[l].size());
        if (horn_clauses[l] != 0)
        {
            hornPercentageImplicate[hornPercentageImplicateLength] = (static_cast<double>(horn_clauses_implicate[l]) / horn_clauses[l]);
            hornPercentageImplicateLength++;
        }
        positiveAppearPercentage[l] = (static_cast<double>(length_appear_positive[l].size()) / length_appear[l].size());
        appearPercentage[l] = (static_cast<double>(length_appear[l].size()) / clauses);

        withVarAppearPercentage[l] =
                (static_cast<double>(computeNonZeroAppearance(clause_num_appear[l], variables)) / variables);
        positiveLiteralsAppearPercentage[l] =
                (static_cast<double>(computeNonZeroAppearance(clause_num_appear_positive[l], variables)) / variables);

        clauseLengthStatistics[l] = computeStatistics(length_appear[l]);
        clauseLengthStatisticsP[l] = computeStatistics(length_appear_positive[l]);
        clauseLengthStatisticsN[l] = computeStatistics(length_appear_negative[l]);
        clauseNumberStatistics[l] = computeStatistics(clause_num_appear[l], variables);
        clauseNumberStatisticsP[l] = computeStatistics(clause_num_appear_positive[l], variables);
        clauseNumberStatisticsN[l] = computeStatistics(clause_num_appear_negative[l], variables);
    }

    end = clock();
    cout << "Statistics: " << static_cast<double>(end - begin) / CLOCKS_PER_SEC << "s" << endl;
    begin = clock();

//    cout << "Duzina pojavljivanja" << endl;
//    printVectorOfVectors(length_appear);
//    cout << "Duzina pojavljivanja pozitivno" << endl;
//    printVectorOfVectors(length_appear_positive);
//    cout << "Duzina pojavljivanja negativno" << endl;
//    printVectorOfVectors(length_appear_negative);

//    cout << "U koliko se pojavljuje" << endl;
//    printArrayOfArray(clause_num_appear, variables, variables);
//    cout << "U koliko se pojavljuje pozitivan" << endl;
//    printArrayOfArray(clause_num_appear_positive, variables, variables);
//    cout << "U koliko se pojavljuje negativan" << endl;
//    printArrayOfArray(clause_num_appear_negative, variables, variables);

//    cout << "Polaritet" << endl;
//    printVectorOfVectors(polarity);

//    cout << "Binarne" << endl;
//    printArray(binary_clauses, variables);

//    cout << "Hornove" << endl;
//    printArray(horn_clauses, variables);

//    cout << "Hornove u zakljucku" << endl;
//    printArray(horn_clauses_implicate, variables);


    //release memory
    for (unsigned l = 0; l < variables; l++)
    {
        free(clause_num_appear_positive[l]);
        free(clause_num_appear_negative[l]);
        free(clause_num_appear[l]);
    }
    free(clause_num_appear);
    free(clause_num_appear_positive);
    free(clause_num_appear_negative);

    free(binary_clauses);
    free(horn_clauses);
    free(horn_clauses_implicate);
}

Statistics Predictor::computeStatistics(vector<int>& sample)
{
    Statistics computed;

    if (sample.size() == 0)
        return computed;

    computed.maximum = *(max_element(sample.begin(), sample.end()));
    computed.minimum = *(min_element(sample.begin(), sample.end()));
    computed.average = accumulate(sample.begin(), sample.end(), 0.0) / sample.size();

    //variation
    double deviation_sum = 0;
    double skew_sum_up = 0, skew_sum_down = 0;
    double kurtosis_sum_up = 0, kurtosis_sum_down = 0;
    unsigned absolute_maximum = abs(computed.maximum) > abs(computed.minimum) ?
                abs(computed.maximum) : abs(computed.minimum);
    vector<unsigned> appear(absolute_maximum + 1, 0);
    int entropy_count = 0;

    for (unsigned i = 0; i < sample.size(); i++)
    {
        double temp = (sample[i] - computed.average);

        //deviation calculation
        deviation_sum += temp*temp;

        //skew calculation
        skew_sum_up += (temp*temp*temp);

        //kurtosis calculation
        kurtosis_sum_up += (temp*temp*temp*temp);

        //appearing calculating / divide by size of array to get probability
        if (appear[static_cast<unsigned>(abs(sample[i]))] == 0)
            entropy_count++;

        appear[static_cast<unsigned>(abs(sample[i]))]++;
    }

    double std_deviation = sqrt(deviation_sum / (sample.size() - 1));

    skew_sum_down = kurtosis_sum_down = deviation_sum;

    skew_sum_up /= sample.size();
    kurtosis_sum_up /= sample.size();

    //consider optimizing this part
    computed.variationCoeficient = computed.average == 0.0 ? 0.0 : std_deviation / computed.average;
    computed.skew = std_deviation == 0.0 ? 0.0 : skew_sum_up / pow(sqrt(skew_sum_down / sample.size()), 3.0);
    computed.kurtosis = std_deviation == 0.0 ? 0.0 : kurtosis_sum_up / pow((kurtosis_sum_down / sample.size()), 2.0);

    //entropy (normalized shannon entropy [0,1])
    computed.entropy = 0;
    for (unsigned i = 0; i < appear.size(); i++)
    {
        if(appear[i] == 0)
            continue;

        double probability = sample.size() == 0 ? 0.0 : static_cast<double>(appear[i]) / sample.size();
        computed.entropy += (probability == 0.0 || entropy_count == 1 || entropy_count == 0)
                ? 0.0 : -(probability*log(probability) / log(entropy_count));
    }

    return computed;
}

Statistics Predictor::computeStatistics(int* sample, unsigned size)
{
    Statistics computed;

    if (size == 0)
        return computed;

    computed.maximum = *(max_element(sample, sample+size));
    computed.minimum = *(min_element(sample, sample+size));
    computed.average = accumulate(sample, sample + size, 0.0) / size;

    //variation
    double deviation_sum = 0;
    double skew_sum_up = 0, skew_sum_down = 0;
    double kurtosis_sum_up = 0, kurtosis_sum_down = 0;
    unsigned absolute_maximum = abs(computed.maximum) > abs(computed.minimum) ?
                abs(computed.maximum) : abs(computed.minimum);
    vector<unsigned> appear(absolute_maximum + 1, 0);
    int entropy_count = 0;

    for (unsigned i = 0; i < size; i++)
    {
        double temp = (sample[i] - computed.average);

        //deviation calculation
        deviation_sum += temp*temp;

        //skew calculation
        skew_sum_up += (temp*temp*temp);

        //kurtosis calculation
        kurtosis_sum_up += (temp*temp*temp*temp);

        //appearing calculating / divide by size of array to get probability
        if (appear[static_cast<unsigned>(abs(sample[i]))] == 0)
            entropy_count++;

        appear[static_cast<unsigned>(abs(sample[i]))]++;
    }

    double std_deviation = sqrt(deviation_sum / (size - 1));

    skew_sum_down = kurtosis_sum_down = deviation_sum;

    skew_sum_up /= size;
    kurtosis_sum_up /= size;

    //consider optimizing this part
    computed.variationCoeficient = computed.average == 0.0 ? 0.0 : std_deviation / computed.average;
    computed.kurtosis = kurtosis_sum_down == 0.0 ? 0.0 : kurtosis_sum_up / pow((kurtosis_sum_down / size), 2.0);
    computed.skew = skew_sum_down == 0.0 ? 0.0 : skew_sum_up / pow(sqrt(skew_sum_down / size), 3.0);

    //entropy (normalized shannon entropy [0,1])
    computed.entropy = 0;
    for (unsigned i = 0; i < appear.size(); i++)
    {
        if(appear[i] == 0)
            continue;

        double probability = entropy_count == 0 ? 0.0 : static_cast<double>(appear[i]) / size;
        computed.entropy += (probability == 0.0 || entropy_count == 1 || entropy_count == 0)
                ? 0.0 : -(probability*log(probability) / log(entropy_count));
    }

    return computed;
}

void Predictor::printArray(int* sample, int length)
{
    cout << endl;
    for (int i = 0; i < length; i++)
    {
        cout << sample[i] << "; ";
    }
    cout << endl;
}

void Predictor::printVector(vector<int> sample)
{
    cout << endl;
    for (unsigned i = 0; i < sample.size(); i++)
    {
        cout << sample[i] << "; ";
    }
    cout << endl;
}

void Predictor::printArrayOfArray(int** sample, int lengthArray, int lengthSubarray)
{
    for (int i = 0; i < lengthArray; i++)
    {
        printArray(sample[i], lengthSubarray);
    }
}

void Predictor::printVectorOfVectors(vector<vector<int> > sample)
{
    cout << endl;
    for (size_t i = 0; i < sample.size(); i++)
    {
        for (size_t j = 0; j < sample[i].size(); j++)
        {
            cout << sample[i][j] << "; ";
        }
        cout << endl;
    }
    cout << endl;
}

void Predictor::printStatistic(Statistics stat)
{
    cout << "Maximum: " << stat.maximum << endl;
    cout << "Minimum: " << stat.minimum << endl;
    cout << "Average: " << stat.average << endl;
    cout << "Variation Coeficient: " << stat.variationCoeficient << endl;
    cout << "Entropy: " << stat.entropy << endl;
    cout << "Skew: " << stat.skew << endl;
    cout << "Kurtosis: " << stat.kurtosis << endl;
}

void Predictor::printFormula()
{
    if(formula != NULL && formulaLength != NULL)
        for(unsigned i = 0; i < clauses; i++)
        {
            for(int j = 0; j < formulaLength[i]; j++)
            {
                cout << formula[i][j] << "; ";
            }
            cout << endl;
        }
}

void Predictor::freeMemory(void** pointer)
{
    if(*pointer != NULL)
    {
        free(*pointer);
        *pointer = NULL;
    }
}

void Predictor::freeObjectMemory()
{
    freeMemory((void**)&clauseLengthStatistics);
    freeMemory((void**)&clauseLengthStatisticsP);
    freeMemory((void**)&clauseLengthStatisticsN);
    freeMemory((void**)&clauseNumberStatistics);
    freeMemory((void**)&clauseNumberStatisticsP);
    freeMemory((void**)&clauseNumberStatisticsN);
    freeMemory((void**)&corelations);
    freeMemory((void**)&binaryPercentage);
    freeMemory((void**)&hornPercentage);
    freeMemory((void**)&hornPercentageImplicate);
    freeMemory((void**)&positiveAppearPercentage);
    freeMemory((void**)&appearPercentage);
    freeMemory((void**)&withVarAppearPercentage);
    freeMemory((void**)&positiveLiteralsAppearPercentage);

    if(formula != NULL && formulaLength != NULL)
        for(unsigned i = 0; i < clauses; i++)
            freeMemory((void**)&(formula[i]));

    freeMemory((void**)&formula);
    freeMemory((void**)&formulaLength);
}

void Predictor::allocateMemory(void** pointer, size_t sizeOf, size_t members)
{
    try
    {
        *pointer = calloc(members, sizeOf);
        *pointer == NULL ? throw logic_error("Error while alocating memory! (Could be also wrong input)") : true;
    }
    catch (const std::exception& a)
    {
        cout << a.what() << endl;
        exit(1);
    }
}

void Predictor::reallocateMemory(void** pointer, size_t sizeOf, size_t members)
{
    try
    {
        *pointer = realloc(*pointer, members*sizeOf);
        *pointer == NULL ? throw logic_error("Error while alocating memory! (Could be also wrong input)") : true;
    }
    catch (const std::exception& a)
    {
        cout << a.what() << endl;
        exit(1);
    }
}

int Predictor::computeNonZeroAppearance(int *array, unsigned sizeArray)
{
    int count = 0;
    for(unsigned i = 0; i < sizeArray; i++)
    {
        if(array[i] != 0)
            count++;
    }
    return count;
}

int Predictor::computeNonZeroAppearance(vector<int> array)
{
    int count = 0;
    for(unsigned i = 0; i < array.size(); i++)
    {
        if(array[i] != 0)
            count++;
    }
    return count;
}

double Predictor::computeCorelation(int* array1, int sizeArray1, int* array2, int sizeArray2)
{
    double sumUp = 0, sumArray1Down = 0, sumArray2Down = 0;

    if(sizeArray1 != sizeArray2)
        return 0;

    double array1Average = accumulate(array1, array1 + sizeArray1, 0.0) / sizeArray1;
    double array2Average = accumulate(array2, array2 + sizeArray2, 0.0) / sizeArray2;

    for (int i = 0; i < sizeArray1; i++)
    {
        sumUp += (static_cast<double>(array1[i]) - array1Average) * (static_cast<double>(array2[i]) - array2Average);
        sumArray1Down += pow((static_cast<double>(array1[i]) - array1Average), 2);
        sumArray2Down += pow((static_cast<double>(array2[i]) - array2Average), 2);
    }

    sumArray1Down = sqrt(sumArray1Down);
    sumArray2Down = sqrt(sumArray2Down);

    return sumUp / (sumArray1Down * sumArray2Down);
}

double Predictor::computeCorelation(vector<int> array1, vector<int> array2)
{
    double sumUp = 0, sumArray1Down = 0, sumArray2Down = 0;

    if(array1.size() != array2.size())
        return 0;

    double array1Average = accumulate(array1.begin(), array1.end(), 0.0) / array1.size();
    double array2Average = accumulate(array2.begin(), array2.end(), 0.0) / array2.size();

    for (unsigned i = 0; i < array1.size(); i++)
    {
        sumUp += (static_cast<double>(array1[i]) - array1Average) * (static_cast<double>(array2[i]) - array2Average);
        sumArray1Down += pow((static_cast<double>(array1[i]) - array1Average), 2);
        sumArray2Down += pow((static_cast<double>(array2[i]) - array2Average), 2);
    }

    sumArray1Down = sqrt(sumArray1Down);
    sumArray2Down = sqrt(sumArray2Down);

    return sumUp / (sumArray1Down * sumArray2Down);
}



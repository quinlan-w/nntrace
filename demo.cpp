#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include "nntrace.h"
using namespace std;

int test_channels(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("demo out.bin\n");
        return 0;
    }
    ifstream inFile(argv[1], ios::binary);
    if (!inFile)
    {
        cout << "error" << endl;
        return 0;
    }
    int len = 0;
    if (inFile.is_open())
    {
        inFile.seekg(0, ios::end);
        len = inFile.tellg();
        inFile.seekg(0, ios::beg);
    }

    char *neusight_data = new char[len];
    inFile.read(neusight_data, len);
    inFile.close();

    vector<string> json = get_nntrace(neusight_data, len);

    // for (auto iter = json.begin(); iter != json.end(); ++iter)
    // {
    //     cout << iter->c_str();
    // }
    ofstream outFile("demo.json");
    for (auto iter = json.begin(); iter != json.end(); ++iter)
    {
        outFile.write(iter->data(), iter->size());
    }
    outFile.close();
    return 0;
}

int test_bandwidth()
{
    return 0;
}

int main(int argc, char **argv)
{
    test_channels(argc, argv);
    return 0;
}
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <map>
#include "SplayTree.h"

using namespace std;

struct CrimeRecord
{
    string date;
    string time;
    string location;
};

string removeExtraSpace(string check)
{
    string result;
    bool inSpace = false;

    for (int i = 0; i < check.length(); ++i)
    {
        if (isspace(check[i]))
        {
            if (!inSpace)
            {
                result += ' ';
                inSpace = true;
            }
        }
        else
        {
            result += check[i];
            inSpace = false;
        }
    }
    return result;
}

int main()
{
    ifstream file("CleanedCrimeData.csv");
    if (!file)
    {
        cerr << "file not found, make sure it's in cmake-build-debug folder\n";
        return 1;
    }

    // skip header line
    string line;
    getline(file, line);

    int count = 0;
    int totalEntries = 500; // for limited print preview, just add && count < totalEntries to while loop

    map<int, CrimeRecord> rbTree;
    SplayTree<int, CrimeRecord> splayTree;

    while (getline(file, line) && count < totalEntries)
    {
        stringstream ss(line);
        CrimeRecord rec;
        string skip;

        getline(ss, rec.date, ',');
        getline(ss, rec.time, ',');

        // skip columns 2–6 to get to location column
        for (int i = 0; i < 4; ++i)
            getline(ss, skip, ',');
        getline(ss, rec.location, ',');
        rec.location = removeExtraSpace(rec.location);

        int recordNumber = count;
        // insert into map
        rbTree[recordNumber] = rec;

        // insert into splay tree
        splayTree.insert(recordNumber, rec);

        // print preview first 500 entries
        cout << "Record #: " << setw(3) << recordNumber << " || Date Occurred: " << setw(15) << rec.date << " || Time Occurred: " << setw(6) << rec.time << " || Location: " << rec.location << '\n';
        ++count;
    }

    file.close();
    return 0;
}
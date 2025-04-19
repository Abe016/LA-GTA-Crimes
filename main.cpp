#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <map>
#include <vector>
#include "SplayTree.h"

using namespace std;

struct CrimeRecord
{
    string date;
    string time;
    string location;
};

// helpers
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

// convert to upper case for string comparison
string toUpper(string s)
{
    for (size_t i = 0; i < s.size(); ++i)
        s[i] = static_cast<char>(toupper(static_cast<unsigned char>(s[i])));
    return s;
}

// remove starting numbers of location to help with searching for street
string removeLeadingNumber(string s) {
    size_t i = 0;
    while (i < s.size() && (isdigit(static_cast<unsigned char>(s[i])) || isspace(static_cast<unsigned char>(s[i])))) {
        ++i;
    }
    return s.substr(i);
}

// search by location
vector<CrimeRecord> searchByLocation(string loc,map<int, CrimeRecord>& rb,SplayTree<int, CrimeRecord>& sp) {
    vector<CrimeRecord> matches;
    string query = toUpper(removeLeadingNumber(removeExtraSpace(loc)));

    // red‑black tree
    for (map<int, CrimeRecord>::iterator it = rb.begin(); it != rb.end(); ++it)
    {
        string candidate = toUpper(removeLeadingNumber(it->second.location));
        if (candidate == query)
            matches.push_back(it->second);
    }

    // splay tree
    sp.forEach([&](int k, CrimeRecord& rec){
        string candidate = toUpper(removeLeadingNumber(rec.location));
        if (candidate == query)
            matches.push_back(rec);
    });

    return matches;
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
    int totalEntries = 100000; // for limited print preview, just add && count < totalEntries to while loop

    map<int, CrimeRecord> rbTree;
    SplayTree<int, CrimeRecord> splayTree;

    while (getline(file, line))
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

        // print preview first totalEntries
        // cout << "Record #: "
        //     << setw(3) << recordNumber
        //     << " || Date Occurred: " << setw(15)
        //     << rec.date << " || Time Occurred: "
        //     << setw(6) << rec.time
        //     << " || Location: " << rec.location << '\n';

        ++count;
    }

    file.close();

    // search test for a specific record with red black tree & splay tree
    int targetRecord = 58093;
    cout << "\nSearch Test (Record #" << targetRecord << ")\n";

    CrimeRecord* resultSplay = splayTree.find(targetRecord);
    auto resultMap = rbTree.find(targetRecord);

    if (resultSplay)
    {
        cout << "Splay Tree: \t "
             << resultSplay->date << " || "
             << resultSplay->time << " || "
             << resultSplay->location << '\n';
    }
    else
    {
        cout << "Splay Tree: Not found.\n";
    }

    if (resultMap != rbTree.end())
    {
        cout << "Red-Black Tree:  "
             << resultMap->second.date << " || "
             << resultMap->second.time << " || "
             << resultMap->second.location << '\n';
    }
    else
    {
        cout << "Red-Black Tree:  Not found.\n";
    }

    cout << "\n";

    // location search test
    string queryLoc = "S Bentley AV";
    vector<CrimeRecord> hits = searchByLocation(queryLoc, rbTree, splayTree);
    cout << "\nIncidents at '" << queryLoc << "'\n";
    for (size_t i = 0; i < hits.size(); ++i) {
        cout << hits[i].date << " | " << hits[i].time << '\n';
    }
    cout << "Total matches: " << hits.size() << "\n";

    return 0;
}
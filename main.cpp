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
    string area;
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
string toUpper(string check)
{
    for (size_t i = 0; i < check.size(); ++i)
        check[i] = static_cast<char>(toupper(static_cast<unsigned char>(check[i])));
    return check;
}

// remove starting numbers of location/space to help with searching for street
string removeLeadingNumber(string check) {
    size_t i = 0;
    while (i < check.size() && (isdigit(static_cast<unsigned char>(check[i])) || isspace(static_cast<unsigned char>(check[i])))) {
        ++i;
    }
    return check.substr(i);
}

// recursively insert middle, then left/right
template<typename K, typename V>
void buildBalanced(SplayTree<K,V>& tree, const vector<pair<K,V>>& data, int low, int high) {
    if (low > high) return;
    int mid = low + (high - low) / 2;
    tree.rawInsert(data[mid].first, data[mid].second);
    buildBalanced(tree, data, low, mid - 1);
    buildBalanced(tree, data, mid + 1, high);
}

int main() {
    // load csv file
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
    map<int, CrimeRecord> rbTree;
    vector<pair<int,CrimeRecord>> allRecords;

    while (getline(file, line))
    {
        stringstream ss(line);
        CrimeRecord rec;
        string skip;

        // taking in data from csv
        getline(ss, rec.date, ','); // date occurred
        getline(ss, rec.time, ','); // time occurred
        getline(ss, rec.area, ','); // area
        // skip columns to get to location column
        for (int i = 0; i < 3; ++i) {
            getline(ss, skip, ',');
        }
        getline(ss, rec.location, ',');
        rec.area = removeExtraSpace(rec.area);
        rec.location = removeExtraSpace(rec.location);

        // insert into map
        rbTree[count] = rec;
        // insert into splay tree
        allRecords.push_back(make_pair(count, rec));
        ++count;
    }
    file.close();

    // build balanced splay tree
    SplayTree<int,CrimeRecord> splayTree;
    buildBalanced(splayTree, allRecords, 0, int(allRecords.size()) - 1);

    // menu loop
    while (true) {
        cout << "\n===== Crime Search Menu =====\n"
             << "1) Search by Area Name\n"
             << "2) Search by Street Location\n"
             << "3) Search by Year (TO DO)\n"
             << "4) Search by Record Number\n"
             << "5) Exit\n"
             << "Choose an option: ";
        int choice;
        cin >> choice;
        if (!cin || choice < 1 || choice > 5) {
            cin.clear();
            cin.ignore(1e6,'\n');
            cout << "Invalid choice.\n";
            continue;
        }
        if (choice == 5) {
            break;
        }
        cin.ignore(1e6,'\n');

        // get query string or number
        string query;
        int recordNumber = 0;
        if (choice == 4) {
            cout << "Enter record number: ";
            cin >> recordNumber;
            cin.ignore(1e6,'\n');
        } else {
            string prompts[] = {
                "",
                "Enter Area Name: ",
                "Enter Street Location: ",
                "Enter Year: ",
                ""
            };
            cout << prompts[choice];
            getline(cin, query);
        }

        // choose data structure
        cout << "1) Map\n"
             << "2) SplayTree:\n"
             << "Choose Data Structure: ";
        int ds;
        cin >> ds;
        cin.ignore(1e6,'\n');
        if (ds != 1 && ds != 2) {
            cout << "Invalid Data Structure Choice.\n";
            continue;
        }

        // perform search
        vector<CrimeRecord> results;

        if (choice == 1) {
            // by Area
            string Q = toUpper(removeExtraSpace(query));
            if (ds == 1) {
                for (auto &p: rbTree) {
                    if (toUpper(removeExtraSpace(p.second.area)) == Q)
                        results.push_back(p.second);
                }
            } else {
                splayTree.forEach([&](int k, CrimeRecord& r){
                    if (toUpper(removeExtraSpace(r.area)) == Q)
                        results.push_back(r);
                });
            }
        }
        else if (choice == 2) {
            // by Street
            string Q = toUpper(removeLeadingNumber(removeExtraSpace(query)));
            if (ds == 1) {
                for (auto &p: rbTree) {
                    if (toUpper(removeLeadingNumber(p.second.location)) == Q)
                        results.push_back(p.second);
                }
            } else {
                splayTree.forEach([&](int k, CrimeRecord& r){
                    if (toUpper(removeLeadingNumber(r.location)) == Q)
                        results.push_back(r);
                });
            }
        }
        else if (choice == 3) {
            // by Year
            cout << "Search by Year: (TO DO)\n";
            continue;
        }
        else if (choice == 4) {
            // by Record Number
            if (ds == 1) {
                auto it = rbTree.find(recordNumber);
                if (it != rbTree.end()) results.push_back(it->second);
            } else {
                CrimeRecord* rp = splayTree.find(recordNumber);
                if (rp) results.push_back(*rp);
            }
        }

        // display results
        cout << "\n===== Results (" << results.size() << ") =====\n";
        for (auto &r : results) {
            cout << setw(15) << r.date
                 << " | " << setw(6)  << r.time
                 << " | " << setw(12) << r.area
                 << " | " << setw(20) << r.location << "\n";
        }
    }

    cout << "Exiting.\n";

    return 0;
}
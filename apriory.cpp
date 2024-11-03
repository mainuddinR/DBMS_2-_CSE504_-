#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

vector<set<string>> generateCandidates(const vector<set<string>>& frequentItemsets, int k) {
    vector<set<string>> candidates;

    int n = frequentItemsets.size();
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            set<string> candidate;
            auto it1 = frequentItemsets[i].begin();
            auto it2 = frequentItemsets[j].begin();
            
            for (int l = 0; l < k - 1; ++l, ++it1, ++it2) {
                if (*it1 != *it2) break;
                candidate.insert(*it1);
            }
            candidate.insert(*it1);
            candidate.insert(*it2);

            if (candidate.size() == k) {
                candidates.push_back(candidate);
            }
        }
    }
    return candidates;
}

map<set<string>, int> calculateSupport(const vector<set<string>>& transactions, const vector<set<string>>& candidates) {
    map<set<string>, int> supportCount;

    for (const auto& candidate : candidates) {
        for (const auto& transaction : transactions) {
            if (includes(transaction.begin(), transaction.end(), candidate.begin(), candidate.end())) {
                supportCount[candidate]++;
            }
        }
    }
    return supportCount;
}

vector<set<string>> filterBySupport(const map<set<string>, int>& supportCount, int minSupport) {
    vector<set<string>> frequentItemsets;

    for (const auto& pair : supportCount) {
        if (pair.second >= minSupport) {
            frequentItemsets.push_back(pair.first);
        }
    }
    return frequentItemsets;
}

pair<vector<set<string>>, map<set<string>, int>> apriori(const vector<set<string>>& transactions, int minSupport) {
    vector<set<string>> frequentItemsets;
    map<set<string>, int> supportCount;
    vector<set<string>> candidates;

    set<string> items;
    for (const auto& transaction : transactions) {
        for (const string& item : transaction) {
            items.insert(item);
        }
    }

    for (const string& item : items) {
        candidates.push_back({item});
    }

    int k = 1;
    while (!candidates.empty()) {
        map<set<string>, int> currentSupportCount = calculateSupport(transactions, candidates);

        // Filter candidates by support
        vector<set<string>> currentFrequentItemsets = filterBySupport(currentSupportCount, minSupport);
        frequentItemsets.insert(frequentItemsets.end(), currentFrequentItemsets.begin(), currentFrequentItemsets.end());

        // Merge current support counts with global support counts
        supportCount.insert(currentSupportCount.begin(), currentSupportCount.end());

        // Generate new candidates from the frequent itemsets
        candidates = generateCandidates(currentFrequentItemsets, ++k);
    }

    return {frequentItemsets, supportCount};
}

vector<pair<set<string>, set<string>>> generateAssociationRules(const vector<set<string>>& frequentItemsets, const map<set<string>, int>& supportCount, double minConfidence) {
    vector<pair<set<string>, set<string>>> rules;

    for (const auto& itemset : frequentItemsets) {
        int n = itemset.size();
        vector<string> items(itemset.begin(), itemset.end());
        
        for (int i = 1; i < (1 << n); ++i) {
            set<string> antecedent;
            set<string> consequent;

            for (int j = 0; j < n; ++j) {
                if (i & (1 << j)) {
                    antecedent.insert(items[j]);
                } else {
                    consequent.insert(items[j]);
                }
            }

            if (!consequent.empty()) {
                // Calculate support for A and A ∪ B
                int supportA = supportCount.at(antecedent); // Support for A
                set<string> unionSet = antecedent; // Create a union set
                unionSet.insert(consequent.begin(), consequent.end()); // Add all items from consequent to the union set
                int supportAB = supportCount.at(unionSet); // Support for A ∪ B

                // Calculate confidence percentage
                double confidence = static_cast<double>(supportAB) / supportA * 100; 

                if (confidence >= minConfidence) {
                    rules.emplace_back(antecedent, consequent);
                }
            }
        }
    }
    return rules;
}

vector<set<string>> readTransactionsFromFile(const string& filename) {
    vector<set<string>> transactions;
    ifstream file(filename);
    string line;

    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return transactions;
    }

    while (getline(file, line)) {
        set<string> transaction;
        stringstream ss(line);
        string item;

        while (ss >> item) {
            transaction.insert(item);
        }

        transactions.push_back(transaction);
    }

    file.close();
    return transactions;
}

int main() {
    string filename = "transactions.txt"; 
    int minSupport = 2;  
    double minConfidence = 0.6;  

    vector<set<string>> transactions = readTransactionsFromFile(filename);

    if (transactions.empty()) {
        cerr << "No transactions found in the file!" << endl;
        return 1;
    }

    pair<vector<set<string>>, map<set<string>, int>> result = apriori(transactions, minSupport);
    vector<set<string>> frequentItemsets = result.first;
    map<set<string>, int> supportCount = result.second;

    int totalTransactions = transactions.size();
    cout << "Frequent Itemsets (min support = " << minSupport << "):\n";
    for (const auto& itemset : frequentItemsets) {
        cout << "{ ";
        for (const string& item : itemset) {
            cout << item << " ";
        }
        cout << "} (Support: " << supportCount[itemset] << ")\n";
    }

    vector<pair<set<string>, set<string>>> rules = generateAssociationRules(frequentItemsets, supportCount, minConfidence);
    cout << "\nAssociation Rules (min confidence = " << minConfidence << "):\n";
    for (const auto& rule : rules) {
        set<string> antecedent = rule.first;
        set<string> consequent = rule.second;

        int supportA = supportCount.at(antecedent); 
        set<string> unionSet = antecedent; 
        unionSet.insert(consequent.begin(), consequent.end()); 
        int supportAB = supportCount.at(unionSet); // Support for A ∪ B

        // Calculate confidence percentage
        double confidence = static_cast<double>(supportAB) / supportA * 100; 

        cout << "{ ";
        for (const string& item : antecedent) {
            cout << item << " ";
        }
        cout << "} -> { ";
        for (const string& item : consequent) {
            cout << item << " ";
        }
        cout << "} (Confidence: " << confidence << "%)\n";
    }

    return 0;
}

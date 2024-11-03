#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <fstream>

using namespace std;

vector<pair<string, string>> graphMake(const vector<vector<string>>& input) {
    vector<pair<string, string>> graph;
    vector<string> transactions;
    vector<char> operations, values;
    int numSteps = input[0].size();
    int numTransactions = input.size();

    cout << "Starting graph construction...\n";
    for (int j = 1; j < numSteps; ++j) {
        for (int i = 0; i < numTransactions; ++i) {
            if (input[i][j] != "-" && input[i][j] != "COM" && input[i][j].size() >= 3) {
                string transaction = input[i][0];
                char operation = input[i][j][0];
                char value = input[i][j][2];
                cout << "Transaction: " << transaction << ", Operation: " << operation << ", Value: " << value << endl;

                for (size_t idx = 0; idx < transactions.size(); ++idx) {
                    if (values[idx] == value && transaction != transactions[idx]) {
                        if (operations[idx] != operation || operation == 'W') {
                            cout << "Adding edge from " << transactions[idx] << " to " << transaction << endl;
                            graph.push_back({transactions[idx], transaction});
                        }
                    }
                }
                transactions.push_back(transaction);
                operations.push_back(operation);
                values.push_back(value);
            }
        }
    }
    return graph;
}

bool cycleCheck(const vector<pair<string, string>>& graph) {
    unordered_map<string, vector<string>> adjList;
    for (const auto& edge : graph) {
        adjList[edge.first].push_back(edge.second);
    }

    cout << "Starting cycle check...\n";
    unordered_set<string> visited, recStack;
    function<bool(const string&)> dfs = [&](const string& node) {
        if (recStack.count(node)) return true;
        if (visited.count(node)) return false;

        recStack.insert(node);
        visited.insert(node);

        for (const string& neighbor : adjList[node]) {
            if (dfs(neighbor)) return true;
        }
        recStack.erase(node);
        return false;
    };

    for (const auto& pair : adjList) {
        const string& node = pair.first;
        if (!visited.count(node)) {
            if (dfs(node)) return true;
        }
    }
    return false;
}

int main() {
    // Temporary small hardcoded input for testing
    vector<vector<string>> input = {
        {"T1", "R(A)", "-", "-", "-", "W(A)", "R(B)", "W(B)", "-"},
        {"T2", "-", "R(A)", "W(A)", "R(B)", "-", "-", "-", "W(B)"}
    };

    vector<pair<string, string>> graph = graphMake(input);

    cout << "Generated Edges:\n";
    for (const auto& edge : graph) {
        cout << edge.first << " -> " << edge.second << endl;
    }

    if (cycleCheck(graph)) {
        cout << "Not Serializable" << endl;
    } else {
        cout << "Serializable" << endl;
    }

    return 0;
}


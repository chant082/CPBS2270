/* Note: refer to the header file (Segment_Tree.h) for documentation of each method. */

#include "Segment_Tree.h"


SGT::SGT() {
    root = nullptr;
    index1 = -1;
    index2 = -1;
}

SGT::~SGT() {
    destroy(root);
}

void SGT::destroy(SGTNode* node) {
    //if node = null
    if (!node) return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}


SGTNode* SGT::init_node(int start, int end) {
    SGTNode* node = new SGTNode();
    node->start = start;
    node->end   = end;
    node->bestCount   = 0;
    node->bestTeamIdx = -1;
    node->left  = nullptr;
    node->right = nullptr;
    node->counts.resize(teams.size(), 0); 
    return node;
}

void SGT::build(vector<string> teamNames, vector<int> matchWinnersTeamIdx) {
    // If there are no teams, stop
    if (teamNames.empty() || matchWinnersTeamIdx.empty()) {
        cout << "Teams must be non-empty." << endl;
        return;
    }
    
    //creates copy of the vectors
    teams = teamNames;
    winnerIdx = matchWinnersTeamIdx;

    // Fill the wins vector with zeros, one entry per team to count the total wins of each team
    wins.clear();
    for (int i = 0; i < (int)teams.size(); i++) {
        wins.push_back(0);
    }

    // Count total wins for each team
    for (int i = 0; i < (int)winnerIdx.size(); i++) {
        int t = winnerIdx[i];
        if (t >= 0 && t < (int)teams.size()) {
            wins[t]++;
        }
    }

    // Remove the old tree and rebuild it from the match history
    destroy(root);
    root = nullptr;
    if (!winnerIdx.empty()) {
        root = buildRec(0, (int)winnerIdx.size() - 1);
    }
}
    
SGTNode* SGT::buildRec(int start, int end) {
    SGTNode* node = init_node(start, end);

    // base case: leaf = a single match
    if (start == end) {
        int t = winnerIdx[start];
        if (t >= 0 && t < (int)teams.size()) {
            node->counts[t] = 1;   // one win for team t in this leaf
            node->bestTeamIdx = t; // best team in this leaf is t
            node->bestCount   = 1; // best count is 1
        }
        return node;
    }

    // split range into two halves
    int mid = (start + end) / 2;

    // build children
    node->left  = buildRec(start, mid);
    node->right = buildRec(mid + 1, end);

    // merge: sum per-team counts from children into this node
    for (int i = 0; i < (int)teams.size(); i++) {
        int leftCount  = 0;
        int rightCount = 0;
        if (node->left){
            leftCount  = node->left->counts[i];
        }
        if (node->right) {
            rightCount = node->right->counts[i];
        }
        node->counts[i] = leftCount + rightCount;
    }

    // choose the best team for this node:
    //   - higher count wins
    //   - tie -> lexicographically smaller team name
    node->bestTeamIdx = -1;
    node->bestCount   = 0;
    for (int i = 0; i < (int)teams.size(); i++) {
        int c = node->counts[i];

        if (c > node->bestCount) {
            node->bestCount = c;
            node->bestTeamIdx = i;
        } else if (c == node->bestCount && c > 0 && node->bestTeamIdx != -1) {
            if (teams[i] < teams[node->bestTeamIdx]) {
                node->bestTeamIdx = i;
            }
        }
    }

    return node;
}
    
const std::string& SGT::leader() const {
    static const std::string leader = "";
    if (!root) return leader;

    int idx = root->bestTeamIdx;
    if (idx < 0 || idx >= (int)teams.size()) return leader;

    return teams[idx];
}

RangeBest SGT::queryMatchRange(int L, int R) {
    // Adjust from 1-based to 0-based indexing
    L -= 1;
    R -= 1;

    RangeBest result = {"", 0, 0};
    if (!root || winnerIdx.empty() || teams.empty()) return result;

    int n = (int)winnerIdx.size();
    
    // Clamp indices to valid range
    if (L < 0) L = 0;
    if (R < 0) R = 0;
    if (L >= n) L = n - 1;
    if (R >= n) R = n - 1;
    if (L > R) std::swap(L, R);

    vector<int> counts((int)teams.size(), 0);
    queryRecHelper(root, L, R, counts);

    int bestIdx = -1;
    int bestCount = 0;
    
    for (int i = 0; i < (int)teams.size(); ++i) {
        int c = counts[i];
        if (c > bestCount ||
            (c == bestCount && c > 0 && bestIdx != -1 && teams[i] < teams[bestIdx])) {
            bestCount = c;
            bestIdx = i;
        }
    }

    if (bestIdx != -1) {
        result.team = teams[bestIdx];
        result.winsInRange = bestCount;
        result.totalWinsOverall = wins[bestIdx];
    }
    return result;
}

void SGT::queryRecHelper(SGTNode* node, int L, int R, vector<int>& outCounts) {
    if (!node) return;
    
    if (node->end < L || node->start > R) return;

    if (L <= node->start && node->end <= R) { 
        for (int i = 0; i < (int)outCounts.size(); ++i) {
            outCounts[i] += node->counts[i];
        }
        return;
    }
    // partial overlap
    queryRecHelper(node->left,  L, R, outCounts);
    queryRecHelper(node->right, L, R, outCounts);
}
    
void SGT::removeTeam(std::string name) {
    // Step 1: Find the index of the team to remove
    int idx = -1;
    for (int i = 0; i < (int)teams.size(); i++) {
        if (teams[i] == name) {
            idx = i;
            break;
        }
    }

    // If team not found, print message and stop
    if (idx == -1) {
        cout << "Team not found: " << name << "\n";
        return;
    }

    // Step 2: Mark all matches won by this team with -1 (means removed)
    // Also, if other teams have indices bigger than this, subtract 1 because one team will be removed
    for (int i = 0; i < (int)winnerIdx.size(); i++) {
        if (winnerIdx[i] == idx) {
            winnerIdx[i] = -1;  // mark as removed
        } else if (winnerIdx[i] > idx) {
            winnerIdx[i] = winnerIdx[i] - 1;  // shift index down by 1
        }
    }

    // Step 3: Remove the team from the teams and wins lists by shifting all elements left
    for (int i = idx; i < (int)teams.size() - 1; i++) {
        teams[i] = teams[i + 1];
        wins[i] = wins[i + 1];
    }
    teams.pop_back();  // remove last duplicate after shifting
    wins.pop_back();

    // Step 4: Create a new list of match winners without the -1 values (which represent removed matches)
    vector<int> newWinnerIdx;
    for (int i = 0; i < (int)winnerIdx.size(); i++) {
        if (winnerIdx[i] != -1) {
            newWinnerIdx.push_back(winnerIdx[i]);
        }
    }
    winnerIdx = newWinnerIdx;  // replace old list with the cleaned one

    // Step 5: Rebuild the segment tree because the data changed
    destroy(root);
    root = nullptr;
    if (!winnerIdx.empty()) {
        root = buildRec(0, (int)winnerIdx.size() - 1);
    }

    cout << "Removed team: " << name << "\n";
}

void SGT::addTeam(const std::string& name, int winsValue) {
    for (const auto& t : teams) {
        if (t == name) {
            cout << "Team already exists: " << name << "\n";
            return;
        }
    }

    if (winsValue < 0) {
        cout << "Wins must be non-negative. Clamping to 0.\n";
        winsValue = 0;
    }

    teams.push_back(name);
    wins.push_back(winsValue);

    destroy(root);
    root = nullptr;
    if (!winnerIdx.empty())
        root = buildRec(0, (int)winnerIdx.size() - 1);

    cout << "Added team: " << name << endl;
}

void SGT::addMatchByName(const std::string& winnerName) {
    int idx = -1;
    for (int i = 0; i < (int)teams.size(); ++i) if (teams[i] == winnerName) { idx = i; break; }
    if (idx == -1) {
        std::cout << "Team '" << winnerName << "' not found. Add it first.\n";
        return;
    }
    winnerIdx.push_back(idx);
    wins[idx] += 1;
    destroy(root);
    root = nullptr;
    if (!winnerIdx.empty())
        root = buildRec(0, (int)winnerIdx.size() - 1);
}

void SGT::printState(std::ostream& os) const {
    os << "Teams: ";
    for (const auto& n : teams) os << n << ' ';

    os << "\nWins:  ";
    for (int w : wins) os << w << ' ';

    // Hide removed matches (-1) when printing
    os << "\nIdx:   ";
    for (int idx : winnerIdx)
        if (idx >= 0) os << idx << ' ';
    cout << "\n\n";
}

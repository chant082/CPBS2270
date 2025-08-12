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
    RangeBest result = {"", 0, 0};
    if (!root || winnerIdx.empty() || teams.empty()) return result;

    int n = (int)winnerIdx.size();
    if (L < 0) L = 0;
    if (R < 0) R = 0;
    if (L >= n) L = n - 1;
    if (R >= n) R = n - 1;
    if (L > R) {
        int temp = L;
        L = R;
        R = temp;
    }

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
    int idx = -1;
    for (int i = 0; i < (int)teams.size(); i++) {
        if (teams[i] == name) { idx = i; break; }
    }
    if (idx < 0 || idx >= (int)teams.size()) {
        cout << "Team not found: " << name << "\n";
        return;
    }

    for (int m = 0; m < (int)winnerIdx.size(); m++) {
        if (winnerIdx[m] == idx)      winnerIdx[m] = -1;
        else if (winnerIdx[m] > idx)  winnerIdx[m]--;
    }

    for (int i = idx; i < (int)teams.size() - 1; i++) {
        teams[i] = teams[i + 1];
        wins[i]  = wins[i + 1];
    }
    teams.pop_back();
    wins.pop_back();

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

#ifndef SEGMENT_TREE_H__
#define SEGMENT_TREE_H__

#include <iostream> 
#include <memory>
#include <vector>
#include <string>   

using namespace std;

// SGTNode is the Segment Tree node structure. This node stores per-match frequency information.
struct SGTNode {
    int start;
    int end;
    int bestCount;
    int bestTeamIdx;    
    vector<int> counts;
    SGTNode* left;
    SGTNode* right;
};

//RangeBest places the query results into this this struct RangeBest
struct RangeBest{
    string team;
    int winsInRange;
    int totalWinsOverall;
};


// Segment Tree Overview

// Each node [start, end] stores the BEST team in that index range, where “best” means:
//   1) Higher wins come first.
//   2) If wins tie, the lexicographically smaller team name wins.
// <<< CHANGE: In this version, [start, end] refers to a RANGE OF MATCHES, not a range of teams.
//             Each leaf represents one match (its winner). Internal nodes aggregate winner counts
//             for all teams in that match subrange using the counts vector.

// Structure:
//   Left child covers [start, mid].
//   Right child covers [mid + 1, end].
//   mid = (start + end) / 2

class SGT{
    public:
    
    //Constructor to initlaize classs variable 
    SGT();
    
    //Destructor - use to clean up all memory that SGT has allocated
    ~SGT();
    
    // build() constructs a segment tree from the given teamNames and the MATCH WINNERS (by team index).
    void build(vector<string> teamNames, vector<int> matchWinnersTeamIdx);
    
    //queryMatchRange() Uses RangeBest Struct to obtain the best team in matches [L, R] inclusive.
    RangeBest queryMatchRange(int L, int R);

    //removeTeam() Removes the team with the given name from the data arrays and rebuilds the tree
    void removeTeam(string name);
    
    // addTeam() Adds a new team and its initial wins, then rebuilds the segment tree.    
    void addTeam(const string& name, int winsValue);
    
    
    //To find the maximum value in the entire tree gets the root node pointers
    const std::string& leader() const;

    
    //addMatchByName() searches for the given team name in the teams vector. If the team is not found, it instructs the user to call addTeam first, then run addMatchByName again.
    void addMatchByName(const std::string& winnerName);
    
    //printState() Prints the current state of the segment tree to the given output stream. Defaults to printing to standard output (std::cout) if no stream is provided.
     void printState(std::ostream& os = std::cout) const;
    
    private:
    
    //Pointer that stores the address of the root node of the Segment Tree
    SGTNode* root = nullptr;
    
    // Stores copies of the data so we can reference the original if needed
    vector<string> teams;      // Team names
    vector<int> wins;          // Wins per team
    vector<int> winnerIdx;     // Index of the winning team for each match
    
    
    // Stores the match winners as team indices — this data is what drives the match-range tree
    // Holds the two indices selected by FindIndex(...) 
    //These are used when performing the next range query
    int index1;
    int index2;
    
    //destroy() a Helper function: Recursively deletes a node and all nodes in its left and right subtrees.
    //Frees all dynamically allocated memory in the Segment Tree.
    //Called during tree rebuilding or destruction to prevent memory leaks.
    void destroy(SGTNode* node);
    
    //init_node() Allocates and initializes a new Segment Tree node on the heap.
    //Sets the node's range to [start, end].
    //Initializes the counts vector with teams.size() zeros.
    //Sets left and right child pointers to nullptr.
    //Returns a pointer to the newly created node.
    SGTNode* init_node(int start, int end); 

    //buildRec() Recursively constructs the Segment Tree subtree covering the inclusive index range [start, end].
    //Called internally by build().
    SGTNode* buildRec(int start, int end);
    
    //queryRecHelper() Recursively queries the Segment Tree by aggregating counts for the inclusive range [L, R].
    //Overlap cases:
    //No overlap: Do nothing
    //Full cover: Add this node's counts into outCounts
    //Partial overlap: Recurse into both children
    void queryRecHelper(SGTNode* node, int L, int R, vector<int>& outCounts);

};    
#endif // SEGMENT_TREE_H__

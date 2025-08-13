# Segment Tree

# Summary
The goal of this project was to implement a Segment Tree that tracks the best team across a range of matches, updating dynamically as results change which is a common need in live eSports or sports tracking applications.

Instead of focusing on raw performance optimizations, I prioritized clarity, simplicity, and correctness, especially for educational and development purposes:
- Rebuilding the tree after updates guarantees consistency without introducing complex logic.
- Alphabetical tie-breaking keeps comparisons straightforward when multiple teams have the same number of wins.
- Exact name matching avoids the need for fuzzy logic and keeps team identification unambiguous.

I implemented the Segment Tree using a vector-based recursive structure, where each node stores:
- A count of wins per team in its range.
- The team with the most wins in that range.
- Tie resolution logic that ensures consistent behavior.

While this approach might use more memory and require rebuilding more often than strictly necessary, it results in clean, bug-free behavior and makes the logic easier to
follow which was ideal for building a working version of the Segment Tree and focusing on learning how it functions.

# Full Explaination
For this project, I implemented a node-based Segment Tree data structure in C++. Conceptually, a Segment Tree resembles a Binary Search Tree (as studied in CPBS 2270), but instead of organizing nodes by key values, it partitions index ranges. It stores aggregated information about intervals of an array, and is constructed using a divide-and-conquer strategy: the index range is repeatedly split in half until each node corresponds to a single element. For instance, an array a[0 ... n-1] is split into [0 ... mid] and [mid+1 ... n-1] segments, recursively, until segments cover exactly one index. Each internal node then stores aggregate data for its range—such as a sum, minimum, maximum, or in this case, the “best team” based on match results within that segment.

This implementation applies the Segment Tree concept to an eSports-style example, tracking the top-performing team over a range of matches. The data is stored in three main vectors. The vectors are teams for team names, wins for total wins per team across all matches, and winnerIdx for the index of the team that won each match. Each node of the tree stores:

counts: A vector sized exactly to teams.size() that records how many wins each team has within that node’s range.

bestCount and bestTeamIdx: Precomputed values representing the number of wins and index of the top-performing team in that range, so queries can return results without rescanning the entire counts vector. In cases where two teams have the same number of wins, the tie is broken alphabetically by team name—but only if both teams have at least one win within that range.

The build() method constructs the tree using the provided teams list and winnerIdx match history. Each node represents a range of match indices [start, end], with the left child covering [start, mid] and the right child covering [mid+1, end]. At a leaf node, where start == end, the node corresponds to a single match, and its counts vector records one win for the winner of that match. Internal nodes aggregate their children’s data by summing the per-team wins, then determine the best team in that range by comparing bestCount values and applying the alphabetical tie-break rule. This approach ensures every node maintains a consistent view of the “best team” for its segment of matches.

The data structure can be updated through several methods:

addMatchByName(): Logs a new match winner in both winnerIdx and wins, then rebuilds the tree so queries reflect the updated data.
addTeam(): Appends a new team with an initial win total, then rebuilds the tree so all nodes’ counts vectors resize to match the updated teams.size().
removeTeam(): Removes a team from teams and wins, iterates through winnerIdx to mark any matches won by that team as -1, decrements indices for teams above the removed team, and rebuilds the tree.

After any operation that changes the number of teams or alters match history (addTeam, removeTeam, addMatchByName), the tree is rebuilt to ensure every node’s counts vector matches the current team count, and all bestTeamIdx and bestCount values remain accurate. This guarantees consistent query results, correct tie-breaking, and proper handling of changes over time.

By the end, the Segment Tree supports building from match history, querying the best team over any match index range, and updating as teams and matches change — all while preserving node data integrity and maintaining efficient, predictable query behavior.

# Code Walkthrough

# Cloning Respistory
First, open VS Code or another coding environment of your choice that can run C++. If you already know how to clone the repository into that environment, skip to "Opening the files". If not, follow the steps below.

To copy the project data, go to the GitHub repository and open the Code section. Copy the link found under the SSH option.

    Example: https://github.com/username/segment-tree.git

Open a new terminal and type in git clone and paste the link into the terminal then 

    Example: git clone https://github.com/username/segment-tree.git
    
# Opening the files     
Open Segment_Tree.h, the header file that contains the declaration of the SGT class. This file defines the public methods for building, querying, and updating the Segment Tree, as well as the private data members used for tracking team win counts, determining the best team in a given range, and running helper functions for internal operations. It serves as the interface for the Segment Tree, while the corresponding .cpp file contains the actual implementations.

Open Segment_Tree.cpp, the implementation file for the SGT class declared in Segment_Tree.h. This file contains the full definitions of all Segment Tree methods, including building the tree from match data, querying the best team in a specific range, updating match results, adding or removing teams, and applying tie-break logic. It holds the actual executable code that carries out the operations defined in the header file.

Lastly, open main.cpp, which serves as the program’s entry point and contains the code walkthrough for demonstrating the Segment Tree. This file sets up predefined match data, builds the tree, runs example queries, performs updates, and prints the results before and after changes. It provides a clear example of how the SGT class is used in practice.

# Running / Compiling 
To compile the project using C++17 and run the program with your implementations, use the following command in your terminal. In the explanation below, the phrase “run/compile code” refers to entering this command:

    g++ -std=c++17 -O2 -o segment_tree main.cpp Segment_Tree.cpp && ./segment_tree

# Creating an object from the Segment Tree class
First, you need to create an object from the SGT class. You can name it whatever you like. Here’s an example

    SGT la_teams;

# Building the Segment Tree
To build a Segment Tree from the predefined data, place the following code inside main in the designated area:

    la_teams.build(EsportsTeams, winnerIdx);

This object represents your Segment Tree and allows you to call its methods for building, querying, and updating the tree.
Congratulations — you’ve built a tree! Although it isn’t visually displayed in the console, it exists internally. Here’s what the tree looks like under the hood:

                                                         [0,11]: Team Beta (4)
                                                   /                               \
                               [0,5]: Team Beta (2)                                 [6,11]: Team Beta (2)
                              /                 \                                     /                 \
                 [0,2]: Team Beta (1)   [3,5]: Team Beta (1)   [6,8]: Team Beta (1)   [9,11]: Team Beta (1)
                /            \             /        \           /               \      /                  \
              [0,1]         [2,2]       [3,4]    [5,5]       [6,7]           [8,8]    [9,10]              [11,11]
             /   \           |         /   \        |        /    \             |      /     \               |
           [0,0] [1,1]     [2,2]   [3,3]    [4,4] [5,5]    [6,6]  [7,7]       [8,8]  [9,9]  [10,10]       [11,11]
 
(Note: Some details have been omitted for simplicity. The brackets [ ] indicate the index range of the matches. The team name (e.g., Team Beta) represents the team with the most wins in that range, and the number in parentheses ( ) shows how many wins that team has within the range.)

# Find Maximum Value by Matches
To find the root node of the Segment Tree (the team with the highest number of wins across all matches), use the following function:

    cout << "Best team overall: " << la_teams.leader() << endl;

The code output should be "Best team overall: Team Beta"


# Query
To find which team had the highest number of wins for a given range of matches (e.g., between match 3 and match 7), use this function. It can be applied to any range of matches. We can verify that our leader() function works with the following code:

    la_teams.queryMatchRange(0, int(winnerIdx.size()) - 1);


    RangeBest result = la_teams.queryMatchRange(0, winnerIdx.size() - 1);

    cout << "Best team: " << result.team << " (" << result.winsInRange << " wins in range, " << result.totalWinsOverall << " total wins overall)" << endl;
     
     
The reason it works this way is due to how we constructed the query function. The function creates a RangeBest structure called result. This struct has three components:

RangeBest
- team
- winsInRange
- totalWinsOverall

To access the values within the result object we created earlier, use this format:

    ____.team, _____winsInRange, _____.totalWinsOverall

Here’s the general format for running more queries:

RangeBest _____ = la_teams.queryMatchRange(____, ____);

    cout << "Best team: " << _____.team << " (" << ____.winsInRange << " wins in range, " << ____.totalWinsOverall << " total wins overall)" << endl;

The code output should be:

    "Best team overall: Team Beta"



Now it’s your turn! I’ll give you sample questions to query for, and I’ll place the correct code below each one so you can check your work.

1.How would you find which team had the most wins between matches 2 and 4?

    RangeBest _____ = la_teams.queryMatchRange(____, ____);

    cout << "Best team: " << _____.team << " (" << ____.winsInRange << " wins in range, " << ____.totalWinsOverall << " total wins overall)" << endl;



**ANSWER**

RangeBest Best2_4 = la_teams.queryMatchRange(2, 4);

cout << "Best team: " << Best2_4.team << endl;


2.How would you find which team had the most wins between matches 9 and 4?

    RangeBest _____ = la_teams.queryMatchRange(____, ____);

    cout << "Best team: " << _____.team << " (" << ____.winsInRange << " wins in range, " << ____.totalWinsOverall << " total wins overall)" << endl;

**ANSWER**
RangeBest Best9_4 = la_teams.queryMatchRange(9, 4); - OR - RangeBest Best4_9 = la_teams.queryMatchRange(9, 4);

(The code will automatically switch the positions of the values if the first value is higher!)

cout << "Best team: " << Best9_4.team << endl;
cout << "Best team: " << Best4_9.team << endl;

As you can see, it’s the same answer!

    vector<int> winnerIdx = {1, 4, 0, 1, 2, 5, 4, 1, 3, 0, 4, 1};
    
You might be wondering — if matches 4 through 9 each team has 1 win, why did it choose Alpha when they’re all equal? As discussed in the explanation above, the code breaks ties alphabetically if the win counts are the same.

# Remove a Team / Add a Team
What if a team gets disqualified and is no longer part of the game? We can use the removeTeam() function.
What if a team joins late after finishing their paperwork? We can use the addTeam() function. 

Running these functions will remove or add the team to the EsportsTeam vector and rebuild the Segment Tree, allowing you to query updated results.

    la_teams.removeTeam("Team Alpha");

This will display a message confirming that the team was successfully removed.

    Removed team: Team Alpha

An example for adding a Team:

    la_teams.addTeam("Team Zeta", 0);
    
Here, 0 is added because they joined the league and currently have no wins. 

After running the code above, a message will confirm that the team was successfully added:

    Added team: Team Zeta
       
**To see the current state of the vectors, run the following code:**

    la_teams.printState();

Example output after removing Team Alpha:

    output:
    Teams: Team Beta Team Gamma Team Delta Team Omega Team Phoenix 
    Wins:  4 1 1 3 1 
    Idx:   0 3 0 1 4 3 0 2 3 0

Example output after adding Team Zeta:

    output:
    Teams: Team Beta Team Gamma Team Delta Team Omega Team Phoenix Team Zeta 
    Wins:  4 1 1 3 1 0 
    Idx:   0 3 0 1 4 3 0 2 3 0 

# Add a win
Your team just won a game! Now we can record that win in the Idx vector using their index. For example, if we use the state above with Team Zeta at index 5, running the following code and then using the printState() function will show that they won a game — their wins increase from 0 to 1.

Run the following code:

    la_teams.addMatchByName("Team Zeta");
    la_teams.printState();

    output:
    Teams: Team Beta Team Gamma Team Delta Team Omega Team Phoenix Team Zeta 
    Wins:  4 1 1 3 1 1 
    Idx:   0 3 0 1 4 3 0 2 3 0 5 


What if Zeta wins multiple times? We can simply run the addMatchByName() function repeatedly to keep increasing their wins.

    la_teams.addMatchByName("Team Zeta");
    la_teams.addMatchByName("Team Zeta");
    la_teams.addMatchByName("Team Zeta");
    la_teams.addMatchByName("Team Zeta");
    la_teams.addMatchByName("Team Zeta");
    la_teams.printState();

    output:
    Teams: Team Beta Team Gamma Team Delta Team Omega Team Phoenix Team Zeta 
    Wins:  4 1 1 3 1 7 
    Idx:   0 3 0 1 4 3 0 2 3 0 5 5 5 5 5 5 5 

The addMatchByName() function automatically rebuilds the Segment Tree, so we can query it right away.

If we want to see which team won the most games between match 11 and match 17, we can use the following code:
RangeBest Best11_17 = la_teams.queryMatchRange(11, 17);

    cout << "Best team: " << Best11_17.team << " (" << Best11_17.winsInRange << " wins in range, " << Best11_17.totalWinsOverall << " total wins overall)" << endl;

    output:
    Best team: Team Zeta (6 wins in range, 7 total wins overall)
    
    
#Limitations
- Exact names only. addMatchByName("Team Beta") must match exactly — capitalization and spacing matter. No fuzzy matching.
- Tie rules are simple (and a bit strict). If two teams have the same number of wins in a range, the one that comes first alphabetically is chosen. That’s easy to understand, but maybe not the rule you’d want in real life.
- Rebuilds the whole tree after changes. When you add/remove a team or add a new match, the code “re-makes” the tree from scratch. That keeps things simple and safe, but it’s slower than updating only the affected part.
- Uses more memory as things grow. Every tree node keeps a list of counts for every team. With many teams and many matches, this adds up. It’s fine for class-sized examples, but not great for huge datasets.
- If I had more time: I’d store less data per node (to save memory), update the tree in place (for speed), keep scores, make tie rules configurable, support queries by team or date, add saving to a file, and maybe even build a small GUI.

Most of these limitations came from my focus on getting a working solution first rather than building a fully optimized system. For example, rebuilding the whole tree after every change was the simplest way to guarantee correctness without introducing tricky bugs. I also kept tie rules simple so I could focus on the Segment Tree logic instead of getting lost in edge cases. The exact name matching was just the quickest way to connect match results to teams without needing extra parsing or search logic. In a real-world scenario, I’d revisit each of these to improve speed, memory use, and flexibility.


#Sources

https://www.geeksforgeeks.org/cpp/vector-in-cpp-stl/
https://www.geeksforgeeks.org/cpp/structures-in-cpp/
https://www.geeksforgeeks.org/dsa/segment-tree-data-structure/
https://learn.zybooks.com/zybook/COLORADOCSPB2270Summer2025/chapter/2/section/1 
https://learn.zybooks.com/zybook/COLORADOCSPB2270Summer2025/chapter/7/section/1 
https://en.cppreference.com/w/cpp/language/range-for.html
https://en.cppreference.com/w/cpp/algorithm.html
https://cplusplus.com/reference/ostream/ostream/
https://en.cppreference.com/w/cpp/io/basic_ostream/basic_ostream

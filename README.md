#include "Segment_Tree.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;


// THIS IS THE SAMPLE DATA DO NOT CHANGE OR ALTER
int main() {
    // Predefined team names
    vector<string> EsportsTeams = {
        "Team Alpha",
        "Team Beta",
        "Team Gamma",
        "Team Delta",
        "Team Omega",
        "Team Phoenix"
    };

    // Predefined wins per team (initial totals — will be updated as matches are played)
    vector<int> EsportTeamWins = {
        5, 7, 3, 4, 6, 2
    };

    // Predefined winner index for each match (0-based indexing for 'teams')
    // Example: '1' means Team Beta won that match
    vector<int> winnerIdx = {
        1, 4, 0, 1, 2, 5, 4, 1, 3, 0, 4, 1
    };
                    
//PLACE YOUR CODE BELOW            

    SGT la_teams;
    la_teams.build(EsportsTeams, winnerIdx);
    cout << "Best team overall: " << la_teams.leader() << endl;
   
    RangeBest result = la_teams.queryMatchRange(0, winnerIdx.size() - 1);

    cout << "Best team: " << result.team << " (" << result.winsInRange << " wins in range, " << result.totalWinsOverall << " total wins overall)" << endl;
    
    RangeBest Best2_4 = la_teams.queryMatchRange(2, 4);

    cout << "Best team: " << Best2_4.team << endl;
    
    
    RangeBest Best9_4 = la_teams.queryMatchRange(9, 4);
    RangeBest Best4_9 = la_teams.queryMatchRange(4, 9);

    cout << "Best team: " << Best9_4.team << endl;
    cout << "Best team: " << Best4_9.team << endl;
    
    la_teams.removeTeam("Team Alpha");
    
    la_teams.printState();
    
    la_teams.addTeam("Team Zeta", 0);
    
    la_teams.printState();
    
    la_teams.addMatchByName("Team Zeta");
    la_teams.printState();
    
    la_teams.addMatchByName("Team Zeta");
    la_teams.addMatchByName("Team Zeta");
    la_teams.addMatchByName("Team Zeta");
    la_teams.addMatchByName("Team Zeta");
    la_teams.addMatchByName("Team Zeta");
    la_teams.printState();
    
    RangeBest Best11_17 = la_teams.queryMatchRange(11, 17);
    cout << "Best team: " << Best11_17.team << " (" << Best11_17.winsInRange << " wins in range, " << Best11_17.totalWinsOverall << " total wins overall)" << endl;
    
};

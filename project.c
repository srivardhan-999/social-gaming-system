#include <stdio.h>
#include <string.h>

#define MAX_PLAYERS 100
#define MAX_GAMES 100
#define MAX_GAMES_PLAYED 100
#define MAX_GUESTS 100
#define MAX_PREFS 10

typedef struct {
    int game_id;
    char name[50];
    int type; 
    int players_req;
    int prizesAwarded;
    int totalPlayTime;
} GameMaster;

typedef struct {
    int games_played;
    int games_won;
    int total_score;
    int prizes;
} PlayerDetails;

typedef struct {
    int player_id;
    char name[50];
    char reg_date[20];
    PlayerDetails stats;
    int game_preferences[MAX_PREFS];
    int pref_count;
} Player;

typedef struct {
    int session_id;
    int game_id;
    int playerIds[10];
    int player_count;
    int scores[10];
    int prizes[10];
    int playTime[10];
    char start_date[20];
    char end_date[20];
    int is_completed;
    int players_quit[10];
} GamePlayed;

typedef struct {
    int guest_id;
    char name[50];
    char start_date[20];
    int active_days;
    int isBlocked;
} GuestUser;

GameMaster games[MAX_GAMES];
Player players[MAX_PLAYERS];
GamePlayed games_played[MAX_GAMES_PLAYED];
GuestUser guests[MAX_GUESTS];

int game_count = 0;
int player_count = 0;
int total_sessions = 0;
int guest_count = 0;
int game_id = 1;
int session_id = 1;
int player_id = 101;
int guest_id = 1;

int no_of_days(char startDate[], char endDate[]) {
    int startDay, startMonth, startYear;
    int endDay, endMonth, endYear;
    int result1 = sscanf(startDate, "%d-%d-%d", &startDay, &startMonth, &startYear);
    int result2 = sscanf(endDate, "%d-%d-%d", &endDay, &endMonth, &endYear);
    
    if (result1 != 3 || result2 != 3) {
        printf("Invalid date format! Use DD-MM-YYYY format.\n");
        return -1;
    }
    
    if (startDay < 1 || startDay > 31 || startMonth < 1 || startMonth > 12 || startYear < 1900 ||
        endDay < 1 || endDay > 31 || endMonth < 1 || endMonth > 12 || endYear < 1900) {
        printf("Invalid date values!\n");
        return -1;
    }
    
    if (endYear < startYear || 
        (endYear == startYear && endMonth < startMonth) || 
        (endYear == startYear && endMonth == startMonth && endDay < startDay)) {
        return -1;
    }
    
    int startTotalDays = startYear * 365 + startMonth * 30 + startDay;
    int endTotalDays = endYear * 365 + endMonth * 30 + endDay;
    return endTotalDays - startTotalDays;
}

// Binary Search for Player ID (assumes players are sorted by player_id)
int find_player_id(int playerId) {
    int left = 0, right = player_count - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (players[mid].player_id == playerId) {
            return mid;
        } else if (players[mid].player_id < playerId) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return -1;
}

// Binary Search for Guest ID (assumes guests are sorted by guest_id)
int find_guest_id(int guestId) {
    int left = 0, right = guest_count - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (guests[mid].guest_id == guestId) {
            return mid;
        } else if (guests[mid].guest_id < guestId) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return -1;
}

// Binary Search for Game ID (assumes games are sorted by game_id)
int find_game_id(int gameId) {
    int left = 0, right = game_count - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (games[mid].game_id == gameId) {
            return mid;
        } else if (games[mid].game_id < gameId) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return -1;
}

// Binary Search for Session ID (assumes sessions are sorted by session_id)
int find_session_id(int sessionId) {
    int left = 0, right = total_sessions - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (games_played[mid].session_id == sessionId) {
            return mid;
        } else if (games_played[mid].session_id < sessionId) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return -1;
}

int count_player_active_games(int playerId) {
    int activeCount = 0;
    for (int i = 0; i < total_sessions; i++) {
        if (!games_played[i].is_completed) {
            for (int j = 0; j < games_played[i].player_count; j++) {
                if (games_played[i].playerIds[j] == playerId && games_played[i].players_quit[j] == 0) {
                    activeCount++;
                    break;
                }
            }
        }
    }
    return activeCount;
}

int total_prizes(int gameId, int score) {
    int gameIndex = find_game_id(gameId);
    if (gameIndex == -1) {
        return 0;
    }
    if (score >= 1000) {
        return 5; 
    } else if (score >= 500) {
        return 3; 
    } else if (score >= 100) {
        return 1; 
    }
    return 0; 
}

void reg_new_player(){
    if(player_count >= MAX_PLAYERS){
        printf("Player database full!\n");
        return;
    }
    Player new_player;
    new_player.player_id = player_id++;
    printf("Enter player name: ");
    if(scanf("%49s", new_player.name) != 1 || strlen(new_player.name) == 0){
        printf("Invalid name input!\n");
        return;
    }
    printf("Enter registration date (DD-MM-YYYY): ");
    if(scanf("%19s", new_player.reg_date) != 1){
        printf("Invalid date input!\n");
        return;
    }
    int dateCheck = no_of_days(new_player.reg_date, new_player.reg_date);
    if(dateCheck < 0){
        printf("Invalid date format!\n");
        return;
    }
    new_player.stats.games_played = 0;
    new_player.stats.games_won = 0;
    new_player.stats.total_score = 0;
    new_player.stats.prizes = 0;

    new_player.pref_count = 0; 
    if(game_count > 0) {
        GameMaster tempGames[MAX_GAMES];
        for(int i = 0; i < game_count; i++) {
            tempGames[i] = games[i];
        }

        for(int i = 0; i < game_count - 1; i++) {
            for(int j = 0; j < game_count - i - 1; j++) {
                if(tempGames[j].totalPlayTime < tempGames[j + 1].totalPlayTime) {
                    GameMaster temp = tempGames[j];
                    tempGames[j] = tempGames[j + 1];
                    tempGames[j + 1] = temp;
                }
            }
        }

        int gamesToAssign = (game_count < 3) ? game_count : 3;
        for(int i = 0; i < gamesToAssign; i++) {
            if(new_player.pref_count < MAX_PREFS) {
                new_player.game_preferences[new_player.pref_count++] = tempGames[i].game_id;
            }
        }
        if(new_player.pref_count > 0) {
            printf("Popular game preferences assigned (based on playtime): \n");
            for(int i = 0; i < new_player.pref_count; i++) {
                int gid = new_player.game_preferences[i];
                int gindex = find_game_id(gid);
                if(gindex != -1) {
                    printf("- %s (ID: %d) - Total Play Time: %d days\n", 
                           games[gindex].name, games[gindex].game_id, games[gindex].totalPlayTime);
                }
            }
            printf("\n");
        }
        else {
            printf("No games available to assign preferences.\n");
        }
    }
    else {
        printf("\n No games available in the system! Add game preferences later.\n");
    }
    players[player_count++] = new_player;
    printf("Player registered successfully with ID: %d\n", new_player.player_id);
}

void display_games() {
    printf("\n Available Games : \n");
    for(int i = 0; i < game_count; i++) {
        printf("ID: %d | %s | Type: %s | Players Required: %d\n",
               games[i].game_id,
               games[i].name,
               games[i].type == 1 ? "Single" : "Multiple",
               games[i].players_req);
        printf("\n");
    }
}

void add_game_preferences(int playerId) {
    int playerIndex = find_player_id(playerId);
    if (playerIndex == -1) {
        printf("Player not found!\n");
        return;
    }
    if(players[playerIndex].pref_count >= MAX_PREFS) {
        printf("Maximum game preferences reached!\n");
        return;
    }
    if(game_count == 0){
        printf("No games available in the system!\n");
        return;
    }
    display_games();
    int gameId;
    printf("Enter Game ID to add to preferences (0 to stop): ");
    if(scanf("%d", &gameId) != 1){
        printf("Invalid input!\n");
        return;
    }
    if(gameId == 0) return;

    int gameIndex = find_game_id(gameId);
    if(gameIndex == -1) {
        printf("Invalid Game ID!\n");
        return;
    }

    // Check if game is already in preferences
    for(int i = 0; i < players[playerIndex].pref_count; i++) {
        if(players[playerIndex].game_preferences[i] == gameId) {
            printf("Game already in preferences!\n");
            return;
        }
    }
    
    // Add game to preferences
    players[playerIndex].game_preferences[players[playerIndex].pref_count++] = gameId;
    printf("Game added to preferences successfully!\n");
}

void start_game_session() {
    if(total_sessions >= MAX_GAMES_PLAYED) {
        printf("Maximum game sessions reached!\n");
        return;
    }
    GamePlayed new_session;
    new_session.session_id = session_id++;
    
    printf("Enter Game ID : ");
    int game_id;
    scanf("%d", &game_id);
    int gameIndex = find_game_id(game_id);

    if(gameIndex == -1) {
        printf("Game not found!\n");
        return;
    }
    new_session.game_id = game_id;

    printf("Enter session start date (DD-MM-YYYY): ");
    scanf("%19s", new_session.start_date);

    strcpy(new_session.end_date, "");
    new_session.player_count = 0;
    new_session.is_completed = 0;

    // Initialize all arrays to 0
    for(int i = 0; i < 10; i++) {
        new_session.playerIds[i] = 0;
        new_session.scores[i] = 0;
        new_session.prizes[i] = 0;
        new_session.playTime[i] = 0;
        new_session.players_quit[i] = 0;
    }
    
    printf("\nAdding players to the session:\n");
    printf("Enter Player ID (0 for guest, -1 to stop):\n");

    while(new_session.player_count < games[gameIndex].players_req) {
        printf("Player %d: ", new_session.player_count + 1);
        int playerId;
        if(scanf("%d", &playerId) != 1){
            printf("Invalid input!\n");
            continue;
        }

        if(playerId == -1) break;

        if(playerId == 0) {
            if(guest_count >= MAX_GUESTS) {
                printf("Guest user database full! Cannot add more guests.\n");
                continue;
            }
            GuestUser new_guest;
            new_guest.guest_id = guest_id++;
            printf("Enter guest name: ");
            if(scanf("%49s", new_guest.name) != 1 || strlen(new_guest.name) == 0){
                printf("Invalid guest name!\n");
                continue;
            }
            strcpy(new_guest.start_date, new_session.start_date);
            new_guest.active_days = 0;
            new_guest.isBlocked = 0;

            guests[guest_count++] = new_guest;
            playerId = -new_guest.guest_id;
 
            printf("Guest '%s' created with ID: %d\n", new_guest.name, new_guest.guest_id);
        }
        else {
            int playerIndex = find_player_id(playerId);
            if(playerIndex == -1) {
                printf("Player not found! Please check the Player ID.\n");
                continue;
            }
            for(int k = 0; k < new_session.player_count; k++){
                if(new_session.playerIds[k] == playerId){
                    printf("Player already added to this session!\n");
                    continue;
                }
            }
        }

        if(playerId < 0) {
            int guestIndex = find_guest_id(-playerId);
            if(guestIndex != -1 && guests[guestIndex].isBlocked) {
                printf("This guest is currently blocked.\n");
                continue;
            }
        }
        new_session.playerIds[new_session.player_count] = playerId;
        new_session.scores[new_session.player_count] = 0;
        new_session.prizes[new_session.player_count] = 0;
        new_session.playTime[new_session.player_count] = 0;
        new_session.players_quit[new_session.player_count] = 0; 
        new_session.player_count++;

        if(playerId > 0) {
            int playerIndex = find_player_id(playerId);
            if(playerIndex != -1) {
                players[playerIndex].stats.games_played++;
            }
        }
        if(new_session.player_count >= games[gameIndex].players_req) {
            printf("Required number of players reached for the session.\n");
            break;
        }
    }
    if(new_session.player_count == 0) {
        printf("No players added to the session. Session not created.\n");
        return;
    }
    if(new_session.player_count < games[gameIndex].players_req) {
        printf("Warning: Not enough players for this game. Required: %d, Added: %d\n", 
               games[gameIndex].players_req, new_session.player_count);
    }
    games_played[total_sessions++] = new_session;

    printf("Game session started successfully with Session ID: %d\n", new_session.session_id);
    printf("Players in this session:\n");
    printf("Game %s (ID: %d)\n", games[gameIndex].name, new_session.game_id);
    printf("--------------------------------------------\n");
    for(int i = 0; i < new_session.player_count; i++) {
        int pid = new_session.playerIds[i];
        if(pid > 0) {
            int pIndex = find_player_id(pid);
            if(pIndex != -1) {
                printf("Player: %s (ID: %d)\n", players[pIndex].name, pid);
            }
        }
        else {
            int gIndex = find_guest_id(-pid);
            if(gIndex != -1) {
                printf("Guest: %s (ID: %d)\n", guests[gIndex].name, -pid);
            }
        }
    }
    printf("--------------------------------------------\n");
    printf("Total Players: %d\n", new_session.player_count);
}

void quit_game_session() {
    printf("\n----- QUIT GAME SESSION ----\n");
    printf("Active Game Sessions:\n");
    printf("Session ID  Game ID  Game Name        Players  Start Date\n");
    printf("----------  -------  ---------------  -------  ----------\n");

    int activeSessionsFound = 0;
    for (int i = 0; i < total_sessions; i++) {
        if (!games_played[i].is_completed) {
            int gameIndex = find_game_id(games_played[i].game_id);
            if (gameIndex != -1) {
                printf("%-10d  %-7d  %-15s  %-7d  %s\n",
                       games_played[i].session_id, games_played[i].game_id,
                       games[gameIndex].name, games_played[i].player_count,
                       games_played[i].start_date);
                activeSessionsFound = 1;
            }
        }
    }

    if (!activeSessionsFound) {
        printf("No active game sessions found.\n");
        return;
    }

    printf("\nEnter Session ID to quit: ");
    int sessionId;
    if(scanf("%d", &sessionId) != 1 || sessionId <= 0){
        printf("Invalid input! Session ID must be positive.\n");
        return;
    }
    
    int sessionIndex = find_session_id(sessionId);

    if (sessionIndex == -1) {
        printf("Session not found!\n");
        return;
    }

    if (games_played[sessionIndex].is_completed) {
        printf("This session is already completed!\n");
        return;
    }
    GamePlayed session = games_played[sessionIndex];
    int gameIndex = find_game_id(session.game_id);

    printf("\nPlayers in this session: %d (%s) \n", sessionId, games[gameIndex].name);
    printf("Index  Player ID   Name/Guest Name   Quit Status\n");
    printf("------------------------------------------------\n");
    for(int i = 0; i < session.player_count; i++) {
        int playerId = session.playerIds[i];
        char status[10] = "Active";
        if(session.players_quit[i] == 1) {
            strcpy(status, "Quit");
        }

        if(playerId > 0) {
            int playerIndex = find_player_id(playerId);
            if(playerIndex != -1) {
                printf("%-5d  %-10d  %-15s  %-10s\n", i + 1, playerId, players[playerIndex].name, status);
            }
        }
        else {
            int guestIndex = find_guest_id(-playerId);
            if(guestIndex != -1) {
                printf("%-5d  %-10d  %-15s  %-10s\n", i + 1, playerId, guests[guestIndex].name, status);
            }
        }
    }

    printf("\nEnter Player Index to quit (1 to %d): ", session.player_count);
    int playerIndexToQuit;
    if(scanf("%d", &playerIndexToQuit) != 1){
        printf("Invalid input!\n");
        return;
    }

    if(playerIndexToQuit < 1 || playerIndexToQuit > session.player_count) {
        printf("Invalid player index!\n");
        return;
    }

    if(session.players_quit[playerIndexToQuit - 1] == 1) {
        printf("This player has already quit the session!\n");
        return;
    }

    int quittingPlayerId = session.playerIds[playerIndexToQuit - 1];

    if(quittingPlayerId < 0) {
        int guestIndex = find_guest_id(-quittingPlayerId);
        if(guestIndex != -1) {
            printf("Guest '%s' (ID: %d) has quit the session.\n", guests[guestIndex].name, -quittingPlayerId);
        }
    }
    else {
        int playerIndex = find_player_id(quittingPlayerId);
        if(playerIndex != -1) {
            printf("Player '%s' (ID: %d) has quit the session.\n", players[playerIndex].name, quittingPlayerId);
        }
    }

    printf(" Are you sure you want to quit? (1 = Yes, 0 = No): ");
    int confirm;
    if(scanf("%d", &confirm) != 1){
        printf("Invalid input!\n");
        return;
    }
    
    if(confirm != 1) {
        printf("Quit action cancelled.\n");
        return;
    }

    games_played[sessionIndex].players_quit[playerIndexToQuit - 1] = 1;
    printf("Player has been marked as quit in the session.\n");
    if(quittingPlayerId > 0) {
        int playerIndex = find_player_id(quittingPlayerId);
        if(playerIndex != -1) {
            players[playerIndex].stats.games_played--;
            printf("Game count updated for player '%s'.\n", players[playerIndex].name);
        }
    }

    int allQuit = 1;
    for(int i = 0; i < session.player_count; i++) {
        if(games_played[sessionIndex].players_quit[i] == 0) {
            allQuit = 0;
            break;
        }
    }
    if(allQuit) {
        printf("All players have quit the session.\n");
        games_played[sessionIndex].is_completed = 1;
        printf("Session marked as completed.\n");
        char currentDate[20];
        printf("Enter session end date (DD-MM-YYYY): ");
        scanf("%19s", currentDate);
        strcpy(games_played[sessionIndex].end_date, currentDate);
    }

    printf("Quit operation completed.\n");
}
void end_game_session() {
    printf("\n=========== END GAME SESSION =========\n");
    printf("Active Game Sessions:\n");
    printf("Session ID  Game ID  Game Name        Players  Start Date\n");
    printf("----------  -------  ---------------  -------  ----------\n");
    
    int activeSessionsFound = 0;
    for (int i = 0; i < total_sessions; i++) {
        if (!games_played[i].is_completed) {
            int gameIndex = find_game_id(games_played[i].game_id);
            if(gameIndex != -1) {
                printf("%-11d %-7d %-15s %-7d %-10s\n",
                       games_played[i].session_id,
                       games_played[i].game_id,
                       games[gameIndex].name,
                       games_played[i].player_count,
                       games_played[i].start_date);
                activeSessionsFound = 1;
            }
        }
    }
    
    if (!activeSessionsFound) {
        printf("No active game sessions found.\n");
        return;
    }
    
    printf("\nEnter Session ID to end: ");
    int sessionId;
    if(scanf("%d", &sessionId) != 1 || sessionId <= 0){
        printf("Invalid input! Session ID must be positive.\n");
        return;
    }
    
    int sessionIndex = find_session_id(sessionId);
    
    if (sessionIndex == -1) {
        printf("Session not found!\n");
        return;
    }

    if (games_played[sessionIndex].is_completed) {
        printf("This session is already completed!\n");
        return;
    }
    
    GamePlayed session = games_played[sessionIndex];
    char endDate[20];
    int validDate = 0;

    while(!validDate) {
        printf("Enter session end date (DD-MM-YYYY): ");
        scanf("%19s", endDate);
        
        int daysActive = no_of_days(session.start_date, endDate);
        if(daysActive < 0) {
            printf("End date cannot be before start date. Please re-enter.\n");
        } else {
            validDate = 1;
            strcpy(games_played[sessionIndex].end_date, endDate);

            if(daysActive == 0) {
                daysActive = 1; 
            }
            printf("Enter scores for each player :\n");
            int totalPrizesAwarded = 0;
            int bestScore = -1;
            int winningPlayerId = -1;
            int sessionTotalPlayTime = daysActive;

            for(int i = 0; i < session.player_count;i++){
                int playerId = session.playerIds[i];
                if(session.players_quit[i] == 1){
                    printf("Player ");
                    if(playerId > 0){
                        int playerIndex = find_player_id(playerId);
                        if(playerIndex != -1){
                            printf("%s has quit the session. Skipping score entry.\n", players[playerIndex].name);
                        }
                    } else {
                        int guestIndex = find_guest_id(-playerId);
                        if(guestIndex != -1){
                            printf("Guest %s has quit the session. Skipping score entry.\n", guests[guestIndex].name);
                        }
                    }
                    printf(" Quit the session - score set to 0\n");
                    games_played[sessionIndex].scores[i] = 0;
                    games_played[sessionIndex].prizes[i] = 0;
                    games_played[sessionIndex].playTime[i] = 0;
                    continue;
                }

                printf("Enter Score for ");
                if(playerId > 0){
                    int playerIndex = find_player_id(playerId);
                    if(playerIndex != -1){
                        printf("Player %s (ID: %d): ", players[playerIndex].name, playerId);
                    }
                } else {
                    int guestIndex = find_guest_id(-playerId);
                    if(guestIndex != -1){
                        printf("Guest %s (ID: %d): ", guests[guestIndex].name, -playerId);
                    }
                }

                games_played[sessionIndex].scores[i] = 0;
                if(scanf("%d", &games_played[sessionIndex].scores[i]) != 1){
                    printf("Invalid input! Setting score to 0.\n");
                    games_played[sessionIndex].scores[i] = 0;
                }
                if(games_played[sessionIndex].scores[i] < 0){
                    printf("Score cannot be negative! Setting score to 0.\n");
                    games_played[sessionIndex].scores[i] = 0;
                }
                
                if(playerId < 0 && daysActive > 15){
                    printf(" Guest players cannot play more than 15 days. Limiting playtime to 15 days.\n");
                    printf("Setting playtime for Guest ID %d to 15 days and blocking \n", -playerId);
                    
                    games_played[sessionIndex].playTime[i] = 15;
                    int guestIndex = find_guest_id(-playerId);

                    if(guestIndex != -1){
                        guests[guestIndex].active_days = 15;
                        guests[guestIndex].isBlocked = 1;
                        printf("Guest %s (ID: %d) has been blocked due to excessive playtime.\n", guests[guestIndex].name, -playerId);
                    }
                } else {
                    games_played[sessionIndex].playTime[i] = daysActive;
                }

                games_played[sessionIndex].prizes[i] = total_prizes(session.game_id, games_played[sessionIndex].scores[i]);
                totalPrizesAwarded += games_played[sessionIndex].prizes[i];

                if(games_played[sessionIndex].scores[i] > bestScore && session.players_quit[i] == 0){
                    bestScore = games_played[sessionIndex].scores[i];
                    winningPlayerId = playerId;
                }

                printf("Active days: %d, Prizes earned: %d\n", games_played[sessionIndex].playTime[i], games_played[sessionIndex].prizes[i]);

                if(playerId > 0){
                    int playerIndex = find_player_id(playerId);
                    if(playerIndex != -1){
                        players[playerIndex].stats.total_score += games_played[sessionIndex].scores[i];
                        players[playerIndex].stats.prizes += games_played[sessionIndex].prizes[i];
                    }
                } else {
                    int guestIndex = find_guest_id(-playerId);
                    if(guestIndex != -1 && !guests[guestIndex].isBlocked){
                        guests[guestIndex].active_days += daysActive;

                        if(guests[guestIndex].active_days >= 20){
                            guests[guestIndex].isBlocked = 1;
                            printf("Guest %s (ID: %d) has been blocked due to excessive total playtime.\n", guests[guestIndex].name, -playerId);
                        }
                    }
                }
            }
            if(winningPlayerId > 0){
                int winnerIndex = find_player_id(winningPlayerId);
                if(winnerIndex != -1){
                    players[winnerIndex].stats.games_won++;
                    printf(" Player %s (ID: %d) is the winner of this session with a score of %d!\n", players[winnerIndex].name, winningPlayerId, bestScore);
                }
            }

            int gameIndex = find_game_id(session.game_id);
            if(gameIndex != -1){
                games[gameIndex].prizesAwarded += totalPrizesAwarded;
                games[gameIndex].totalPlayTime += sessionTotalPlayTime;
            }
            games_played[sessionIndex].is_completed = 1;

            printf(" Game session %d ended successfully.\n", sessionId);
            printf("Total Prizes Awarded in this session: %d\n", totalPrizesAwarded);
            printf("Session duration : %d days\n", daysActive);
        }
    }
}

void remove_inactive_guests(){
    printf("\nRemoving guests active for more than 20 days...\n");

    int removedCount = 0;
    for(int i = guest_count -1; i >= 0; i--) {
        if(guests[i].active_days > 20) {
            printf("Removing Guest %s {ID: %d} - Active Days: %d\n",
                   guests[i].name,
                   guests[i].guest_id,
                   guests[i].active_days);
            for(int j = 0; j < total_sessions; j++) {
                for(int k = 0; k < games_played[j].player_count; k++) {
                    if(games_played[j].playerIds[k] == -guests[i].guest_id) {
                        games_played[j].players_quit[k] = 1;
                        printf("Marked as quit in Session ID %d\n", games_played[j].session_id);
                    }
                }
            }
            for(int j = i; j < guest_count - 1; j++) {
                guests[j] = guests[j + 1];
            }
            guest_count--;
            removedCount++;
        }
    }

    printf("Total guests removed: %d\n", removedCount);
}

void search_games_played_by_others(){
    int playerId, gameId;
    printf("Enter Player ID to search for: ");
    if(scanf("%d", &playerId) != 1 || playerId <= 0){
        printf("Invalid input!\n");
        return;
    }
    printf("Enter Game ID to search for: ");
    if(scanf("%d", &gameId) != 1 || gameId <= 0){
        printf("Invalid input!\n");
        return;
    }

    int playerIndex = find_player_id(playerId);
    if (playerIndex == -1) {
        printf("Player with ID %d not found!\n", playerId);
        return;
    }

    int gameIndex = find_game_id(gameId);
    if (gameIndex == -1) {
        printf("Game with ID %d not found!\n", gameId);
        return;
    }

    char gameName[50];
    strcpy(gameName, games[gameIndex].name);
    char playerName[50];
    strcpy(playerName, players[playerIndex].name);

    printf("\nGames played by other players for %s (Game ID %d):\n", gameName, gameId);
    printf("Player %s (ID: %d) wants to see who else played this game\n", playerName, playerId);
    
    int found = 0;
    printf("Session ID  Game Name        Player ID   Type       Player/Guest Name  Score \n");
    printf("----------  ---------------  ---------   ---------  ----------------   ----- \n");

    for(int i = 0; i < total_sessions; i++) {
        if(games_played[i].game_id == gameId && games_played[i].is_completed) {
            
            for(int j = 0; j < games_played[i].player_count; j++) {
                int pid = games_played[i].playerIds[j];
                
                if(pid != playerId) {
                    if(pid > 0){
                        int pIndex = find_player_id(pid);
                        if(pIndex != -1){
                            printf("%-11d  %-15s  %-9d  %-9s  %-16s  %-5d\n",
                                   games_played[i].session_id,
                                   gameName,
                                   pid,
                                   "Player",
                                   players[pIndex].name,
                                   games_played[i].scores[j]);
                            found = 1;
                        }
                    }
                    else{
                        int gIndex = find_guest_id(-pid);
                        if(gIndex != -1){
                            printf("%-11d  %-15s  %-9d  %-9s  %-16s  %-5d\n",
                                   games_played[i].session_id,
                                   gameName,
                                   pid,
                                   "Guest",
                                   guests[gIndex].name,
                                   games_played[i].scores[j]);
                            found = 1;
                        }
                    }
                }
            }
        }
    }

    if(!found) {
        printf("No other players found who played '%s' (Game ID: %d).\n", gameName, gameId);
    }
}

void display_gamewise_inactive_users(){
    printf("\nGame-wise Inactive Users:\n");
    printf("Game ID  Game Name    Player ID     Player Name\n");
    printf("-------  ---------    ---------     -----------\n");

    int found = 0;
    for(int i = 0; i < total_sessions; i++){
        if(!games_played[i].is_completed){
            int gIndex = find_game_id(games_played[i].game_id);
            if(gIndex != -1){
                for(int j = 0; j < games_played[i].player_count; j++){
                    if(games_played[i].players_quit[j] == 0 && games_played[i].playerIds[j] > 0){
                        int pid = games_played[i].playerIds[j];
                        int pIndex = find_player_id(pid);
                        if(pIndex != -1){
                            printf("%-7d  %-11s  %-11d  %-13s\n",
                                   games[gIndex].game_id,
                                   games[gIndex].name,
                                   pid,
                                   players[pIndex].name);
                            found = 1;
                        }
                    }
                }
            }
        }
    }

    if(!found){
        printf("No inactive users found.\n");
    }
}

void display_active_users(){
    int k;
    printf("Enter minimum number of active games (K): ");
    if(scanf("%d", &k) != 1 || k < 0){
        printf("Invalid input! K must be a non-negative number.\n");
        return;
    }

    printf("\nPlayers with more than %d active games:\n", k);
    printf("Player ID   Player Name       Active Games\n");
    printf("---------   ---------------   ------------\n");

    int found = 0;
    for(int i = 0; i < player_count; i++) {
        int activeGames = count_player_active_games(players[i].player_id);
        if(activeGames > k) {
            printf("%-10d  %-15s  %-12d\n",
                   players[i].player_id,
                   players[i].name,
                   activeGames);
            found = 1;
        }
    }

    if(!found) {
        printf("No players found with more than %d active games.\n", k);
    }
}
void display_games_with_most_prizes(){
    GameMaster tempGames[MAX_GAMES];
    for(int i = 0; i < game_count; i++) {
        tempGames[i] = games[i];
    }
    for(int i = 0; i < game_count - 1; i++) {
        for(int j = 0; j < game_count - i - 1; j++) {
            if(tempGames[j].prizesAwarded < tempGames[j + 1].prizesAwarded) {
                GameMaster temp = tempGames[j];
                tempGames[j] = tempGames[j + 1];
                tempGames[j + 1] = temp;
            }
        }
    }

    printf("\nGames with Most Prizes Awarded:\n");
    printf("Game ID  Game Name        Prizes Awarded\n");
    printf("-------  ---------------  --------------\n");

    for(int i = 0; i < game_count; i++) {
        printf("%-7d  %-15s  %-14d\n",
               tempGames[i].game_id,
               tempGames[i].name,
               tempGames[i].prizesAwarded);
    }
}

void display_top5_longest_played_games(){
    GameMaster tempGames[MAX_GAMES];
    for(int i = 0; i < game_count; i++) {
        tempGames[i] = games[i];
    }
    for(int i = 0; i < game_count - 1; i++) {
        for(int j = 0; j < game_count - i - 1; j++) {
            if(tempGames[j].totalPlayTime < tempGames[j + 1].totalPlayTime) {
                GameMaster temp = tempGames[j];
                tempGames[j] = tempGames[j + 1];
                tempGames[j + 1] = temp;
            }
        }
    }

    printf("\nTop 5 Longest Played Games:\n");
    printf("Game ID  Game Name        Total Play Time (days)\n");
    printf("-------  ---------------  ----------------------\n");

    int topCount = game_count < 5 ? game_count : 5;
    for(int i = 0; i < topCount; i++) {
        printf("%-7d  %-15s  %-22d\n",
               tempGames[i].game_id,
               tempGames[i].name,
               tempGames[i].totalPlayTime);
    }
}

// Players of Game G
void display_players_of_game(){
    int gameId;
    printf("Enter Game ID : ");
    if(scanf("%d", &gameId) != 1 || gameId <= 0){
        printf("Invalid input!\n");
        return;
    }
    if(find_game_id(gameId) == -1){
        printf("Game not found!\n");
        return;
    }
    int playingPlayers[MAX_PLAYERS];
    int playerScores[MAX_PLAYERS];
    int playingCount = 0;

    for(int i = 0; i < total_sessions; i++) {
        if(games_played[i].game_id == gameId && !games_played[i].is_completed) {
            for(int j = 0; j < games_played[i].player_count; j++) {
                if(games_played[i].players_quit[j] == 0 && games_played[i].playerIds[j] > 0){
                    playingPlayers[playingCount] = games_played[i].playerIds[j];
                    playerScores[playingCount] = games_played[i].scores[j];
                    playingCount++;
                }
            }
        }        
    }

    if(playingCount == 0) {
        printf("No active players found for Game ID %d.\n", gameId);
        return;
    }
    for(int i = 0; i < playingCount - 1; i++) {
        for(int j = 0; j < playingCount - i - 1; j++) {
            if(playerScores[j] < playerScores[j + 1]) {
                int tempScore = playerScores[j];
                playerScores[j] = playerScores[j + 1];
                playerScores[j + 1] = tempScore;

                int tempId = playingPlayers[j];
                playingPlayers[j] = playingPlayers[j + 1];
                playingPlayers[j + 1] = tempId;
            }
        }
    }

    printf("\nPlayers currently playing Game ID %d:\n", gameId);
    printf("Player ID   Player Name       Score\n");
    printf("---------   ---------------   -----\n");
    for(int i = 0; i < playingCount; i++) {
        int playerIndex = find_player_id(playingPlayers[i]);
        if(playerIndex != -1) {
            printf("%-10d  %-15s  %-5d\n",
                   playingPlayers[i],
                   players[playerIndex].name,
                   playerScores[i]);
        }
    }
}

int main() {
    int choice;
    games[0].game_id = game_id++;
    strcpy(games[0].name, "Chess");
    games[0].type = 1;
    games[0].players_req = 2;
    games[0].prizesAwarded = 25;
    games[0].totalPlayTime = 120;

    games[1].game_id = game_id++;
    strcpy(games[1].name, "Poker");
    games[1].type = 2;
    games[1].players_req = 4;
    games[1].prizesAwarded = 45;
    games[1].totalPlayTime = 85;

    games[2].game_id = game_id++;
    strcpy(games[2].name, "Monopoly");
    games[2].type = 2;
    games[2].players_req = 4;
    games[2].prizesAwarded = 30;
    games[2].totalPlayTime = 200;

    games[3].game_id = game_id++;
    strcpy(games[3].name, "Checkers");
    games[3].type = 1;
    games[3].players_req = 2;
    games[3].prizesAwarded = 20;
    games[3].totalPlayTime = 75;

    games[4].game_id = game_id++;
    strcpy(games[4].name, "Uno");
    games[4].type = 2;
    games[4].players_req = 4;
    games[4].prizesAwarded = 35;
    games[4].totalPlayTime = 60;

    game_count = 5;

    players[0].player_id = player_id++;
    strcpy(players[0].name, "bhAAi");
    strcpy(players[0].reg_date, "15-01-2024");
    players[0].stats.games_played = 8;  
    players[0].stats.games_won = 5;     
    players[0].stats.total_score = 2500; 
    players[0].stats.prizes = 15;      

    players[0].game_preferences[0] = 1; 
    players[0].game_preferences[1] = 3; 
    players[0].game_preferences[2] = 5; 
    players[0].game_preferences[3] = 2; 
    players[0].pref_count = 4;

    players[1].player_id = player_id++;
    strcpy(players[1].name, "Bob");
    strcpy(players[1].reg_date, "20-02-2024");
    players[1].stats.games_played = 12; 
    players[1].stats.games_won = 8;     
    players[1].stats.total_score = 3200; 
    players[1].stats.prizes = 20;      

    players[1].game_preferences[0] = 2; 
    players[1].game_preferences[1] = 4; 
    players[1].game_preferences[2] = 3; 
    players[1].game_preferences[3] = 1; 
    players[1].game_preferences[4] = 5; 
    players[1].pref_count = 5;

    players[2].player_id = player_id++;
    strcpy(players[2].name, "praBoss");
    strcpy(players[2].reg_date, "10-03-2024");
    players[2].stats.games_played = 6;  
    players[2].stats.games_won = 3;     
    players[2].stats.total_score = 1800; 
    players[2].stats.prizes = 8;        
    
    players[2].game_preferences[0] = 5; 
    players[2].game_preferences[1] = 3; 
    players[2].game_preferences[2] = 1; 
    players[2].game_preferences[3] = 4; 
    players[2].pref_count = 4;

    players[3].player_id = player_id++;
    strcpy(players[3].name, "leo");
    strcpy(players[3].reg_date, "05-04-2024");
    players[3].stats.games_played = 4;
    players[3].stats.games_won = 2;
    players[3].stats.total_score = 900;
    players[3].stats.prizes = 4;
    
    players[3].game_preferences[0] = 3; 
    players[3].game_preferences[1] = 1; 
    players[3].game_preferences[2] = 4;
    players[3].pref_count = 3;

    players[4].player_id = player_id++;
    strcpy(players[4].name, "janaki ram");
    strcpy(players[4].reg_date, "12-04-2024");
    players[4].stats.games_played = 10;
    players[4].stats.games_won = 6;
    players[4].stats.total_score = 2800;
    players[4].stats.prizes = 18;

    players[4].game_preferences[0] = 2;
    players[4].game_preferences[1] = 3; 
    players[4].game_preferences[2] = 1; 
    players[4].game_preferences[3] = 5;
    players[4].game_preferences[4] = 4; 
    players[4].pref_count = 5;

    player_count = 5;
    
    guests[0].guest_id = guest_id++;
    strcpy(guests[0].name, "Guest1");
    strcpy(guests[0].start_date, "01-04-2024");
    guests[0].active_days = 12;
    guests[0].isBlocked = 0;

    guests[1].guest_id = guest_id++;
    strcpy(guests[1].name, "Guest2");
    strcpy(guests[1].start_date, "05-04-2024");
    guests[1].active_days = 25;
    guests[1].isBlocked = 1;
    
    guests[2].guest_id = guest_id++;
    strcpy(guests[2].name, "Guest3");
    strcpy(guests[2].start_date, "10-04-2024");
    guests[2].active_days = 8;
    guests[2].isBlocked = 0;

    guest_count = 3;

    games_played[0].session_id = session_id++;
    games_played[0].game_id = 1; 
    games_played[0].playerIds[0] = 101; 
    games_played[0].playerIds[1] = 102;
    games_played[0].player_count = 2;
    games_played[0].scores[0] = 850;
    games_played[0].scores[1] = 720;
    games_played[0].prizes[0] = 3;
    games_played[0].prizes[1] = 1;
    games_played[0].playTime[0] = 5;
    games_played[0].playTime[1] = 5;
    strcpy(games_played[0].start_date, "10-04-2024");
    strcpy(games_played[0].end_date, "15-04-2024");
    games_played[0].is_completed = 1;
    games_played[0].players_quit[0] = 0;
    games_played[0].players_quit[1] = 0;

    games_played[1].session_id = session_id++;
    games_played[1].game_id = 3; 
    games_played[1].playerIds[0] = 101; 
    games_played[1].playerIds[1] = 103; 
    games_played[1].playerIds[2] = -1; 
    games_played[1].player_count = 3;
    games_played[1].scores[0] = 450;
    games_played[1].scores[1] = 380;
    games_played[1].scores[2] = 290;
    games_played[1].prizes[0] = 1;
    games_played[1].prizes[1] = 0;
    games_played[1].prizes[2] = 0;
    games_played[1].playTime[0] = 8;
    games_played[1].playTime[1] = 8;
    games_played[1].playTime[2] = 8;
    strcpy(games_played[1].start_date, "18-04-2024");
    strcpy(games_played[1].end_date, "26-04-2024");
    games_played[1].is_completed = 1;
    games_played[1].players_quit[0] = 0;
    games_played[1].players_quit[1] = 0;
    games_played[1].players_quit[2] = 0;

    games_played[2].session_id = session_id++;
    games_played[2].game_id = 2; 
    games_played[2].playerIds[0] = 102; 
    games_played[2].playerIds[1] = 103; 
    games_played[2].playerIds[2] = -2; 
    games_played[2].player_count = 3;
    games_played[2].scores[0] = 0;
    games_played[2].scores[1] = 0;
    games_played[2].scores[2] = 0;
    games_played[2].prizes[0] = 0;
    games_played[2].prizes[1] = 0;
    games_played[2].prizes[2] = 0;
    games_played[2].playTime[0] = 3;
    games_played[2].playTime[1] = 3;
    games_played[2].playTime[2] = 3;
    strcpy(games_played[2].start_date, "28-04-2024");
    strcpy(games_played[2].end_date, "");
    games_played[2].is_completed = 0;
    games_played[2].players_quit[0] = 0;
    games_played[2].players_quit[1] = 0;
    games_played[2].players_quit[2] = 0;

    games_played[3].session_id = session_id++;
    games_played[3].game_id = 4; 
    games_played[3].playerIds[0] = 104; 
    games_played[3].playerIds[1] = 105; 
    games_played[3].playerIds[2] = -3; 
    games_played[3].player_count = 3;
    games_played[3].scores[0] = 620;
    games_played[3].scores[1] = 580;
    games_played[3].scores[2] = 490;
    games_played[3].prizes[0] = 1;
    games_played[3].prizes[1] = 0;
    games_played[3].prizes[2] = 0;
    games_played[3].playTime[0] = 6;
    games_played[3].playTime[1] = 6;
    games_played[3].playTime[2] = 6;
    strcpy(games_played[3].start_date, "20-04-2024");
    strcpy(games_played[3].end_date, "26-04-2024");
    games_played[3].is_completed = 1;
    games_played[3].players_quit[0] = 0;
    games_played[3].players_quit[1] = 0;
    games_played[3].players_quit[2] = 0;

    games_played[4].session_id = session_id++;
    games_played[4].game_id = 5; 
    games_played[4].playerIds[0] = 101; 
    games_played[4].playerIds[1] = 105; 
    games_played[4].playerIds[2] = 103; 
    games_played[4].playerIds[3] = 104; 
    games_played[4].player_count = 4;
    games_played[4].scores[0] = 0;
    games_played[4].scores[1] = 0;
    games_played[4].scores[2] = 0;
    games_played[4].scores[3] = 0;
    games_played[4].prizes[0] = 0;
    games_played[4].prizes[1] = 0;
    games_played[4].prizes[2] = 0;
    games_played[4].prizes[3] = 0;
    games_played[4].playTime[0] = 2;
    games_played[4].playTime[1] = 2;
    games_played[4].playTime[2] = 2;
    games_played[4].playTime[3] = 2;
    strcpy(games_played[4].start_date, "29-04-2024");
    strcpy(games_played[4].end_date, "");
    games_played[4].is_completed = 0;
    games_played[4].players_quit[0] = 0;
    games_played[4].players_quit[1] = 0;
    games_played[4].players_quit[2] = 0;
    games_played[4].players_quit[3] = 0;

    total_sessions = 5;

    do {
        printf("\n=============================\n         SOCIAL GAMING SYSTEM   \n=============================\n");
        printf("1. Register New Player\n");
        printf("2. Add Game Preferences\n");
        printf("3. Remove Inactive Guests\n");
        printf("4. Start Game Session\n");
        printf("5. Quit Game Session\n");
        printf("6. End Game Session\n");
        printf("7. Display All Games\n");
        printf("8. Display Players of a Game\n");
        printf("9. Display Game-wise Inactive Users\n");
        printf("10. Display Games with Most Prizes\n");
        printf("11. Display Top 5 Longest Played Games\n");
        printf("12. Search Games Played by Others\n");
        printf("13. Display Active Users\n");
        printf("14. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1:
                reg_new_player();
                break;
            case 2:
                {
                    int playerId;
                    printf("Enter Player ID: ");
                    scanf("%d", &playerId);
                    add_game_preferences(playerId);
                }
                break;
            case 3:
                remove_inactive_guests();     
                break;
            case 4:
                start_game_session();
                break;
            case 5:
                quit_game_session();               
                break;
            case 6:
                end_game_session();                
                break;
            case 7:
                display_games();                
                break;
            case 8:
                display_players_of_game();              
                break;
            case 9:
                display_gamewise_inactive_users();
                break;
            case 10:
                display_games_with_most_prizes();
                break;
            case 11:
                display_top5_longest_played_games();
                break;
            case 12:
                search_games_played_by_others();
                break;
            case 13:
                display_active_users();
                break;
            case 14:
                printf("Exiting the system. Goodbye!\n");
                break;
            default:
                printf("Invalid choice! Please try again.\n");
        }
    } while(choice != 14);
    
    return 0;
}
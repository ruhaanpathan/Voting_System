#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CANDIDATES 10
#define MAX_VOTERS 100

// Candidate and Voter Structures
typedef struct {
    int id;
    char name[50];
    int votes;
} Candidate;

typedef struct {
    int id;
    char name[50];
    int age;
    int hasVoted;
    int votedFor;
} Voter;

// Global variables
Candidate candidates[MAX_CANDIDATES];
Voter voters[MAX_VOTERS];
int candidateCount = 0;
int voterCount = 0;
int votingOpen=0;
time_t votingEndTime;

// Function Prototypes
void adminMenu();
void setVotingTime();
void endVoting();
void castVote(int voterIndex);
void viewResults();
void saveData();
void loadData();
int findCandidate(int id);
int findVoter(int id);
void goBack();

// Main Function
int main() {
    int choice;
    loadData(); // Load previous voting data

    while (1) {
        printf("\nVoting System Main Menu\n");
        printf("1. Admin Login\n");
        printf("2. Voter Login\n");

        // Only show View Results option if voting is closed
        if (votingOpen==0) {
            printf("3. View Results\n");
        }
        printf("4. Exit\n");
        printf("Choose an option: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                adminMenu();
                break;
            case 2:
                {
                    int voterId;
                    int age;
                    printf("Enter your age: ");
                    scanf("%d",&age);
                    if(age>=18){
                    printf("Enter Voter ID: ");
                    scanf("%d", &voterId);

                    int voterIndex = findVoter(voterId);
                    if (voterIndex != -1) {
                        printf("Voter '%s' is already registered.\n", voters[voterIndex].name);
                        printf("Do you want to proceed with voting? (yes/no): ");
                        char proceed[4];
                        scanf("%s", proceed);

                        if (strcmp(proceed, "yes") == 0) {
                            if (voters[voterIndex].hasVoted) {
                                printf("You have already voted.\n");
                            } else {
                                castVote(voterIndex);
                            }
                        } else {
                            printf("Returning to main menu...\n");
                        }
                    } else {
                        char name[50];
                        printf("Voter not registered. Please enter your name: ");
                        scanf("%s", name);

                        // Register the voter
                        voters[voterCount].id = voterId;
                        strcpy(voters[voterCount].name, name);
                        voters[voterCount].hasVoted = 0;
                        voterCount++;

                        printf("Registration successful for Voter '%s' with ID %d.\n", name, voterId);
                    }}
                    else{printf("You are not eligible to vote");}
                }
                break;
            case 3:

                if (votingOpen==0) {
                    viewResults();
                } else {
                    printf("Voting is still ongoing. Results will be available after voting ends.\n");
                }
                break;
            case 4:
                saveData();
                printf("Exiting...\n");
                return 0;
            default:
                printf("Invalid choice! Try again.\n");
        }
    }
}

// Admin Menu
void adminMenu() {
    char id[50] = "admin";
    char password[50] = "password";
    char enteredId[50], enteredPass[50];

    printf("Admin Login\n");
    printf("Enter ID: ");
    scanf("%s", enteredId);
    printf("Enter Password: ");
    scanf("%s", enteredPass);

    if (strcmp(enteredId, id) == 0 && strcmp(enteredPass, password) == 0) {
        int choice;
        while (1) {
            printf("\nAdmin Menu\n");
            printf("1. Add Candidate\n");
            printf("2. Remove Candidate\n");
            printf("3. View Results\n");
            printf("4. Set Voting Time\n");
            printf("5. Restart Voting\n");
            printf("6. End Voting\n");
            printf("7. Exit\n");
            printf("Choose an option: ");
            scanf("%d", &choice);

            switch (choice) {
                case 1:
                    {
                        char exitChoice[10];
                        int id;
                        char name[50];
                        
                        while (1) {
                            printf("Enter Candidate ID (or type 'exit' to stop): ");
                            scanf("%s", exitChoice);

                            if (strcmp(exitChoice, "exit") == 0) {
                                break;  // Exit the candidate addition loop
                            }

                            id = atoi(exitChoice); // Convert to integer if a valid ID is provided

                            if (findCandidate(id) != -1) {
                                printf("Error: Candidate ID %d already exists. Please choose a different ID.\n", id);
                                continue;
                            }

                            printf("Enter Candidate Name: ");
                            scanf("%s", name);
                            candidates[candidateCount].id = id;
                            strcpy(candidates[candidateCount].name, name);
                            candidates[candidateCount].votes = 0;
                            candidateCount++;
                            printf("Candidate '%s' added successfully with ID: %d!\n", name, id);
                        }
                        break;
                    }
                case 2:
                    {
                        int id;
                        printf("Enter Candidate ID to remove: ");
                        scanf("%d", &id);
                        int index = findCandidate(id);
                        if (index != -1) {
                            for (int i = index; i < candidateCount - 1; i++) {
                                candidates[i] = candidates[i + 1];
                            }
                            candidateCount--;
                            printf("Candidate with ID %d has been removed.\n", id);
                        } else {
                            printf("Candidate not found!\n");
                        }
                        break;
                    }
                case 3:
                    viewResults();
                    break;
                case 4:
                    setVotingTime();
                    break;
                case 5:
                    if (votingOpen) {
                        printf("Error: Voting is still ongoing. You cannot restart the voting now.\n");
                    } else {
                        printf("Voting has been restarted.\n");
                        candidateCount = 0;
                        voterCount = 0;
                        votingOpen = 1;
                    }
                    break;
                case 6:
                    endVoting();
                    break;
                case 7:
                    return;
                default:
                    printf("Invalid choice! Try again.\n");
            }
        }
    } else {
        printf("Invalid ID or Password. Access Denied.\n");
    }
}

// Set Voting Time
void setVotingTime() {
    int hours, minutes;
    printf("Enter voting time (hours minutes): ");
    scanf("%d %d", &hours, &minutes);

    // Get the current time
    time_t currentTime = time(NULL);
    struct tm *votingTime = localtime(&currentTime); // Get local time using localtime()

    // Set the voting end time based on input hours and minutes
    votingTime->tm_hour = hours;
    votingTime->tm_min = minutes;
    votingTime->tm_sec = 0; 

    // Convert tm structure back to time_t for comparison
    votingEndTime = mktime(votingTime);

    printf("Voting time has been set to %d hours %d min.\n",hours,minutes);
}

// End the voting process
void endVoting() {
    votingOpen = 0;
    printf("Voting has ended.\n");
}

// View Results (admin can see votes for all candidates)
void viewResults() {
    
    if (candidateCount == 0) {
        printf("No candidates to display.\n");
        return;
    }
if (votingOpen){printf("Voting is still ongoing. You cannot see the result now.\n");}
else{
    printf("Candidate Results:\n");
    int maxVotes = -1;
    int tie = 0;
int q;
    for (int i = 0; i < candidateCount; i++) {
        printf("%s (%d votes)\n", candidates[i].name, candidates[i].votes);
        if (candidates[i].votes > maxVotes) {
            maxVotes = candidates[i].votes;
            q=i;
            tie = 0;
        } else if (candidates[i].votes == maxVotes) {
            tie = 1;
        }
    }

    if (tie) {
        printf("It's a tie!\n");
    } else {
        printf("Winner: %s\n",candidates[q].name );
    }
}}

// Cast a vote for a candidate
void castVote(int voterIndex) {
    if (votingOpen){
    printf("Candidates:\n");
    for (int i = 0; i < candidateCount; i++) {
        printf("%d. %s\n", candidates[i].id, candidates[i].name);
    }

    int choice;
    printf("Enter the ID of the candidate you want to vote for: ");
    scanf("%d", &choice);

    int candidateIndex = findCandidate(choice);
    if (candidateIndex == -1) {
        printf("Invalid candidate ID.\n");
        return;}
    
    

    // Register the vote
    voters[voterIndex].hasVoted = 1;
    voters[voterIndex].votedFor = choice;
    candidates[candidateIndex].votes++;

    printf("Vote cast successfully!\n");}
    else{printf("voting has beenn ended.\n You can now view the results\n");
    return;}
}

// Save data to file
void saveData() {
    FILE *file = fopen("votingData.dat", "wb");
    if (file) {
        fwrite(&candidateCount, sizeof(int), 1, file);
        fwrite(candidates, sizeof(Candidate), candidateCount, file);
        fwrite(&voterCount, sizeof(int), 1, file);
        fwrite(voters, sizeof(Voter), voterCount, file);
        fclose(file);
    } else {
        printf("Error saving data.\n");
    }
}

// Load data from file
void loadData() {
    FILE *file = fopen("votingData.dat", "rb");
    if (file) {
        fread(&candidateCount, sizeof(int), 1, file);
        fread(candidates, sizeof(Candidate), candidateCount, file);
        fread(&voterCount, sizeof(int), 1, file);
        fread(voters, sizeof(Voter), voterCount, file);
        fclose(file);
    }
}

// Find a candidate by ID
int findCandidate(int id) {
    for (int i = 0; i < candidateCount; i++) {
        if (candidates[i].id == id) {
            return i;
        }
    }
    return -1;
}

// Find a voter by ID
int findVoter(int id) {
    for (int i = 0; i < voterCount; i++) {
        if (voters[i].id == id) {
            return i;
        }
    }
    return -1;
}


/*

PS C:\Users\Ruhaan Pathan\Ruhaan\Python\Practice\.vscode> gcc .\voting.c
PS C:\Users\Ruhaan Pathan\Ruhaan\Python\Practice\.vscode> .\a.exe

Voting System Main Menu
1. Admin Login
2. Voter Login
3. View Results
4. Exit
Choose an option: 1
Admin Login
Enter ID: admin
Enter Password: pass
Invalid ID or Password. Access Denied.

Voting System Main Menu
1. Admin Login
2. Voter Login
3. View Results
4. Exit
Choose an option: admin
Admin Login
Enter ID: Enter Password: password

Admin Menu
1. Add Candidate
2. Remove Candidate
3. View Results
4. Set Voting Time
5. Restart Voting
6. End Voting
7. Exit
Choose an option: 5
Voting has been restarted.

Admin Menu
1. Add Candidate
2. Remove Candidate
3. View Results
4. Set Voting Time
5. Restart Voting
6. End Voting
7. Exit
Choose an option: 1
Enter Candidate ID (or type 'exit' to stop): 1
Enter Candidate Name: ruhaan
Candidate 'ruhaan' added successfully with ID: 1!
Enter Candidate ID (or type 'exit' to stop): 2
Enter Candidate Name: dhruvin
Candidate 'dhruvin' added successfully with ID: 2!
Enter Candidate ID (or type 'exit' to stop): exit

Admin Menu
1. Add Candidate
2. Remove Candidate
3. View Results
4. Set Voting Time
5. Restart Voting
6. End Voting
7. Exit
Choose an option: 7

Voting System Main Menu
1. Admin Login
2. Voter Login
4. Exit
Choose an option: 2
Enter your age: 18
Enter Voter ID: 1
Voter not registered. Please enter your name: hello
Registration successful for Voter 'hello' with ID 1.

Voting System Main Menu
1. Admin Login
2. Voter Login
4. Exit
Choose an option: 2
Enter your age: 1
You are not eligible to vote
Voting System Main Menu
1. Admin Login
2. Voter Login
4. Exit
Choose an option: 2
Enter your age: 18
Enter Voter ID: 1
Voter 'hello' is already registered.
Do you want to proceed with voting? (yes/no): yes
Candidates:
1. ruhaan
2. dhruvin
Enter the ID of the candidate you want to vote for: 1
Vote cast successfully!

Voting System Main Menu
1. Admin Login
2. Voter Login
4. Exit
Choose an option: 2
Enter your age: 19
Enter Voter ID: 1
Voter 'hello' is already registered.
Do you want to proceed with voting? (yes/no): yes
You have already voted.

Voting System Main Menu
1. Admin Login
2. Voter Login
4. Exit
Choose an option: 2
Enter your age: 19
Enter Voter ID: 1
Voter 'hello' is already registered.
Do you want to proceed with voting? (yes/no): no
Returning to main menu...

Voting System Main Menu
1. Admin Login
2. Voter Login
4. Exit
Choose an option: 2
Enter your age: 18 
Enter Voter ID: 2
Voter not registered. Please enter your name: hii
Registration successful for Voter 'hii' with ID 2.

Voting System Main Menu
1. Admin Login
2. Voter Login
4. Exit
Choose an option: 2
Enter your age: 22
Enter Voter ID: 2
Voter 'hii' is already registered.
Do you want to proceed with voting? (yes/no): yes
Candidates:
1. ruhaan
2. dhruvin
Enter the ID of the candidate you want to vote for: 2
Vote cast successfully!

Voting System Main Menu
1. Admin Login
2. Voter Login
4. Exit
Choose an option: 2
Enter your age: 33
Enter Voter ID: 3
Voter not registered. Please enter your name: hiii
Registration successful for Voter 'hiii' with ID 3.

Voting System Main Menu
1. Admin Login
2. Voter Login
4. Exit
Choose an option: 2
Enter your age: 33
Enter Voter ID: 3
Voter 'hiii' is already registered.
Do you want to proceed with voting? (yes/no): yes
Candidates:
1. ruhaan
2. dhruvin
Enter the ID of the candidate you want to vote for: 1
Vote cast successfully!

Voting System Main Menu
1. Admin Login
2. Voter Login
4. Exit
Choose an option: 1
Admin Login
Enter ID: admin
Enter Password: password

Admin Menu
1. Add Candidate
2. Remove Candidate
3. View Results
4. Set Voting Time
5. Restart Voting
6. End Voting
7. Exit
Choose an option: 5
Error: Voting is still ongoing. You cannot restart the voting now.

Admin Menu
1. Add Candidate
2. Remove Candidate
3. View Results
4. Set Voting Time
5. Restart Voting
6. End Voting
7. Exit
Choose an option: 1
Enter Candidate ID (or type 'exit' to stop): 3
Enter Candidate Name: hello
Candidate 'hello' added successfully with ID: 3!
Enter Candidate ID (or type 'exit' to stop): exit

Admin Menu
1. Add Candidate
2. Remove Candidate
3. View Results
4. Set Voting Time
5. Restart Voting
6. End Voting
7. Exit
Choose an option: 7

Voting System Main Menu
1. Admin Login
2. Voter Login
4. Exit
Choose an option: 2
Enter your age: 22
Enter Voter ID: 2
Voter 'hii' is already registered.
Do you want to proceed with voting? (yes/no): yes
You have already voted.

Voting System Main Menu
1. Admin Login
2. Voter Login
4. Exit
Choose an option: 2
Enter your age: 4
You are not eligible to vote
Voting System Main Menu
1. Admin Login
2. Voter Login
4. Exit
Choose an option: 2  
Enter your age: 22
Enter Voter ID: 4
Voter not registered. Please enter your name: hee
Registration successful for Voter 'hee' with ID 4.

Voting System Main Menu
1. Admin Login
2. Voter Login
4. Exit
Choose an option: 2
Enter your age: 22
Enter Voter ID: 4
Voter 'hee' is already registered.
Do you want to proceed with voting? (yes/no): yes
Candidates:
1. ruhaan
2. dhruvin
3. hello
Enter the ID of the candidate you want to vote for: 1
Vote cast successfully!

Voting System Main Menu
1. Admin Login
2. Voter Login
4. Exit
Choose an option: 1
Admin Login
Enter ID: admin
Enter Password: password

Admin Menu
1. Add Candidate
2. Remove Candidate
3. View Results
4. Set Voting Time
5. Restart Voting
6. End Voting
7. Exit
Choose an option: 2
Enter Candidate ID to remove: 3
Candidate with ID 3 has been removed.

Admin Menu
1. Add Candidate
2. Remove Candidate
3. View Results
4. Set Voting Time
5. Restart Voting
6. End Voting
7. Exit
Choose an option: 7

Voting System Main Menu
1. Admin Login
2. Voter Login
4. Exit
Choose an option: 1
Admin Login
Enter ID: admin
Enter Password: password

Admin Menu
1. Add Candidate
2. Remove Candidate
3. View Results
4. Set Voting Time
5. Restart Voting
6. End Voting
7. Exit
Choose an option: 6
Voting has ended.

Admin Menu
1. Add Candidate
2. Remove Candidate
3. View Results
4. Set Voting Time
5. Restart Voting
6. End Voting
7. Exit
Choose an option: 7

Voting System Main Menu
1. Admin Login
2. Voter Login
3. View Results
4. Exit
Choose an option: 3
Candidate Results:
ruhaan (3 votes)
dhruvin (1 votes)
Winner: ruhaan

Voting System Main Menu
1. Admin Login
2. Voter Login
3. View Results
4. Exit
Choose an option: 4
Exiting...
PS C:\Users\Ruhaan Pathan\Ruhaan\Python\Practice\.vscode> .\a.exe

Voting System Main Menu
1. Admin Login
2. Voter Login
3. View Results
4. Exit
Choose an option: 3
Candidate Results:
ruhaan (3 votes)
dhruvin (1 votes)
Winner: ruhaan

Voting System Main Menu
1. Admin Login
2. Voter Login
3. View Results
4. Exit
Choose an option: 4
Exiting...
*/
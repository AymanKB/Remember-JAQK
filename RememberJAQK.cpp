#include <iostream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <cstdio> // for printf
using namespace std;

// Constants 
enum DisplayMode {FACE_DOWN, FACE_UP, REMOVED};

static const string RANKS[] = {"J", "Q", "K", "A"};
static const char   SUITS[] = {char(3), char(5), char(4), char(6)}; // alternate red and black
static const int NumRanks = sizeof(RANKS) / sizeof(string);
static const int NumSuits = sizeof(SUITS) / sizeof(char);
static const int NumCard  = NumRanks * NumSuits;

// Globals
static string ranks[NumCard]; 
static char   suits[NumCard] = {0};
static DisplayMode  state[NumCard] = {FACE_DOWN};

// Player
static const int NumPlayer = 2;
static int playerScore[NumPlayer] = {0};
static int whichPlayer = 0;

// Card Display Settings
static const int NumRow = 2;
static const int NumColumn = NumCard / NumRow;

// Gameplay
static int numPairs = 0;

void ShowCards(int numRow, int numColumn)
{
	string formatShow[5] = {"+---+", "|%-2s |", "| %c |", "| %2s|", "+---+"};
	string formatHide[5] = {"+---+", "| %2d|",  "|   |",  "|   |",  "+---+"};
	
	for (int row = 0; row < numRow; ++row)
	{
		for (int f = 0; f < 5; ++f)
		{
			int card = row * numColumn;
			
			for (int col = 0; col < numColumn; ++col)
			{
                switch(state[card])
                {
					// Show card
                    case FACE_UP:
                    {
                        if (f == 0 || f == 4)
                            printf(formatShow[f].c_str());
                        else if (f == 1 || f == 3)
                            printf(formatShow[f].c_str(), ranks[row * numColumn + col].c_str());
                        else if (f == 2)
                            printf(formatShow[f].c_str(), suits[row * numColumn + col]);
                    } break;
                    // Hide card
                    case FACE_DOWN:
                    {
                        if (f == 0 || f == 4)
                            printf(formatHide[f].c_str());
                        else if (f == 1 || f == 3)
                            printf(formatHide[f].c_str(), card + 1);
                        else if (f == 2)
                            printf(formatHide[f].c_str());
                    } break;
                    // Gone
                    case REMOVED:
                    {
                        printf("     ");
                    } break;
                }
				
				card++;
				printf(" ");
			}
			cout << endl;
		}
		cout << endl;
	}
}

void GameFirstScreen()
{
	system("cls");
	cout << "+---------------+" << endl;
	cout << "| Frank Carrano |" << endl;
	cout << "|               |" << endl;
	cout << "| 1112223344    |" << endl;
	cout << "+---------------+" << endl << endl; 
	system("pause");
}

void Menu()
{
    system("cls");
	cout << "\n\t   ...:: Remember JAQK ::...\n" << endl;
    ShowCards(NumRow, NumColumn);
    printf("\tPlayer 1 ->  %d : %d  <- Player 2\n\n", playerScore[0], playerScore[1]);
}

void GameInit()
{		
	// Create the Deck
	int k = 0;
	for (int js = 0; js < NumSuits; ++js)
	for (int jr = 0; jr < NumRanks; ++jr)
	{
		ranks[k] = RANKS[jr];
		suits[k] = SUITS[js];
		k++;
	}
	
	srand(time(NULL));
	for (int i = 0; i < NumCard; ++i)
	{
        // Shuffle the Deck
		int index = rand() % (NumCard - i) + i;
		swap(ranks[i], ranks[index]);
		swap(suits[i], suits[index]);
        
        // Reset state array
        state[i] = FACE_DOWN;
	}
    
    playerScore[0] = playerScore[1] = 0;
    whichPlayer = 0;
    numPairs = 0;
}

bool GameLoop(int& whichPlayer)
{
	int cardChoice[NumPlayer] = {0};
	int cardNum;
	
	// Ask player for card number (2 cards)
    const char* Label[] = {"FIRST", "SECOND"};
    
	for (int i = 0; i < 2; ++i)
	{
        // Show cards and scores
        Menu();

        cout << "Player " << whichPlayer + 1 << ": " << endl;
        cout << "=========" << endl;
        
		// Verify the card number (i.e. must not have been state)
        bool error;
		do
		{
			printf("Enter your %s card number to flip (1 to %d): ", Label[i], NumCard);
			
			// if not an integer or there exist at least one non-newline letter after the integer
			if ( !(cin >> cardNum) || cin.peek() != '\n')
			{
				cin.clear();
				cin.ignore(numeric_limits<int>::max(), '\n');
				cardNum = 0; // reset to zero so that it makes error = true later.
			}
            cardNum--; // revert to index numbering
			
            error = (cardNum < 0 || cardNum >= NumCard || state[cardNum] != FACE_DOWN);
            if (error)
                cout << "\aERROR: Invalid Card Number" << endl;
            
		} while (error);
		// repeat above if cardNum is in invalid range or the card number is not in FACE_DOWN state.
		
		state[cardNum] = FACE_UP;
		cardChoice[i] = cardNum;
	}
	
	// Check if both chosen cards forms a pair
    bool matchSuit = false;
    for (int i = 0; i < NumSuits; ++i)
        if (suits[ cardChoice[0] ] == SUITS[i])
        {
            int j = (i + 2) % NumSuits;
            matchSuit = (suits[ cardChoice[1] ] == SUITS[j]);
            break;
        }
    
    // Show cards and scores again
    Menu();
    
    // If yes, update player score and number of pairs taken
    if (ranks[ cardChoice[0] ] == ranks[ cardChoice[1] ] && matchSuit)
    {
        cout << "MATCH! Your turn continues." << endl;
        state[ cardChoice[0] ] = state[ cardChoice[1] ] = REMOVED;
        playerScore[whichPlayer]++;
        numPairs++;
    }
    // If no, close chosen cards and change player turn
    else
    {
        cout << "NO MATCH! Your turn ends." << endl;
        state[ cardChoice[0] ] = state[ cardChoice[1] ] = FACE_DOWN;
        whichPlayer = (whichPlayer + 1) % 2;
    }
    system("pause");
    
    return (numPairs != NumCard / 2);
}

void GameEnd()
{
    system("cls");
	printf("Player 1 ->  %d : %d  <- Player 2\n\n", playerScore[0], playerScore[1]);
	
    cout << setw(13) << setfill('*') << "" << endl;
    
    // Check who wins
    if (playerScore[0] > playerScore[1])
        cout << "Player 1 wins" << endl;
    else if (playerScore[0] < playerScore[1])
        cout << "Player 2 wins" << endl;
    else if (playerScore[0] == playerScore[1])
        cout << " It's a tie! " << endl;
    
    cout << setw(13) << setfill('*') << "" << endl;
}

bool GameAskRepeat()
{
    char yesNo;
    do {
		cin.ignore(numeric_limits<int>::max(), '\n');
        cout << "\nPlay Again? (y/n): ";
        cin >> yesNo;
        yesNo = tolower(yesNo);
    } while ((yesNo != 'y' && yesNo != 'n') || cin.peek() != '\n');
    
    return yesNo == 'y';
}

int main()
{
	bool GameRepeat = false;
    bool GameInPlay = true;
    
	GameFirstScreen();
	do
	{
		GameInit();
		
		while (GameInPlay = GameLoop(whichPlayer));
		
		GameEnd();
		
		GameRepeat = GameAskRepeat();
		
	} while (GameRepeat);
    
    cout << "\nThank you for playing! Goodbye!" << endl;
}
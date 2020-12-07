#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define ROW				10
#define COL				10
#define BATTLESHIP_LEN	3
#define SUBMARINE_LEN	2

#define HORIZONTAL	0
#define VERTICAL	1
#define NONE		2

#define NORTH	0
#define SOUTH	1
#define EAST	2
#define WEST	3

#define PLAYER_ONE	1
#define PLAYER_TWO	2

#define WATER		'.'
#define SHIP		'0'
#define HIT			'X'
#define MISS		'm'

#define YES         'Y'
#define NO          'N'

char playerOneBoard[ROW][COL];
char playerTwoBoard[ROW][COL];

int BotRow, BotCol, BotDirection, BotFlag, BotLine;
int BotHV = NONE; // whether the bot is horizontal or vertical
int step = 1;
/* BotRow and BotCol is used to store the original center hit block
   BotFlag is for testing fire direction
   BotLine is for counting how many ship blocks have been hit on the correct line*/


   /*=================================================================================================*/
   /*      Define Boolean variables, where FALSE = 0, and TRUE = 1		*/
typedef enum {
	FALSE, TRUE
}Boolean;

/*      Define a structure for game statistics		*/
struct stats {
	int		numHits;
	int		numMiss;
	int		totalShots;
	double	totalHitRatio;
};

struct stats Stats01 = { 0, 0, 0, 0 };
struct stats Stats02 = { 0, 0, 0, 0 };

/*=================================================================================================*/
//Functions & prototypes

void	startScreen(void);
void	initializeBattleBoard(char battleBoard[][COL]);
void	printBattleBoard(char battleBoard[][COL], Boolean flag);
void	updateBattleBoard(char battleBoard[][COL], int);
void	PlaceShip(char battleBoard[][COL], int);
int 	checkWinner(char battleBoard[][COL]);
void	Refresh(void);
int		randomNumGenerator(int);
void	randomPlaceShip(char battleBoard[][COL]);
void	BotTestAttackUpdate(char battleBoard[][COL], int, int);
void	RandomBotAttack(char battleBoard[][COL]);
void	PatternBotAttack(char battleBoard[][COL]);
void	SmartBotAttack(char battleBoard[][COL]);
void	printStats(void);

//==================================================================================================
// Main Function

void main() {
	int mode;

	startScreen();//welcome page

	// Game mode choice (PVP / PVE)
	do {
		printf("\nThere are three modes in this battle game.\n");
		printf("1. PVP - two human players play against each other.\n");
		printf("2. PVE - one human player and one 'AI' player\n");
		printf("3. SOLO - single player attempt to finish the game using the least steps.\n");
		printf("Please enter the num of the mode you want to play: ");
		scanf("%d", &mode);
		if (mode < 1 || mode > 3) {
			printf("\nInvalid input, please enter a integer between 1 to 3.\n");
		}
	} while (mode < 1 || mode > 3);

	switch (mode) {

	case 1:
		//Initialize the battle boards for the two players
		initializeBattleBoard(playerOneBoard);
		initializeBattleBoard(playerTwoBoard);

		//Place ships for the two players
		PlaceShip(playerOneBoard, 1);
		PlaceShip(playerTwoBoard, 2);

		//Battle process, each player takes one move each round
		for (int i = 0; i < 200; i++) {
			if (i % 2 == 0) {
				Refresh();
				printf("\nPlayer 1, IT IS YOUR TURN!!!\n");
				updateBattleBoard(playerTwoBoard, 1); // Player 1 attacks Player 2's board
				sleep(3); // delay for 3 seconds for the user to check results

				//Start to check the winning condition when the num of steps meets the requirement
				if (i >= 23) {
					if (checkWinner(playerTwoBoard) == 1) {
						printf("\n\nCongratulation Player ONE!! You have successfully destroys all your enemies.\n");
						printf("You are the new king of this sea!!!\n");
						printStats();	//Print the game stats
						return;
					}
				}
			}
			else if (i % 2 == 1) {
				Refresh();
				printf("\nPlayer 2, IT IS YOUR TURN!!!\n");
				updateBattleBoard(playerOneBoard, 2);	// Player 2 attacks Player 1's board
				sleep(3); // delay for 3 seconds for the user to check results

				//Start to check the winning condition when the num of steps meets the requirement
				if (i >= 23) {
					if (checkWinner(playerOneBoard) == 1) {
						printf("\n\nCongratulation Player TWO!! You have successfully destroys all your enemies.\n");
						printf("You are the new king of this sea!!!\n");
						printStats();	//Print the game stats
						return;
					}
				}
			}
		}
	case 2:
		/* Player One is human player
		   Player Two is the bot player */

		   //Initialize the battle boards for the two players
		initializeBattleBoard(playerOneBoard);
		initializeBattleBoard(playerTwoBoard); // BOT

		//Place ships for the two players
		PlaceShip(playerOneBoard, 1);
		randomPlaceShip(playerTwoBoard); // BOT

		//Battle process, each player takes one move each round
		for (int j = 0; j < 200; j++) {
			if (j % 2 == 0) {
				Refresh();
				printf("\nPlayer 1, IT IS YOUR TURN!!!\n");
				printf("\nGo Beat those bots!!!\n");
				updateBattleBoard(playerTwoBoard, 1); // Player 1 attacks Player 2's board
				sleep(3); // delay for 3 seconds for the user to check results

				//Start to check the winning condition when the num of steps meets the requirement
				if (j >= 23) {
					if (checkWinner(playerTwoBoard) == 1) {
						printf("\n\nCongratulation Player ONE!! You have successfully destroys all your enemies.\n");
						printf("You are the new king of this sea!!!\n");
						printStats();	//Print the game stats
						return;
					}
				}
			}
			else if (j % 2 == 1) {
				Refresh();
				printf("\nPlayer Bots, IT IS YOUR TURN!!!\n");

				SmartBotAttack(playerOneBoard);	// Player BOT attacks Player 1's board

				sleep(3); // delay for 3 seconds for the user to check results

				//Start to check the winning condition when the num of steps meets the requirement
				if (j >= 23) {
					if (checkWinner(playerOneBoard) == 1) {
						printf("\n\nCongratulation Player BOTS!! You have successfully destroys all your enemies.\n");
						printf("You are the new king of this sea!!!\n");
						printf("HUMAN, YOU LOSE!!!\n");
						printStats();	//Print the game stats
						return;
					}
				}
			}
		}

	case 3:
		//Initialize the battle board
		initializeBattleBoard(playerOneBoard);

		//Randomize the placement of the ships
		randomPlaceShip(playerOneBoard);

		//Solo battle process
		for (int k = 0; k < 100; k++) {
			Refresh();
			printf("\nPlayer, IT IS YOUR TURN!!!\n");
			printf("You have now made %d steps.\n", k);
			updateBattleBoard(playerOneBoard, 1); // Player 1 attacks Player 2's board
			sleep(3); // delay for 3 seconds for the user to check results

			//Start to check the winning condition when the num of steps meets the requirement
			if (k >= 11) {
				if (checkWinner(playerOneBoard) == 1) {
					printf("\n\nCongratulation Player ONE!! You have successfully destroys all your enemies.\n");
					printf("You are the new king of this sea!!!\n");
					printStats();	//Print the game stats
					return;
				}
			}
		}
	}


}

//=============================================================================
// Functions

/*
Function:	startScreen()
Purpose:	This function is used to print a startup page along with the basic Battleship rules and manuals
*/
void startScreen(void) {
	char response;

	printf("Welcome to the Battleship Game!!!!\n");
	printf("\n");
	printf("Rules of the Game\n");
	printf("1. Each player will need to place two Battleships and three Submarines.\n");
	printf("   The Battleship has 3 blocks; The Submarine has 2 blocks\n");
	printf("2. The ships can only be placed vertically and horizontally.\n");
	printf("3. One player's ships must not overlap with each other.\n");
	printf("4. Each player can only fire one cannon per round, and each cannon can only damage one block.\n");
	printf("5. After firing a cannon, there will be a response status displayed to the player\n");
	printf("		-Hit(X): the player has damaged part of the enemy's ship\n");
	printf("		-Miss(m): the player did not hit anything but water\n");
	printf("6. The player that first sinks all ships of another player wins the game and will be honored with the holy title.\n\n");


	printf("\nDo you wish to start to play the game? (Y|N): ");
	scanf("%c", &response);
	if (response == 'Y' || response == 'y') {
		sleep(1);
	}
	else if (response == 'N' || response == 'n') {
		exit(0);
	}
}

/*
Function:	initialzeBattleBoard()
Purpose:	This function is used to initialize/clear up the board with WATER symbol
*/
void initializeBattleBoard(char battleBoard[][COL]) {

	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			battleBoard[i][j] = WATER;
		}
	}
}

/*
Function:	printBattleBoard()
Purpose:	This function is used to print the current battle board map
			It has two modes, one to show ships, and the other one to hide the ship.
			TRUE is to show ships; FALSE is to hide ships
*/
void printBattleBoard(char battleBoard[][COL], Boolean flag) {

	printf("  0 1 2 3 4 5 6 7 8 9\n");

	for (int i = 0; i < ROW; i++) {
		printf("%d ", i);

		for (int j = 0; j < COL; j++) {
			if (flag == TRUE) {
				printf("%c ", battleBoard[i][j]);
			}
			else {
				if (battleBoard[i][j] == '0') {
					printf("%c ", WATER);
				}
				else {
					printf("%c ", battleBoard[i][j]);
				}
			}
		}
		printf("\n");
	}
}

/*
Function:	updateBattleBoard()
Purpose:	This function is used to update the status of the current battle board map
			It can be used to update the hit status.
*/
void updateBattleBoard(char battleBoard[][COL], int playerNum) {

	int row, col;
	printBattleBoard(battleBoard, FALSE);
	printf("\nPlease enter the coordinate of the block you want to hit.\n");
	do {

		do {
			printf("> Row: ");
			scanf("%d", &row);
			if (row < 0 || row > 9) {
				printf("\nTHAT IS AN INCORRECT ENTRY, PLEASE ENTER A VALUE BETWEEN 0 AND 9 !!!\n");
			}
		} while (row < 0 || row > 9);

		do {
			printf("> Column: ");
			scanf("%d", &col);
			if (col < 0 || col > 9) {
				printf("\nTHAT IS AN INCORRECT ENTRY, PLEASE ENTER A VALUE BETWEEN 0 AND 9 !!!\n");
			}
		} while (col < 0 || col > 9);

		if (battleBoard[row][col] == MISS || battleBoard[row][col] == HIT) {
			printf("\nThe selected area has already been destoryed\n");
			printf("Please choose another block.\n");
		}
	} while (battleBoard[row][col] == MISS || battleBoard[row][col] == HIT);

	switch (battleBoard[row][col]) {
		/*    Miss    */
	case WATER:
		battleBoard[row][col] = MISS;
		printf("\nMISS !!!\n");
		printBattleBoard(battleBoard, FALSE);

		// Update game statiscs
		if (playerNum == 1) {
			Stats01.numMiss++;
			Stats01.totalShots++;
		}
		else if (playerNum == 2) {
			Stats02.numMiss++;
			Stats02.totalShots++;
		}
		break;

		/*    Hit    */
	case SHIP:
		battleBoard[row][col] = HIT;
		printf("\nHIT !!!\n");
		printBattleBoard(battleBoard, FALSE);

		// Update game statiscs
		if (playerNum == 1) {
			Stats01.numHits++;
			Stats01.totalShots++;
		}
		else if (playerNum == 2) {
			Stats02.numHits++;
			Stats02.totalShots++;
		}
		break;
	}


}

/*
Function:	PlaceShip()
Purpose:	This function is used to place Battleships and Submarines on the
			Battle board map.
			There are validation inside the functions.
*/
void PlaceShip(char battleBoard[][COL], int playerNum) {

	int battleship_num = 0, submarine_num = 0;
	int row, col, direction, shipLen, total, num, nameNum;

	char* shipname[2] = { "Battleship", "Submarine" };// 0 is battleship, 1 is submarine

	char nameFlag;//Signal variable used to indicate which ship is being placing. 
	int flag = 0;//Whether there is error input 

	do {
		if (battleship_num < 2) {
			shipLen = 3;
			total = 2;
			num = battleship_num;
			nameFlag = 'b';
			nameNum = 0;  //Used to display the name of the ship the user is placing
		}
		else if (submarine_num < 3) {
			shipLen = 2;
			total = 3;
			num = submarine_num;
			nameFlag = 's';
			nameNum = 1; //Used to display the name of the ship the user is placing
		}

		Refresh(); // Refresh the page

		if (playerNum == 1) {
			printf("\nPlayer ONE, please place your ships\n");
		}
		else if (playerNum == 2) {
			printf("\nPlayer TWO, please place your ships\n");
		}

		if (flag == 1) {
			flag = 0; // Reset the Error flag
			printf("\n> ERROR: There are ships that overlap OR the ship is out of the battle board\n\n");
		}

		printBattleBoard(battleBoard, TRUE);
		printf("\n# Please provide the location to place the head of your %s\n", shipname[nameNum]);
		printf("# Please provide the direction you want the body of the %s to extend\n", shipname[nameNum]);
		printf("# %s each will has %d blocks\n", shipname[nameNum], shipLen);
		printf("# You still have %d %s to place\n", total - num, shipname[nameNum]);
		printf("\n> Please enter two integer numbers between 0-9 for row and column.\n");

		//Asking for inputs to place the Battleships
		do {
			printf("> Row: ");
			scanf("%d", &row);
			if (row < 0 || row > 9) {
				printf("\nTHAT IS AN INCORRECT ENTRY, PLEASE ENTER A VALUE BETWEEN 0 AND 9 !!!\n");
			}
		} while (row < 0 || row > 9);

		do {
			printf("> Column: ");
			scanf("%d", &col);
			if (col < 0 || col > 9) {
				printf("\nTHAT IS AN INCORRECT ENTRY, PLEASE ENTER A VALUE BETWEEN 0 AND 9 !!!\n");
			}
		} while (col < 0 || col > 9);

		printf("\n> Please enter the direction you want the body of the ship to extend\n");
		printf("> NORTH = 0; SOUTH = 1; EAST = 2; WEST = 3\n");

		do {
			printf("> Direction: ");
			scanf("%d", &direction);
			if (direction < 0 || direction > 3) {
				printf("\nTHAT IS AN INCORRECT ENTRY, PLEASE ENTER A CORRECT DIRECTION VALUE !!!\n");
				printf("> NORTH = 0; SOUTH = 1; EAST = 2; WEST = 3\n");
			}
		} while (direction < 0 || direction > 3);

		//Placing the ships

		if (direction == NORTH) {
			for (int i = 0; i < shipLen; i++) {
				if (battleBoard[row - i][col] == WATER && (row - i) >= 0)
					battleBoard[row - i][col] = SHIP;
				else {
					//Backtracking the ships
					if (i != 0) {
						while (i >= 0) {
							battleBoard[row - i + 1][col] = WATER;
							i--;
						}
					}
					flag = 1;
					break;
				}
			}

			if (flag == 1) {
				continue;
			}
		}
		else if (direction == SOUTH) {
			for (int i = 0; i < shipLen; i++) {
				if (battleBoard[row + i][col] == WATER && (row + i) <= 9)
					battleBoard[row + i][col] = SHIP;
				else {
					//Backtracking the ships
					if (i != 0) {
						while (i >= 0) {
							battleBoard[row + i - 1][col] = WATER;
							i--;
						}
					}
					flag = 1;
					break;
				}
			}

			if (flag == 1) {
				continue;
			}
		}
		else if (direction == EAST) {
			for (int j = 0; j < shipLen; j++) {
				if (battleBoard[row][col + j] == WATER && (col + j) <= 9)
					battleBoard[row][col + j] = SHIP;
				else {
					//Backtracking the ships
					if (j != 0) {
						while (j >= 0) {
							battleBoard[row][col + j - 1] = WATER;
							j--;
						}
					}
					flag = 1;
					break;
				}
			}

			if (flag == 1) {
				continue;
			}
		}
		else if (direction == WEST) {
			for (int j = 0; j < shipLen; j++) {
				if (battleBoard[row][col - j] == WATER && (col - j) >= 0)
					battleBoard[row][col - j] = SHIP;
				else {
					//Backtracking the ships
					if (j != 0) {
						while (j >= 0) {
							battleBoard[row][col - j + 1] = WATER;
							j--;
						}
					}
					flag = 1;
					break;
				}
			}

			if (flag == 1) {
				continue;
			}
		}

		if (nameFlag == 'b') {
			battleship_num++;
		}
		else if (nameFlag == 's') {
			submarine_num++;
		}

	} while ((battleship_num + submarine_num) < 5);

	printf("\n\nAll the battleships and submarines are in battle positions!!!! Ready to fight!!\n\n");
}

/*
Function:	checkWinner()
Purpose:	This function is used to check if the player
			has acheived the winning condition.
*/
int checkWinner(char battleBoard[][COL]) {

	int hitNum = 0;

	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			if (battleBoard[i][j] == HIT) {
				hitNum++;
			}
			if (hitNum == 12) {
				return 1;
			}
		}
	}

	return 0;
}

/*
Function:	Refresh()
Purpose:	This function is to use Linux command "clear" to refresh the page
			in order to make the refresh of the game board more smoothly.
*/
void Refresh(void) {

	int status = system("clear -x");//clear the page OR refresh the page
	status = status / 256;
	//Check the error code
	if (status != 0) {
		printf("\nRefresh Error!!!\n");
	}
}

/*
Function:	randomNumGenerator()
Purpose:	This function is to generate a random number in a given range.
*/
int randomNumGenerator(int max) {

	int randomNum;
	srand(time(NULL));
	randomNum = rand() % max;
	return randomNum;
}

/*
Function:	randomPlaceShip()
Purpose:	This function is to pseudorandomly place the ships into the game board
			based on the direction and the ship length.
*/
void randomPlaceShip(char battleBoard[][COL]) {
	int row, col, direction;
	int shipLen, total, num, flag;
	int battleship_num = 0, submarine_num = 0;
	char nameflag;

	char* shipname[2] = { "Battleship", "Submarine" };// 0 is battleship, 1 is submarine

	do {
		if (battleship_num < 2) {
			shipLen = 3;
			total = 2;
			num = battleship_num;
			nameflag = 'b';
		}
		else if (submarine_num < 3) {
			shipLen = 2;
			total = 3;
			num = submarine_num;
			nameflag = 's';
		}

		//Random Generate the positions
		row = randomNumGenerator(9);
		col = randomNumGenerator(9);
		direction = randomNumGenerator(3);

		//Placing the ships

		if (direction == NORTH) {
			for (int i = 0; i < shipLen; i++) {
				if (battleBoard[row - i][col] == WATER && (row - i) >= 0)
					battleBoard[row - i][col] = SHIP;
				else {
					//Backtracking the ships
					if (i != 0) {
						while (i >= 0) {
							battleBoard[row - i + 1][col] = WATER;
							i--;
						}
					}
					flag = 1;
					break;
				}
			}

			if (flag == 1) {
				flag = 0; // Reset the flag
				continue;
			}
		}
		else if (direction == SOUTH) {
			for (int i = 0; i < shipLen; i++) {
				if (battleBoard[row + i][col] == WATER && (row + i) <= 9)
					battleBoard[row + i][col] = SHIP;
				else {
					//Backtracking the ships
					if (i != 0) {
						while (i >= 0) {
							battleBoard[row + i - 1][col] = WATER;
							i--;
						}
					}
					flag = 1;
					break;
				}
			}

			if (flag == 1) {
				flag = 0; // Reset the flag
				continue;
			}
		}
		else if (direction == EAST) {
			for (int j = 0; j < shipLen; j++) {
				if (battleBoard[row][col + j] == WATER && (col + j) <= 9)
					battleBoard[row][col + j] = SHIP;
				else {
					//Backtracking the ships
					if (j != 0) {
						while (j >= 0) {
							battleBoard[row][col + j - 1] = WATER;
							j--;
						}
					}
					flag = 1;
					break;
				}
			}

			if (flag == 1) {
				flag = 0; // Reset the flag
				continue;
			}
		}
		else if (direction == WEST) {
			for (int j = 0; j < shipLen; j++) {
				if (battleBoard[row][col - j] == WATER && (col - j) >= 0)
					battleBoard[row][col - j] = SHIP;
				else {
					//Backtracking the ships
					if (j != 0) {
						while (j >= 0) {
							battleBoard[row][col - j + 1] = WATER;
							j--;
						}
					}
					flag = 1;
					break;
				}
			}

			if (flag == 1) {
				flag = 0; // Reset the flag
				continue;
			}
		}

		if (nameflag == 'b') {
			battleship_num++; // accumulate the num of battleships
		}
		else if (nameflag == 's') {
			submarine_num++; // accumulate the num of submarines
		}

	} while ((battleship_num + submarine_num) < 5);
}

/*
Function:	BotTestAttackUpdate()
Purpose:	This function is used for the BOT player to update
			the board based on their attack.
*/
void BotTestAttackUpdate(char battleBoard[][COL], int row, int col) {
	switch (battleBoard[row][col]) {
		/*    Miss    */
	case WATER:
		battleBoard[row][col] = MISS;
		printf("\nMISS !!!\n");
		printBattleBoard(battleBoard, FALSE);

		// Update game statistics
		Stats02.numMiss++;
		Stats02.totalShots++;

		break;

		/*    Hit    */
	case SHIP:
		battleBoard[row][col] = HIT;
		printf("\nHIT !!!\n");
		printBattleBoard(battleBoard, FALSE);

		// Update game statistics
		Stats02.numHits++;
		Stats02.totalShots++;

		break;
	}
}

/*
Function:	RandomBotAttack()
Purpose:	This function is to randomly attack a block,
			If the previous shot hit the ship, the coordinate will
			be recorded.
*/
void RandomBotAttack(char battleBoard[][COL]) {
	int row, col;
	
	// Generate a valid coordinate to fire
	do {
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		row = randomNumGenerator(9);
		col = randomNumGenerator(9);

	} while (battleBoard[row][col] == MISS || battleBoard[row][col] == HIT);

	switch (battleBoard[row][col]) {
		/*    Miss    */
	case WATER:
		battleBoard[row][col] = MISS;
		printf("\nMISS !!!\n");
		printBattleBoard(battleBoard, FALSE);

		// Update game statistics
		Stats02.numMiss++;
		Stats02.totalShots++;

		break;

		/*    Hit    */
	case SHIP:
		battleBoard[row][col] = HIT;
		printf("\nHIT !!!\n");
		printBattleBoard(battleBoard, FALSE);

		BotFlag = 1; // Fire the next one based on this hit block
		BotRow = row;
		BotCol = col;
		BotDirection = 0; // Starts with NORTH
		BotLine = 1; //Now, one ship block of that ship got hit

		// Update game statistics
		Stats02.numHits++;
		Stats02.totalShots++;

		break;
	}
}

/*
Function:	PatternBotAttack()
Purpose:	This function is to attack a block based on the
			previous shot that hit the ship, the following shots
			will be fired based on that.
*/
void PatternBotAttack(char battleBoard[][COL]) {

	// Fire based on the original center hit block
	int row = BotRow;
	int col = BotCol;

	// This is for deciding the direction to fire
	if (BotLine < 3) {
		switch (BotDirection) {
		case NORTH:
			if (row - 1 * step >= 0 && battleBoard[row - 1 * step][col] != HIT && battleBoard[row - 1 * step][col] != MISS) {
				BotTestAttackUpdate(battleBoard, row - 1 * step, col);

				if (battleBoard[row - 1 * step][col] == HIT) {
					BotLine++; // Increase the num of ship blocks of that ship got hit
					step++;
					BotHV = VERTICAL; // the ship should be place in vertical 
				}
				else {
					BotDirection++; // switch to the next direction
					step = 1; //reset
				}
				break;
			}
			else {
				BotDirection++; // switch to the next direction
				step = 1; //reset
				PatternBotAttack(battleBoard);
				return;
			}
			
		case SOUTH:
			if (row + 1 * step <= 9 && battleBoard[row + 1 * step][col] != HIT && battleBoard[row + 1 * step][col] != MISS) {
				BotTestAttackUpdate(battleBoard, row + 1 * step, col);

				if (battleBoard[row + 1 * step][col] == HIT) {
					BotLine++; // Increase the num of ship blocks of that ship got hit
					step++;
					BotHV = VERTICAL; // the ship should be place in vertical 
				}
				else {
					BotDirection++; // switch to the next direction
					step = 1; //reset 
				}
				break;
			}
			else {
				BotDirection++; // switch to the next direction
				step = 1; //reset
				PatternBotAttack(battleBoard);
				return;
			}
		case EAST:
			if (col + 1 * step <= 9 && battleBoard[row][col + 1 * step] != HIT && battleBoard[row][col + 1 * step] != MISS) {
				BotTestAttackUpdate(battleBoard, row, col + 1 * step);

				if (battleBoard[row][col + 1 * step] == HIT) {
					BotLine++; // Increase the num of ship blocks of that ship got hit
					step++;

					// If there is another ship next to the current ship
					if (BotHV == VERTICAL) {
						BotRow = row;
						BotCol = col + 1;
						BotDirection = 0; // Starts with NORTH
						BotLine = 1; //Now, one ship block of that ship got hit
						BotHV = NONE;
						step = 1;
						return;
					}
					else {
						BotHV = HORIZONTAL; // the ship should be place in vertical 
					}
				}
				else {
					BotDirection++; // switch to the next direction
					step = 1; //reset
				}
				break;
			}
			else {
				BotDirection++; // switch to the next direction
				step = 1; //reset
				PatternBotAttack(battleBoard);
				return;
			}
		case WEST:
			if (col - 1 * step >= 0 && battleBoard[row][col - 1 * step] != HIT && battleBoard[row][col - 1 * step] != MISS) {
				BotTestAttackUpdate(battleBoard, row, col - 1 * step);

				if (battleBoard[row][col - 1 * step] == HIT) {
					BotLine++; // Increase the num of ship blocks of that ship got hit
					step++;

					// If there is another ship next to the current ship
					if (BotHV == VERTICAL) {
						BotRow = row;
						BotCol = col - 1;
						BotDirection = 0; // Starts with NORTH
						BotLine = 1; //Now, one ship block of that ship got hit
						BotHV = NONE;
						step = 1;
						return;
					}
					else {
						BotHV = HORIZONTAL; // the ship should be place in vertical 
					}
				}
				else {
					BotLine = 404;
					step = 1; // reset
				}
				break;
			}
			else {
				BotLine = 404;
				step = 1; // reset
			}
		}
	}

	if (BotLine == 404) {
		BotLine = 1;
		BotFlag = 0; // Reset
		BotHV = NONE;

		// when the length of the ship is 2
		// If nowhere to hit, Go back to random select a block to attack
		SmartBotAttack(battleBoard);

	}
	else if (BotLine == 3) {
		step = 1;
		BotLine = 1;
		BotFlag = 0; // Reset
		BotHV = NONE;
	}

}




/*
Function:	SmartBotAttack()
Purpose:	This function is to randomly attack a block,
			If the previous shot hit the ship, the following shots
			will be fired based on that.
*/
void	SmartBotAttack(char battleBoard[][COL]) {
	int row, col;

	if (BotFlag == 0) {

		RandomBotAttack(battleBoard); // Randomly attack a block with update on the board.

	}
	else if(BotFlag = 1){
		
		PatternBotAttack(battleBoard); // Attack a block based on the coordinate of the previous hit block
	}

}

/*
Function:	printStats()
Purpose:	This function is to print out the two players' game stats
*/
void printStats(void) {
	Stats01.totalHitRatio = (double)Stats01.numHits / Stats01.totalShots;
	Stats02.totalHitRatio = (double)Stats02.numHits / Stats02.totalShots;
	printf("\n===============================================================\n");
	printf("Player ONE game stats\n");
	printf("Number of Hits:				%d\n", Stats01.numHits);
	printf("Number of Miss:				%d\n", Stats01.numMiss);
	printf("Number of Total Shots:				%d\n", Stats01.totalShots);
	printf("Hit/Total Ratio:			%.2f\n", Stats01.totalHitRatio);
	printf("\n===============================================================\n");
	printf("Player TWO game stats\n");
	printf("Number of Hits:				%d\n", Stats02.numHits);
	printf("Number of Miss:				%d\n", Stats02.numMiss);
	printf("Number of Total Shots:				%d\n", Stats02.totalShots);
	printf("Hit/Total Ratio:			%.2f\n", Stats02.totalHitRatio);
}
/*
Student: Matan Hayon

This program is the game of TAKI! :)

it asks the user for ammount of players, and then runs a game based on the taki rules.

the porpose of the game is to finish all your cards.
each player starts with 4 cards, and can throw a card to the deck if it has the same color or number as the upper card.
the card that was thrown will be the next upper card.
a player can use "regular" cards - numbers from 1-9, and special cards.
the special cards has a special ability:
PLUS - the player can throw another card
TAKI - the player can throw as many cards as he can if they are legit.
CHANGE DIRECTION - after thrown the direction of the game will be changed
COLOR - the player can choose the next color of the upper card
STOP - once thrown, the turn will skip the next player.
if a player doesn't have a legit card, he has to draw a card from the deck.

after the game ends, statistics of the game will be printed.
*/


#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define startingCardsNum 4
#define NameSize 20
#define NumOfCardOptions 14
#define NoColor 'N'
#define YELLOW 'Y'
#define RED 'R'
#define BLUE 'B'
#define GREEN 'G'
#define PLUS 10
#define STOP 11
#define DIRECTION 12
#define TAKI 13
#define ColorCard 14
#define NumOfSpecialCards 5
#define DrawCard 0

typedef struct statisCard //will be used in an array, the method of counting array.
{
	int index; //represents the card number, will be used to merge sort the array later.
	int count;
}statisCard;

typedef struct CARD
{
	int num;
	char color;
}CARD;

typedef struct HAND //the cards each player has.
{
	CARD* cards; //array of cards
	int logiSize;
	int physSize;
}HAND;

typedef struct PLAYER
{
	char name[NameSize];
	HAND hand;
}PLAYER;

typedef struct GAME //all the game data
{
	PLAYER* players; //array of all the players. 
	int numOfPlayers;
	statisCard StatisticsArr[NumOfCardOptions]; //array of all the statistics
	CARD TopCard; //will represent the current upper card.
	bool GameOver; //True if game is over, false if not
	bool clockwise; //represents the direction of the play
	bool isTaki; //represents if a player is playing the special card taki
	int turn; // will be used to determine who plays. we calculate the turn%(num of players), and we can get the index of the current player who's turn is.
	int indexWinner; //after the game finished, the inded of the winner player will be stored here.
}GAME;

void getNumOfPlayers(GAME* game);
//gets the num of players from the user.

void restartPlayersData(GAME* game);
//initate data for each player

void getPlayerCards(GAME* pGame, int playerIndex);
//initate the hand of player cards

CARD getRandomCard(GAME* pGame);
//the function randomize a card and return it.

void playturn(GAME* pGame, int playerIndex, CARD TopCard);
//play the turn of the playerindex recived.

void printCard(CARD card);
//gets a card and prints it

void printPlayerDeck(PLAYER* player);
//gets a player, and prints all the card he has.

GAME initiateGame();
//initate all the game data.

CARD getRandomTopCard(GAME* pGame);
//gets a randop top card, should'nt be a special card.

void initateStatistics(GAME* pGame);
//initate the counting array of the statistics.

void updateStatistic(GAME* pGame, int cardIndex);
//update the statistics of the game, in the method of counting array.

void GamePlay(GAME* pGame);
//playing the game until game is over and a player finished his cards.
//when the game ends, prints the statistics of the game.

CARD* HandRealloc(GAME* pGame, int playerIndex, int oldSize, int newSize);
// realloc array of cards.

void playRegularCard(GAME* pGame, int playerIndex, int cardIndex);
//takes the last card in the hand of the player, copys it to the wanted index to be thrown, and resize -1 the array.
//basically - deletes the thrown card with last one in array, and resize -1.

void playSpecialCard(GAME* pGame, int playerIndex, int cardIndex);
//if a special card was chosen - the function decides with special card should be played.

void updateTopCard(GAME* pGame, CARD newCard);
//updates the top card.

int getPlayerChoice(GAME* pGame, int playerIndex);
//this function gets the valid choice of a player - based on the rules of the game.
//if the card he chose could'nt be played, he will be asked to enter a different choice.

void playCardPlus(GAME* pGame, int playerIndex, int cardIndex);
//Playing the card PLUS by the game rules.
//the player gets another turn. if the plus was he's last card - he gets automaticliy another card instead of him.

void playColorCard(GAME* pGame, int playerIndex, int cardIndex);
//playing the color card, asking the current player for the color wanted and updates the color of the top card.

void throwCard(GAME* pGame, int playerIndex, int cardIndex);
//takes the last card in the hand of the player, copys it to the wanted index to be thrown, and resize -1 the array.
//basically - deletes the thrown card with last one in array, and resize -1.

void playChangeDirectionCard(GAME* pGame, int playerIndex, int cardIndex);
//changing the direction of the play.

int getPlayingIndex(GAME* pGame);
//determine the index of the current player's turn.
//we use the integer turn%num of players, the result is the index in the player array. 

void playCardStop(GAME* pGame, int playerIndex, int cardIndex);
//play the card stop - skip the next player.

void playCardTaki(GAME* pGame, int playerIndex, int cardIndex);
//play the special card TAKI - keep asking the player for cards, 
//until he finished his cards or he asks to draw a card and finish his turn.

void printStatistic(GAME* pGame);
//print the summary of statistics when the game ends

void mergeSortStats(statisCard statArr[], int size); //Merge sort of stats array
//Merge sort of stats array

void mergeStat(statisCard a1[], int n1, statisCard a2[], int n2, statisCard res[]);
//merge between to arrays to one.

void copyArr(statisCard dest[], statisCard src[], int size);
//copy arr of cards to destination arr

void freeGameData(GAME* pGame);

void main()
{
	srand(time(NULL));
	GAME game = initiateGame();
	GamePlay(&game);
	freeGameData(&game);
}

void freeGameData(GAME* pGame)
{
	int i;
	for (i = 0; i < pGame->numOfPlayers; i++)
	{
		free(pGame->players[i].hand.cards);
	}
	free(pGame->players);
	return;
}

void copyArr(statisCard dest[], statisCard src[], int size) //copy arr of cards to destination arr
{
	int i;
	for (i = 0; i < size; i++)
		dest[i] = src[i];
}

void mergeStat(statisCard a1[], int n1, statisCard a2[], int n2, statisCard res[]) //merge between to arrays to one.
{
	int ind1, ind2, resInd;
	ind1 = ind2 = resInd = 0;
	while ((ind1 < n1) && (ind2 < n2))
	{
		if (a1[ind1].count > a2[ind2].count)
		{
			res[resInd] = a1[ind1];
			ind1++;
		}
		else
		{
			res[resInd] = a2[ind2];
			ind2++;
		}
		resInd++;
	}
	while (ind1 < n1)
	{
		res[resInd] = a1[ind1];
		ind1++;
		resInd++;
	}
	while (ind2 < n2)
	{
		res[resInd] = a2[ind2];
		ind2++;
		resInd++;
	}
}

void mergeSortStats(statisCard statArr[], int size) //Merge sort of stats array
{
	statisCard* tempArr = NULL;
	if (size <= 1)
		return;

	mergeSortStats(statArr, size / 2);
	mergeSortStats(statArr + size / 2, size - size / 2);

	tempArr = (statisCard*)malloc(size * sizeof(statisCard));
	if (tempArr)
	{
		mergeStat(statArr, size / 2, statArr + size / 2, size - size / 2, tempArr);
		copyArr(statArr, tempArr, size);
		free(tempArr);
	}
	else
	{
		printf("memory allocation failure!!!\n");
		exit(1);
	}

}

void printStatistic(GAME* pGame)
//print the summary of statistics when the game ends. merge sort all the counting array and the prints it.
{
	int i;
	mergeSortStats(pGame->StatisticsArr, NumOfCardOptions);
	printf("\n********* GAME Statistics *********\nCARD # | Frequency\n__________________\n");
	for (i = 0; i < NumOfCardOptions; i++)
	{
		switch (pGame->StatisticsArr[i].index)
		{
		case TAKI:
		{
			printf(" TAKI  ");
			break;
		}
		case DIRECTION:
		{
			printf("  <->  ");
			break;
		}
		case ColorCard:
		{
			printf(" COLOR ");
			break;
		}
		case STOP:
		{
			printf(" STOP  ");
			break;
		}
		case PLUS:
		{
			printf("   +   ");
			break;
		}
		default:
		{
			printf("   %d   ", pGame->StatisticsArr[i].index);
		}
		}
		printf("|    %d\n", pGame->StatisticsArr[i].count);
	}
	return;
}

int getPlayerChoice(GAME* pGame, int playerIndex)
{//this function gets the valid choice of a player - based on the rules of the game.
	//if the card he chose could'nt be played, he will be asked to enter a different choice.
	int choice;
	CARD chosenCard;
	if (pGame->isTaki)
		printf("\nPlease enter 0 if you want to finish your turn");
	else
		printf("\nPlease enter 0 if you want to take a card from the deck");
	printf("\nor %d-%d if you want to put one of your cards in the middle:\n", 1, pGame->players[playerIndex].hand.logiSize);
	scanf("%d", &choice);
	if (choice == 0)
		return choice;
	chosenCard = pGame->players[playerIndex].hand.cards[choice - 1];
	while (!(choice <= pGame->players[playerIndex].hand.logiSize && choice >= 0 && (chosenCard.num == pGame->TopCard.num || chosenCard.color == pGame->TopCard.color || chosenCard.num == ColorCard)))
	{//check if the card is valid by the rules of the game or the size available.
		if (choice > pGame->players[playerIndex].hand.logiSize || choice < 1)
			printf("\nInvalid choice! Try again.");
		else
			printf("\nInvalid card! Try again.");
		if (pGame->isTaki)
			printf("\nPlease enter 0 if you want to finish your turn");
		else
			printf("\nPlease enter 0 if you want to take a card from the deck");
		printf("\nor %d-%d if you want to put one of your cards in the middle:\n", 1, pGame->players[playerIndex].hand.logiSize);
		scanf("%d", &choice);
		if (choice == 0)
			return choice;
		chosenCard = pGame->players[playerIndex].hand.cards[choice - 1]; //update the chosen card to the choice of the player

	}
	return choice;
}

void updateTopCard(GAME* pGame, CARD newCard)
//update the top card of the game to the new card
{
	pGame->TopCard = newCard;
}

void playCardTaki(GAME* pGame, int playerIndex, int cardIndex)
{//play the special card TAKI - keep asking the player for cards, until he finished his cards or he asks to draw a card and finish his turn.
	pGame->isTaki = true;
	throwCard(pGame, playerIndex, cardIndex);
	while (pGame->isTaki)
	{
		if (pGame->players[playerIndex].hand.logiSize == 0)
			return;
		playturn(pGame, playerIndex, pGame->TopCard);
	}
	return;
}

void playColorCard(GAME* pGame, int playerIndex, int cardIndex)
{//playing the color card, asking the current player for the color wanted and updates the color of the top card.
	int colorChoice;
	printf("\nPlease enter your color choice:\n1 - Yellow\n2 - Red\n3 - Blue\n4 - Green\n");
	scanf("%d", &colorChoice);
	switch (colorChoice)
	{
	case 1:
		colorChoice = YELLOW;
		break;
	case 2:
		colorChoice = RED;
		break;
	case 3:
		colorChoice = BLUE;
		break;
	case 4:
		colorChoice = GREEN;
		break;
	}
	throwCard(pGame, playerIndex, cardIndex);
	pGame->TopCard.color = colorChoice;
	pGame->TopCard.num = ColorCard;
	return;
}

void playCardStop(GAME* pGame, int playerIndex, int cardIndex)
{ //play the card stop - skip the next player.
	if (pGame->clockwise)
	{
		pGame->turn += 1;//skip the next player
		throwCard(pGame, playerIndex, cardIndex);
	}
	else
	{
		pGame->turn -= 1;//skip the next player
		throwCard(pGame, playerIndex, cardIndex);
	}
	return;
}

void playChangeDirectionCard(GAME* pGame, int playerIndex, int cardIndex)
{ //changing the direction of the play.
	pGame->clockwise = !pGame->clockwise;
	throwCard(pGame, playerIndex, cardIndex);
	return;
}

void throwCard(GAME* pGame, int playerIndex, int cardIndex)
{ //takes the last card in the hand of the player, copys it to the wanted index to be thrown, and resize -1 the array.
	//basically - deletes the thrown card with last one in array, and resize -1.
	CARD lastCardInHand = pGame->players[playerIndex].hand.cards[pGame->players[playerIndex].hand.logiSize - 1];
	updateTopCard(pGame, pGame->players[playerIndex].hand.cards[cardIndex]);
	pGame->players[playerIndex].hand.cards[cardIndex] = lastCardInHand;
	pGame->players[playerIndex].hand.logiSize--;
	return;
}

void playCardPlus(GAME* pGame, int playerIndex, int cardIndex)
{//Playing the card PLUS by the game rules.
//the player gets another turn. if the plus was he's last card - he gets automaticliy another card instead of him.
	if (pGame->players[playerIndex].hand.logiSize == 1)
	{
		pGame->players[playerIndex].hand.cards[0] = getRandomCard(pGame);
		return;
	}
	throwCard(pGame, playerIndex, cardIndex);
	playturn(pGame, playerIndex, pGame->TopCard);
	return;
}

void playSpecialCard(GAME* pGame, int playerIndex, int cardIndex)
{//if a special card was chosen - the function decides with special card should be played.
	CARD chosenCard = pGame->players[playerIndex].hand.cards[cardIndex];
	switch (chosenCard.num)
	{
	case PLUS:
	{
		playCardPlus(pGame, playerIndex, cardIndex);
		break;
	}
	case STOP:
	{
		playCardStop(pGame, playerIndex, cardIndex);
		break;
	}
	case DIRECTION:
	{
		playChangeDirectionCard(pGame, playerIndex, cardIndex);
		break;
	}
	case TAKI:
	{
		playCardTaki(pGame, playerIndex, cardIndex);
		break;
	}
	case ColorCard:
	{
		playColorCard(pGame, playerIndex, cardIndex);
		break;
	}
	}
}

void playRegularCard(GAME* pGame, int playerIndex, int cardIndex)
{//takes the last card in the hand of the player, copys it to the wanted index to be thrown, and resize -1 the array.
	//basically - deletes the thrown card with last one in array, and resize -1.
	CARD lastCardInHand = pGame->players[playerIndex].hand.cards[pGame->players[playerIndex].hand.logiSize - 1];
	updateTopCard(pGame, pGame->players[playerIndex].hand.cards[cardIndex]);
	pGame->players[playerIndex].hand.cards[cardIndex] = lastCardInHand;
	pGame->players[playerIndex].hand.logiSize--;
	return;
}

CARD* HandRealloc(GAME* pGame, int playerIndex, int oldSize, int newSize)
{ //realloc array of cards.
	int i;
	CARD* newHand = (CARD*)malloc(newSize * sizeof(CARD));
	if (newHand == NULL)
	{
		printf("Memory allocation failed!!!\n");
		exit(1);
	}
	else
	{
		for (i = 0; i < oldSize && i < newSize; i++)
			newHand[i] = pGame->players[playerIndex].hand.cards[i];
		free(pGame->players[playerIndex].hand.cards);
		return newHand;
	}
}

int getPlayingIndex(GAME* pGame)
{  //determine the index of the current player's turn.
	//we use the integer turn%num of players, the result is the index in the player array. 
	if (pGame->turn < 0)
		return ((pGame->turn * -1) % pGame->numOfPlayers);
	else
		return pGame->turn % pGame->numOfPlayers;
}

void GamePlay(GAME* pGame)
{//playing the game until game is over and a player finished his cards.
 //when the game ends, prints the statistics of the game.
	while (!pGame->GameOver)
	{
		int playerIndex = getPlayingIndex(pGame);
		playturn(pGame, playerIndex, pGame->TopCard);
		if (pGame->clockwise)
			pGame->turn++;
		else
			pGame->turn--;
		if (pGame->players[playerIndex].hand.logiSize == 0)
		{
			pGame->GameOver = true;
			pGame->indexWinner = playerIndex;
		}
	}
	printf("\n\nThe winnder is :%s\n\n", pGame->players[pGame->indexWinner].name);
	printStatistic(pGame);
}

void updateStatistic(GAME* pGame, int cardIndex)
{ //update the statistics of the game, in the method of counting array.
	pGame->StatisticsArr[cardIndex].count++;
}

void initateStatistics(GAME* pGame)
{ //initate the counting array of the statistics.
	int i;
	for (i = 0; i < NumOfCardOptions; i++)
	{
		pGame->StatisticsArr[i].index = i + 1;
		pGame->StatisticsArr[i].count = 0;
	}
}

CARD getRandomTopCard(GAME* pGame)
{ //gets a randop top card, should'nt be a special card.
	CARD rndCard;
	int rndcolor;
	rndCard.num = 1 + rand() % (NumOfCardOptions - NumOfSpecialCards); // get a number between 1-9
	rndcolor = rand() % 4;
	switch (rndcolor)
	{
	case 0:
		rndCard.color = YELLOW;
		break;
	case 1:
		rndCard.color = RED;
		break;
	case 2:
		rndCard.color = BLUE;
		break;
	case 3:
		rndCard.color = GREEN;
		break;
	}
	updateStatistic(pGame, rndCard.num - 1);
	return rndCard;
}

GAME initiateGame()
{ //initate all the game data.
	GAME game;
	game.clockwise = true;
	game.GameOver = false;
	initateStatistics(&game);
	game.TopCard = getRandomTopCard(&game);
	getNumOfPlayers(&game);
	restartPlayersData(&game);
	game.turn = 0;
	return game;
}

void printPlayerDeck(PLAYER* player)
{ //gets a player, and prints all the card he has.
	int i;
	printf("\n%s's turn:\n", player->name);
	for (i = 0; i < player->hand.logiSize; i++)
	{
		printf("\nCard #%d:", i + 1);
		printCard(player->hand.cards[i]);
	}
}

void printCard(CARD card)
{//gets a card and prints it
	printf("\n*********");
	printf("\n*       *");
	switch (card.num)
	{
	case ColorCard:
		printf("\n* COLOR *");
		break;
	case PLUS:
		printf("\n*   +   *");
		break;
	case STOP:
		printf("\n*  STOP *");
		break;
	case DIRECTION:
		printf("\n*  <->  *");
		break;
	case TAKI:
		printf("\n*  TAKI *");
		break;
	default:
		printf("\n*   %d   *", card.num);
	}
	if (card.color == NoColor)
		printf("\n*       *");
	else
		printf("\n*   %c   *", card.color);
	printf("\n*       *");
	printf("\n*********\n");

}



void playturn(GAME* pGame, int playerIndex, CARD TopCard)
{ //play the turn of the playerindex recived.
	int choice;
	printf("\nUpper Card:");
	printCard(pGame->TopCard);
	printPlayerDeck(&pGame->players[playerIndex]);
	choice = getPlayerChoice(pGame, playerIndex);
	if (choice == DrawCard) //Player wants to get a card from the deck
	{
		if (pGame->isTaki) //check if he's playing the special card TAKI.
			pGame->isTaki = false; //if he draws a card, the TAKI ends.
		if (pGame->players[playerIndex].hand.logiSize < pGame->players[playerIndex].hand.physSize) //Available room in array
		{
			pGame->players[playerIndex].hand.cards[pGame->players[playerIndex].hand.logiSize] = getRandomCard(pGame);
			pGame->players[playerIndex].hand.logiSize++;
		}
		else //No room in array, need to realloc.
		{
			pGame->players[playerIndex].hand.cards = HandRealloc(pGame, playerIndex, pGame->players[playerIndex].hand.logiSize, pGame->players[playerIndex].hand.logiSize * 2);
			pGame->players[playerIndex].hand.cards[pGame->players[playerIndex].hand.logiSize] = getRandomCard(pGame);
			pGame->players[playerIndex].hand.logiSize++;
			pGame->players[playerIndex].hand.physSize *= 2;
		}
		return;
	}
	else
	{
		int cardValue = pGame->players[playerIndex].hand.cards[choice - 1].num; //the type of card the player chose to play 
		if (cardValue < PLUS) //is regular card
			playRegularCard(pGame, playerIndex, choice - 1);
		else //is special card
			playSpecialCard(pGame, playerIndex, choice - 1);
	}

}

CARD getRandomCard(GAME* pGame)
{ //the function randomize a card and return it.
	CARD rndCard;
	int rndcolor;
	rndCard.num = 1 + rand() % (NumOfCardOptions); // get a number between 1-14
	if (rndCard.num == ColorCard)
		rndCard.color = NoColor;
	else
	{
		rndcolor = rand() % 4;
		switch (rndcolor)
		{
		case 0:
			rndCard.color = YELLOW;
			break;
		case 1:
			rndCard.color = RED;
			break;
		case 2:
			rndCard.color = BLUE;
			break;
		case 3:
			rndCard.color = GREEN;
			break;
		}
	}
	updateStatistic(pGame, rndCard.num - 1);
	return rndCard;
}

void getNumOfPlayers(GAME* pGame)
{//initate array of players in the game.
	printf("Please enter the number of players :\n");
	scanf("%d", &pGame->numOfPlayers);
	pGame->players = (PLAYER*)malloc((pGame->numOfPlayers) * sizeof(PLAYER));
	if (pGame->players == NULL)
	{
		printf("Memory allocation failed!!!\n");
		exit(1);
	}
}
void restartPlayersData(GAME* pGame)
{//initate data for each player
	int i;
	for (i = 0; i < pGame->numOfPlayers; i++)
	{
		printf("\nPlease enter the first name of player #%d:", i + 1);
		scanf("%s", pGame->players[i].name);
		getPlayerCards(pGame, i); //initate player cards
	}
}


void getPlayerCards(GAME* pGame, int playerIndex)
{//initate the hand of player cards
	int i;
	pGame->players[playerIndex].hand.cards = (CARD*)malloc(startingCardsNum * sizeof(CARD));
	if (pGame->players[playerIndex].hand.cards == NULL)
	{
		printf("Memory allocation failed!!!");
		exit(1);
	}
	pGame->players[playerIndex].hand.logiSize = startingCardsNum;
	pGame->players[playerIndex].hand.physSize = startingCardsNum;

	for (i = 0; i < startingCardsNum; i++)
	{
		pGame->players[playerIndex].hand.cards[i] = getRandomCard(pGame);
	}
}
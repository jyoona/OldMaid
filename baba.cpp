#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <random>
#include <time.h>
using namespace std;


#define ROWWIDTH 80


class Card {
	protected:
		int num;
		string suit;
	public:
		void setNumber(int i) {
			if (i<0 || i>13) {
				cout << "Invalid number." << endl;
				abort();
			}
			else 
				num = i;
		}

		int getNum() {
			return num;
		}

		void setSuit(int i) {
			string suits[] = {"Spades", "Hearts", "Clubs", "Diamonds", "Joker"};
			if (i>4) {
				cout << "Invalid suit." << endl;
				abort();
			}
			else
				suit = suits[i];
		}

		string getSuit() {
			return suit;
		}
};



/* declare functions for sort and random number generation */
bool sortFunc(Card i, Card j) { return (i.getNum() < j.getNum()); }
int RNGs(int n) { return rand() % n;}



class Player {
	protected:
		string name;
		vector<Card> cards;
		bool you; /* true if this player is you */
	public:
		void setName(string n) {
			name = n;
		}

		string getName() {
			return name;
		}

		vector<Card> *getCards() {
			return &cards;
		}

		void setYou(bool yesorno) {
			you = yesorno;
		}


		bool getYou() {
			return you;
		}

		/* add card to deck then sort cards */
		void addCard(Card c) {
			cards.push_back(c);
			sort(cards.begin(), cards.end(), sortFunc);
		}

		/* display deck */
		void displayCards() {
			int numCards = cards.size();
			int whitespaces = (int)((ROWWIDTH-(numCards*7))/2);
			int rem = (int)((ROWWIDTH-(numCards*7))%2);

			/* Row 1 - card top */
			cout << "   |" + string(whitespaces, ' ');
			for (int i=0; i<numCards; i++)
				cout << "   __  ";
			cout << string(whitespaces, ' ') + string(rem, ' ') + "|" << endl;

			/* Row 2 - card sides & numbers */
			cout << "   |" + string(whitespaces, ' ');
			for (int i=0; i<numCards; i++) {
				cout << " | ";

				int num = cards[i].getNum();
				switch(num) {
					case 0:
						cout << "Jo";
						break;
					case 1:
						cout << "A";
						break;
					case 11:
						cout << "J";
						break;
					case 12:
						cout << "Q";
						break;
					case 13:
						cout << "K";
						break;
					default:
						cout << num;
						break;
				}

				if (num!=0 && num!=10)
					cout << " ";
				cout << "| ";
			}
			cout << string(whitespaces, ' ') + string(rem, ' ') + "|" << endl;

			/* Row 3 - card bottom */
			cout << "   |" + string(whitespaces, ' ');
			for (int i=0; i<numCards; i++)
				cout << "   ‾‾  ";
			cout << string(whitespaces, ' ') + string(rem, ' ') + "|" << endl;
		}

		/* Trash duplicated cards */
		void trash() {
			if (you)
				cout << "Trashed cards." << endl;

			vector<Card>::iterator it = cards.begin();
			while (it < cards.end()-1) {
				if (it->getNum()==(it+1)->getNum()) {
					it = cards.erase(it);
					it = cards.erase(it);
				}
				else
					++it;
			}
		}
};





class Game {
	protected:
		vector<Player> players;
		vector<Card> cards;
		bool end;
		vector<int> order; /* order of play */
	public:
		vector<Player> getPlayers() {
			return players;
		}

		bool hasEnded() {
			return end;
		}

		vector<int> getOrder() {
			return order;
		}

		void removePlayer(int i) {
			order.erase(remove(order.begin(), order.end(), i), order.end());

			/* After removing the player, if only you are left, end the game */
			if (order.size()==1 && order[0]==0)
				end = true;
		}

		/* initialize game */
		void initGame() {
			end = false;

			/* Explain rules */
			cout << endl;
			cout << "RULES for OLD MAID:" << endl;
			cout << "Take turns going around in a circle picking a card from the next player's hand. If you get a pair, you may trash the cards. The last person left with the Joker loses the game." << endl;
			cout << endl;

			/* initialize players - doon't forget to add yourself */
			int num = -1;
			while (num<1 || num>7) {
				cout << "Enter number of players to go against (1~7): " << endl;
				cin >> num;
			}

			Player *you = new Player;
			you->setName("You");
			you->setYou(true);
			players.push_back(*you);

			for (int i=1; i<=num; i++) {
				Player *newPlayer = new Player;
				newPlayer->setName("P" + to_string(i));
				newPlayer->setYou(false);
				players.push_back(*newPlayer);
			}

			/* initialize order */
			static const int arr[] = {0, 4, 2, 6, 1, 7, 3, 5};
			vector<int> temp (arr, arr + sizeof(arr) / sizeof(arr[0]) );
			order = temp;

			vector<int>::iterator it = order.begin();
			while (it < order.end()) {
				if (*it > players.size()-1)
					it = order.erase(it);
				else
					++it;
			}

			/* initialize cards */
			for (int j=0; j<4; j++) {
				for (int i=1; i<=13; i++) {
					Card *newCard = new Card;
					newCard->setNumber(i);
					newCard->setSuit(j);
					cards.push_back(*newCard);
				}
			}
			Card *newCard = new Card;
			newCard->setNumber(0);
			newCard->setSuit(4);
			cards.push_back(*newCard);

			/* Hand out cards */
			srand(unsigned(time(NULL)));
			random_shuffle(cards.begin(), cards.end(), RNGs);
			int j=0;
			for (int i=0; i<cards.size(); i++) {
				players[j].addCard(cards[i]);
				if (j==players.size()-1)
					j = 0;
				else
					j++;
			}

			/* everyone trashes cards once */
			for (int i=0; i<players.size(); i++)
				players[i].trash();
			displayBoard();

			cout << "Press any key to START game." << endl;
			cin.get();
		}

		/* Player's turn */
		/* i=current player's index. j=next player's index */
		void turn(int i, int j) {
			bool you = players[i].getYou();

			if (!you)
				cout << players[i].getName() << "'s turn: " << endl;
			else
				cout << "Your turn" << endl;

			Card newCard = pick(i, j);
			players[i].addCard(newCard);

			if (you)
				displayBoard();

			players[i].trash();

			/* if the player's cards are empty, remove him */
			/* if this is you, end the game */
			if (players[i].getCards()->empty()) {
				if (you)
					end = true;
				removePlayer(i);
			}

			displayBoard();
		}

		/* Pick a new card */
		/* i=current player's index. j=next player's index */
		Card pick(int i, int j) {
			cout << players[i].getName() << " picked a card from " << players[j].getName() << endl;

			vector<Card> *playerCards = players[j].getCards();
			int length = playerCards->size();

			/* pick random card and add to deck */
			int randNum;
			Card newCard;
			if (length!=1) {
				randNum = (rand()% length );
				newCard = (*playerCards)[randNum];
				playerCards->erase(playerCards->begin()+randNum);
			}
			/* if the player had his last card taken, remove him from game */
			else {
				randNum = 0;
				newCard = (*playerCards)[randNum];
				playerCards->erase(playerCards->begin()+randNum);
				removePlayer(j);

				/* if you had your last card taken, end game */
				if (players[j].getYou())
					end = true;
			}

			return newCard;
		}

		/* display playing board */
		void displayBoard() {
			vector<bool> inPlay;
			inPlay.push_back(true);
			inPlay.push_back(find(order.begin(), order.end(), 1) != order.end());
			inPlay.push_back(find(order.begin(), order.end(), 2) != order.end());
			inPlay.push_back(find(order.begin(), order.end(), 3) != order.end());
			inPlay.push_back(find(order.begin(), order.end(), 4) != order.end());
			inPlay.push_back(find(order.begin(), order.end(), 5) != order.end());
			inPlay.push_back(find(order.begin(), order.end(), 6) != order.end());
			inPlay.push_back(find(order.begin(), order.end(), 7) != order.end());


			/* Row 1 */
			int whitespaces = (int)((ROWWIDTH-2*3)/4);

			cout << string(4, ' ') + string(whitespaces, ' ');
			if (inPlay[6])
				cout << players[6].getName();
			else
				cout << string(2, ' ');
			cout << string(whitespaces, ' ');
			if (inPlay[1])
				cout << players[1].getName();
			else
				cout << string(2, ' ');
			cout << string(whitespaces, ' ');
			if (inPlay[7])
				cout << players[7].getName();
			else
				cout << string(2, ' ');
			cout << endl;

			/* Row 2 */
			cout << string(3, ' ') + string(ROWWIDTH+2, '-') << endl;

			/* Row 3 */
			int rem = (int)((ROWWIDTH-2*3)%4);
			cout << "   |" + string(whitespaces, ' ');
			if (inPlay[6]) {
				if (players[6].getCards()->size()/10 == 0)
					cout << players[6].getCards()->size() << " ";
				else
					cout << players[6].getCards()->size();
			}
			else 
				cout << string(2, ' ');
			cout << string(whitespaces, ' ');
			if (rem>0)
				cout << string(1, ' ');
			if (inPlay[1]) {
				if (players[1].getCards()->size()/10 == 0)
					cout << players[1].getCards()->size() << " ";
				else
					cout << players[1].getCards()->size();
			}
			else 
				cout << string(2, ' ');
			cout << string(whitespaces, ' ');
			if (rem>1)
				cout << string(1, ' ');
			if (inPlay[7]) {
				if (players[7].getCards()->size()/10 == 0)
					cout << players[7].getCards()->size() << " ";
				else
					cout << players[7].getCards()->size();
			}
			else 
				cout << string(2, ' ');
			cout << string(whitespaces, ' ') + "|" << endl;

			/* Row 4 */
			cout << "   |" + string(ROWWIDTH, ' ') + "|" << endl;

			/* Row 5 */
			if (inPlay[2]) {
				if (players[2].getCards()->size()/10 == 0)
					cout << players[2].getName() << " |  " << players[2].getCards()->size() << " ";
				else
					cout << players[2].getName() << " |  " << players[2].getCards()->size();
			}
			else
				cout << "   |    ";

			cout << string((ROWWIDTH-8), ' ');
			if (inPlay[3]) {
				if (players[3].getCards()->size()/10 == 0)
					cout << " " << players[3].getCards()->size() << "  | " << players[3].getName();
				else
					cout << players[3].getCards()->size() << "  | " << players[3].getName();
			}
			else
				cout << "    |";
			cout << endl;

			/* Rows 6, 7, 8 */
			cout << "   |" + string(ROWWIDTH, ' ') + "|" << endl;
			cout << "   |" + string(ROWWIDTH, ' ') + "|" << endl;
			cout << "   |" + string(ROWWIDTH, ' ') + "|" << endl;

			/* Row 9 */
			if (inPlay[4]) {
				if (players[4].getCards()->size()/10 == 0)
					cout << players[4].getName() << " |  " << players[4].getCards()->size() << " ";
				else
					cout << players[4].getName() << " |  " << players[4].getCards()->size();
			}
			else
				cout << "   |    ";

			cout << string((ROWWIDTH-8), ' ');
			if (inPlay[5]) {
				if (players[5].getCards()->size()/10 == 0)
					cout << " " << players[5].getCards()->size() << "  | " << players[5].getName();
				else
					cout << players[5].getCards()->size() << "  | " << players[5].getName();
			}
			else
				cout << "    |";
			cout << endl;

			/* Row 10 */
			cout << "   |" + string(ROWWIDTH, ' ') + "|" << endl;

			/* Row 11 */
			players[0].displayCards();

			/* Row 12*/
			cout << string(3, ' ') + string(ROWWIDTH+2, '-') << endl;

			/* Row 13 */
			whitespaces = (int)((ROWWIDTH-3)/2);
			rem = (int)((ROWWIDTH-3)%2);
			cout << string(4, ' ') + string(whitespaces, ' ') + string(rem, ' ') << players[0].getName() << string(whitespaces, ' ') << endl;

			cin.get();
		}

		/* End Game */
		void endGame() {
			if (players[0].getCards()->size()==0)
				cout << "YOU WIN!!!!!" << endl;
			else
				cout << "You Lose...You are the King of the Weakest." << endl;
			cout << "Game Ended." << endl;
			cout << endl;
		}
};

int main() {

	/* initialize game */
	Game *game = new Game;
	game->initGame();
	vector<int> order;

	order = game->getOrder();

	int current = 0;
	int next = 1;

	/* take turns while the game is not over */
	srand(time(NULL)); 
	while (!game->hasEnded()) {
		game->turn(order[current], order[next]);

		order = game->getOrder();

		if (next>=order.size()-1) {
			current = next;
			next = 0;
		}
		else {
			current = next;
			next++;
		}
	}

	game->endGame();

	return 0;
};





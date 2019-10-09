#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include "player.h"
#include "deck.h"
#include "rand.h"

using namespace std;

void random_shuffle(Deck & deck, Player* player){
    cout << "# Shuffling the deck\n";
    int cut;
    for(int i=0;i<7;i++) {
        cut = get_cut();
        cout << "cut at " << cut << endl;
        deck.shuffle(cut);
        player->shuffled();
    }
}

int main(int argc, char* argv[]) {
    Player* player;
    string check=argv[4];
    if(check=="simple") player=get_Simple();
    else player=get_Counting();
    int bankroll=atoi(argv[1]);
    int min_bet=atoi(argv[2]);
    int hand_num=atoi(argv[3]);
    Deck deck;
    int cut;
    if(argc==6){
        string inputfile;
        istringstream input;
        input.str(argv[5]);
        input>>inputfile;
        fstream iStream;
        iStream.open(inputfile);
        cout << "# Shuffling the deck\n";
        while(iStream>>cut){
            cout << "cut at " << cut << endl;
            deck.shuffle(cut);
            player->shuffled();
        }
        iStream.close();
    }
    else{
        random_shuffle(deck,player);
    }
    Hand player_hand,dealer_hand;
    int wager;
    int thishand;
    for (thishand=1;thishand<=hand_num;thishand++){
        if(bankroll<min_bet)break;
        cout << "# Hand " << thishand << " bankroll " << bankroll << endl;
        if(deck.cardsLeft()<20) random_shuffle(deck,player);
        player_hand.discardAll();
        dealer_hand.discardAll();
        wager=player->bet((unsigned int)bankroll,(unsigned int) min_bet);
        cout << "# Player bets " << wager << endl;
        Card first = deck.deal();
        cout << "Player dealt " << SpotNames[first.spot]
             << " of " << SuitNames[first.suit] << endl;
        player_hand.addCard(first);
        player->expose(first);
        Card second=deck.deal();
        cout << "Dealer dealt " << SpotNames[second.spot]
             << " of " << SuitNames[second.suit] << endl;
        dealer_hand.addCard(second);
        player->expose(second);
        Card third = deck.deal();
        cout << "Player dealt " << SpotNames[third.spot]
             << " of " << SuitNames[third.suit] << endl;
        player_hand.addCard(third);
        player->expose(third);
        Card fourth = deck.deal();
        dealer_hand.addCard(fourth);
        if(player_hand.handValue().count==21){
            bankroll+=(wager*3/2);
            cout << "# Player dealt natural 21\n";
        }
        else{
            while(player->draw(second,player_hand)){
                Card dealt=deck.deal();
                cout<<"Player dealt "<<SpotNames[dealt.spot]<<" of "<<SuitNames[dealt.suit]<<endl;
                player_hand.addCard(dealt);
                player->expose(dealt);
            }
            cout<<"Player's total is "<<player_hand.handValue().count<<endl;
            if(player_hand.handValue().count>21) {
                bankroll-=wager;
                cout<<"# Player busts\n";
            }
            else {
                player->expose(fourth);
                cout<<"Dealer's hole card is "<<SpotNames[fourth.spot]<<" of "<<SuitNames[fourth.suit]<<endl;
                while(dealer_hand.handValue().count<17){
                    Card card=deck.deal();
                    cout << "Dealer dealt " << SpotNames[card.spot]
                         << " of " << SuitNames[card.suit] << endl;
                    player->expose(card);
                    dealer_hand.addCard(card);
                }
                cout<<"Dealer's total is " << dealer_hand.handValue().count << endl;
                if(dealer_hand.handValue().count>21){
                    bankroll+=wager;
                    cout << "# Dealer busts\n";
                }
                else{
                    if(dealer_hand.handValue().count>player_hand.handValue().count){
                        bankroll-=wager;
                        cout << "# Dealer wins\n";
                    }
                    else if(dealer_hand.handValue().count<player_hand.handValue().count){
                        bankroll+=wager;
                        cout << "# Player wins\n";
                    }
                    else{
                        cout << "# Push\n";
                    }
                }
            }
        }
    }
    cout << "# Player has " << bankroll << " after " << thishand-1 << " hands\n";
    delete player;
    return 0;
}

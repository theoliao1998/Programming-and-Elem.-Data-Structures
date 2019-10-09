#include "player.h"
#include "rand.h"

class Simple_Player : public Player{
public:
    Simple_Player();
    int bet(unsigned int bankroll, unsigned int minimum);
    bool draw(Card dealer, const Hand &player);
    void expose(Card c){}
    void shuffled(){}
};

Simple_Player::Simple_Player(){
}

int Simple_Player::bet(unsigned int bankroll, unsigned int minimum) {
    return minimum;
}

bool Simple_Player::draw(Card dealer, const Hand &player) {
    if(!player.handValue().soft){
        if(player.handValue().count<=11) return 1;
        else if(player.handValue().count==12){
            if (dealer.spot>=FOUR && dealer.spot<=SIX) return 0;
            else return 1;
        }
        else if(player.handValue().count>=13 && player.handValue().count<=16) {
            if (dealer.spot>=TWO && dealer.spot<=SIX) return 0;
            else return 1;
        }
        else return 0;
    }
    else{
        if(player.handValue().count<=17) return 1;
        else if(player.handValue().count==18) {
            if (dealer.spot == TWO || dealer.spot == SEVEN || dealer.spot == EIGHT) return 0;
            else return 1;
        }
        else{
            return 0;
        }
    }

}

extern Player *get_Simple(){
    return new Simple_Player;
}

class Counting_Player : public Player{
    int count=0;
public:
    Counting_Player();
    int bet(unsigned int bankroll, unsigned int minimum);
    bool draw(Card dealer, const Hand &player);
    void expose(Card c);
    void shuffled();
};

Counting_Player::Counting_Player() : count(0) {
}

int Counting_Player::bet(unsigned int bankroll, unsigned int minimum) {
    if(count>=2 && bankroll>=2*minimum)return 2*minimum;
    else return minimum;
}

bool Counting_Player::draw(Card dealer, const Hand &player) {
    Simple_Player simple_player;
    return simple_player.draw(dealer,player);
}

void Counting_Player::expose(Card c) {
    switch(c.spot){
        case TEN  :
        case JACK :
        case QUEEN :
        case KING :
        case ACE :
            count--;
            break;
        case TWO :
        case THREE :
        case FOUR :
        case FIVE :
        case SIX :
            count++;
            break;
        default :
            break;
    }
}

void Counting_Player::shuffled() {
    count=0;
}

extern Player *get_Counting(){
    return new Counting_Player;
}
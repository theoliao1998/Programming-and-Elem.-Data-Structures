#include "deck.h"

Deck::Deck() {
    reset();
}

void Deck::reset() {
    int i,j;
    int card_num;
    for (i=0;i<SUIT_SIZE;i++){
        for(j=0;j<SPOT_SIZE;j++){
            card_num=i*SPOT_SIZE+j;
            deck[card_num].suit=Suit (i);
            deck[card_num].spot=Spot (j);
        }
    }
    next=0;
}

void Deck::shuffle(int n) {
    Card left[n], right[DeckSize-n];
    int i=0;
    for(i=0;i<DeckSize;i++){
        if(i<n) left[i]=deck[i];
        else right[i-n]=deck[i];
    }
    i=0;
    while(i<n && i<DeckSize-n){
        deck[2*i]=right[i];
        deck[2*i+1]=left[i];
        i++;
    }
    Card* rest;
    if(i==n) rest = &right[i];
    else rest = &left[i];
    int j=2*i;
    while(j<DeckSize){
        deck[j]=rest[j-2*i];
        j++;
    }
    next=0;
}

Card Deck::deal() {
    if(next<DeckSize)return deck[next++];
    else {
        DeckEmpty Error;
        throw Error;
    }
}

int Deck::cardsLeft() {
    return (DeckSize-next);
}
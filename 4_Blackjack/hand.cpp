#include "hand.h"

Hand::Hand() {
    curValue={0,0};
}

void Hand::discardAll() {
    curValue={0, false};
}

void Hand::addCard(Card c) {
    if(c.spot<=TEN){
        if(curValue.soft==1 && curValue.count+c.spot+2>21) {
            curValue.soft=0;
            curValue.count=curValue.count-10+c.spot+2;
        }
        else curValue.count+=(c.spot+2);
    }
    else if (c.spot>TEN && c.spot<ACE){
        if(curValue.soft==1 && curValue.count+10>21){
            curValue.soft=0;
        }
        else curValue.count+=10;
    }
    else if(curValue.count>10) curValue.count++;
    else {
        curValue.count+=11;
        curValue.soft=1;
    }
}

HandValue Hand::handValue() const {
    return curValue;
}


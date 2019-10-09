#include <iostream>
#include <cstdlib>
#include <string>
#include "dlist.h"

using namespace std;




const string customer_status[4]={"platinum","gold","silver","regular"};

struct customer{
    int timestamp;
    string name;
    string status;
    int duration;
    bool answered;
};

void answer(int& answered_num, Dlist<customer> queue[]);

int main(int argc, char *argv[]) {
    Dlist<customer> queue[4];
    int num;
    int answered_num=0;
    int tick=0;
    cout<<"Starting tick #0"<<endl;
    cin>>num;
    for(int i=0;i<num;i++){
        customer* Customer=new customer;
        cin>>Customer->timestamp;
        cin>>Customer->name;
        cin>>Customer->status;
        cin>>Customer->duration;
        Customer->answered=0;
        if(Customer->timestamp>tick){
            for(tick;tick<Customer->timestamp;tick++ ){
                answer(answered_num,queue);
                cout<<"Starting tick #"<<tick+1<<endl;
            }
        }
        cout<<"Call from "<<Customer->name<<" a "<<Customer->status<<" member\n";
        for(int j=0;j<4;j++){
            if(Customer->status==customer_status[j]){
                queue[j].insertBack(Customer);
            }
        }
    }
    while(answered_num<num){
        answer(answered_num,queue);
        cout<<"Starting tick #"<<++tick<<endl;
    }
}

void answer(int& answered_num, Dlist<customer> queue[] ){
    for(int j=0;j<4;j++){
        if(!queue[j].isEmpty()){
            customer* C=queue[j].removeFront();
            if(C->answered==0) {
                cout << "Answering call from " << C->name << endl;
                C->answered=1;
                answered_num++;
            }
            if((--C->duration)>0) queue[j].insertFront(C);
            else delete C;
            break;
        }
    }
}


#include <iostream>
#include <cstdlib>
#include <string>
#include "dlist.h"

using namespace std;

bool checkint(string input);

int main(int argc, char *argv[]) {
    Dlist<int> stack,copy;
    string input;
    cin>>input;
    while(input!="q"){
        if(input=="+" || input =="-" || input=="*" || input=="/"||input=="r"){
            copy=stack;
            try{
                int* A= stack.removeFront();
                int a=*A;
                delete A;
                int* B= stack.removeFront();
                int b=*B;
                delete B;
                if(input=="r"){
                    stack.insertFront(new int(a));
                    stack.insertFront(new int(b));
                }
                else{
                    int* C=new int;
                    if(input=="+") *C=a+b;
                    else if(input=="-") *C=b-a;
                    else if(input=="*") *C=a*b;
                    else if (a==0) cout<<"Divide by zero\n";
                    else *C=b/a;
                    stack.insertFront(C);
                }
            }
            catch(emptyList e){
                cout<<"Not enough operands\n";
                stack=copy;
            }
        }
        else if(input=="n"||input=="d"||input=="p"){
            copy=stack;
            try{
                int * A= stack.removeFront();
                int a=*A;
                delete A;
                if(input=="n"){
                    stack.insertFront(new int(-a));
                }
                else if(input=="d"){
                    stack.insertFront(new int(a));
                    stack.insertFront(new int(a));
                }
                else{
                    cout<<a<<endl;
                    stack.insertFront(new int(a));
                }
            }
            catch(emptyList e){
                cout<<"Not enough operands\n";
                stack=copy;
            }
        }
        else if(input=="c") {
            while (!stack.isEmpty()){
                delete stack.removeFront();
            }
        }
        else if(input=="a"){
            while (!stack.isEmpty()){
                int* top=stack.removeFront();
                cout<<*top<<" ";
                delete top;
            }
            cout<<endl;
        }
        else if(checkint(input)){
            stack.insertFront(new int(atoi(input.c_str())));
        }
        else {
            cout << "Bad input\n";
        }
        cin>>input;
    }
}

bool checkint(string input){
    if((input[0]>'9' || input[0]<'0') && input[0]!='-') return false;
    if(input.length()>1){
        for(int i=1;i<input.length();i++){
            if (input[i]>'9' || input[i]<'0') return false;
        }
    }
    return true;
}
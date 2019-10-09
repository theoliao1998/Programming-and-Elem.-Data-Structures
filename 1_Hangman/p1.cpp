#include <iostream>
#include "p1.h"
using namespace std;

void current_state(const int & score, const string & displayed_word, const string & proposed_char);
//EFFECTS: display the current state involving the score, the word to be guessed with guessed characters, and the proposed characters
int rand_choose();
//EFFECTS: ask for a random integer as an input from keyboard, returns an integer n where 0<=n<=48
bool check_whether_proposed(const string & proposed_chars, char input);
//MODIFIES: score;
//EFFECT: if the character input is in the string proposed_chars, return true, otherwise return false
bool check_whether_in_chosen_word(char input, string &  displayed_word, int rand_int);
//MODIFIES: displayed_word;
//EFFECT: if the character input is in the chosen word words[rand_int], replace the character '_' in the same place of the string displayed_word
//and return true, otherwise return false
void insert_proposed_chars(char input, string & proposed_chars);
//MODIFIES: proposed_word;
//EFFECT: insert the character input into the string proposed_chars according to the alphabetic order

int main(){
    int rand_int=rand_choose();
    //initialize a random integer between 0 and 48 which corresponds to a word in the string array words in p1.h
    string chosen_word=words[rand_int];
    string displayed_word(words[rand_int].size(),'_');
    //initialize a string containing the guessed letters and '_' in the order according to the chosen word
    int score=0;
    //initialize the score to be 0
    string proposed_chars="";
    //initialize a string containing the proposed letters in alphabetic order
    char input;
    bool exist;
    do {
        current_state(score,displayed_word,proposed_chars);
        //display the current state
        cout<<"Please enter a letter (a-z): ";
        cin>>input;
        if(input<=122 && input>=97){
            //make sure the input character is a lower-case letter from a to z, otherwise ignore it
            exist=check_whether_proposed(proposed_chars, input);
            //check whether the letter is already proposed and not in the word
            if(!exist) {
                //if not proposed
                exist=check_whether_in_chosen_word(input, displayed_word, rand_int);
                //check whether in the chosen word, change the corresponding '_' to the input if true
                if (!exist) {
                    //if also not in the chosen word
                    insert_proposed_chars(input,proposed_chars);
                    //insert the input into the string proposed_chars according to the alphabetic order
                    score++;
                    //add 1 to the score
                }
            }
        }
    }while((score<10) && !(displayed_word==words[rand_int]));
    //The program loops and asks for another letter. The loop only stops when score reaches 10 or all the letters of the chosen word are guessed
    cout<<"The word was: "<<chosen_word<<endl;
    //Display the chosen word
    if(score>=10)cout<<"You lost!"<<endl;
    //If score reaches 10, output "You lost!"
    if(displayed_word==chosen_word)cout<<"You won!"<<endl;
    //If  all the letters of the chosen word are guessed, output "You won!"
}

void current_state(const int & score, const string & displayed_word, const string & proposed_chars){
    cout<<"Score: "<<score<<endl;
    //display the current score
    for(unsigned int i=0;i<displayed_word.size();i++){
        cout<<displayed_word[i]<<' ';
    }
    //display all the '_' and guessed letters one by one separated with ' ' in the order of the chosen word
    cout<<endl;
    cout<<"Already proposed characters: \n";
    for(unsigned int i=0;i<proposed_chars.size();i++){
        cout<<proposed_chars[i]<<' ';
    }
    //display the proposed letters one by one separated with ' ' in alphabetic order
    cout<<endl;
}

int rand_choose(){
    int seed;
    cout<<"Please input seed: ";
    cin>>seed;
    //gives a random integer as a seed, it should be an int type integer
    p1_srand(seed);
    int rand_int=p1_rand()%49;
    //initialize an integer between 0 and 48 according to the seed
    return rand_int;
};

bool check_whether_proposed(const string & proposed_chars, char input){
    bool exist=0;
    for(unsigned int i=0;i<proposed_chars.size();i++){
        if(proposed_chars[i]==input) {
            exist=1;
            //make exist to be true if the input is found in the proposed_chars
            break;
            //once input is found in proposed_chars, stop the loop
        }
    }
    return exist;
}

bool check_whether_in_chosen_word(char input, string &  displayed_word, int rand_int){
    bool exist=0;
    for (unsigned int i = 0; i < words[rand_int].size(); i++) {
        if (words[rand_int][i] == input) {
            exist = 1;
            //make exist to be true if the input is found in the chosen word
            displayed_word[i] = input;
            //change the corresponding '_' to the input
        }
    }
    return exist;
}

void insert_proposed_chars(char input, string & proposed_chars){
    unsigned int i=0;
    for(i=0; i < proposed_chars.size(); i++){
        if (input < proposed_chars[i]) break;
    }
    //find the location i where the input should be inserted according to alphabetic order
    proposed_chars.insert(i,1,input);
    //insert the input in the location i
}

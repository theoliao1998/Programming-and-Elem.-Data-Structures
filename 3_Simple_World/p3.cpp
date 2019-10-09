#include <iostream>
#include "simulation.h"

int main(int argc, char *argv[]) {
    try{
        simulation sim(argc,argv); //start the simulation
    }
    catch (string ErrMessage){    // catch the error message during the simulation as exception
        cout<<ErrMessage<<endl;    // cout the error message
    }
}
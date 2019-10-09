#ifndef P3_CLASS_H
#define P3_CLASS_H
#include "world_type.h"

class error{
    // a class representing all kinds of error
    // we can throw it's error message as exception
    // or we can directly throw itself
public:
    virtual string errmessage(){}
};

class MissArguErr : public error{
public:
    string errmessage();
    // EFFECTS: return the required error message about some missed arguments
};

class NegRoundErr : public error{
public:
    string errmessage();
    // EFFECTS: return the required error message about a negative round number
};

class FileOpenErr : public error{
private:
    string Filename;
public:
    FileOpenErr(string filename="");
    // EFFECTS: initialize the error for failing opening a file with a given filename
    string errmessage();
    // EFFECTS: return the required error message about failing opening the file
};

class UnknownInstructionErr : public error{
private:
    string name;
public:
    UnknownInstructionErr(string InstructionName="");
    // EFFECTS: initialize the error for failing identifying the instruction with a given instruction name
    string errmessage();
    // EFFECTS: return the required error message about some unkown instruction
};

class UnknownSpeciesErr : public error{
private:
    string name;
public:
    UnknownSpeciesErr(string Name="");
    // EFFECTS: initialize the error for failing identifying the species with a given species name
    string errmessage();
    // EFFECTS: return the required error message about some unknown species
};

class UnknownDirectionErr : public error{
private:
    string name;
public:
    UnknownDirectionErr(string Name="");
    // EFFECTS: initialize the error for failing identifying the direction with a given direction name
    string errmessage();
    // EFFECTS: return the required error message about some unknown direction
};

class MaxSpeciesErr : public error{
public:
    string errmessage();
    // EFFECTS: return the required error message about too many species
};

class MaxProgramErr : public error{
private:
    string name;
public:
    MaxProgramErr(string Name);
    // EFFECTS: initialize the error for the size of the program of reaches its max
    string errmessage();
    // EFFECTS: return the required error message about too many programs
};

class MaxHeightErr : public error{
public:
    string errmessage();
    // EFFECTS: return the required error message about the over-limit given height
};

class MaxWidthErr : public error{
public:
    string errmessage();
    // EFFECTS: return the required error message about the over-limit given width
};

class MaxCreaturesErr : public error{
public:
    string errmessage();
    // EFFECTS: return the required error message about too many creatures
};

class IllegalTerrainErr : public error{
private:
    string name;
    point_t pos;
public:
    IllegalTerrainErr(string Name, int j, int k);
    // EFFECTS: initialize the error for failing identifying the terrain at pos with a given terrain name and its positioin pos
    string errmessage();
    // EFFECTS: return the required error message about illegal terrain
};

class OutsideBoundaryErr : public error{
private:
    string detail;
    int height;
    int width;
public:
    OutsideBoundaryErr(string d="",int h=0,int w=0);
    // EFFECTS: initialize the error for a creature outside the boundary
    string errmessage();
    // EFFECTS: return the required error message about a creature outside the boundary
};

class InvalidAbilityErr : public error{
private:
    string detail;
    string name;
public:
    InvalidAbilityErr(string d="",string n="");
    // EFFECTS: initialize the error for failing identifying the ability with a given ability name
    string errmessage();
    // EFFECTS: return the required error message about the invalid ability
};

class OverlapErr: public error{
private:
    string detail1;
    string detail2;
public:
    OverlapErr(string d1,string d2);
    // EFFECTS: initialize the error for 2 creatures overlapping with given details of the creatures
    string errmessage();
    // EFFECTS: return the required error message about overlapping creatures
};

class CannotFlyErr: public error{
private:
    string detail;
public:
    CannotFlyErr(string d);
    // EFFECTS: initialize the error for a creature that can't fly on a lake with given details of the creature
    string errmessage();
    // EFFECTS: return the required error message about a creature that can't fly on a lake
};

class simulation{
private:
    int present_round, round_num;
    bool Occupy[MAXHEIGHT][MAXWIDTH]={0}; //used to check whether a point is occupied by a creature
    bool checkverbose;
    world_t World;
    void createspecies(ifstream& iFilespecies);
    // MODIFIES: this->world
    // EFFECTS: Initialize the species of the world "world"
    // given the species summary file "speciesFile"
    void createworld(ifstream& iFileworld);
    // MODIFIES: this->World
    // EFFECTS: Initialize "World" given the world description file
    // "worldFile". This initializes all the components other
    // then the species of "World".
    void simulate();
    // REQUIRES: creature is inside the grid.
    // MODIFIES: this->World.creatures[MAXCREATURES], this->World->grid, cout.
    // EFFECTS: Simulate the turns of the worls and update the creatures,
    // the infected creatures, and the grid if necessary.
    // The creature programID is always updated. The function
    // also prints to the stdout the procedure. If verbose is
    // true, it prints more information.
    string Detail(point_t CreatureLocation);
    // REQUIRES: the point_t CreatureLocation of the grid is occupied by some creature
    // EFFECTS: Return the details of the creature in the form (<SPECIES> <DIR> <R> <C>)
    void DisplayLayout(string layout[MAXHEIGHT][MAXWIDTH]);
    // MODIFIES: cout.
    // EFFECTS: print a grid representation of the creature world.
    point_t FrontPos(point_t pos, direction_t dir);
    // EFFECTS: Returns a point that results from moving one square
    // in the direction given from the point pos, the postion dosen't
    // change if it's toward a boundary
    point_t FrontPos(point_t pos);
    // REQUIRES: the point_t pos of the grid is occupied by some creature
    // EFFECTS: Returns a point that results from moving one square
    // in the direction of the creature from the point pos, the postion dosen't
    // change if it's toward a boundary
    void CreatureMove(point_t pos1,point_t pos2);
    // REQUIRES: the point_t pos1 of the grid is occupied by some creature
    // and the point_t pos2 is not occupied
    // MODIFIES: this->World.grid.squares
    // EFFECTS: Move the creature at pos1 to pos2 and change pos1 of the grid to be unoccupied
    void CreatureHop(point_t pos);
    // REQUIRES: the point_t pos of the grid is occupied by some creature
    // MODIFIES: this->World.grid.squares
    // EFFECTS: Let the creature at pos hop according the given rules
    void CreatureLeft(point_t pos);
    // REQUIRES: the point_t pos of the grid is occupied by some creature
    // MODIFIES: this->World.grid.squares[pos.r][pos.c]->direction
    // EFFECTS: Change direction that results from turning
    // left from the original direction of the creature.
    void CreatureRight(point_t pos);
    // REQUIRES: the point_t pos of the grid is occupied by some creature
    // MODIFIES: this->World.grid.squares[pos.r][pos.c]->direction
    // EFFECTS: Change direction that results from turning
    // right from the original direction of the creature.
    void CreatureInfect(point_t pos);
    // REQUIRES: the point_t pos of the grid is occupied by some creature
    // MODIFIES: this->World.grid.squares
    // EFFECTS: Infect the creatures in the World according the rules
    void IfEmpty(point_t pos);
    // REQUIRES: the point_t pos of the grid is occupied by some creature
    // MODIFIES: this->World.grid.squares[pos.r][pos.c]->programID
    // EFFECTS: Change the programID of the creature if it's empty
    // in front of it according to the rules
    void IfEnemy(point_t pos);
    // REQUIRES: the point_t pos of the grid is occupied by some creature
    // MODIFIES: this->World.grid.squares[pos.r][pos.c]->programID
    // EFFECTS: Change the programID of the creature if there's enemy
    // in front of it according to the rules
    void IfSame(point_t pos);
    // REQUIRES: the point_t pos of the grid is occupied by some creature
    // MODIFIES: this->World.grid.squares[pos.r][pos.c]->programID
    // EFFECTS: Change the programID of the creature if there's a creature with
    // the same species in front of it according to the rules
    void IfWall(point_t pos);
    // REQUIRES: the point_t pos of the grid is occupied by some creature
    // MODIFIES: this->World.grid.squares[pos.r][pos.c]->programID
    // EFFECTS: Change the programID of the creature if there's wall
    // in front of it according to the rules
    void Go(point_t pos);
    // REQUIRES: the point_t pos of the grid is occupied by some creature
    // MODIFIES: this->World.grid.squares[pos.r][pos.c]->programID
    // EFFECTS: Change the programID of the creature according to the rules
public:
    simulation(int argc, char *argv[]);
    // MODIFIES: this, cout
    // EFFECTS: initialize this with the given arguments including the
    // World and species, and start simulating, typing the required outputs, and
    // throw exception if there's some error when initializing
};

#endif //P3_CLASS_H

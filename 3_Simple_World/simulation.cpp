#include "simulation.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>

simulation::simulation(int argc, char *argv[]) {
    if (argc<4){
        MissArguErr missargu;
        throw missargu.errmessage();
    } // Guarantee that there are enough arguments given
    istringstream input;
    input.str(argv[3]);
    input>>round_num;
    if (round_num<0){
        NegRoundErr neground;
        throw neground.errmessage();
    } // Guarantee that the given round number is not negative
    present_round=1;
    ifstream iFileWorld,iFileSpecies;
    iFileSpecies.open(argv[1]);
    if(!iFileSpecies){
        FileOpenErr fail_open_species(argv[1]);
        throw fail_open_species.errmessage();
    } // Guarantee that the species summery file is opened successfully
    createspecies(iFileSpecies); // initialize the species in this->World
    iFileSpecies.close();
    iFileWorld.open(argv[2]);
    if(!iFileWorld){
        FileOpenErr fail_open_world(argv[2]);
        throw fail_open_world.errmessage();
    }  // Guarantee that the world file is opened successfully
    createworld(iFileWorld); // initialize the world
    iFileWorld.close();
    checkverbose=0;
    if (argc>=5){
        stringstream verbose;
        verbose<<argv[4];
        if(verbose.str()=="v" || verbose.str()=="verbose") checkverbose=1;
    } // check whether concise mode or verbose mode is needed
    simulate();
    // start the turns of simulation
}

void simulation::createspecies(ifstream& iFileSpecies){
    string dir0;
    getline(iFileSpecies,dir0);
    ifstream iSpcies[MAXSPECIES];
    unsigned int speciesnum=0;
    int i;
    string dir, check, comment;
    bool legal=0;
    streampos pos;
    while(getline(iFileSpecies,dir)){
        if(speciesnum>=MAXSPECIES){
            MaxSpeciesErr MaxSp;
            iFileSpecies.close(); // every time there's error, close the files opened
            throw MaxSp.errmessage();
        }
        World.species[speciesnum].name=dir;
        World.species[speciesnum].programSize=0;
        dir=dir0+"/"+dir; // get the detailed directory of a species file for some species
        iSpcies[speciesnum].open(dir); // open the species file
        if(!iSpcies[speciesnum]){
            FileOpenErr fail_species(dir);
            iFileSpecies.close();
            iSpcies[speciesnum].close();
            throw fail_species.errmessage();
        }   // Guarantee the species file is opened successfully
        while(iSpcies[speciesnum]>>check){
            for(i=0;i<OP_SIZE;i++){
                if (check==opName[i]){
                    legal=1; // check which instruction it is
                    if((++World.species[speciesnum].programSize)>MAXPROGRAM){
                        iSpcies[speciesnum].close();
                        iFileSpecies.close();
                        MaxProgramErr MaxPro(World.species[speciesnum].name);
                        throw MaxPro.errmessage();
                    } // Guarantee the program size is legal
                    World.species[speciesnum].program[World.species[speciesnum].programSize-1].op=opcode_t(i);
                    if(i>INFECT){
                        break;
                    }
                }
            }
            if(legal==0){
                iSpcies[speciesnum].close();
                iFileSpecies.close();
                UnknownInstructionErr UnkownIn(check);
                throw UnkownIn.errmessage();
            } // if no such instruction found, close the files and throw the error message
            legal=0;
            getline(iSpcies[speciesnum],comment);
            if(i<OP_SIZE && comment!="\0") {
                World.species[speciesnum].program[World.species[speciesnum].programSize-1].address=(unsigned int)atoi(comment.c_str());
            } // for some instruction containing an address, read the address and save it
            pos = iSpcies[speciesnum].tellg();
            getline(iSpcies[speciesnum],comment);
            if(comment=="\0") break;
            else iSpcies[speciesnum].seekg(pos);
        } // check whther it's the last line for instruction, if it is not, come back to the endline of last line
        iSpcies[speciesnum++].close(); // close this species file and check next species
    }
    World.numSpecies=speciesnum;
}

void simulation::createworld(ifstream& iFileworld){
    if(!(iFileworld>>World.grid.height) || World.grid.height<1 || World.grid.height>MAXHEIGHT){
        iFileworld.close();
        MaxHeightErr MaxH;
        throw MaxH.errmessage();
    }
    if(!(iFileworld>>World.grid.width) || World.grid.width<1 || World.grid.width>MAXWIDTH){
        iFileworld.close();
        MaxWidthErr MaxW;
        throw MaxW.errmessage();
    }
    // Guarantee that the height and width are legal
    int i,j=0,k=0;
    bool legal=0;
    char T;
    iFileworld.get(T);
    while(iFileworld.get(T)){
        if(j>=World.grid.height || k>World.grid.width) break;
        if(T=='\n'){
            if((++j)==World.grid.height)break;
            k=0;
            continue;
        } // turn to a new line and stop reading the terrain type according the given height and width
        for(i=0;i<TERRAIN_SIZE ;i++) {
            if(T==terrainShortName[i][0]) {
                legal=1;
                World.grid.terrain[j][k]=terrain_t(i);
            }
        } // read and save the terrain type
        if(legal==0){
            iFileworld.close();
            IllegalTerrainErr IllegalTerr(&T,j,k);
            throw IllegalTerr.errmessage();
        } // Guarantee the terrain types are legal
        legal=0;
        k++;
    }
    string line,compare,detail;
    World.numCreatures=0;
    while(getline(iFileworld,line)){
        stringstream check;
        check<<line;
        check>>compare;
        legal=0;
        for(j=0;j<World.numSpecies;j++){
            if(compare==World.species[j].name){
                legal=1;
                World.creatures[World.numCreatures].species=&World.species[j];
            }
        } //read and save the creatures in order
        if(legal==0){
            iFileworld.close();
            UnknownSpeciesErr UnknownS(compare);
            throw UnknownS.errmessage();
        } // Guarantee that the creatures have legal species
        detail=compare;
        check>>compare;
        legal=0;
        for(j=0;j<DIRECT_SIZE;j++){
            if(compare==directName[j]){
                legal=1;
                World.creatures[World.numCreatures].direction=direction_t(j);
            }
        }   //read and save the directions
        if(legal==0){
            iFileworld.close();
            UnknownDirectionErr UnknownD(compare);
            throw UnknownD.errmessage();
        }// Guarantee that the directions are legal
        detail=detail+" "+compare;
        check>>j;
        stringstream J,K;
        J<<j;
        check>>k;
        K<<k;
        detail=detail+" "+J.str()+" "+K.str(); // get a string of the detailed information of a still-not-saved creature
        if(j>=World.grid.height || k>=World.grid.width){
            iFileworld.close();
            OutsideBoundaryErr OutsideB(detail,World.grid.height,World.grid.width);
            throw OutsideB.errmessage();
        } // check whether it's outside the boundary
        else if(Occupy[j][k]){
            iFileworld.close();
            point_t CreatureLocation={j,k};
            OverlapErr Overlap(detail,Detail(CreatureLocation));
            throw Overlap.errmessage();
        } // check whether it's overlapping
        else{
            Occupy[j][k]=1;
            World.creatures[World.numCreatures].location.r=j;
            World.creatures[World.numCreatures].location.c=k;
            World.grid.squares[j][k]=&World.creatures[World.numCreatures];
        } //save the creature, and save that it's location is occupied
        while(check>>compare){
            if(compare[0]==abilityName[FLY][0] && compare[1]=='\0'){
                World.creatures[World.numCreatures].ability[FLY]=1;
            }
            else if(compare[0]==abilityName[ARCH][0] && compare[1]=='\0'){
                World.creatures[World.numCreatures].ability[ARCH]=1;
            }
            else{
                iFileworld.close();
                InvalidAbilityErr InvalidA(detail,compare);
                throw InvalidA.errmessage();
            }
        } // check its abilities
        if(!World.grid.squares[j][k]->ability[FLY] && World.grid.terrain[j][k]==LAKE){
            iFileworld.close();
            CannotFlyErr CannotFly(detail);
            throw CannotFly.errmessage();
        } // check whether it can't fly but on a lake
        World.creatures[World.numCreatures].programID=1;
        World.creatures[World.numCreatures].hillActive=0;
        if(!World.creatures[World.numCreatures].ability[FLY] &&
           World.grid.terrain[World.creatures[World.numCreatures].location.r][World.creatures[World.numCreatures].location.c]==HILL){
            World.creatures[World.numCreatures].hillActive=1;
        } // check whether it's initially on a hill and can't fly
        World.numCreatures++;
    }
    if(World.numCreatures>MAXCREATURES){
        iFileworld.close();
        MaxCreaturesErr MaxC;
        MaxC.errmessage();
    }
}

void simulation::simulate() {
        cout<<"Initial state"<<endl;
        string layout[MAXHEIGHT][MAXWIDTH];
        DisplayLayout(layout);
        bool idle[World.numCreatures];
        int i;
        for(i=0;i<World.numCreatures;i++)
            idle[i]=1; // make sure every animal is initailly idle, so if initailly it's hill active, it can't move for the 1st round
        while (present_round<=round_num){
            cout<<"Round "<<present_round<<endl;
            for(i=0;i<World.numCreatures;i++){
                if( World.creatures[i].hillActive && idle[i]){
                    idle[i]=0;
                    continue;
                } // a creature which is hill active and idle doen't move for one round and no longer idle
                cout<<"Creature ("<<Detail(World.creatures[i].location)<<") takes action:";
                if(checkverbose) cout<<endl; // output according to the modes
                else cout<<" ";
                while(World.creatures[i].species->program[World.creatures[i].programID-1].op>INFECT){
                    if(checkverbose){
                        cout<<"Instruction "<<World.creatures[i].programID<<": "<<
                            opName[World.creatures[i].species->program[World.creatures[i].programID-1].op]<<" "<<
                            World.creatures[i].species->program[World.creatures[i].programID-1].address<<endl;
                    }
                    switch (World.creatures[i].species->program[World.creatures[i].programID-1].op){
                        case IFEMPTY :
                            IfEmpty(World.creatures[i].location);
                            break;
                        case IFENEMY :
                            IfEnemy(World.creatures[i].location);
                            break;
                        case IFSAME :
                            IfSame(World.creatures[i].location);
                            break;
                        case IFWALL :
                            IfWall(World.creatures[i].location);
                            break;
                        case GO :
                            Go(World.creatures[i].location);
                            break;
                        default:
                            break;
                    }
                } // change programID according the above instruction
                if(checkverbose) cout<<"Instruction "<<World.creatures[i].programID<<": ";
                cout<< opName[World.creatures[i].species->program[World.creatures[i].programID-1].op]<<endl;
                switch (World.creatures[i].species->program[World.creatures[i].programID-1].op){
                    case HOP :
                        CreatureHop(World.creatures[i].location);
                        break;
                    case LEFT :
                        CreatureLeft(World.creatures[i].location);
                        break;
                    case RIGHT :
                        CreatureRight(World.creatures[i].location);
                        break;
                    case INFECT :
                        CreatureInfect(World.creatures[i].location);
                        break;
                    default:
                        break;
                } // do the final movement
                if(!World.creatures[i].ability[FLY] &&
                        World.grid.terrain[World.creatures[i].location.r][World.creatures[i].location.c]==HILL){
                    World.creatures[i].hillActive=1;
                    idle[i]=1;
                } // initialize the hill effects
                if(checkverbose && (i!=World.numCreatures-1)) DisplayLayout(layout);
            }
            DisplayLayout(layout);
            // in verbose mode, display the layout after every creature's movement
            // otherwise, display the layout only after the last creature takes action
            present_round++;
        }


}

string simulation::Detail(point_t CreatureLocation) {
    stringstream J,K;
    J<<CreatureLocation.r;
    K<<CreatureLocation.c;
    string detail=World.grid.squares[CreatureLocation.r][CreatureLocation.c]->species->name+" "+
                  directName[World.grid.squares[CreatureLocation.r][CreatureLocation.c]->direction]+" "+J.str()+" "+K.str();;
    return detail; //return the details for a creature
}

void simulation::CreatureMove(point_t pos1, point_t pos2) {
    World.grid.squares[pos1.r][pos1.c]->location=pos2;
    World.grid.squares[pos2.r][pos2.c]=World.grid.squares[pos1.r][pos1.c];
    Occupy[pos2.r][pos2.c]=1;
    Occupy[pos1.r][pos1.c]=0;
    // move a creature from pos1 to pos2
}

void simulation::DisplayLayout(string layout[MAXHEIGHT][MAXWIDTH]) {
    int j,k;
    for (j=0;j<World.grid.height;j++){
        for(k=0;k<World.grid.width;k++){
            layout[j][k]="____ ";
            if(Occupy[j][k]){
                layout[j][k][0]=World.grid.squares[j][k]->species->name[0];
                layout[j][k][1]=World.grid.squares[j][k]->species->name[1];
                layout[j][k][3]=directShortName[World.grid.squares[j][k]->direction][0];
            }
            cout<<layout[j][k];
            if((k+1)==World.grid.width)
                cout<<endl;
        }
    }
}

point_t simulation::FrontPos(point_t pos, direction_t dir) {
    point_t NewPos=pos;
    switch(dir){
        case EAST :
            if((pos.c+1)<World.grid.width) {
                NewPos.c++;
            }
            break;
        case SOUTH :
            if((pos.r+1)<World.grid.height) {
                NewPos.r++;
            }
            break;
        case WEST :
            if((pos.c-1)>=0) {
                NewPos.c--;
            }
            break;
        case NORTH :
            if((pos.r-1)>=0) {
                NewPos.r--;
            }
            break;
        default:
            break;
    }
    return NewPos;
    // Returns a point that results from moving one square
    // in the direction given from the point pos, the postion dosen't
    // change if it's toward a boundary
}

point_t simulation::FrontPos(point_t pos) {
    return FrontPos(pos,World.grid.squares[pos.r][pos.c]->direction);
    // Returns a point that results from moving one square
    // in the direction of the creature from the point pos, the postion dosen't
    // change if it's toward a boundary
}


void simulation::CreatureHop(point_t pos) {
    point_t NewPos=FrontPos(pos);
    if(!Occupy[NewPos.r][NewPos.c] &&
            (World.grid.terrain[NewPos.r][NewPos.c]!=LAKE || World.grid.squares[pos.r][pos.c]->ability[FLY] )) {
        CreatureMove(pos,NewPos);
        World.grid.squares[NewPos.r][NewPos.c]->programID++;
    }
    else World.grid.squares[pos.r][pos.c]->programID++;
    // Let the creature at pos hop according the given rules
}

void simulation::CreatureLeft(point_t pos) {
    if(World.grid.squares[pos.r][pos.c]->direction==EAST)
        World.grid.squares[pos.r][pos.c]->direction=NORTH;
    else {
        World.grid.squares[pos.r][pos.c]->direction=direction_t ( World.grid.squares[pos.r][pos.c]->direction-1);
    }
    World.grid.squares[pos.r][pos.c]->programID++;
}

void simulation::CreatureRight(point_t pos) {
    if(World.grid.squares[pos.r][pos.c]->direction==NORTH)
        World.grid.squares[pos.r][pos.c]->direction=EAST;
    else {
        World.grid.squares[pos.r][pos.c]->direction=direction_t ( World.grid.squares[pos.r][pos.c]->direction+1);
    }
    World.grid.squares[pos.r][pos.c]->programID++;
}

void simulation::CreatureInfect(point_t pos) {
    if(!World.grid.squares[pos.r][pos.c]->ability[ARCH]){
        point_t frontpos=FrontPos(pos);
        if(frontpos.r!=pos.r || frontpos.c!=pos.c){
            if(Occupy[frontpos.r][frontpos.c]){
                if(World.grid.terrain[frontpos.r][frontpos.c]!=FOREST){
                    if(World.grid.squares[frontpos.r][frontpos.c]->species!=World.grid.squares[pos.r][pos.c]->species){
                        World.grid.squares[frontpos.r][frontpos.c]->species=World.grid.squares[pos.r][pos.c]->species;
                        World.grid.squares[frontpos.r][frontpos.c]->programID=1;
                    }
                }
            }
        }
        World.grid.squares[pos.r][pos.c]->programID++;
    } // if a creature can't arch, it can only infect a creature directly in front of it
    else{
        point_t pos0=pos;
        while(FrontPos(pos,World.grid.squares[pos0.r][pos0.c]->direction).r!=pos.r ||
                FrontPos(pos,World.grid.squares[pos0.r][pos0.c]->direction).c!=pos.c){
            if(Occupy[FrontPos(pos,World.grid.squares[pos0.r][pos0.c]->direction).r][FrontPos(pos,World.grid.squares[pos0.r][pos0.c]->direction).c] &&
                World.grid.squares[FrontPos(pos,World.grid.squares[pos0.r][pos0.c]->direction).r][FrontPos(pos,World.grid.squares[pos0.r][pos0.c]->direction).c]->species
                != World.grid.squares[pos0.r][pos0.c]->species) {
                World.grid.squares[FrontPos(pos,World.grid.squares[pos0.r][pos0.c]->direction).r][FrontPos(pos,World.grid.squares[pos0.r][pos0.c]->direction).c]->species
                        =World.grid.squares[pos0.r][pos0.c]->species;
                World.grid.squares[FrontPos(pos,World.grid.squares[pos0.r][pos0.c]->direction).r][FrontPos(pos,World.grid.squares[pos0.r][pos0.c]->direction).c]->programID=1;
                break;
            }
            else{
                pos=FrontPos(pos,World.grid.squares[pos0.r][pos0.c]->direction);

            }
            // constantly get a new position in the direction until it reaches boundary or find a creature
            // to be infected
        }
        World.grid.squares[pos0.r][pos0.c]->programID++;
    }
}

void simulation::IfEmpty(point_t pos) {
    if((FrontPos(pos).r!=pos.r || FrontPos(pos).c!=pos.c) && !Occupy[FrontPos(pos).r][FrontPos(pos).c]
            && World.grid.terrain[FrontPos(pos).r][FrontPos(pos).c]!=FOREST){
        World.grid.squares[pos.r][pos.c]->programID=
                World.grid.squares[pos.r][pos.c]->species->program[World.grid.squares[pos.r][pos.c]->programID-1].address;
    }
    else World.grid.squares[pos.r][pos.c]->programID++;
}

void simulation::IfEnemy(point_t pos) {
    if(World.grid.terrain[FrontPos(pos).r][FrontPos(pos).c]!=FOREST && Occupy[FrontPos(pos).r][FrontPos(pos).c]&&
                World.grid.squares[pos.r][pos.c]->species!=World.grid.squares[FrontPos(pos).r][FrontPos(pos).c]->species){
        World.grid.squares[pos.r][pos.c]->programID=
                World.grid.squares[pos.r][pos.c]->species->program[World.grid.squares[pos.r][pos.c]->programID-1].address;
    }
    else World.grid.squares[pos.r][pos.c]->programID++;
}

void simulation::IfSame(point_t pos) {
    if(World.grid.terrain[FrontPos(pos).r][FrontPos(pos).c]!=FOREST &&
            (FrontPos(pos).r!=pos.r || FrontPos(pos).c!=pos.c) && Occupy[FrontPos(pos).r][FrontPos(pos).c]&&
                    World.grid.squares[pos.r][pos.c]->species==World.grid.squares[FrontPos(pos).r][FrontPos(pos).c]->species){
        World.grid.squares[pos.r][pos.c]->programID=
                    World.grid.squares[pos.r][pos.c]->species->program[World.grid.squares[pos.r][pos.c]->programID-1].address;
    }
    else World.grid.squares[pos.r][pos.c]->programID++;
}

void simulation::IfWall(point_t pos) {
    if((FrontPos(pos).r==pos.r && FrontPos(pos).c==pos.c) ||
            (World.grid.terrain[FrontPos(pos).r][FrontPos(pos).c]==LAKE && !World.grid.squares[pos.r][pos.c]->ability[FLY]) ){
        World.grid.squares[pos.r][pos.c]->programID=
                World.grid.squares[pos.r][pos.c]->species->program[World.grid.squares[pos.r][pos.c]->programID].address;
    }
    else World.grid.squares[pos.r][pos.c]->programID++;
}

void simulation::Go(point_t pos) {
    World.grid.squares[pos.r][pos.c]->programID=
            World.grid.squares[pos.r][pos.c]->species->program[World.grid.squares[pos.r][pos.c]->programID-1].address;
}

string MissArguErr::errmessage() {
    string message="Error: Missing arguments!\n"
            "Usage: ./p3 <species-summary> <world-file> <rounds> [v|verbose]";
    return message;
}

string NegRoundErr::errmessage() {
    string message="Error: Number of simulation rounds is negative!";
    return message;
}

FileOpenErr::FileOpenErr(string filename) {
    Filename=filename;
}

string FileOpenErr::errmessage() {
    string message;
    message="Error: Cannot open file "+ Filename+"!";
    return message;
}
string MaxSpeciesErr::errmessage(){
    stringstream Stream;
    Stream << MAXSPECIES;
    string message="Error: Too many species!\n"
            "Maximal number of species is "+ Stream.str()+".";
    return message;
}

MaxProgramErr::MaxProgramErr(string Name){
    name=Name;
}

string MaxProgramErr::errmessage(){
    stringstream Stream;
    Stream << MAXPROGRAM;
    string message="Error: Too many instructions for species "+ name+"!\n"
            "Maximal number of instructions is "+Stream.str()+ ".";
    return message;
}

UnknownInstructionErr::UnknownInstructionErr(string InstructionName){
    name=InstructionName;
}

string UnknownInstructionErr::errmessage() {
    string message;
    message="Error: Instruction "+ name+" is not recognized!";
    return message;
}

UnknownSpeciesErr::UnknownSpeciesErr(string Name){
    name=Name;
}

string UnknownSpeciesErr::errmessage() {
    string message;
    message="Error: Species "+ name+" is not found!";
    return message;
}

UnknownDirectionErr::UnknownDirectionErr(string Name){
    name=Name;
}

string UnknownDirectionErr::errmessage() {
    string message;
    message="Error: Direction "+ name+" is not recognized!";
    return message;
}

string MaxHeightErr::errmessage(){
    string message="Error: The grid height is illegal!";
    return message;
}

string MaxWidthErr::errmessage(){
    string message="Error: The grid width is illegal!";
    return message;
}

string MaxCreaturesErr::errmessage() {
    stringstream Stream;
    Stream<<MAXCREATURES;
    string message="Error: Too many creatures!\n"
            "Maximal number of creatures is "+ Stream.str()+".";
    return message;
}

IllegalTerrainErr::IllegalTerrainErr(string Name, int j, int k){
    name=Name;
    pos.r=j;
    pos.c=k;
}

string IllegalTerrainErr::errmessage() {
    stringstream R,C;
    R<<pos.r;
    C<<pos.c;
    string message="Error: Terrain square ("+name+" "+R.str()+" "+ C.str() + ") is invalid!";
    return message;
}

OutsideBoundaryErr::OutsideBoundaryErr(string d,int h,int w){
    detail=d;
    height=h;
    width=w;
}

string OutsideBoundaryErr::errmessage() {
    stringstream H,W;
    H<<height;
    W<<width;
    string message="Error: Creature ("+detail+") is out of bound!\n"
            "The grid size is "+ H.str()+"-by-"+W.str()+".";
    return message;
}

InvalidAbilityErr::InvalidAbilityErr(string d,string n) {
    detail=d;
    name=n;
}

string InvalidAbilityErr::errmessage() {
    string message="Error: Creature ("+detail+") has an invalid ability "+name+"!";
    return message;
}

OverlapErr::OverlapErr(string d1,string d2){
    detail1=d1;
    detail2=d2;
}

string OverlapErr::errmessage(){
    string message="Error: Creature ("+detail1+") overlaps with creature ("+detail2+")!";
    return message;
}

CannotFlyErr::CannotFlyErr(string d){
    detail=d;
}

string CannotFlyErr::errmessage() {
    string message="Error: Creature ("+detail+") is in a lake square!\n"
            "The creature cannot fly!";
    return message;
}

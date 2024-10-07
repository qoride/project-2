#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <limits>

using namespace std;
const string symMap[] = {"  ","░░","▒▒","▓▓","██"}; //ASCII reference map

struct discipline{
    bool holy;
    string path;
};

struct checks{
    int reqLVL;
    float reqHP;
    float reqMP;
    float reqATK;
    float reqDEF;
    float reqSPD;
    float reqINT;
};

struct statusEffect{
    int ID;
    int potency;
    int count;
    vector<string> procs;
};

struct item{
    string type;
    checks requirements;
    vector<statusEffect> bonuses;
};

struct equipment{
    item head;
    item body;
    item legs;
    item weapon;
    item offhand;
    vector<item> accessories;
};

struct gameActor{
    //max values
    float maxHP;
    float maxMP;

    //base stats
    float statATK;
    float statDEF;
    float statSPD;
    float statINT;

    //properties
    float health;
    float shield;
    float mana;
    float gold;
    discipline path;
    float attack;
    float defense;
    float speed;
    float intel;
    vector<statusEffect> status;
    vector<item> inventory;
    equipment equips;
};

//PROTOTYPES
int Reference(string s, string l);
void clearStream();
void onHit(gameActor attacker, gameActor target);
void onAttacked(gameActor attacker, gameActor target);
void onDefense(gameActor attacker, gameActor target);
void onEvade(gameActor attacker, gameActor target);
void turnStart(gameActor subject);
void turnEnd(gameActor subject);

int main(){
    string event = "none";

    return 0;
}

//DEFINITIONS
struct statusEffect newEffect(string name, int p, int c, vector<string> activate){
    struct statusEffect effect;
    effect.ID = Reference(name,"effects");
    effect.potency = p;
    effect.count = c;
    effect.procs = activate;
}

void procEffect(gameActor subject, statusEffect effect){
    switch(effect.ID){
    case 0:
        break;
    
    default: //unrecognized effect
        break;
    }
}

void onHit(gameActor attacker, gameActor target){

}
void onAttacked(gameActor attacker, gameActor target){

}
void onDefense(gameActor attacker, gameActor target){

}
void onEvade(gameActor attacker, gameActor target){

}
void turnStart(gameActor subject){

}
void turnEnd(gameActor subject){

}

void clearStream(){ //reject the rest of the inputs
    cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    cin.clear();
}

int Reference(string s, string l){ //returns the index of a string from the given array
    return -1;
}

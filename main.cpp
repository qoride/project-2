#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <limits>

using namespace std;
const string symMap[] = {"  ","░░","▒▒","▓▓","██"}; //ASCII reference map
const vector<string> effectLibrary = {"strength","endurance","haste","focus","bleed","burn","regeneration","poison","","","","","",};

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
int Reference(string s, vector<string> l);
void clearStream();
void onHit(gameActor attacker, gameActor target, string result);
void turnStart(gameActor subject);
void turnEnd(gameActor subject);

int main(){
    string event = "none";

    return 0;
}

//DEFINITIONS
struct statusEffect newEffect(string name, int p, int c, vector<string> activate){
    struct statusEffect effect;
    effect.ID = Reference(name,effectLibrary);
    effect.potency = p;
    effect.count = c;
    effect.procs = activate;
}
//"strength","endurance","haste","focus","bleed","burn","regeneration","poison","","","","","",
void procEffect(gameActor subject, statusEffect effect){
    switch(effect.ID){
    case -1: //unknown effect
        break;
    case 0: //strength
        subject.attack += effect.potency;
        break;
    case 1: //endurance
        subject.defense += effect.potency;
        break;
    case 2: //haste
        subject.speed += effect.potency;
        break;
    case 3: //focus
        subject.intel += effect.potency;
        break;
    case 4: //bleed
        subject.health -= effect.potency;
        break;
    case 5: //burn
        subject.health -= effect.potency;
        break;
    case 6: //regeneration
        subject.health += effect.potency;
        break;
    case 7: //poison
        subject.health -= subject.health*0.01*effect.potency;
        break;
    
    default:
        break;
    }
    effect.count--;
    if(effect.count == 0){
        for(int i = 0; i < subject.status.size(); i++){
            if(subject.status.at(i).ID == effect.ID)subject.status.erase(subject.status.begin()+i);
            break;
        }
    }
}

void onHit(gameActor attacker, gameActor target, string result){
    //result can be "Hit", "Blocked", or "Dodged"
    string targetResult = (result == "Hit")?"getHit":(result == "Blocked")?"onBlock":"onDodge";
    for(int i = 0; i < attacker.status.size(); i++){
        if(Reference(result,attacker.status.at(i).procs)!=-1){
            procEffect(attacker,attacker.status.at(i));
        }
    }
    for(int i = 0; i < target.status.size(); i++){
        if(Reference(targetResult,target.status.at(i).procs)!=-1){
            procEffect(target,target.status.at(i));
        }
    }
}
void turnStart(gameActor subject){
    for(int i = 0; i < subject.status.size(); i++){
        if(Reference("turnStart",subject.status.at(i).procs)!=-1){
            procEffect(subject,subject.status.at(i));
        }
    }
}
void turnEnd(gameActor subject){
    for(int i = 0; i < subject.status.size(); i++){
        if(Reference("turnEnd",subject.status.at(i).procs)!=-1){
            procEffect(subject,subject.status.at(i));
        }
    }
}

void clearStream(){ //reject the rest of the inputs
    cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    cin.clear();
}

int Reference(string s, vector<string> l){ //returns the index of a string from the given vector
    for(int i = 0; i < l.size(); i++){
        if(s == l.at(i))return i;
    }
    return -1;
}

#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <limits>

using namespace std;
const string symMap[] = {"  ","░░","▒▒","▓▓","██"}; //ASCII reference map
const vector<string> effectLibrary = {"strength","endurance","haste","focus","bleed","burn","regeneration"};

struct charClass{
    string className;
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

struct skill{
    string name;
    int min;
    int max;
    int flat;
    int cost;
    string scalingType;
    string damageType;
    string targetType;
    vector<statusEffect> effects;
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
    charClass path;
    float attack;
    float defense;
    float speed;
    float intel;
    vector<statusEffect> status;
    string modus;
    vector<item> inventory;
    equipment equips;
    vector<skill> skills;
};

//PROTOTYPES
statusEffect newEffect(string name, int p, int c, vector<string> activate);
skill newSkill(string name, int lowest, int highest, int base, int cost, string scale, string damage, string target, vector<string> bonuses);
int Reference(string s, vector<string> l);
void clearStream();
void checkAll(gameActor subject, string check);
void onHit(gameActor attacker, gameActor target, skill attack, string result);
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
    return effect;
}

struct skill newSkill(string name, int lowest, int highest, int base, int price, string scale, string damage, string target, vector<statusEffect> bonuses){
    struct skill move;
    move.name = name;
    move.min = lowest;
    move.max = highest;
    move.flat = base;
    move.cost = price;
    move.scalingType = scale;
    move.damageType = damage;
    move.targetType = target;
    move.effects = bonuses;
    return move;
}

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
    default: //you should not be able to trigger this
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

void checkAll(gameActor subject, string check){
    //STATUS
    for(int i = 0; i < subject.status.size(); i++){
        if(Reference(check,subject.status.at(i).procs)!=-1){
            procEffect(subject,subject.status.at(i));
        }
    }
    //EQUIPS
    for(int i = 0; i < subject.equips.head.bonuses.size(); i++){
        if(Reference(check,subject.equips.head.bonuses.at(i).procs)!=-1){
            procEffect(subject,subject.equips.head.bonuses.at(i));
        }
    }
    for(int i = 0; i < subject.equips.body.bonuses.size(); i++){
        if(Reference(check,subject.equips.body.bonuses.at(i).procs)!=-1){
            procEffect(subject,subject.equips.body.bonuses.at(i));
        }
    }
    for(int i = 0; i < subject.equips.legs.bonuses.size(); i++){
        if(Reference(check,subject.equips.legs.bonuses.at(i).procs)!=-1){
            procEffect(subject,subject.equips.legs.bonuses.at(i));
        }
    }
    for(int i = 0; i < subject.equips.weapon.bonuses.size(); i++){
        if(Reference(check,subject.equips.weapon.bonuses.at(i).procs)!=-1){
            procEffect(subject,subject.equips.weapon.bonuses.at(i));
        }
    }
    for(int i = 0; i < subject.equips.offhand.bonuses.size(); i++){
        if(Reference(check,subject.equips.offhand.bonuses.at(i).procs)!=-1){
            procEffect(subject,subject.equips.offhand.bonuses.at(i));
        }
    }
    //ACCESSORIES
    for(int j = 0; j < 3; j++){
        if(subject.equips.accessories.at(j).type != "null"){
            for(int i = 0; i < subject.equips.accessories.at(j).bonuses.size(); i++){
                if(Reference(check,subject.equips.accessories.at(j).bonuses.at(i).procs)!=-1){
                    procEffect(subject,subject.equips.accessories.at(j).bonuses.at(i));
                }
            }
        }
    }
}

void onHit(gameActor attacker, gameActor target, skill attack, string result){
    //result can be "Hit", "Blocked", or "Dodged"
    string targetResult = (result == "Hit")?"getHit":(result == "Blocked")?"onBlock":"onDodge";
    
    checkAll(attacker,result);

    checkAll(target,targetResult);
}
void turnStart(gameActor subject){
    checkAll(subject,"turnStart");
}
void turnEnd(gameActor subject){
    checkAll(subject,"turnEnd");
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

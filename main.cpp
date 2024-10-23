#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <limits>

using namespace std;
const string symMap[] = {"  ","░░","▒▒","▓▓","██"}; //ASCII reference map
const vector<string> effectLibrary = {"strength","endurance","haste","focus","bleed","burn","regeneration","injured"};

//STRUCTURES
struct CharClass{
    string className;
};

struct Checks{
    int reqLVL;
    float reqHP;
    float reqMP;
    float reqATK;
    float reqDEF;
    float reqSPD;
    float reqINT;
};

struct StatusEffect{
    int ID;
    GameActor parent;
    int potency;
    int count;
    vector<string> procs;

    void Proc(){
        switch(ID){
        case -1: //NULL effect
            break;
        case 0: //strength
            parent.attack += potency;
            break;
        case 1: //endurance
            parent.defense += potency;
            break;
        case 2: //haste
            parent.speed += potency;
            break;
        case 3: //focus
            parent.intel += potency;
            break;
        case 4: //bleed
            parent.Damage(potency);
            break;
        case 5: //burn
            parent.Damage(potency);
            break;
        case 6: //regeneration
            parent.Heal(potency);
            break;
        default: //unknown effect
            break;
        }
        if(count!=-1){ //-1 count == infinite duration usually for equipment
            count--;
            if(count <= 0){
                for(int i = 0; i < parent.status.size(); i++){
                    if(&parent.status.at(i) == this)Destroy();
                    break;
                }
            }
        }
    }

    void Destroy(){
        delete this;
    }
};

struct Item{
    string type;
    Checks requirements;
    vector<StatusEffect> bonuses;
};

struct Equipment{
    Item head;
    Item body;
    Item legs;
    Item weapon;
    Item offhand;
    vector<Item> accessories;
};

struct Skill{
    string name;
    int min;
    int max;
    int flat;
    int cost;
    string scalingType;
    string damageType;
    string targetType;
    vector<StatusEffect> effects;
};

struct GameActor{
    //max values
    float maxHP;
    float maxMP;

    //base stats
    string name;
    float statATK;
    float statDEF;
    float statSPD;
    float statINT;

    //properties
    float health;
    float shield;
    float mana;
    float gold;
    CharClass path;
    float attack;
    float defense;
    float speed;
    float intel;
    vector<StatusEffect> status;
    vector<Item> inventory;
    Equipment equips;
    vector<Skill> skills;

    void Damage(float dmg){

    }

    void Heal(float amt){

    }
};

//PROTOTYPES
GameActor newActor();
StatusEffect newEffect(string name="NULL", GameActor subject=newActor(), int p=1, int c=1, vector<string> activate={"turnStart"});
Skill newSkill(string name, int lowest, int highest, int base, int cost, string scale, string damage, string target, vector<StatusEffect> bonuses);
int Reference(string s, vector<string> l);
void clearStream();
void checkAll(GameActor subject, string check);
void onHit(GameActor attacker, GameActor target, Skill attack, string result);
void turnStart(GameActor subject);
void turnEnd(GameActor subject);

int main(){
    string event = "none";

    GameActor none = newActor();

    vector<StatusEffect> test;
    test.push_back(newEffect("strength",none));
    test.push_back(newEffect("endurance",none));
    test.push_back(newEffect("haste",none));

    for(int i = 0; i<test.size();i++){
        cout << test.at(i).ID << endl;
    }

    return 0;
}

//CONSTRUCTORS
struct StatusEffect newEffect(string name, GameActor subject, int p, int c, vector<string> activate){
    struct StatusEffect effect;
    effect.ID = Reference(name,effectLibrary);
    effect.parent = subject;
    effect.potency = p;
    effect.count = c;
    effect.procs = activate;
    return effect;
}

struct Skill newSkill(string SKILLname, int DMG_min, int DMG_max, int DMG, int MPcost, string scale, string damage, string target, vector<StatusEffect> bonuses){
    struct Skill move;
    move.name = SKILLname;
    move.min = DMG_min;
    move.max = DMG_max;
    move.flat = DMG;
    move.cost = MPcost;
    move.scalingType = scale;
    move.damageType = damage;
    move.targetType = target;
    move.effects = bonuses;
    return move;
}

struct GameActor newActor(){
    struct GameActor actor;
    return actor;
}

//DEFINITIONS

void checkAll(GameActor subject, string check){
    //STATUS
    for(int i = 0; i < subject.status.size(); i++){
        if(Reference(check,subject.status.at(i).procs)!=-1){
            subject.status.at(i).Proc();
        }
    }
    //EQUIPS
    for(int i = 0; i < subject.equips.head.bonuses.size(); i++){
        if(Reference(check,subject.equips.head.bonuses.at(i).procs)!=-1){
            subject.equips.head.bonuses.at(i).Proc();
        }
    }
    for(int i = 0; i < subject.equips.body.bonuses.size(); i++){
        if(Reference(check,subject.equips.body.bonuses.at(i).procs)!=-1){
            subject.equips.body.bonuses.at(i).Proc();
        }
    }
    for(int i = 0; i < subject.equips.legs.bonuses.size(); i++){
        if(Reference(check,subject.equips.legs.bonuses.at(i).procs)!=-1){
            subject.equips.legs.bonuses.at(i).Proc();
        }
    }
    for(int i = 0; i < subject.equips.weapon.bonuses.size(); i++){
        if(Reference(check,subject.equips.weapon.bonuses.at(i).procs)!=-1){
            subject.equips.weapon.bonuses.at(i).Proc();
        }
    }
    for(int i = 0; i < subject.equips.offhand.bonuses.size(); i++){
        if(Reference(check,subject.equips.offhand.bonuses.at(i).procs)!=-1){
            subject.equips.offhand.bonuses.at(i).Proc();
        }
    }
    //ACCESSORIES
    for(int j = 0; j < 3; j++){
        if(subject.equips.accessories.at(j).type != "null"){
            for(int i = 0; i < subject.equips.accessories.at(j).bonuses.size(); i++){
                if(Reference(check,subject.equips.accessories.at(j).bonuses.at(i).procs)!=-1){
                    subject.equips.accessories.at(j).bonuses.at(i).Proc();
                }
            }
        }
    }
}

void onHit(GameActor attacker, GameActor target, Skill attack, string result){
    //result can be "Hit", "Blocked", or "Dodged"
    string targetResult = (result == "Hit")?"getHit":(result == "Blocked")?"onBlock":"onDodge";
    
    checkAll(attacker,result);

    checkAll(target,targetResult);
}
void turnStart(GameActor subject){
    checkAll(subject,"turnStart");
}
void turnEnd(GameActor subject){
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

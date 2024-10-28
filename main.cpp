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
    int potency;
    int count;
    vector<string> procs;
    vector<StatusEffect>* parent;

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
GameActor* newActor();
StatusEffect* newEffect(string name="NULL", int p=1, int c=1, vector<string> activate={"turnStart"});
void applyEffect(StatusEffect* effect, vector<StatusEffect> &subject);
Skill newSkill(string name, int lowest, int highest, int base, int cost, string scale, string damage, string target, vector<StatusEffect> bonuses);
int Reference(string s, vector<string> l);
void clearStream();
void checkAll(GameActor &subject, string check);
void onHit(GameActor &attacker, GameActor target, Skill attack, string result);
void turnStart(GameActor &subject);
void turnEnd(GameActor &subject);

int main(){
    string event = "none";

    GameActor* actor = newActor();

    applyEffect(newEffect("strength",1,3),actor->status);
    applyEffect(newEffect("endurance",1,2),actor->status);
    applyEffect(newEffect("haste",1,1),actor->status);

    for(int i = 0; i<actor->status.size();i++){
        cout << actor->status.at(i).count << endl;
    }

    turnStart(*actor);

    for(int i = 0; i<actor->status.size();i++){
        cout << actor->status.at(i).count << endl;
    }

    return 0;
}

//CONSTRUCTORS
struct StatusEffect* newEffect(string name, int p, int c, vector<string> activate){
    struct StatusEffect* effect = new StatusEffect;
    effect->ID = Reference(name,effectLibrary);
    effect->potency = p;
    effect->count = c;
    effect->procs = activate;
    return effect;
}

void applyEffect(StatusEffect* effect, vector<StatusEffect> &subject){
    subject.push_back(*effect);
    effect->parent = &subject;
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

struct GameActor* newActor(){
    struct GameActor* actor = new GameActor;
    return actor;
}

//DEFINITIONS
void Proc(GameActor &subject, StatusEffect* effect){
    switch(effect->ID){
    case -1: //NULL effect
        break;
    case 0: //strength
        subject.attack += effect->potency;
        break;
    case 1: //endurance
        subject.defense += effect->potency;
        break;
    case 2: //haste
        subject.speed += effect->potency;
        break;
    case 3: //focus
        subject.intel += effect->potency;
        break;
    case 4: //bleed
        subject.Damage(effect->potency);
        break;
    case 5: //burn
        subject.Damage(effect->potency);
        break;
    case 6: //regeneration
        subject.Heal(effect->potency);
        break;
    default: //unknown effect
        break;
    }
    if(effect->count!=-1){ //-1 count == infinite duration usually for equipment
        effect->count--;
        if(effect->count == 0){
            for(auto it = effect->parent->begin(); it != effect->parent->end(); ++it){
                if(&(*it) == effect){
                    cout << "hi";
                }
            }
        }
    }
}

void checkAll(GameActor &subject, string check){
    //STATUS
    if(subject.status.size()!=0){
        for(int i = 0; i < subject.status.size(); i++){
            if(Reference(check,subject.status.at(i).procs)!=-1){
                Proc(subject,&subject.status.at(i));
            }
        }
    }
    //EQUIPS
    if(subject.equips.head.bonuses.size()!=0){
        for(int i = 0; i < subject.equips.head.bonuses.size(); i++){
            if(Reference(check,subject.equips.head.bonuses.at(i).procs)!=-1){
                Proc(subject,&subject.equips.head.bonuses.at(i));
            }
        }
    }
    if(subject.equips.body.bonuses.size()!=0){
        for(int i = 0; i < subject.equips.body.bonuses.size(); i++){
            if(Reference(check,subject.equips.body.bonuses.at(i).procs)!=-1){
                Proc(subject,&subject.equips.body.bonuses.at(i));
            }
        }
    }
    if(subject.equips.legs.bonuses.size()!=0){
        for(int i = 0; i < subject.equips.legs.bonuses.size(); i++){
            if(Reference(check,subject.equips.legs.bonuses.at(i).procs)!=-1){
                Proc(subject,&subject.equips.legs.bonuses.at(i));
            }
        }
    }
    if(subject.equips.weapon.bonuses.size()!=0){
        for(int i = 0; i < subject.equips.weapon.bonuses.size(); i++){
            if(Reference(check,subject.equips.weapon.bonuses.at(i).procs)!=-1){
                Proc(subject,&subject.equips.weapon.bonuses.at(i));
            }
        }
    }
    if(subject.equips.offhand.bonuses.size()!=0){
        for(int i = 0; i < subject.equips.offhand.bonuses.size(); i++){
            if(Reference(check,subject.equips.offhand.bonuses.at(i).procs)!=-1){
                Proc(subject,&subject.equips.offhand.bonuses.at(i));
            }
        }
    }
    //ACCESSORIES
    /*
    for(int j = 0; j < 3; j++){
        if(subject.equips.accessories.at(j).type != "null"){
            for(int i = 0; i < subject.equips.accessories.at(j).bonuses.size(); i++){
                if(Reference(check,subject.equips.accessories.at(j).bonuses.at(i).procs)!=-1){
                    Proc(subject,subject.equips.accessories.at(j).bonuses.at(i));
                }
            }
        }
    }*/
}

void onHit(GameActor &attacker, GameActor target, Skill attack, string result){
    //result can be "Hit", "Blocked", or "Dodged"
    string targetResult = (result == "Hit")?"getHit":(result == "Blocked")?"onBlock":"onDodge";
    
    checkAll(attacker,result);

    checkAll(target,targetResult);
}
void turnStart(GameActor &subject){
    checkAll(subject,"turnStart");
}
void turnEnd(GameActor &subject){
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
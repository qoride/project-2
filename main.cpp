#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <limits>
#include "struct.hpp"

using namespace std;

//CONSTANTS
const string symMap[] = {"  ","░░","▒▒","▓▓","██"}; //ASCII reference map
const vector<string> effectLibrary = {"strength","endurance","haste","focus","bleed","burn",
                                      "regeneration","barrier","injured"};

const vector<string> damageTypes = {"HP","MP","AC"};

//PROTOTYPES
GameActor newActor(string ACTORname="NULL",float HP_max=100,float MP_max=100,
                   int I_gold=0,float I_shield=0,CharClass I_path={},
                   vector<float> STATS={0,0,0,0},vector<Item> INV={},Equipment EQUIP={},vector<Skill> SKILLS={});

StatusEffect newEffect(string name="NULL", int p=1, int c=1, vector<string> activate={"turnStart"});
void apply(StatusEffect effect, vector<StatusEffect> &subject);

Skill newSkill(string SKILLname, int DMG_min, int DMG_max, int MP_cost, string DMG_scale, string DMG_type, string target, vector<StatusEffect> bonuses);

void checkAll(GameActor &subject, string check);
void onHit(GameActor &attacker, GameActor &target, Skill attack, string result);
void turnStart(GameActor &subject);
void turnEnd(GameActor &subject);

void clearStream();

//GLOBAL VARIABLES
vector<GameActor> Enemies;

int main(){
    string event = "none";

    GameActor actor = newActor();

    apply(newEffect("strength",1,3),actor.status);
    apply(newEffect("endurance",1,2),actor.status);
    apply(newEffect("haste",1,1),actor.status);

    for(int i = 0; i<actor.status.size();i++){
        cout << actor.status.at(i).count << endl;
    }

    turnStart(actor);

    for(int i = 0; i<actor.status.size();i++){
        cout << actor.status.at(i).count << endl;
    }

    return 0;
}

//CONSTRUCTORS
struct StatusEffect newEffect(string name, int p, int c, vector<string> activate){
    struct StatusEffect effect;
    effect.ID = Reference(name,effectLibrary);
    effect.potency = p;
    effect.count = c;
    effect.procs = activate;
    return effect;
}

void apply(StatusEffect effect, vector<StatusEffect> &subject){
    bool duplicate = false;
    for(int i = 0; i < subject.size(); i++){
        if(subject.at(i)==effect){
            duplicate = true;
            subject.at(i).count += effect.count;
            break;
        }
    }
    if(duplicate == false){
        subject.push_back(effect);
    }
    
    effect.parent = &subject;
}

struct Skill newSkill(string SKILLname, int DMG_min, int DMG_max, int MP_cost, string DMG_scale, string DMG_type,
                      string target, vector<StatusEffect> bonuses)
{
    struct Skill move;
    move.name = SKILLname;
    move.min = DMG_min;
    move.max = DMG_max;
    move.cost = MP_cost;
    move.scalingType = DMG_scale;
    move.damageType = DMG_type;
    move.targetType = target;
    move.effects = bonuses;
    return move;
}

struct GameActor newActor(string ACTORname,float HP_max,float MP_max,int I_gold,float I_shield,CharClass I_path,
                          vector<float> STATS,vector<Item> INV,Equipment EQUIP,vector<Skill> SKILLS)
{
    struct GameActor actor;
    actor.name = ACTORname;

    actor.maxHP = HP_max;
    actor.maxMP = MP_max;

    actor.dead = false;
    actor.health = HP_max;
    actor.mana = MP_max;

    actor.gold = I_gold;
    actor.shield = I_shield;
    actor.path = I_path;

    actor.attack = (STATS.size()>0)?STATS[0]:0;
    actor.defense = (STATS.size()>1)?STATS[1]:0;
    actor.speed = (STATS.size()>2)?STATS[2]:0;
    actor.intel = (STATS.size()>3)?STATS[3]:0;

    actor.status = {};
    actor.inventory = INV;
    actor.equips = EQUIP;
    actor.skills = SKILLS;

    return actor;
}

//DEFINITIONS
void Proc(GameActor &subject, StatusEffect &effect){
    switch(effect.ID){
    case -1: //NULL effect
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
        subject.Damage(effect.potency);
        break;
    case 5: //burn
        subject.Damage(effect.potency);
        break;
    case 6: //regeneration
        subject.Heal(effect.potency);
        break;
    case 7: //barrier
        subject.Heal(effect.potency,"AC");
        break;
    default: //unknown effect
        break;
    }
    if(effect.count!=-1){ //-1 count == infinite duration usually for equipment
        effect.count--;
        if(effect.count == 0){
            for(int i = 0; i < effect.parent->size(); i++){
                //cout << (effect.parent->at(i) == effect) << endl;
                if(effect.parent->at(i) == effect){
                    effect.parent->erase(effect.parent->begin()+i);
                    break;
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
                Proc(subject,subject.status.at(i));
            }
        }
    }
    //EQUIPS
    if(subject.equips.head.bonuses.size()!=0){
        for(int i = 0; i < subject.equips.head.bonuses.size(); i++){
            if(Reference(check,subject.equips.head.bonuses.at(i).procs)!=-1){
                Proc(subject,subject.equips.head.bonuses.at(i));
            }
        }
    }
    if(subject.equips.body.bonuses.size()!=0){
        for(int i = 0; i < subject.equips.body.bonuses.size(); i++){
            if(Reference(check,subject.equips.body.bonuses.at(i).procs)!=-1){
                Proc(subject,subject.equips.body.bonuses.at(i));
            }
        }
    }
    if(subject.equips.legs.bonuses.size()!=0){
        for(int i = 0; i < subject.equips.legs.bonuses.size(); i++){
            if(Reference(check,subject.equips.legs.bonuses.at(i).procs)!=-1){
                Proc(subject,subject.equips.legs.bonuses.at(i));
            }
        }
    }
    if(subject.equips.weapon.bonuses.size()!=0){
        for(int i = 0; i < subject.equips.weapon.bonuses.size(); i++){
            if(Reference(check,subject.equips.weapon.bonuses.at(i).procs)!=-1){
                Proc(subject,subject.equips.weapon.bonuses.at(i));
            }
        }
    }
    if(subject.equips.offhand.bonuses.size()!=0){
        for(int i = 0; i < subject.equips.offhand.bonuses.size(); i++){
            if(Reference(check,subject.equips.offhand.bonuses.at(i).procs)!=-1){
                Proc(subject,subject.equips.offhand.bonuses.at(i));
            }
        }
    }
    //ACCESSORIES
    for(int j = 0; j < subject.equips.accessories.size(); j++){
        for(int i = 0; i < subject.equips.accessories.at(j).bonuses.size(); i++){
            if(Reference(check,subject.equips.accessories.at(j).bonuses.at(i).procs)!=-1){
                Proc(subject,subject.equips.accessories.at(j).bonuses.at(i));
            }
        }
    }
}

void onHit(GameActor &attacker, GameActor &target, Skill attack, string result){
    //result can be "Hit", "Blocked", or "Dodged"
    string targetResult = (result == "Hit")?"getHit":(result == "Blocked")?"onBlock":"onDodge";
    
    checkAll(attacker,result);
    checkAll(target,targetResult);
}

void turnStart(GameActor &subject){
    subject.Start();
    checkAll(subject,"turnStart");
}

void turnEnd(GameActor &subject){
    checkAll(subject,"turnEnd");
}

void clearStream(){ //reject the rest of the inputs
    cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    cin.clear();
}
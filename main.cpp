#include <iostream>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <limits>
#include "struct.hpp"

using namespace std;

//CONSTANTS
const string symMap[] = {"  ","░░","▒▒","▓▓","██"}; //ASCII reference map
const vector<string> effectLibrary = {"strength","endurance","haste","focus","bleed","burn",
                                      "regeneration","barrier","injured"};

//PROTOTYPES
GameActor newActor(string ACTORname,float HP_max,float MP_max,vector<int> I_Val,float I_shield,CharClass I_path,
                          vector<StatusEffect> efx, vector<float> STATS,
                          vector<Item> INV,Equipment EQUIP,vector<Skill> SKILLS);

StatusEffect newEffect(string name="NULL", int p=1, int c=1, vector<string> activate={"turnStart"}, string txt="");
void apply(StatusEffect effect, vector<StatusEffect> &subject);

Skill newSkill(string SKILLname, int DMG_min, int DMG_max, int MP_cost, string DMG_scale, string DMG_type, string target, vector<StatusEffect> bonuses);

void checkAll(GameActor &subject, string check);
StatusEffect checkFor(GameActor &subject, int EffectID);
vector<StatusEffect> allOf(GameActor &subject, string check = "");
void Attack(GameActor &attacker, GameActor &target, Skill attack, string result);
void turnStart(GameActor &subject);
void turnEnd(GameActor &subject);

void clearStream();

//GLOBAL VARIABLES
vector<GameActor> Enemies;

int main(){
    string event = "none";
    string input;

    GameActor actor = newActor("Player",100,100,    //Health
                               {1,0,0},0,{},{},{2,0,0,4}, //Stats
                               {},{},{});           //Items

    GameActor goblin = newActor("Goblin",50,10,     //Health
                               {1,0,20},0,{},{},{0,0,0,0},//Stats
                               {},{},{});           //Items

    GameActor golem = newActor("Golem",40,15,
                               {1,10,5},10,{},{newEffect("barrier",5,3)},{0,2,0,0},
                               {},{},{});

    Enemies.push_back(goblin);
    Enemies.push_back(golem);

    actor.Start();
    for(int i = 0; i < Enemies.size(); i++)Enemies.at(i).Start();
    while(Enemies.size() > 0 && actor.dead == false){
        turnStart(actor);
        cout << endl << "┌─────────────────────────────────────────────────┐" << endl
        <<              "│ HP: 50/100 ██████████▓▓▓▓▓▓▓▓▓▓          PLAYER │" << endl
        <<              "│ MP: 8/10   ▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒░░░░          LVL.10 │" << endl
        <<              "├─────────────────────────────────────────────────┤" << endl
        <<              "│ > ATTACK            > SKILL                     │" << endl
        <<              "│ > BLOCK             > DODGE                     │" << endl
        <<              "│ > INFO              > ITEM                      │" << endl
        <<              "├─────────────────────────────────────────────────┤" << endl
        <<              "│ \"exit\" - Exit the program.                      │" << endl
        <<              "└─────────────────────────────────────────────────┘" << endl
        <<              "Please enter a command: ";
        cin >> input;
    }

    return 0;
}

//CONSTRUCTORS
struct StatusEffect newEffect(string name, int p, int c, vector<string> activate, string txt){
    struct StatusEffect effect;
    effect.ID = Reference(name,effectLibrary);
    effect.potency = p;
    effect.count = c;
    effect.procs = activate;
    effect.inspectTxt = txt;
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

struct GameActor newActor(string ACTORname,float HP_max,float MP_max,vector<int> I_Val,float I_shield,CharClass I_path,
                          vector<StatusEffect> efx, vector<float> STATS,
                          vector<Item> INV,Equipment EQUIP,vector<Skill> SKILLS)
{
    struct GameActor actor;
    actor.name = ACTORname;

    actor.maxHP = HP_max;
    actor.maxMP = MP_max;

    actor.dead = false;
    actor.health = HP_max;
    actor.mana = MP_max;

    actor.level = (I_Val.size()>0)?I_Val[0]:1;
    actor.experience = (I_Val.size()>1)?I_Val[1]:0;
    actor.gold = (I_Val.size()>2)?I_Val[2]:0;
    actor.shield = I_shield;
    actor.path = I_path;

    actor.attack = (STATS.size()>0)?STATS[0]:0;
    actor.defense = (STATS.size()>1)?STATS[1]:0;
    actor.speed = (STATS.size()>2)?STATS[2]:0;
    actor.intel = (STATS.size()>3)?STATS[3]:0;

    actor.status = efx;
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
        cout << subject.name << " is glowing " << ((effect.potency>3)?((effect.potency>6)?"intensely":"bright"):"faintly") << " red!" << endl;
        break;
    case 1: //endurance
        subject.defense += effect.potency;
        cout << subject.name << " is glowing " << ((effect.potency>3)?((effect.potency>6)?"intensely":"bright"):"faintly") << " yellow!" << endl;
        break;
    case 2: //haste
        subject.speed += effect.potency;
        cout << subject.name << " is glowing " << ((effect.potency>3)?((effect.potency>6)?"intensely":"bright"):"faintly") << " green!" << endl;
        break;
    case 3: //focus
        subject.intel += effect.potency;
        cout << subject.name << " is glowing " << ((effect.potency>3)?((effect.potency>6)?"intensely":"bright"):"faintly") << " blue!" << endl;
        break;
    case 4: //bleed
        subject.Damage(effect.potency,"HP",false);
        cout << subject.name << " took " << effect.potency << " bleed damage!" << endl;
        break;
    case 5: //burn
        subject.Damage(effect.potency,"HP",false);
        cout << subject.name << " took " << effect.potency << " burn damage!" << endl;
        break;
    case 6: //regeneration
        subject.Heal(effect.potency);
        break;
    case 7: //barrier
        subject.Heal(effect.potency,"AC");
        break;
    case 8: //injured
        cout << subject.name << " is " << ((effect.potency>3)?((effect.potency>6)?"critically":"heavily"):"slightly") << " injured!";
        break;
    default: //unknown effect
        break;
    }
    if(effect.count!=-1){ //-1 count == infinite duration usually for equipment
        effect.count--;
        if(effect.count == 0){
            for(int i = 0; i < effect.parent->size(); i++){
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

StatusEffect checkFor(GameActor &subject, int EffectID){
    //STATUS
    if(subject.status.size()!=0){
        for(int i = 0; i < subject.status.size(); i++){
            if(subject.status.at(i).ID == EffectID){
                return subject.status.at(i);
            }
        }
    }
    //EQUIPS
    if(subject.equips.head.bonuses.size()!=0){
        for(int i = 0; i < subject.equips.head.bonuses.size(); i++){
            if(subject.equips.head.bonuses.at(i).ID == EffectID){
                return subject.equips.head.bonuses.at(i);
            }
        }
    }
    if(subject.equips.body.bonuses.size()!=0){
        for(int i = 0; i < subject.equips.body.bonuses.size(); i++){
            if(subject.equips.body.bonuses.at(i).ID == EffectID){
                return subject.equips.body.bonuses.at(i);
            }
        }
    }
    if(subject.equips.legs.bonuses.size()!=0){
        for(int i = 0; i < subject.equips.legs.bonuses.size(); i++){
            if(subject.equips.legs.bonuses.at(i).ID == EffectID){
                return subject.equips.legs.bonuses.at(i);
            }
        }
    }
    if(subject.equips.weapon.bonuses.size()!=0){
        for(int i = 0; i < subject.equips.weapon.bonuses.size(); i++){
            if(subject.equips.weapon.bonuses.at(i).ID == EffectID){
                return subject.equips.weapon.bonuses.at(i);
            }
        }
    }
    if(subject.equips.offhand.bonuses.size()!=0){
        for(int i = 0; i < subject.equips.offhand.bonuses.size(); i++){
            if(subject.equips.offhand.bonuses.at(i).ID == EffectID){
                return subject.equips.offhand.bonuses.at(i);
            }
        }
    }
    //ACCESSORIES
    for(int j = 0; j < subject.equips.accessories.size(); j++){
        for(int i = 0; i < subject.equips.accessories.at(j).bonuses.size(); i++){
            if(subject.equips.accessories.at(j).bonuses.at(i).ID == EffectID){
                return subject.equips.accessories.at(j).bonuses.at(i);
            }
        }
    }
    return newEffect("NULL");
}

vector<StatusEffect> allOf(GameActor &subject, string check){
    vector<StatusEffect> EffectPool;
    //STATUS
    if(subject.status.size()!=0){
        for(int i = 0; i < subject.status.size(); i++){
            if(Reference(check,subject.status.at(i).procs)!=-1 || check == ""){
                EffectPool.push_back(subject.status.at(i));
            }
        }
    }
    //EQUIPS
    if(subject.equips.head.bonuses.size()!=0){
        for(int i = 0; i < subject.equips.head.bonuses.size(); i++){
            if(Reference(check,subject.equips.head.bonuses.at(i).procs)!=-1 || check == ""){
                EffectPool.push_back(subject.equips.head.bonuses.at(i));
            }
        }
    }
    if(subject.equips.body.bonuses.size()!=0){
        for(int i = 0; i < subject.equips.body.bonuses.size(); i++){
            if(Reference(check,subject.equips.body.bonuses.at(i).procs)!=-1 || check == ""){
                EffectPool.push_back(subject.equips.body.bonuses.at(i));
            }
        }
    }
    if(subject.equips.legs.bonuses.size()!=0){
        for(int i = 0; i < subject.equips.legs.bonuses.size(); i++){
            if(Reference(check,subject.equips.legs.bonuses.at(i).procs)!=-1 || check == ""){
                EffectPool.push_back(subject.equips.legs.bonuses.at(i));
            }
        }
    }
    if(subject.equips.weapon.bonuses.size()!=0){
        for(int i = 0; i < subject.equips.weapon.bonuses.size(); i++){
            if(Reference(check,subject.equips.weapon.bonuses.at(i).procs)!=-1 || check == ""){
                EffectPool.push_back(subject.equips.weapon.bonuses.at(i));
            }
        }
    }
    if(subject.equips.offhand.bonuses.size()!=0){
        for(int i = 0; i < subject.equips.offhand.bonuses.size(); i++){
            if(Reference(check,subject.equips.offhand.bonuses.at(i).procs)!=-1 || check == ""){
                EffectPool.push_back(subject.equips.offhand.bonuses.at(i));
            }
        }
    }
    //ACCESSORIES
    for(int j = 0; j < subject.equips.accessories.size(); j++){
        for(int i = 0; i < subject.equips.accessories.at(j).bonuses.size(); i++){
            if(Reference(check,subject.equips.accessories.at(j).bonuses.at(i).procs)!=-1 || check == ""){
                EffectPool.push_back(subject.equips.accessories.at(j).bonuses.at(i));
            }
        }
    }

    return EffectPool;
}

void Attack(GameActor &attacker, GameActor &target, Skill attack, string result){
    //result can be "Hit", "Blocked", or "Dodged"
    string targetResult = (result == "Hit")?"getHit":(result == "Blocked")?"onBlock":"onDodge";
    
    checkAll(attacker,result);
    checkAll(target,targetResult);

    if(target.dead)checkAll(attacker,"onKill");
}

void turnStart(GameActor &subject){
    cout << "It's " << subject.name << "'s turn!" << endl;
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
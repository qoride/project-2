#ifndef STRUCT_HPP_
#define STRUCT_HPP_

#include <vector>;
#include <string>;

const std::vector<std::string> damageTypes = {"HP","MP","AC"};

int Reference(std::string s, std::vector<std::string> l){ //returns the index of a string from the given vector
    for(int i = 0; i < l.size(); i++){
        if(s == l.at(i))return i;
    }
    return -1;
}

//STRUCTS
struct CharClass{
    std::string className;
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
    std::vector<std::string> procs;
    std::vector<StatusEffect>* parent;

    bool operator ==(StatusEffect comp){
        return(ID==comp.ID && potency==comp.potency && procs==comp.procs && parent==comp.parent);
    }
};

struct Item{
    std::string type;
    Checks requirements;
    std::vector<StatusEffect> bonuses;
};

struct Equipment{
    Item head;
    Item body;
    Item legs;
    Item weapon;
    Item offhand;
    std::vector<Item> accessories;
};

struct Skill{
    std::string name;
    int min;
    int max;
    int cost;
    std::string scalingType;
    std::string damageType;
    std::string targetType;
    std::vector<StatusEffect> effects;
};

struct GameActor{
    std::string name;
    
    //max values
    float maxHP;
    float maxMP;

    //base stats
    float statATK;
    float statDEF;
    float statSPD;
    float statINT;

    //properties
    bool dead;
    float health;
    float shield;
    float mana;

    int gold;
    int level;
    int experience;
    CharClass path;

    float attack;
    float defense;
    float speed;
    float intel;
    std::vector<StatusEffect> status;
    std::vector<Item> inventory;
    Equipment equips;
    std::vector<Skill> skills;

    void Damage(float dmg = 0, std::string damageType = "HP"){
        switch(Reference(damageType,damageTypes)){
            case 0: //Health (HP)
                if(shield > 0)shield -= dmg;
                if(shield < 0){
                    dmg += shield;
                    shield = 0;
                }
                health -= (defense < dmg)?dmg-defense:0;
                break;
            case 1: //Mana (MP)
                mana -= dmg;
                break;
            case 2: //Shield (AC)
                shield -= dmg;
                if(shield < 0)shield = 0;
                break;
            default: //Unknown
                break;
        }
    }

    void Heal(float amt = 0, std::string damageType = "HP"){
        switch(Reference(damageType,damageTypes)){
            case 0: //Health (HP)
                health += amt;
                if(health > maxHP)health = maxHP;
                break;
            case 1: //Mana (MP)
                mana += amt;
                if(mana > maxMP)mana = maxMP;
                break;
            case 2: //Shield (AC)
                shield += amt;
                if(shield > maxHP)shield = maxHP;
                break;
            default: //Unknown
                break;
        }
    }

    void Start(){
        statATK = attack;
        statDEF = defense;
        statSPD = speed;
        statINT = intel;
    }
};

#endif /* STRUCT_HPP_ */
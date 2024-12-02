#ifndef STRUCT_HPP_
#define STRUCT_HPP_

#include <vector>
#include <string>
#include <map>

const std::vector<std::string> damageTypes = {"HP","MP","AC"};

int Reference(std::string s, std::vector<std::string> l){ //returns the index of a string from the given vector
    for(int i = 0; i < l.size(); i++){
        if(s == l.at(i))return i;
    }
    return -1;
}

enum Option{
    attackOption = 0,
    defendOption,
    evadeOption,
    skillOption,
    inventoryOption,
    inspectOption,
    cancelOption,
    yesOption,
    noOption,
    invalidOption,
};

const std::string OptionToString(Option option){
    switch(option){
        case attackOption:      return "attack";
        case defendOption:      return "block";
        case evadeOption:       return "dodge";
        case skillOption:       return "skill";
        case inspectOption:     return "info";
        case inventoryOption:   return "item";
        case cancelOption:      return "cancel";
        case yesOption:         return "Y";
        case noOption:          return "N";
        case invalidOption:     return "invalid";
        default:                return "unknown";
    }
}

const map<std::string, Option> menuMap = {
    {"attack"   ,   attackOption},
    {"block"    ,   defendOption},
    {"dodge"    ,   evadeOption},
    {"skill"    ,   skillOption},
    {"item"     ,   inventoryOption},
    {"info"     ,   inspectOption},
    {"cancel"   ,   cancelOption},
    {"Y"        ,   yesOption},
    {"N"        ,   noOption},
    {"invalid"  ,   invalidOption},
};

Option parseIn(std::string s){
    auto it = menuMap.find(s);
    return (it == menuMap.end())?invalidOption:it->second;
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
    std::string inspectTxt;

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

    void Damage(float dmg = 0, std::string damageType = "HP", bool narrate = true){
        switch(Reference(damageType,damageTypes)){
            case 0: //Health (HP)
                int final = dmg, blocked = 0;
                if(shield > 0){
                    final = dmg-shield;
                    if(final < 0)final = 0;
                    blocked = shield+final-dmg;
                    this->Damage(dmg,"AC",narrate);
                }

                if(final > 0){
                    final -= defense;
                    if(final > 0){
                        checkAll(*this,"onHPHit");
                        health -= final;
                        if(narrate)cout << name << " took " << final << " damage!" << endl;
                    }else{
                        checkAll(*this,"onInef");
                        if(narrate)cout << name << " took no damage!" << endl;
                    }
                }

                if(health <= 0){
                    checkAll(*this,"onDeath");
                    cout << name << " was defeated!" << endl;
                    health = 0;
                    dead = true;
                }
                break;
            case 1: //Mana (MP)
                checkAll(*this,"onMPHit");
                mana -= dmg;
                if(mana < 0){
                    mana = 0;
                    checkAll(*this,"onMPBreak");
                    if(narrate)cout << name << " is fatigued!" << endl;
                }
                if(narrate)cout << name << " lost " << dmg << " mana!" << endl;
                break;
            case 2: //Shield (AC)
                checkAll(*this,"onACHit");
                shield -= dmg;
                if(shield < 0){
                    shield = 0;
                    checkAll(*this,"onACBreak");
                    if(narrate)cout << name << "'s armor shattered!" << endl;
                }else{
                    if(narrate)cout << name << " lost " << dmg << " armor!" << endl;
                }
                break;
            default: //Unknown
                break;
        }
    }

    void Heal(float amt = 0, std::string damageType = "HP", bool narrate = true){
        checkAll(*this,"onHeal");
        switch(Reference(damageType,damageTypes)){
            case 0: //Health (HP)
                checkAll(*this,"onHPHeal");
                if(checkFor(*this,8).ID == 8){ //Check for Injured
                    if(narrate)cout << name << " could not heal!" << endl;
                    break;
                }
                health += amt;
                if(health > maxHP){
                    health = maxHP;
                    checkAll(*this,"onHPFull");
                    if(narrate)cout << name << " fully recovered!" << endl;
                }else{
                    if(narrate)cout << name << " healed " << amt << " health!" << endl;
                }
                break;
            case 1: //Mana (MP)
                checkAll(*this,"onMPHeal");
                mana += amt;
                if(mana > maxMP){
                    mana = maxMP;
                    checkAll(*this,"onMPFull");
                    if(narrate)cout << name << " fully restored mana!" << endl;
                }else{
                    if(narrate)cout << name << " restored " << amt << " mana!" << endl;
                }
                break;
            case 2: //Shield (AC)
                checkAll(*this,"onACHeal");
                shield += amt;
                if(shield > maxHP){
                    shield = maxHP;
                    checkAll(*this,"onACFull");
                    if(narrate)cout << name << "'s armor is fully repaired!" << endl;
                }else{
                    if(narrate)cout << name << " repaired " << amt << " armor!" << endl;
                }
                break;
            default: //Unknown
                break;
        }
    }

    void Start(){
        attack = statATK;
        defense = statDEF;
        speed = statSPD;
        intel = statINT;
    }

    ostream& operator <<(std::ostream& out){
        out << "LVL." << level << " " << name << endl
        << "HP: " << health << ((shield > 0)?"(+"+to_string(shield)+")/":"/") << maxHP << endl
        << "MP: " << mana << "/" << maxMP << endl;
        for(StatusEffect s : allOf(*this)){
            cout << s.inspectTxt << endl;
        }
    }
};

#endif
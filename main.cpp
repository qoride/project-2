#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <limits>

using namespace std;
const string symMap[] = {"  ","░░","▒▒","▓▓","██"}; //ASCII reference map

struct gameActor{

};

//PROTOTYPES
int Reference(string s, string l = "pattern");
void clearStream();

vector<tileSpace> customs;

int main(){
    
    return 0;
}

//DEFINITIONS
void clearStream(){ //reject the rest of the inputs
    cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    cin.clear();
}

int Reference(string s, string l){ //returns the index of a string from the given array
    int size = (l == "operator")?5:patterns.size();
    for(int i = 0; i < size; i++){
        if(l == "operator"){
            if(s == operators[i])return i;
        }else{
            if(s == patterns.at(i))return i;
        }
    }
    return -1;
}

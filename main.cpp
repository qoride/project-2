#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <limits>

using namespace std;
const string symMap[] = {"  ","░░","▒▒","▓▓","██"}; //ASCII reference map

struct gameActor{
    //MAX values

};

//PROTOTYPES
int Reference(string s, string l);
void clearStream();


int main(){
    
    return 0;
}

//DEFINITIONS
void clearStream(){ //reject the rest of the inputs
    cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    cin.clear();
}

int Reference(string s, string l){ //returns the index of a string from the given array
    return -1;
}

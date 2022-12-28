#include <stdio.h>
#include <furi.h>
#include <gui/gui.h>
#include "furi_hal_random.h"
#include <input/input.h>
#include <gui/elements.h>
#include "scrambler.h"

//6 moves along with direction
char moves[7]= {"RUFBLD"};
char dir[4]={" 2'"};
//Scramble length
const int SLEN=10;

//Structure which holds main scramble
struct GetScramble{
	char mainScramble [25][3];
};struct GetScramble a;//Its object

//Function prototypes to avoid bugs
void scrambleReplace ();
void genScramble ();
void valid ();
int getRand (int upr,int lwr);
char *printData ();
void writeToFile ();

//Main function
/* int main(){
	genScramble ();//Calling genScramble
	scrambleReplace();//Calling scrambleReplace
	valid();//Calling valid to validate the scramble
	printData ();//Printing the final scramble
	//writeToFile();//If you want to write to a file, please uncomment this
	
	return 0;
} */

void genScramble (){
	//Stage 1
	for(int i=0; i<SLEN; i++) {
		strcpy(a.mainScramble[i],"00");
	}
	//This makes array like this 00 00 00.......
}

void scrambleReplace (){
	//Stage 2
	//Actual process begins here
	//Random ints are generated and respective values are fed inside the array
	for (int i = 0; i<SLEN; i++){
		a.mainScramble[i][0] = moves[getRand (5,0)];
		a.mainScramble[i][1] = dir[getRand (2,0)];
	}
	//But scramble is still isn't correct due to repeating moves
}

void valid (){
	//Stage 3
	//Variables for loop
	int loopOne, loopTwo;
	
	//This will actually start to make the scramble usable
	//It will remove stuff like R R F L, etc.
	for (loopOne=1;loopOne<SLEN;loopOne++){
		while (a.mainScramble[loopOne][0] == a.mainScramble[loopOne-1][0]){
			a.mainScramble[loopOne][0]=moves[getRand(5,0)];
		}
	}
	
	//This will further check it and remove stuff like R L R
	for (loopTwo=2; loopTwo<SLEN; loopTwo++){
		while ((a.mainScramble[loopTwo][0] == a.mainScramble[loopTwo-2][0]) || (a.mainScramble[loopTwo][0]) == a.mainScramble[loopTwo-1][0]){
			a.mainScramble[loopTwo][0]=moves[getRand(5,0)];
		}
	}
	//Scramble generation complete
}

int getRand(int upr, int lwr){
	int randNum;
	randNum=(rand() % (upr - lwr + 1)) + lwr;
	return randNum;
}

char *printData () {
    char *result = malloc(100);
    int offset = 0;
    for (int loop = 0; loop < SLEN; loop++) {
        offset += snprintf(result + offset, 100 - offset, "%s ", a.mainScramble[loop]);
    }
    return result;
}


//Marcela Echavarria

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define START_ROOM 1
#define MID_ROOM 2
#define END_ROOM 3

//room struct
struct aRoom {
    char roomName [9];
    int roomType;
    int roomOutbound [6];
    int nextOutbound;
};

//struct pointer
struct aRoom *theRoom [7];

//buffer for directory name
char dirName [256];

//character names
char * roomNames [10] = {
    "Arya",
    "Jon",
    "Daenerys",
    "Cersei",
    "Sansa",
    "Tyrion",
    "Viserion",
    "Drogon",
    "Rhaegal",
    "Ghost"
};


int randomRoomNumber(){
    return rand()%7;
}

//make sure there are enough connections
int everyOutboundHasEnough(){
    int rtn = 1;
    for (int i =0;i < 7; i++){
        if (theRoom[i]->nextOutbound < 3){
            return 0;
        }
    }
    return rtn;
}

//too many connection
int isOutboundFull(int roomNumber){
    return theRoom[roomNumber]->nextOutbound >= 6;
}


//duplicate roome name
int isDuplicate(int roomIDX, int outBound){
    for(int i=0; i < theRoom[roomIDX]->nextOutbound;i++){
        if(theRoom[roomIDX]->roomOutbound[i] == outBound){
            return 1;
        }
    }
    return 0;
}

//create connections
int makeConnection(int room1, int room2){
    #ifdef DEBUG
    fprintf(stderr, "makeConnection(%d,%d)\n",room1,room2);
    #endif
    if (room1==room2){
        return 0;
    }
    if (isDuplicate(room1, room2)){
        return 0;
    }
    if (isOutboundFull(room1) || isOutboundFull(room2)){
        return 0;

    }
    theRoom[room1]->roomOutbound[theRoom[room1]->nextOutbound++] = room2;
    theRoom[room2]->roomOutbound[theRoom[room2]->nextOutbound++] = room1;
    return 1;
}


//
int isNameUsed(char * name){
    int rtn = 0;
    for(int i = 0; i<7;i++){
        if (strcmp(theRoom[i]->roomName, name)==0){
            rtn = 1;
            break;
        }
    }
    return rtn;
}

//randomize room
char * getRoomName(){
    int index;
    do {
        index = rand()%10;
    } while (isNameUsed(roomNames[index]));
    return roomNames[index];

}

//build rooms and assign end and start
void makeRooms(){
    for (int i = 0; i<7; i++) {
        if ((theRoom[i]=malloc(sizeof(struct aRoom))) == NULL){
            fprintf(stderr, "Error Malloc Failed!\n");
            exit(-1);
        }
        theRoom[i]->roomName[0] = '\0';
        theRoom[i]->nextOutbound = 0;
    }
    for (int i = 0; i<7; i++) {
        strcpy(theRoom[i]->roomName,getRoomName());
        theRoom[i]->roomType = MID_ROOM;
    }
        theRoom[0]->roomType = START_ROOM;
        theRoom[6]->roomType = END_ROOM;

    do{
        makeConnection(randomRoomNumber(),randomRoomNumber());
    } while (! everyOutboundHasEnough());
}

//print room syntax
void printRooms(){
    FILE * fd;
    for (int i = 0; i<7; i++){
        if ((fd=fopen(theRoom[i]->roomName, "w"))==NULL){
            fprintf(stderr, "Cannot Open File\n");
            exit(-1);
        }
        fprintf(fd, "ROOM NAME: %s\n", theRoom[i]->roomName);
        for (int j=0; j<theRoom[i]->nextOutbound; j++){
            fprintf(fd, "CONNECTION %d: %s\n",
            j+1,theRoom[theRoom[i]->roomOutbound[j]]->roomName);
        }
        if (theRoom[i]->roomType==START_ROOM){
            fprintf(fd, "ROOM TYPE: START_ROOM\n");

        } else if(theRoom[i]->roomType==END_ROOM){
            fprintf(fd, "ROOM TYPE: END_ROOM\n");
        } else {
            fprintf(fd, "ROOM TYPE: MID_ROOM\n");
        }

    }
}

//executables
int main() {
    srand(time(0)); // to do: random seed
    printf ("step 1 creating directory\n");
    sprintf(dirName,"echavarm.rooms.%d", getpid());
    printf("directory is: %s\n", dirName);
    mkdir(dirName, 0700);
    makeRooms();
    chdir(dirName);
    printRooms();
}

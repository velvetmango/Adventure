//Marcela Echavarria

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <pthread.h>

//globals
#define START_ROOM 1
#define MID_ROOM 2
#define END_ROOM 3

int path [1024];
int pathIndex = 0;

//room struct
struct aRoom {
    char roomName [9];
    int roomType;
    int roomOutbound [6];
    int nextOutbound;
};

//allocate room
struct aRoom *theRoom [7];
int nextRoom = 0;

//time function
char * printTime(){
    time_t t;
    t = time(NULL);
    return ctime(&t);
}
pthread_mutex_t lock;

//mutex
void * timeThread(void * args){
    pthread_mutex_lock(&lock);
    printf("\n\n%s\n\n",printTime());
    pthread_mutex_unlock(&lock);
    return NULL;
}


void addRoomToPath(int roomIndex){
    path[pathIndex++] = roomIndex;

}

void printPath() {
    for(int i = 0; i < pathIndex; i++){
        printf("%s\n", theRoom[path[i]]->roomName);
    }
    printf("It took you %d steps \n", pathIndex);
}

//calling roomFiles
void createRoom(char * fileName){
    //printf("creating room fileName: %s\n",fileName);
theRoom[nextRoom] = malloc(sizeof(struct aRoom));
strcpy(theRoom[nextRoom]->roomName, fileName);
nextRoom++;
}

//roome name index
int roomIndex(char * name) {
    for(int i=0; i<7; i++){
        //printf("name:%s ROOM NAME: %s\n", name, theRoom[i]->roomName);
        if (strstr(name, theRoom[i]->roomName) != 0) {
            return i;
        }
    }
    return -1;
}

char * strp(char* s){
    char * x = strchr(s,'\n');
    * x = '\0';
    return s;
}

//loads one room at a time
void loadRoom(char* fileName){
    int index = roomIndex(fileName);
    int next;
    FILE * f;
    char fileBuffer[128];
    //printf("loading room name %s\n", fileName);
    //check for readibility
    if ((f = fopen(fileName, "r")) == NULL){
        fprintf(stderr, "Unable to open file: %s \n", fileName);
        return;

    }
    //read each line of room file
while(fgets(fileBuffer, 128, f)!= NULL){
    //printf("%s\n", fileBuffer);
    if(strstr(fileBuffer, "ROOM NAME:") != 0 ){
        continue;
    }

    if(strstr(fileBuffer, "CONNECTION ") != 0 ){
        //printf("Connecting Room%s\n", fileBuffer+13);
        next = theRoom[index]->nextOutbound;
        theRoom[index]-> roomOutbound[next] = roomIndex(strp(fileBuffer+13));
        theRoom[index]->nextOutbound++;
    }
    //assign room type
    if(strstr(fileBuffer, "ROOM TYPE:") != 0 ){
        if(strstr(fileBuffer, "START_ROOM") != 0 ){
            theRoom[index]->roomType = START_ROOM;
        }
            if(strstr(fileBuffer, "END_ROOM") != 0 ){
            theRoom[index]->roomType = END_ROOM;
        }
            if(strstr(fileBuffer, "MID_ROOM") != 0 ){
            theRoom[index]->roomType = MID_ROOM;
            }

        }
    }
}

void loadRooms(char* dir) {
    struct dirent *de;
    DIR *dr;
    chdir(dir); //change directory to room directory
    if ((dr = opendir(".")) == NULL)  // if opendir returns NULL couldn't open directory
    {
        fprintf(stderr, "Could not open current directory\n" );
        return;
    }
    //iterating through each directory
    while ((de = readdir(dr)) != NULL){

        //dont worry about . or .. directory
        if((strcmp(".", de->d_name) == 0) ||
            (strcmp("..", de->d_name) == 0)){
            continue;
        }
        //room created as loop iterates
        createRoom(de->d_name);
    }

    closedir(dr);

    if ((dr = opendir(".")) == NULL)  // if opendir returns NULL couldn't open directory
    {
        fprintf(stderr, "Could not open current directory\n" );
        return;
    }
    //iterating through each directory
    while ((de = readdir(dr)) != NULL){

        //dont worry about . or .. directory
        if((strcmp(".", de->d_name) == 0) ||
            (strcmp("..", de->d_name) == 0)){
            continue;
        }
        //room name called as loop iterates
        loadRoom(de->d_name);
    }

    closedir(dr);
}
//most recent game directory
char * recentDirectory(){
    struct dirent *de;  // Pointer for directory entry
    struct stat st; //allocate space for stat
    // opendir() returns a pointer of DIR type.
    DIR *dr;
    char * newestFileName;
    time_t newestModTime = 0;

    if ((dr = opendir(".")) == NULL)  // opendir returns NULL if couldn't open directory
    {
        fprintf(stderr, "Could not open current directory\n" );
        return 0;
    }

    // Refer http://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html
    // for readdir()
    while ((de = readdir(dr)) != NULL){
            //compare string return null if it doesnt find string
        if (strstr(de->d_name, "echavarm.rooms.")!=NULL){
            #ifdef DEBUG
            printf("%s\n", de->d_name); //prints directory name
                //address of stat stucture
            #endif
            if  ((stat(de->d_name, &st))!=0){
                fprintf(stderr, "Unable to stat file\n");
                return 0;
                }
                //checks to see if mod time is later than last saved mod time
                if (st.st_mtime > newestModTime){
                    newestFileName = de->d_name;
                    newestModTime = st.st_mtime;
                }
                #ifdef DEBUG
                printf("%s: %ld\n", de->d_name, st.st_mtime);
                #endif
            }
    }
    closedir(dr);
    return newestFileName;

}
int findStartRoom(){
    for(int i=0; i<7; i++){
        if (theRoom[i]->roomType == START_ROOM){
            return i;
        }
    }
    return -1;
}

//game function
void play(){
    char input[128];
    int currentRoom = findStartRoom();
    int nextRoom;
    while(theRoom[currentRoom]->roomType != END_ROOM){
        printf("CURRENT LOCATION: %s\n", theRoom[currentRoom]->roomName);
        printf("POSSIBLE CONNECTIONS: ");
        for(int i=0; i<theRoom[currentRoom]->nextOutbound;i++){
            printf("%s",theRoom[theRoom[currentRoom]->roomOutbound[i]]->roomName); //roomindex
            if (theRoom[currentRoom]->nextOutbound-1 !=i)
                printf(",");
            //printf("%d,",theRoom[currentRoom]->roomOutbound[i]);
        }
        printf(".\n");
        printf("WHERE TO? >");
        fgets(input,128, stdin);
        if (strstr(input,"time") != 0 ){
            //printf("%s\n", printTime());
            pthread_mutex_unlock(&lock);
            continue;
        }
        nextRoom = roomIndex(strp(input));
        if (nextRoom == -1){
            printf("HUH?! I DONT UNDERSTAND THAT ROOM. TRY AGAIN.\n");
            continue;
        }
        currentRoom = nextRoom;
        addRoomToPath(currentRoom);
    }
    printf("YOU MADE IT\n");
    printPath();

}

//executables
int main(){
    char * dir;
    int err;
    //multithreading and lock
    pthread_t tid;
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init has failed\n");
        return 1;
    }
    if ((err = pthread_create(&tid, NULL, &timeThread, NULL)) != 0) {
        printf("thread cant be created\n");
        return 1;

    }

    pthread_mutex_lock(&lock);

    dir = recentDirectory();
    loadRooms(dir);
    play();
    #ifdef DEBUG
    printf("newestDirectory: %s \n",recentDirectory());
    #endif
}

to compile: gcc –o echavarm.adventure echavarm.adventure.c -lpthread 
gcc –o echavarm.buildrooms echavarm.buildrooms.c

This assignment is split up into two C programs (no other languages is allowed). The first program (hereafter called the "rooms program") will be contained in a file named "<STUDENT ONID USERNAME>.buildrooms.c", which when compiled with the same name (minus the extension) and run creates a series of files that hold descriptions of the in-game rooms and how the rooms are connected.

The second program (hereafter called the "game") will be called "<STUDENT ONID USERNAME>.adventure.c" and when compiled with the same name (minus the extension) and run provides an interface for playing the game using the most recently generated rooms.

In the game, the player will begin in the "starting room" and will win the game automatically upon entering the "ending room", which causes the game to exit, displaying the path taken by the player.

During the game, the player can also enter a command that returns the current time - this functionality utilizes mutexes and multithreading.

For this assignment, do not use the C99 standard: this should be done using raw C (which is C89). In the complete example and grading instructions below, note the absence of the -c99 compilation flag.

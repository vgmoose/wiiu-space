## Space Game
This is a Wii U homebrew application that is currently in progress. You can play the latest version for 5.5.0 by clicking [here](http://wiiu.vgmoose.com/sg550.mp4). At the moment, the ship can be moved around with the d-pad, but not much else.

Space game is an attempt to create a graphical shooter game on the Wii U. It is currently based off the [libwiiu pong example](https://github.com/wiiudev/libwiiu/tree/master/osscreenexamples/pong)

### Bitmaps
This code leverages a [helper python script](https://gist.github.com/vgmoose/1a6810aacc46c28344ab) that converts a bitmap to a C char array. The bitmap is then drawn via modifications to the draw.c library.

### Todo
Currently the game only displays a single space ship image while the 1P's paddle is moving.

 - Move the spaceship with the analog stick
 - Rotate the spaceship bitmap in the direction its facing
 - Allowing firing by tapping on the screen at the target
 - Some kind of enemies
 - Starry night sky background

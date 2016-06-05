## Space Game
This is a Wii U homebrew game. You can run the [latest release](https://gbatemp.net/threads/release-space-game.414342/) via Homebrew Launcher. To see the original, non-HBL version (libwiiu only) of the code, see the [master branch](https://github.com/vgmoose/space/tree/master). Watch a video of the gameplay in action [here](https://www.youtube.com/watch?v=KMuicPmOIHw)!

![Logo](http://vgmoose.com/posts/24261201%20-%20[release]%20Space%20Game!%20(for%20Wii%20U).post/title.png)

Space game is a graphical shooter game on the Wii U! Since it was originally designed to be executed via the .mp4 exploit in 5.5.x, there were several challenges that were imposed on its development in terms of efficiency and storage.

### Binary Size Tricks
A main issue with the webkit exploit is that binaries that can be executed in the browser are capped at a certain file size. I hit issues when my binary (code550.bin) exceeded 21,400 bytes. It may not seem like it, but that's not much! I employed a couple of tricks to keep the binary size small:

#### Compiling with -O1 as a CFLAG
If you look at my Makefile and compare it to other ones for the libwiiu examples, you'll notice a few differences near the top of the file. The most important of these differences is the addition of the -O1 parameter to the CFLAGS variable. This sets the compiler's [optimization level](http://www.rapidtables.com/code/linux/gcc/gcc-o.htm) and can usually shave off up to 6,000 bytes! 

There was one big issue though: I found that, whenever I compiled with -O1, very strangely, calling OSScreenFlipBuffersEx(0) in draw.c caused a crash. This didn't happen with OSScreenFlipBuffersEx(1)! I'm still not sure why this happens. I think, when compiled with -O1, the first screen buffer does not get properly initialized. Since this happens in loader.c, I moved the Makefile around to compile every file **except loader.c** with -O1. This way, it's all still linked together into one mostly compact binary and won't crash.

**Note**: I also compiled the [https://github.com/wiiudev/libwiiu](https://github.com/wiiudev/libwiiu) repo with the -O1 flag.

#### Compressing Bitmaps
A fair amount of the filesize, even after compression, is due to the bitmap image storing that is employed. Images are stored directly in images.c, with the assistance of [this script](https://gist.github.com/vgmoose/1a6810aacc46c28344ab) that converts a bitmap to a compressed C char array. The bitmap is then drawn via modifications to the draw.c library. It does not use GX2 at this time.

My compresion algorithm is a little complicated, but it is detailed at the top of images.c. It tries to store information about streaks of pixels as efficiently as possible. The way I did it only allows for 125 total colors in a palette. The algorithm can also be used to compress arrays, the trick is storing sequences of similar numbers as instructions, such as: {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1} becomes {8, 1, 3, 0, 10, 1} (eight ones, three zeroes, ten ones).

### Speedup Tricks
Drawing was a pain here, but I found I was able to greatly increase my drawing speed by moving all of the coreinit.rpl function pointers into a struct and passing that around to the drawing library. This is possible because the pointers do not change, so looking them up every time a pixel is drawn is, as far as I can tell, just extra time that could be spent drawing!

I also had to wrap my head around the concept of using flipBuffers. From what I gather, calling "flipBuffers" acts as a sort of "commit" for everything you've previously drawn to appear on screne. Then the next calls you make to draw will be put into the next upcoming buffer that you then flip to again. This is done so that a seamless gameplay can be presented without flickering, despite the entire screen being redrawn every frame.

![In game](http://vgmoose.com/posts/24261201%20-%20[release]%20Space%20Game!%20(for%20Wii%20U).post/gameplay.png)


### Issues
If you have any issues with the code here when you try to use it in your own app, feel free to contact me!

If you have any filesize issues when trying to run this app, I suggest you try commenting out the giant logo byte array in images.c.

### Misc Math

#### Trig functions
A big issue I had using this program was with not using the standard math library. I was not able to get it working without the file size shooting way up. As such, I implemented estimations for sin, cos, and arctan in math.h. These are primarly used to calculate angles and spin the spaceship. I found myself drawing many right traingles on paper and reciting SOHCAHTOA over and over again before I finally managed to get it right!

#### (Pseudo-)Random numbers
This was another area that I struggled with. Fortunately, there's a method to get the current time, so that can be used as a seed. The implementation I ended up going with is based on an older version of [glibc's rand()](http://stackoverflow.com/questions/1026327/what-common-algorithms-are-used-for-cs-rand).

#### Matrix multiplication
In order to rotate the bitmap to the angle calculated by the trig functions, I had to use a [rotation matrix](https://en.wikipedia.org/wiki/Rotation_matrix). This is actually a lot easier than it looks once you have the algorithm to multiply two matrices. What did stump me for a while is that the matrix has to first be translated up and left by half its height and width before rotating, since the rotation happens around the top left point. By translating it, you put the center in the top left, which allows for the rotation to take place around the center. This is then rotated back.

I also make use of a scaling matrix for explosions, which similarly gets multiplied by the bitmap when it's time to blow up

### Creating bitmaps
I created my bitmaps in Photoshop using Mode -> Index color. I extracted the palettes manually by using a hex editor. I've provided the three bitmaps that I used in this repo, although the actual image files are not used by the game. There are other ways to create bitmaps like this (I believe older mspaint supports it)

### Credits and License
This program is licensed under [the MIT license](https://opensource.org/licenses/MIT), which grants anyone permission to do pretty much whatever they want as long as the copyright notice stays intact.
 - Programmed by [VGMoose](http://vgmoose.com)
 - Based on Pong by [Relys](https://github.com/Relys)
 - Music by [(T-T)b](https://t-tb.bandcamp.com/)
 - Space ship sprite by [Gungriffon Geona](http://shmups.system11.org/viewtopic.php?p=421436&sid=c7c9dc0b51eb40aa10bd77f724f45bb1#p421436)
 - Logo font by [Iconian Fonts](http://www.dafont.com/ozda.font) 	
 - libwiiu/bugfixes: [MarioNumber1](https://github.com/MarioNumber1), [CreeperMario](https://github.com/CreeperMario),  [brienj](https://github.com/xhp-creations), [NWPlayer123](https://github.com/NWPlayer123), [dimok](https://github.com/dimok789)
 
### How to Compile and Run
See [building the Homebrew Launcher](https://github.com/dimok789/homebrew_launcher#building-the-homebrew-launcher) 	

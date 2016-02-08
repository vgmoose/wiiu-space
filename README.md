## Space Game
This is a Wii U homebrew application. Due to an exploit on the latest Wii U firmware, you can play latest version on 5.5.0/5.5.1 by clicking [here](http://wiiu.vgmoose.com/sg550.mp4).

Space game is an attempt to create a graphical shooter game on the Wii U! Since it is designed to be executed via the .mp4 exploit in 5.5.x, there were several challenges that were imposed on its development in terms of efficiency and storage.

### Binary Size Tricks
A main issue with the webkit exploit is that binaries that can be executed in the browser are capped at a certain file size. I hit issues when my binary (code550.bin) exceeded 21,400 bytes. It may not seem like it, but that's not much! I employed a couple of tricks to keep the binary size small:

#### Compiling with -O2 as a CFLAG
If you look at my Makefile and compare it to other ones for the libwiiu examples, you'll notice a few differences near the top of the file. The most important of these differences is the addition of the -O2 parameter to the CFLAGS variable. This sets the compiler's [optimization level](http://www.rapidtables.com/code/linux/gcc/gcc-o.htm) and can usually shave off up to 6,000 bytes! 

There was one big issue though: I found that, whenever I compiled with -O2, very strangely, calling OSScreenFlipBuffersEx(0) in draw.c caused a crash. This didn't happen with OSScreenFlipBuffersEx(1)! I'm still not sure why this happens. I think, when compiled with -O2, the first screen buffer does not get properly initialized. Since this happens in loader.c, I moved the Makefile around to compile every file except loader.c with -O2. This way, it's all still linked together into one mostly compact binary.

#### Compressing Bitmaps
A fair amount of the filesize, even after compression, is due to the bitmap image storing that is employed. Images are stored directly in images.c, with the assistance of [this hscript](https://gist.github.com/vgmoose/1a6810aacc46c28344ab) that converts a bitmap to a compressed C char array. The bitmap is then drawn via modifications to the draw.c library. It does not use GX2 at this time.

My compresion algorithm is a little complicated, but it is detailed at the top of images.c. It tries to store information about streaks of pixels as efficiently as possible. The way I did it only allows for 125 total colors in a palette.

### Speedup Tricks
Drawing was a pain here, but I found I was able to greatly increase my drawing speed by moving all of the coreinit.rpl function pointers into a struct and passing that around to the drawing library. This is possible because the pointers do not change, so looking them every time a pixel is drawn is, as far as I can tell, just extra time that could be spent drawing!

I also had to wrap my head around the concept of using flipBuffers. From what I gather, calling "flipBuffers" acts as a sort of "commit" for everything you've previously drawn to appear on screne. Then the next calls you make to draw will be put into the next upcoming buffer that you then flip to again. This is done so that a seamless gameplay can be presented without flickering, despite the entire screen being redrawn every frame.

### Misc Math

#### Trig functions
A big issue I had using this program was with not using the standard math library. I was not able to get it working without the file size shooting way up. As such, I implemented estimations for sin, cos, and arctan in math.h. These are primarly used to calculate angles and spin the spaceship. I found myself drawing many right traingles on paper and reciting SOHCAHTOA over and over again before I finally managed to get it right!

#### (Pseudo-)Random numbers
This was another area that I struggled with. Fortunately, there's a method to get the current time, so that can be used as a seed. The implementation I ended up going with is based on an older version of [glibc's rand()](http://stackoverflow.com/questions/1026327/what-common-algorithms-are-used-for-cs-rand).

#### Matrix multiplication
In order to rotate the bitmap to the angle calculated by the trig functions, I had to use a [rotation matrix](http://stackoverflow.com/questions/1026327/what-common-algorithms-are-used-for-cs-rand). This is actually a lot easier said than done once you have the algorithm to multiply two matrices, so this wasn't too bad. What did stump me for a while is that the matrix has to first be translated up and left by half its height and width before rotating, since the rotation happens around the top left point. By translating it, you put the center in the top left, which allows for the rotation to take place around the center. This is then rotated back.

### Creating bitmaps
I created my bitmaps in Photoshop using Mode -> Index color. I extracted the palettes manually by using a hex editor. I've provided the three bitmaps that I used in this repo, although the actual image files are not used by the game. There are other ways to create bitmaps like this (I believe older mspaint supports it)
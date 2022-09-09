## Space Game
Space game is a graphical shooter game on the Wii U! This is a homebrew application from 2016. To view a more recent PC port of it, see [this repo](https://github.com/vgmoose/space-game).

![Logo](http://vgmoose.com/posts/24261201%20-%20[release]%20Space%20Game!%20(for%20Wii%20U).post/title.png)

### Credits and License
This program is licensed under [the MIT license](https://opensource.org/licenses/MIT), which grants anyone permission to do pretty much whatever they want as long as the copyright notice stays intact.*
 - Programmed by [VGMoose](https://github.com/vgmoose), with features and bugfixes by [CreeperMario](https://github.com/CreeperMario), [brienj](https://github.com/xhp-creations), [Compucat](https://github.com/compucat)
 - Original codebase based on [libwiiu pong](https://github.com/wiiudev/libwiiu/tree/master/osscreenexamples/pong) by [Relys](https://github.com/Relys)
 - Music is [\~\*cruise\*\~](https://t-tb.bandcamp.com/track/cruise) by [(T-T)b](https://t-tb.bandcamp.com/)*
 - Space ship sprite by [Gungriffon Geona](http://shmups.system11.org/viewtopic.php?p=421436&sid=c7c9dc0b51eb40aa10bd77f724f45bb1#p421436)
 - Logo font by [Iconian Fonts](http://www.dafont.com/ozda.font) 
 - OSScreen-similar font by [M+ Fonts](http://mplus-fonts.osdn.jp/about-en2.html)
 - libwiiu/library: [MarioNumber1](https://github.com/MarioNumber1), [NWPlayer123](https://github.com/NWPlayer123), [dimok](https://github.com/dimok789), [Maschell](https://github.com/Maschell), [GaryOderNichts](https://github.com/GaryOderNichts), [QuarkTheAwesome](QuarkTheAwesome)

 *The song [\~\*cruise\*\~](https://t-tb.bandcamp.com/track/cruise) is available under [CC BY-NC-ND](https://ptesquad.bandcamp.com/album/pizza-planet-ep), and is excluded from the MIT licensing.

![In game](http://vgmoose.com/posts/24261201%20-%20[release]%20Space%20Game!%20(for%20Wii%20U).post/gameplay.png)

### Techniques and Info

Since it was originally designed to be executed via the .mp4 exploit in 5.5.x, there were several challenges that were imposed on its development in terms of efficiency and storage. Some workarounds to the constraints are detailed below.

#### Compressing Bitmaps
A fair amount of the filesize, even after compression, is due to the bitmap image storing that is employed. Images are stored directly in images.c, with the assistance of [this script](https://gist.github.com/vgmoose/1a6810aacc46c28344ab) that converts a bitmap to a compressed C char array. The bitmap is then drawn via modifications to the draw.c library. It does not use GX2 at this time.

My compresion algorithm is a little complicated, but it is detailed at the top of images.c. It tries to store information about streaks of pixels as efficiently as possible. The way I did it only allows for 125 total colors in a palette. The algorithm can also be used to compress arrays, the trick is storing sequences of similar numbers as instructions, such as: {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1} becomes {8, 1, 3, 0, 10, 1} (eight ones, three zeroes, ten ones).

#### Trig functions
A big issue I had using this program was with not using the standard math library. I was not able to get it working without the file size shooting way up. As such, I implemented estimations for sin, cos, and arctan in math.h. These are primarly used to calculate angles and spin the spaceship. I found myself drawing many right traingles on paper and reciting SOHCAHTOA over and over again before I finally managed to get it right!

#### (Pseudo-)Random numbers
This was another area that I struggled with. Fortunately, there's a method to get the current time, so that can be used as a seed. The implementation I ended up going with is based on an older version of [glibc's rand()](http://stackoverflow.com/questions/1026327/what-common-algorithms-are-used-for-cs-rand).

#### Matrix multiplication
In order to rotate the bitmap to the angle calculated by the trig functions, I had to use a [rotation matrix](https://en.wikipedia.org/wiki/Rotation_matrix). This is actually a lot easier than it looks once you have the algorithm to multiply two matrices. What did stump me for a while is that the matrix has to first be translated up and left by half its height and width before rotating, since the rotation happens around the top left point. By translating it, you put the center in the top left, which allows for the rotation to take place around the center. This is then rotated back.

I also make use of a scaling matrix for explosions, which similarly gets multiplied by the bitmap when it's time to blow up

### Creating bitmaps
I created my bitmaps in Photoshop using Mode -> Index color. I extracted the palettes manually by using a hex editor. I've provided the three bitmaps that I used in this repo, although the actual image files are not used by the game. There are other ways to create bitmaps like this (I believe older mspaint supports it)

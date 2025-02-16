Space Game was originally designed to be executed via the .mp4 exploit in 5.5.x, which was an environment with several challenges terms of efficiency and storage.

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

#include "trigmath.h"
#include "space.h"
///** Modified Math.h to include sin, cos, and tan **/
#define M_PI   3.14159265
#define M_2_PI 6.28318531
#define M_PI_2 1.57079632

float sincos(float x)
{
	if (x >  M_PI)
		x -= M_2_PI;
	
	// common code for both sin and cos
	return 1.27323954 * x + ((x<0)? 1:-1)*0.405284735 * x * x;
}

// This sine function is mostly accurate for -pi < x < pi
float sin(float x)
{
	if (x < -M_PI)
    	x += M_2_PI;
		
	//compute sine
	return sincos(x);

}

// This cosine function is mostly accurate for -pi < x < pi
float cos(float x)
{
	x += M_PI_2;
	
	return sincos(x);
}

// This tangent function is mostly accurate for -1 < x < 1. It uses the taylor series
float atan(float x)
{
    return x - (x*x*x)/3 + (x*x*x*x*x)/5;
}

// This absolute value function returns float, rather than the long provided in abs in math.h
float fabs(float value) {
    if (value < 0) {
        return -value;
    }   
    else {
        return value;
    }   
}

// Long story short, this function calls atan(y/x), but with some special sauce to allow it to
// work in every quadrant of the graph: https://en.wikipedia.org/wiki/Atan2
// It is used to get the angle given an <x, y> vector (such as joystick input)
float atan2(float y, float x)
{

    float offset = 0;

    if (fabs(y) > fabs(x))
    {   
        // swap x and y if  y is greater
        float temp = x;
        x = y;
        y = temp;

        // provide an offset for the other angle
        offset = (M_PI_2);
    }   

    float ans = 0;
	float m = y/x;

    if (x > 0)
        ans = atan(m);
    if (x < 0 && y >= 0)
        ans = atan(m) + M_PI;
    if (x < 0 && y < 0)
        ans = atan(m) + M_PI * ((offset != 0)? 1 : -1);
    if (x == 0 && y > 0)
        ans = M_PI_2;
    if (x == 0 && y < 0)
        ans = -M_PI_2;

    if (offset != 0)
        return offset - ans;
    else
        return ans;
}

int square(int x)
{
	return x*x;
}

// This is from here: http://stackoverflow.com/a/1026370/1871287
// it is initially seeded with the time
float prand (unsigned int *seed)
{
  unsigned int next = *seed;
  int result;
  next *= 1103515245;
  next += 12345;
  result = (unsigned int) (next / 65536) % 2048;

  next *= 1103515245;
  next += 12345;
  result <<= 10;
  result ^= (unsigned int) (next / 65536) % 1024;

  next *= 1103515245;
  next += 12345;
  result <<= 10;
  result ^= (unsigned int) (next / 65536) % 1024;

  *seed = next;

  return result / 2147483647.0;
}

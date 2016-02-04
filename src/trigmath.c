#include "trigmath.h"
#include "space.h"
/** Modified Math.h to include sin, cos, and tan **/
#define M_PI   3.141592654

float sin(float x)
{
	if (x < -3.14159265)
    	x += 6.28318531;
	else if (x >  3.14159265)
		x -= 6.28318531;
		
	//compute sine
	if (x < 0)
		return 1.27323954 * x + .405284735 * x * x;
	else
		return 1.27323954 * x - 0.405284735 * x * x;
}

float cos(float x)
{
	x += 1.57079632;
	if (x >  3.14159265)
		x -= 6.28318531;

	if (x < 0)
		return 1.27323954 * x + 0.405284735 * x * x;
	else
		return 1.27323954 * x - 0.405284735 * x * x;
}

double atan(double x)
{
    //printf("%f\n", x);
    return x - (x*x*x)/3 + (x*x*x*x*x)/5 - (x*x*x*x*x*x*x)/7 + (x*x*x*x*x*x*x*x*x)/9;
}

float fabs(float value) {
    if (value < 0) {
        return -value;
    }   
    else {
        return value;
    }   
}

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
        offset = (M_PI/2);
    }   

    float ans = 0;

    if (x > 0)
        ans = atan(y/x);
    if (x < 0 && y >= 0)
        ans = atan(y/x) + 3.14159265359;
    if (x < 0 && y < 0)
        ans = atan(y/x) + 3.14159265359 * ((offset != 0)? 1 : -1);
    if (x == 0 && y > 0)
        ans = 1.570796325;
    if (x == 0 && y < 0)
        ans = -1.570796325;

    if (offset != 0)
        return offset - ans;
    else
        return ans;
}

#define M 2147483647
#define A 16807
#define Q ( M / A )
#define R ( M % A )

float sqrt(float z)
{
    union
    {
        int tmp;
        float f;
    } u;
    u.f = z;
    u.tmp -= 1 << 23; /* Subtract 2^m. */
    u.tmp >>= 1; /* Divide by 2. */
    u.tmp += 1 << 29; /* Add ((b + 1) / 2) * 2^m. */
    return u.f;
}


float prand(struct SpaceGlobals * mySpaceGlobals)
{
	long seed = mySpaceGlobals->seed;
	
	seed = A * (seed % Q) - R * (seed / Q);

    if (seed <= 0)
    {
        seed += M;
    }

	return seed/16807.0;
}

// turns 34.02834237 -> 34.02
float round2decimals(float x)
{
	x *= 100;
	int y = (int)x;
	float fPart = x - y;
	x -= fPart;
	
	return (x / 100.0);
}



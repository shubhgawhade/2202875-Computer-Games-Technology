#include "pch.h"
#include "ClassicPerlinNoiseAlgorithm.h"

void ClassicPerlinNoiseAlgorithm::Initialize()
{
    for(int i=0; i<512; i++)
    {
        perm[i]=p[i & 255];
    }
}

int ClassicPerlinNoiseAlgorithm::FastFloor(double x)
{
     return x>0 ? (int)x : (int)x-1;
}

double ClassicPerlinNoiseAlgorithm::Dot(int g[], double x, double y, double z)
{
    return g[0]*x + g[1]*y + g[2]*z;
}

double ClassicPerlinNoiseAlgorithm::Mix(double a, double b, double t)
{
    return (1-t)*a + t*b;
}

double ClassicPerlinNoiseAlgorithm::Fade(double t)
{
    return t*t*t*(t*(t*6-15)+10);
}

double ClassicPerlinNoiseAlgorithm::Noise(double x, double y, double z)
{
    Initialize();
    
    // Find unit grid cell containing point
    int X = FastFloor(x);
    int Y = FastFloor(y);
    int Z = FastFloor(z);

    // Get relative xyz coordinates of point within that cell
    x = x - X;
    y = y - Y;
    z = z - Z;

    // Wrap the integer cells at 255 (smaller integer period can be introduced here)
    X = X & 255;
    Y = Y & 255;
    Z = Z & 255;

    // Calculate a set of eight hashed gradient indices
    int gi000 = perm[X+perm[Y+perm[Z]]] % 12;
    int gi001 = perm[X+perm[Y+perm[Z+1]]] % 12;
    int gi010 = perm[X+perm[Y+1+perm[Z]]] % 12;
    int gi011 = perm[X+perm[Y+1+perm[Z+1]]] % 12;
    int gi100 = perm[X+1+perm[Y+perm[Z]]] % 12;
    int gi101 = perm[X+1+perm[Y+perm[Z+1]]] % 12;
    int gi110 = perm[X+1+perm[Y+1+perm[Z]]] % 12;
    int gi111 = perm[X+1+perm[Y+1+perm[Z+1]]] % 12;

    // The gradients of each corner are now:
    // g000 = grad3[gi000];
    // g001 = grad3[gi001];
    // g010 = grad3[gi010];
    // g011 = grad3[gi011];
    // g100 = grad3[gi100];
    // g101 = grad3[gi101];
    // g110 = grad3[gi110];
    // g111 = grad3[gi111];
    // Calculate noise contributions from each of the eight corners
    double n000= Dot(grad3[gi000], x, y, z);
    double n100= Dot(grad3[gi100], x-1, y, z);
    double n010= Dot(grad3[gi010], x, y-1, z);
    double n110= Dot(grad3[gi110], x-1, y-1, z);
    double n001= Dot(grad3[gi001], x, y, z-1);
    double n101= Dot(grad3[gi101], x-1, y, z-1);
    double n011= Dot(grad3[gi011], x, y-1, z-1);
    double n111= Dot(grad3[gi111], x-1, y-1, z-1);
    // Compute the fade curve value for each of x, y, z
    double u = Fade(x);
    double v = Fade(y);
    double w = Fade(z);
    // Interpolate along x the contributions from each of the corners
    double nx00 = Mix(n000, n100, u);
    double nx01 = Mix(n001, n101, u);
    double nx10 = Mix(n010, n110, u);
    double nx11 = Mix(n011, n111, u);
    // Interpolate the four results along y
    double nxy0 = Mix(nx00, nx10, v);
    double nxy1 = Mix(nx01, nx11, v);
    // Interpolate the two last results along z
    double nxyz = Mix(nxy0, nxy1, w);

    return nxyz;
}
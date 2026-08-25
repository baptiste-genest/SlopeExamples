// The scene, the motion and the shading are all in spheres.frag, and every
// number it reads is declared in deck.yaml, so there is nothing to do in C++.
//
//   ./spheres --project_path spheres
#include "slope.h"

using namespace slope;

DeckLoader deck;

int main(int argc, char** argv)
{
    deck.init("spheres", "deck.yaml", argc, argv);
    deck.run();
    return 0;
}

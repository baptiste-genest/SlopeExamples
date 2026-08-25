// A moving 3D scene with no code in it. The geometry and its animation are
// sections of snippets.lua, turned into objects by deck.yaml.
//
//   ./orbit --project_path orbit
#include "slope.h"

using namespace slope;

DeckLoader deck;

int main(int argc, char** argv)
{
    deck.init("orbit", "deck.yaml", argc, argv);
    deck.run();
    return 0;
}

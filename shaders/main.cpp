// Three shaders with no code behind them. Each .frag is a whole picture, their
// inputs are declared in deck.yaml, and snippets.lua supplies the sources.
//
//   ./shaders --project_path shaders
#include "slope.h"

using namespace slope;

DeckLoader deck;

int main(int argc, char** argv)
{
    deck.init("shaders", "deck.yaml", argc, argv);
    deck.run();
    return 0;
}

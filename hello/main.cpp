// A deck with no code in it. Every slide is written in deck.yaml.
//
//   ./hello --project_path hello
#include "slope.h"

using namespace slope;

DeckLoader deck;

int main(int argc, char** argv)
{
    deck.init("hello", "deck.yaml", argc, argv);
    deck.run();
    return 0;
}

// A mesh built in C++, an updater whose shape is a snippet section, and the two
// ways of showing a scalar field.
//
//   ./ripple --project_path ripple
#include "slope.h"

using namespace slope;

DeckLoader deck;

namespace {

constexpr int N = 128;

Mesh::MeshPtr sheet;
vecs rest;
scalars height;
scalars envelope;

// ripple/speed is declared in snippets.lua instead.
Params::ScalarParam amplitude;

void buildSheet()
{
    vecs V;
    Faces F;

    for (int j = 0; j <= N; ++j)
        for (int i = 0; i <= N; ++i)
            V.push_back(vec(2.0 * i / N - 1.0, 2.0 * j / N - 1.0, 0.0));

    auto id = [](int i, int j) { return size_t(j * (N + 1) + i); };
    for (int j = 0; j < N; ++j)
        for (int i = 0; i < N; ++i) {
            F.push_back({id(i, j), id(i + 1, j), id(i + 1, j + 1)});
            F.push_back({id(i, j), id(i + 1, j + 1), id(i, j + 1)});
        }

    sheet = Mesh::Add(V, F);
    rest = V;
    height.assign(V.size(), 0.0);

    envelope.clear();
    for (const auto& v : V)
        envelope.push_back(std::exp(-1.5 * v.head<2>().squaredNorm()));
}

} // namespace

int main(int argc, char** argv)
{
    deck.init("ripple", "deck.yaml", argc, argv);

    buildSheet();

    // bounded, so the Tuner draws a slider. Press A to open it, Ctrl+S to save.
    amplitude = Params::Add("ripple/amplitude", 0.18, 0.0, 0.5);

    // Wrapping any polyscope quantity makes it a primitive the deck can put
    // on its own slide.
    auto field = AddPolyscopeQuantity(
        sheet->pc->addVertexScalarQuantity("height", Vec::Zero((int)rest.size())));
    field->q->setColorMap("coolwarm");

    // MeshScalarField takes vertex scalars and holds them. It adds an intro
    // that fades the surface into the colour map, then grows the field.
    auto profile = MeshScalarField::Add(sheet, "envelope", envelope, "viridis");
    profile->color_split = 0.4;   // how much of the intro is the colour fade

    sheet->pc->setSurfaceColor(glm::vec3(0.93f, 0.90f, 0.86f));
    sheet->pc->setMaterial("wax");

    // The loop and the upload stay compiled. The wave's shape is a snippet
    // section, re-read on save.
    sheet->updater = [field](TimeObject) {
        auto wave = Snippet::fn<scalar(scalar)>("wave", 0.0);   // once per frame

        vecs V = rest;
        for (size_t i = 0; i < V.size(); ++i) {
            const scalar r2 = V[i].head<2>().squaredNorm();
            height[i] = (scalar)amplitude * wave(r2);
            V[i](2) = height[i];
        }
        sheet->updateMesh(V);

        field->q->updateData(Vec::Map(height.data(), (int)height.size()));
        // polyscope rescales to the data unless the range is set
        field->q->setMapRange({-0.25, 0.25});
    };

    // the names deck.yaml places with `object:`
    deck.registerObject("sheet", sheet);
    deck.registerObject("height", field);
    deck.registerObject("envelope", profile);

    deck.run();
    return 0;
}

// As-rigid-as-possible deformation, presented with slope.
//
// The solve is in arap.h, the slides in deck.yaml, the timing in snippets.lua,
// and the two panels are shaders. Only this file needs a rebuild.
//
//   ./arap --project_path arap
#include "slope.h"
#include "arap.h"
#include <set>
#include <map>

using namespace slope;

DeckLoader deck;

namespace {

constexpr int MAX_SWEEPS = 10;

// the point the ear is pulled to. snippets.lua says how far along we are.
const vec PULL(0.34, 0.13, -0.05);

// Built in main, read by the updaters.
struct Demo {
    Mesh::MeshPtr mesh;
    arap::Solver  solver;
    vecs P0, P;
    int  handle = 0;
    std::vector<int> pinned;
    int  ring_v = 0;
    std::vector<int> ring_js;
    std::vector<float> curve;
    Params::VecParam grab;      // the goal position, written by the talk and by the gizmo
    Params::IntParam live_sweeps;
};
Demo D;

// Where the ear should be. snippets.lua moves it until the "live" keyframe,
// the gizmo after it.
vec target(const TimeObject& t)
{
    if (t.afterKeyframe("live")) return (vec)D.grab;
    return D.P0[D.handle] + (scalar)Snippet::get("pull") * PULL;
}

// The goal position, marked with p in the deck. The parameter holds it: the
// mesh updater writes it while the talk runs, the gizmo writes it on the last
// slide.
vec goal() { return (vec)D.grab; }

// How many local+global sweeps to run. Reading it from the snippet means the
// pace can be changed by saving a file instead of rebuilding.
int sweepCount(const TimeObject& t)
{
    int n = t.afterKeyframe("live") ? (int)D.live_sweeps : (int)Snippet::get("sweeps");
    return std::clamp(n, 0, MAX_SWEEPS);
}

void solve(const vec& goal, int sweeps)
{
    D.P = D.P0;
    D.P[D.handle] = goal;

    D.curve.clear();
    D.solver.localStep(D.P);
    D.curve.push_back((float)D.solver.energy(D.P));

    for (int k = 0; k < sweeps; ++k) {
        D.solver.globalStep(D.P);
        D.solver.localStep(D.P);
        D.curve.push_back((float)D.solver.energy(D.P));
    }

    scalar e0 = std::max<scalar>(D.curve.front(), 1e-12);
    for (auto& e : D.curve) e = float(e / e0);
}

// the obj is y up and slope is z up. Also centre the bunny in the unit box.
void loadBunny()
{
    D.mesh = Mesh::Add("bunny.obj",false);
    D.mesh->normalize();

    vecs V = D.mesh->getVertices();
    for (auto& v : V) v = vec(v(0), -v(2), v(1));

    vec lo = V[0], hi = V[0];
    for (const auto& v : V) { lo = lo.cwiseMin(v); hi = hi.cwiseMax(v); }
    scalar span = (hi - lo).maxCoeff();
    for (auto& v : V) v = (v - 0.5 * (lo + hi)) / span;

    D.mesh->updateMesh(V);
    D.P0 = D.P = V;

    const int n = (int)V.size();
    std::vector<int> by_height(n);
    std::iota(by_height.begin(), by_height.end(), 0);
    std::sort(by_height.begin(), by_height.end(),
              [&V](int a, int b) { return V[a](2) < V[b](2); });

    D.pinned.assign(by_height.begin(), by_height.begin() + n / 20);
    D.handle = by_height.back();

    std::vector<int> fixed = D.pinned;
    fixed.push_back(D.handle);

    D.solver.build(D.P0, D.mesh->getFaces(), fixed);
}

// Polyscope numbers edges in the order the faces first mention them. Walking
// the faces the same way gives each edge the index polyscope will use, and the
// identity permutation keeps the two aligned.
std::vector<double> ringEdges()
{
    const std::set<int> ring(D.ring_js.begin(), D.ring_js.end());
    std::map<std::pair<size_t, size_t>, bool> seen;
    std::vector<double> on;

    for (const auto& f : D.mesh->getFaces())
        for (size_t j = 0; j < 3; ++j) {
            auto key = std::minmax(f[j], f[(j + 1) % 3]);
            if (seen.count(key)) continue;
            seen[key] = true;
            const int a = (int)key.first, b = (int)key.second;
            on.push_back((a == D.ring_v && ring.count(b))
                      || (b == D.ring_v && ring.count(a)));
        }
    return on;
}

vecs pinnedPoints()
{
    vecs pins;
    for (int i : D.pinned) pins.push_back(D.P0[i]);
    return pins;
}

} // namespace

int main(int argc, char** argv)
{
    deck.init("arap", "deck.yaml", argc, argv);

    loadBunny();

    solve(D.P0[D.handle] + PULL, 6);
    {
        auto angle = D.solver.rotationAngles();
        scalar best = -1;
        const vec towards_camera = vec(2.05, -2.15, 1.05).normalized();
        for (size_t i = 0; i < angle.size(); ++i) {
            if ((D.P0[i] - D.P0[D.handle]).norm() < 0.06) continue;
            if (D.P0[i].normalized().dot(towards_camera) < 0.10) continue;
            if (angle[i] > best) { best = angle[i]; D.ring_v = (int)i; }
        }
        D.ring_js = D.solver.neighbours(D.ring_v);
        const vec n = D.P0[D.ring_v].normalized();
        spdlog::info("[ring] vertex {} turns {:.2f} rad  at {} {} {}  out {} {} {}",
                     D.ring_v, best,
                     D.P[D.ring_v](0), D.P[D.ring_v](1), D.P[D.ring_v](2),
                     n(0), n(1), n(2));
    }

    // Both parameters are shown on the last slide without opening the Tuner.
    // slope shows a parameter on the slides that read it: sweeps is only read
    // there, so declaring it visible is enough. The handle is read on every
    // slide, because the marker follows it, so the mesh updater switches its
    // gizmo on and off.
    D.grab        = Params::AddVec("arap/handle", D.P0[D.handle] + PULL);
    D.live_sweeps = Params::AddInt("arap/sweeps", 4, 0, MAX_SWEEPS)
                        .show(Params::Visible::Panel);

    auto turn = AddPolyscopeQuantity(
        D.mesh->pc->addVertexScalarQuantity("rotation", Vec::Zero((int)D.P0.size())));
    turn->q->setColorMap("reds");

    D.mesh->pc->setSurfaceColor(glm::vec3(0.93f, 0.90f, 0.86f));
    D.mesh->pc->setMaterial("wax");
    D.mesh->pc->setSmoothShade(false);
    D.mesh->pc->setEdgeWidth(0.6);

    D.mesh->updater = [turn](TimeObject t) {
        // Before the "live" keyframe the talk owns the goal, so it writes the
        // parameter and the gizmo stays hidden. On the last slide the gizmo
        // appears where the talk left the ear.
        const bool live = t.afterKeyframe("live");
        Params::setVisible("arap/handle",
                           live ? Params::Visible::Handle : Params::Visible::None);
        if (!live) D.grab.set(D.P0[D.handle] + PULL);

        vec goal   = target(t);
        int sweeps = sweepCount(t);

        // the updater runs every frame, so only re-solve when an input changed
        static vec last_goal = vec::Constant(1e30);
        static int last_sweeps = -1;
        if (goal != last_goal || sweeps != last_sweeps) {
            solve(goal, sweeps);
            last_goal = goal;
            last_sweeps = sweeps;
            D.mesh->updateMesh(D.P);
        }

        auto angle = D.solver.rotationAngles();
        turn->q->updateData(Vec::Map(angle.data(), (int)angle.size()));
        turn->q->setMapRange({0., 1.1});
    };

    std::vector<double> on = ringEdges();
    std::vector<size_t> perm(on.size());
    std::iota(perm.begin(), perm.end(), 0);
    D.mesh->pc->setEdgePermutation(perm);

    auto ring_q = AddPolyscopeQuantity(
        D.mesh->pc->addEdgeScalarQuantity("one ring", on));
    ring_q->q->setColorMap("reds");
    ring_q->q->setMapRange({0., 1.});

    auto pins = PointCloud::Add(pinnedPoints(), 0.011);
    pins->pc->setPointColor(glm::vec3(0.16f, 0.20f, 0.25f));

    auto grip = Point::Add([](TimeObject) { return goal(); }, 0.022);
    grip->pc->setPointColor(glm::vec3(0.886f, 0.447f, 0.357f));

    auto rest_grip = Point::Add(D.P0[D.handle], 0.016);
    rest_grip->pc->setPointColor(glm::vec3(0.482f, 0.545f, 0.639f));

    // deck.yaml's "follow:" reads these, and slope re-projects them every frame
    deck.registerPlacer("rest_point",   [] { return WorldToScreen(D.P0[D.handle]); });
    deck.registerPlacer("target_point", [] { return WorldToScreen(goal()); });

    // The frames name one of these three groups instead of listing objects.
    PrimitiveGroup bunny;
    bunny << D.mesh << pins << grip << rest_grip;

    PrimitiveGroup bunny_field;
    bunny_field << D.mesh << pins << grip << rest_grip << turn;

    PrimitiveGroup one_ring;
    one_ring << D.mesh << pins << ring_q;

    deck.registerObject("bunny", bunny);
    deck.registerObject("bunny_field", bunny_field);
    deck.registerObject("one_ring", one_ring);

    // the curve is about a dozen floats, so it goes into an array uniform.
    // slope sets energies_count in the shader along with it.
    auto plot = Shader::FromFile("energy.frag");
    plot->setResolution(760, 380);
    plot->set("max_sweeps", float(MAX_SWEEPS));
    plot->updater = [plot](TimeObject) { plot->set("energies", D.curve); };
    deck.registerObject("energy_plot", plot);

    deck.run();
    return 0;
}

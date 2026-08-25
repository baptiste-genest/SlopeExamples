#pragma once
// As-rigid-as-possible surface deformation (Sorkine & Alexa, 2007).
//
// Plain Eigen, with no slope in it. main.cpp presents it.
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <vector>

namespace arap {

using scalar = double;
using vec    = Eigen::Vector3d;
using mat    = Eigen::Matrix3d;
using vecs   = std::vector<vec>;
using Faces  = std::vector<std::vector<size_t>>;

class Solver {
public:
    void build(const vecs& P0, const Faces& F, const std::vector<int>& fixed);

    void localStep(const vecs& P);

    void globalStep(vecs& P) const;

    scalar energy(const vecs& P) const;

    std::vector<scalar> rotationAngles() const;

    const vecs& rest() const { return P0; }
    size_t size() const { return P0.size(); }

    std::vector<int> neighbours(int i) const;
    const mat& rotation(int i) const { return R[i]; }

private:
    struct Neighbor { int j; scalar w; };

    vecs P0;
    std::vector<std::vector<Neighbor>> ring;
    std::vector<mat> R;
    std::vector<int> to_free;
    std::vector<int> to_vertex;

    std::vector<int> fixed_verts;
    Eigen::SparseMatrix<scalar> Lfc;
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<scalar>> chol;
};

}

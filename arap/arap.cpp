#include "arap.h"
#include <algorithm>
#include <cmath>
#include <map>

namespace arap {

void Solver::build(const vecs& rest_positions, const Faces& F,
                   const std::vector<int>& fixed)
{
    P0 = rest_positions;
    const int n = (int)P0.size();

    std::map<std::pair<int,int>, scalar> w;
    for (const auto& f : F) {
        if (f.size() != 3) continue;
        for (int k = 0; k < 3; ++k) {
            int a = (int)f[k], b = (int)f[(k+1)%3], c = (int)f[(k+2)%3];
            vec u = P0[a] - P0[c], v = P0[b] - P0[c];
            scalar cot = u.dot(v) / u.cross(v).norm();
            w[{std::min(a,b), std::max(a,b)}] += 0.5 * cot;
        }
    }

    ring.assign(n, {});
    for (const auto& [e, weight] : w) {
        ring[e.first].push_back({e.second, weight});
        ring[e.second].push_back({e.first, weight});
    }

    std::vector<bool> imposed(n, false);
    for (int i : fixed) imposed[i] = true;

    to_free.assign(n, -1);
    std::vector<int> to_fixed(n, -1);
    to_vertex.clear();
    fixed_verts.clear();
    for (int i = 0; i < n; ++i) {
        if (imposed[i]) { to_fixed[i] = (int)fixed_verts.size(); fixed_verts.push_back(i); }
        else            { to_free[i]  = (int)to_vertex.size();   to_vertex.push_back(i); }
    }
    const int m = (int)to_vertex.size();

    std::vector<Eigen::Triplet<scalar>> free_block, fixed_block;
    for (int fi = 0; fi < m; ++fi) {
        int i = to_vertex[fi];
        scalar diagonal = 0;
        for (const auto& nb : ring[i]) {
            diagonal += nb.w;
            if (to_free[nb.j] >= 0) free_block.emplace_back(fi, to_free[nb.j], -nb.w);
            else                    fixed_block.emplace_back(fi, to_fixed[nb.j], -nb.w);
        }
        free_block.emplace_back(fi, fi, diagonal);
    }

    Eigen::SparseMatrix<scalar> Lff(m, m);
    Lff.setFromTriplets(free_block.begin(), free_block.end());
    Lfc.resize(m, (int)fixed_verts.size());
    Lfc.setFromTriplets(fixed_block.begin(), fixed_block.end());

    chol.compute(Lff);

    R.assign(n, mat::Identity());
}

void Solver::localStep(const vecs& P)
{
    R.resize(P0.size());
    for (size_t i = 0; i < P0.size(); ++i) {
        mat S = mat::Zero();
        for (const auto& nb : ring[i])
            S += nb.w * (P0[i] - P0[nb.j]) * (P[i] - P[nb.j]).transpose();

        Eigen::JacobiSVD<mat> svd(S, Eigen::ComputeFullU | Eigen::ComputeFullV);
        mat V = svd.matrixV();
        if ((V * svd.matrixU().transpose()).determinant() < 0)
            V.col(2) *= -1;
        R[i] = V * svd.matrixU().transpose();
    }
}

void Solver::globalStep(vecs& P) const
{
    const int m = (int)to_vertex.size();

    Eigen::MatrixXd B(m, 3);
    for (int fi = 0; fi < m; ++fi) {
        int i = to_vertex[fi];
        vec b = vec::Zero();
        for (const auto& nb : ring[i])
            b += 0.5 * nb.w * (R[i] + R[nb.j]) * (P0[i] - P0[nb.j]);
        B.row(fi) = b.transpose();
    }

    Eigen::MatrixXd C((int)fixed_verts.size(), 3);
    for (size_t k = 0; k < fixed_verts.size(); ++k)
        C.row((int)k) = P[fixed_verts[k]].transpose();

    Eigen::MatrixXd X = chol.solve(B - Lfc * C);
    for (int fi = 0; fi < m; ++fi)
        P[to_vertex[fi]] = X.row(fi).transpose();
}

scalar Solver::energy(const vecs& P) const
{
    scalar E = 0;
    for (size_t i = 0; i < P0.size(); ++i)
        for (const auto& nb : ring[i])
            E += nb.w * ((P[i] - P[nb.j]) - R[i] * (P0[i] - P0[nb.j])).squaredNorm();
    return E;
}

std::vector<int> Solver::neighbours(int i) const
{
    std::vector<int> js;
    for (const auto& nb : ring[i]) js.push_back(nb.j);
    return js;
}

std::vector<scalar> Solver::rotationAngles() const
{
    std::vector<scalar> angle(R.size());
    for (size_t i = 0; i < R.size(); ++i)
        angle[i] = std::acos(std::clamp((R[i].trace() - 1) * 0.5, -1.0, 1.0));
    return angle;
}

}

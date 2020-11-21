#pragma once
#include <iostream>
#include <vector>
#include <Eigen/Core>
#include <Eigen/Dense>

class Gaussian {
    /* N-dimensional Gaussian Distribution
     * Uses canonical form rather than standard form
     * i.e. normally Gaussian distribution is represented as
     * P(x) ~ N(mu, sig), where mu is the mean, and sig is the covariance.
     * In information form, the Gaussian distribution is parameterised using
     * eta = sig ^ -1 * mu
     * lam = sig ^ -1
     * */
private:
    Eigen::VectorXd eta_;
    Eigen::MatrixXd lam_;
public:
    Gaussian() {}
    Gaussian(const Eigen::VectorXd &eta, const Eigen::MatrixXd &lam) :
        eta_(eta), lam_(lam) {}

    /* Getters/Setters */
    const Eigen::VectorXd &eta() const { return eta_; }
    const Eigen::MatrixXd &lam() const { return lam_; }
    Eigen::VectorXd &eta() { return eta_; }
    Eigen::MatrixXd &lam() { return lam_; }
    void setEta(const Eigen::VectorXd & eta) {eta_ = eta;}
    void setLam(const Eigen::MatrixXd & lam) {lam_ = lam;}

    /* Helpers to convert between canonical form and standard form */
    Eigen::VectorXd mu() const { return lam_.inverse() * eta_; }
    Eigen::MatrixXd sig() const { return lam_.inverse(); }

    /* Marginalization */
    inline Gaussian marginalize(uint32_t i, uint32_t j) const {
        uint32_t k = eta_.size();
        /* Indices excluding [i, j] */
        // std::vector<int> N;
        // for (int n = 0; n < k; ++n) { if (n < i || n > j) { N.push_back(n); } }

        Eigen::VectorXd eta_a;
        Eigen::VectorXd eta_b;

        Eigen::MatrixXd lam_aa;
        Eigen::MatrixXd lam_ab;
        Eigen::MatrixXd lam_ba;
        Eigen::MatrixXd lam_bb;
        
        if (i == 0) {
            eta_a = eta_(Eigen::seq(0, 1));
            eta_b = eta_(Eigen::seq(2, 3));

            lam_aa = lam_(Eigen::seq(0, 1), Eigen::seq(0, 1));
            lam_ab = lam_(Eigen::seq(0, 1), Eigen::seq(2, 3));
            lam_ba = lam_(Eigen::seq(2, 3), Eigen::seq(0, 1));
            lam_bb = lam_(Eigen::seq(2, 3), Eigen::seq(2, 3));
        }

        if (i == 2) {
            eta_a = eta_(Eigen::seq(2, 3));
            eta_b = eta_(Eigen::seq(0, 1));

            lam_aa = lam_(Eigen::seq(2, 3), Eigen::seq(2, 3));
            lam_ab = lam_(Eigen::seq(2, 3), Eigen::seq(0, 1));
            lam_ba = lam_(Eigen::seq(0, 1), Eigen::seq(2, 3));
            lam_bb = lam_(Eigen::seq(0, 1), Eigen::seq(0, 1));
        }

        Eigen::MatrixXd lam_bb_inv = lam_bb.inverse();

        Eigen::VectorXd eta = eta_a - lam_ab * lam_bb_inv * eta_b;
        Eigen::MatrixXd lam = lam_aa - lam_ab * lam_bb_inv * lam_ba;

        return Gaussian(eta, lam);
    }
};

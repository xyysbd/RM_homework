#include <iostream>
#include <Eigen/Dense>
#include <ceres/jet.h>
#include "matplotlibcpp.h"
using namespace Eigen;
using namespace ceres;
namespace plt = matplotlibcpp;

template<typename Scalar, int NumberOfStates, int NumberOfObservations>
class ExtendedKalmanFilter {
    using MatrixX = Matrix<Scalar, NumberOfStates, NumberOfStates>;
    using MatrixZX = Matrix<Scalar, NumberOfObservations, NumberOfStates>;
    using MatrixXZ = Matrix<Scalar, NumberOfStates, NumberOfObservations>;
    using MatrixZZ = Matrix<Scalar, NumberOfObservations, NumberOfObservations>;
    using VectorX = Matrix<Scalar, 1, NumberOfStates>;
    using VectorZ = Matrix<Scalar, 1, NumberOfObservations>;

public:
    VectorX states;
    MatrixX P;
    MatrixZZ R;
    MatrixXZ K;
    MatrixZX H;
    VectorZ z_prior;

    explicit ExtendedKalmanFilter(const VectorX &initial_states) : states(initial_states), P(MatrixX::Identity()), R(MatrixZZ::Identity()) {}

    template<class Func>
    VectorX predictAndUpdate(Func &&func, const VectorZ &z) {
        Jet<Scalar> states_auto_jet[NumberOfStates];
        for (int i = 0; i < NumberOfStates; i++) {
            states_auto_jet[i].a = states[i];
            states_auto_jet[i].v[i] = 1;
        }
        Jet<Scalar> z_prior_auto_jet[NumberOfObservations];
        func(states_auto_jet, z_prior_auto_jet);
        for (int i = 0; i < NumberOfObservations; i++) {
            z_prior[i] = z_prior_auto_jet[i].a;
            H.block(i, 0, 1, NumberOfStates) = z_prior_auto_jet[i].v.transpose();
        }
        K = P * H.transpose() * (H * P + R).inverse();
        states = states + K * (z - z_prior);
        P = (MatrixX::Identity() - K * H) * P;
        return states;
    }
};

constexpr int NumberOfStates = 2;
constexpr int NumberOfObservations = 1;
struct Predict {
    double delta_t = 1;
};
struct Measure {
    template<typename Scalar>
    void operator () (const Scalar x0[NumberOfStates], Scalar z0[NumberOfObservations]) {
        z0[0] = x0[0];
    }
};
int main() {
    plt::figure_size(800, 500);
    freopen("../dollar.txt", "r", stdin);
    std::vector<double> initial_states = {vals[0], 0};
    ExtendedKalmanFilter<double, NumberOfStates, NumberOfObservations> ekf(initial_states);
    ekf.Q << 0.001, 0, 0, 0.0001;
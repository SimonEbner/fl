/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2014 Max-Planck-Institute for Intelligent Systems,
 *                     University of Southern California
 *    Jan Issac (jan.issac@gmail.com)
 *    Manuel Wuthrich (manuel.wuthrich@gmail.com)
 *
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Willow Garage, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */

/**
 * \date 2014
 * \author Jan Issac (jan.issac@gmail.com)
 * Max-Planck-Institute for Intelligent Systems,
 * University of Southern California
 */

#include <gtest/gtest.h>

#include <Eigen/Dense>

#include <cmath>
#include <iostream>

#include <fl/model/process/linear_process_model.hpp>


class LinearGaussianProcessModelTests:
        public testing::Test
{
public:
    template <typename Model>
    void InitDimensionTests(Model& model,
                            size_t dim,
                            typename Model::SecondMoment& cov)
    {
        EXPECT_EQ(model.dimension(), dim);
        EXPECT_EQ(model.standard_variate_dimension(), dim);
        EXPECT_TRUE(model.A().isIdentity());
        EXPECT_TRUE(fl::are_similar(model.covariance(), cov));
    }
};

TEST_F(LinearGaussianProcessModelTests, init_fixedsize_dimension)
{
    typedef Eigen::Matrix<double, 10, 1> State;
    typedef fl::LinearGaussianProcessModel<State> LGModel;

    LGModel::SecondMoment cov = LGModel::SecondMoment::Identity() * 5.5465;
    LGModel model(cov);

    InitDimensionTests(model, 10, cov);
}

TEST_F(LinearGaussianProcessModelTests, init_dynamicsize_dimension)
{
    const size_t dim = 10;
    typedef Eigen::VectorXd State;
    typedef fl::LinearGaussianProcessModel<State> LGModel;

    LGModel::SecondMoment cov = LGModel::SecondMoment::Identity(dim, dim) * 5.5465;
    LGModel model(cov, dim);

    InitDimensionTests(model, dim, cov);
}

TEST_F(LinearGaussianProcessModelTests, predict_fixedsize_with_zero_noise)
{
    typedef Eigen::Matrix<double, 10, 1> State;
    const size_t dim = State::SizeAtCompileTime;
    typedef fl::LinearGaussianProcessModel<State> LGModel;

    State state = State::Random(dim, 1);
    LGModel::Noise sample = LGModel::Noise::Zero(dim, 1);
    LGModel::SecondMoment cov = LGModel::SecondMoment::Identity(dim, dim) * 5.5465;
    LGModel model(cov, dim);

    EXPECT_TRUE(model.map_standard_normal(sample).isZero());

    EXPECT_FALSE(fl::are_similar(model.map_standard_normal(sample), state));
    model.condition(1.0, state);
    EXPECT_TRUE(fl::are_similar(model.map_standard_normal(sample), state));
}

TEST_F(LinearGaussianProcessModelTests, predict_dynamic_with_zero_noise)
{
    const size_t dim = 10;
    typedef Eigen::VectorXd State;
    typedef fl::LinearGaussianProcessModel<State> LGModel;

    State state = State::Random(dim, 1);
    LGModel::Noise sample = LGModel::Noise::Zero(dim, 1);
    LGModel::SecondMoment cov = LGModel::SecondMoment::Identity(dim, dim) * 5.5465;
    LGModel model(cov, dim);

    EXPECT_TRUE(model.map_standard_normal(sample).isZero());

    EXPECT_FALSE(fl::are_similar(model.map_standard_normal(sample), state));
    model.condition(1.0, state);
    EXPECT_TRUE(fl::are_similar(model.map_standard_normal(sample), state));
}

TEST_F(LinearGaussianProcessModelTests, predict_fixedsize_with_noise)
{
    typedef Eigen::Matrix<double, 10, 1> State;
    const size_t dim = State::SizeAtCompileTime;
    typedef fl::LinearGaussianProcessModel<State> LGModel;

    State state = State::Random(dim, 1);
    LGModel::Noise noise = LGModel::Noise::Random(dim, 1);
    LGModel::SecondMoment cov = LGModel::SecondMoment::Identity(dim, dim);
    LGModel model(cov, dim);

    EXPECT_TRUE(fl::are_similar(model.map_standard_normal(noise), noise));

    EXPECT_FALSE(fl::are_similar(model.map_standard_normal(noise), state + noise));
    model.condition(1.0, state);
    EXPECT_TRUE(fl::are_similar(model.map_standard_normal(noise), state + noise));
}

TEST_F(LinearGaussianProcessModelTests, predict_dynamic_with_noise)
{
    const size_t dim = 10;
    typedef Eigen::VectorXd State;
    typedef fl::LinearGaussianProcessModel<State> LGModel;

    State state = State::Random(dim, 1);
    LGModel::Noise noise = LGModel::Noise::Random(dim, 1);
    LGModel::SecondMoment cov = LGModel::SecondMoment::Identity(dim, dim);
    LGModel model(cov, dim);

    EXPECT_TRUE(fl::are_similar(model.map_standard_normal(noise), noise));

    EXPECT_FALSE(fl::are_similar(model.map_standard_normal(noise), state + noise));
    model.condition(1.0, state);
    EXPECT_TRUE(fl::are_similar(model.map_standard_normal(noise), state + noise));
}

TEST_F(LinearGaussianProcessModelTests, dynamics_matrix)
{
    const size_t dim = 10;
    typedef Eigen::VectorXd State;
    typedef fl::LinearGaussianProcessModel<State> LGModel;

    State state = State::Random(dim, 1);
    LGModel::Noise sample = LGModel::Noise::Zero(dim, 1);
    LGModel::SecondMoment cov = LGModel::SecondMoment::Identity(dim, dim) * 5.5465;
    LGModel model(cov, dim);
    LGModel::DynamicsMatrix A = LGModel::DynamicsMatrix::Identity(dim, dim);

    State state_expected = state;
    state_expected(0) *= 2.;
    A(0,0) = 2.;
    model.A(A);

    model.condition(1.0, state);

    EXPECT_FALSE(fl::are_similar(model.map_standard_normal(sample), state));
    EXPECT_TRUE(fl::are_similar(model.map_standard_normal(sample), state_expected));
}

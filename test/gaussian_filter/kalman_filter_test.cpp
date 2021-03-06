/*
 * This is part of the fl library, a C++ Bayesian filtering library
 * (https://github.com/filtering-library)
 *
 * Copyright (c) 2015 Max Planck Society,
 * 				 Autonomous Motion Department,
 * 			     Institute for Intelligent Systems
 *
 * This Source Code Form is subject to the terms of the MIT License (MIT).
 * A copy of the license can be found in the LICENSE file distributed with this
 * source code.
 */

/**
 * \file gaussian_filter_kf_test.cpp
 * \date Febuary 2015
 * \author Jan Issac (jan.issac@gmail.com)
 */

#include <gtest/gtest.h>
#include "../typecast.hpp"

#include <Eigen/Dense>

#include "gaussian_filter_test_suite.hpp"

#include <fl/filter/gaussian/gaussian_filter_linear.hpp>

template <
    int StateDimension,
    int InputDimension,
    int ObsrvDimension,
    int FilterIterations = 100
>
struct KalmanFilterTestConfiguration
{
    enum: signed int
    {
        StateDim = StateDimension,
        InputDim = InputDimension,
        ObsrvDim = ObsrvDimension,
        Iterations = FilterIterations
    };

    template <typename ModelFactory>
    struct FilterDefinition
    {
        typedef fl::GaussianFilter<
                    typename ModelFactory::LinearStateTransition,
                    typename ModelFactory::LinearObservation
                > Type;
    };

    template <typename ModelFactory>
    static typename FilterDefinition<ModelFactory>::Type
    create_filter(ModelFactory&& factory)
    {
        return typename FilterDefinition<ModelFactory>::Type(
            factory.create_linear_state_model(),
            factory.create_observation_model());
    }
};

typedef ::testing::Types<
            fl::StaticTest<KalmanFilterTestConfiguration<3, 1, 2>>,
            fl::StaticTest<KalmanFilterTestConfiguration<3, 3, 10>>,
            fl::StaticTest<KalmanFilterTestConfiguration<10, 10, 20>>,

            fl::DynamicTest<KalmanFilterTestConfiguration<3, 1, 2>>,
            fl::DynamicTest<KalmanFilterTestConfiguration<3, 3, 10>>,
            fl::DynamicTest<KalmanFilterTestConfiguration<10, 10, 20>>
        > TestTypes;

INSTANTIATE_TYPED_TEST_CASE_P(KalmanFilterTest,
                              GaussianFilterTest,
                              TestTypes);

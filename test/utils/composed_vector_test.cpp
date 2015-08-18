/*
 * This is part of the FL library, a C++ Bayesian filtering library
 * (https://github.com/filtering-library)
 *
 * Copyright (c) 2014 Jan Issac (jan.issac@gmail.com)
 * Copyright (c) 2014 Manuel Wuthrich (manuel.wuthrich@gmail.com)
 *
 * Max-Planck Institute for Intelligent Systems, AMD Lab
 * University of Southern California, CLMC Lab
 *
 * This Source Code Form is subject to the terms of the MIT License (MIT).
 * A copy of the license can be found in the LICENSE file distributed with this
 * source code.
 */

/**
 * @date 2015
 * @author Manuel Wuthrich (manuel.wuthrich@gmail.com)
 * Max-Planck-Institute for Intelligent Systems
 */

#include <gtest/gtest.h>

#include <Eigen/Core>
#include <fl/util/math/composed_vector.hpp>
#include <fl/util/math/pose_vector.hpp>


using namespace fl;


TEST(composed_vector, count_dynamic_size)
{
    typedef  Eigen::VectorXd Vector;
    typedef Eigen::VectorBlock<Vector, 4> Block;

    ComposedVector<Block, Vector> vector;

    vector.resize(12);
    EXPECT_TRUE(vector.count() == 3);

    vector.resize(15);
    EXPECT_TRUE(vector.count() == 3);

    vector.resize(16);
    EXPECT_TRUE(vector.count() == 4);
}


TEST(composed_vector, count_fixed_size)
{
    typedef  Eigen::Matrix<Real, 16, 1> Vector;
    typedef Eigen::VectorBlock<Vector, 4> Block;

    ComposedVector<Block, Vector> vector;

    EXPECT_TRUE(vector.count() == 4);
}


TEST(composed_vector, recount_dynamic_size)
{
    typedef  Eigen::VectorXd Vector;
    typedef Eigen::VectorBlock<Vector, 4> Block;

    ComposedVector<Block, Vector> vector;

    vector.recount(3);
    EXPECT_TRUE(vector.size() == 12);

    vector.recount(15);
    EXPECT_TRUE(vector.size() == 60);
}




TEST(composed_vector, mutators_dynamic_size)
{  
    typedef  Eigen::VectorXd Vector;
    typedef PoseBlock<Vector> Block;

    ComposedVector<Block, Vector> vector;
    vector.recount(3);

    PoseVector pose0 = PoseVector::Random();
    PoseVector pose1 = PoseVector::Random();
    PoseVector pose2 = PoseVector::Random();

    vector.component(0) = pose0;
    vector.component(1).euler_vector() = pose1.euler_vector();
    vector.component(1).position() = pose1.position();
    vector.component(2).euler_vector().quaternion(
                                pose2.euler_vector().quaternion());
    vector.component(2).position() = pose2.position();

    EXPECT_TRUE(pose0.isApprox(vector.component(0)));
    EXPECT_TRUE(pose1.isApprox(vector.component(1)));
    EXPECT_TRUE(pose2.isApprox(vector.component(2)));
}



TEST(composed_vector, mutators_fixed_size)
{
    typedef  Eigen::Matrix<Real, 3*PoseVector::SizeAtCompileTime, 1> Vector;
    typedef PoseBlock<Vector> Block;

    ComposedVector<Block, Vector> vector;

    PoseVector pose0 = PoseVector::Random();
    PoseVector pose1 = PoseVector::Random();
    PoseVector pose2 = PoseVector::Random();

    vector.component(0) = pose0;
    vector.component(1).euler_vector() = pose1.euler_vector();
    vector.component(1).position() = pose1.position();
    vector.component(2).euler_vector().quaternion(
                                pose2.euler_vector().quaternion());
    vector.component(2).position() = pose2.position();

    EXPECT_TRUE(pose0.isApprox(vector.component(0)));
    EXPECT_TRUE(pose1.isApprox(vector.component(1)));
    EXPECT_TRUE(pose2.isApprox(vector.component(2)));
}


TEST(composed_vector, accessors_dynamic_size)
{
    typedef  Eigen::VectorXd Vector;
    typedef PoseBlock<Vector> Block;

    ComposedVector<Block, Vector> vector;
    vector.recount(3);
    vector = ComposedVector<Block, Vector>::Random(vector.size());

    PoseVector pose0, pose1, pose2;
    pose0                 = vector.component(0);
    pose1.euler_vector()  = vector.component(1).euler_vector();
    pose1.position()      = vector.component(1).position();
    pose2.position()      = vector.component(2).position();
    pose2.euler_vector().quaternion(
                               vector.component(2).euler_vector().quaternion());

    EXPECT_TRUE(vector.component(0).isApprox(pose0));
    EXPECT_TRUE(vector.component(1).isApprox(pose1));
    EXPECT_TRUE(vector.component(2).isApprox(pose2));
}


TEST(composed_vector, accessors_fixed_size)
{
    typedef  Eigen::Matrix<Real, 3*PoseVector::SizeAtCompileTime, 1> Vector;
    typedef PoseBlock<Vector> Block;

    ComposedVector<Block, Vector> vector
                        = ComposedVector<Block, Vector>::Random();

    PoseVector pose0, pose1, pose2;
    pose0                 = vector.component(0);
    pose1.euler_vector()  = vector.component(1).euler_vector();
    pose1.position()      = vector.component(1).position();
    pose2.position()      = vector.component(2).position();
    pose2.euler_vector().quaternion(
                               vector.component(2).euler_vector().quaternion());

    EXPECT_TRUE(vector.component(0).isApprox(pose0));
    EXPECT_TRUE(vector.component(1).isApprox(pose1));
    EXPECT_TRUE(vector.component(2).isApprox(pose2));
}











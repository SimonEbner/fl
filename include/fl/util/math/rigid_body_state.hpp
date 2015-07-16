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
 * \date July 2015
 * \author Manuel Wuthrich (manuel.wuthrich@gmail.com)
 */


#ifndef FL__UTIL__MATH__RIGID_BODY_STATE_HPP
#define FL__UTIL__MATH__RIGID_BODY_STATE_HPP

#include <Eigen/Dense>
#include <fl/util/types.hpp>
#include <fl/util/math/euler_vector.hpp>

namespace fl
{

class RigidBodyState: public Eigen::Matrix<Real, 12, 1>
{
public:
    enum
    {
        BLOCK_SIZE = 3,
        POSITION_INDEX = 0,
        EULER_VECTOR_INDEX = 3,
        LINEAR_VELOCITY_INDEX = 6,
        ANGULAR_VELOCITY_INDEX = 9
    };

    typedef Eigen::Matrix<Real, 12, 1>       StateVector;
    typedef Eigen::Matrix<Real, 3, 1>               Vector;



    // rotation types
    typedef Eigen::AngleAxis<Real>    AngleAxis;
    typedef Eigen::Quaternion<Real>   Quaternion;
    typedef Eigen::Matrix<Real, 3, 3> RotationMatrix;
    typedef Eigen::Matrix<Real, 4, 4> HomogeneousMatrix;
    typedef typename Eigen::Transform<Scalar, 3, Eigen::Affine> Affine;

    typedef Eigen::VectorBlock<StateVector, BLOCK_SIZE>   StateBlock;
    typedef EulerBlock<StateVector>                       EulerStateBlock;


    // constructor and destructor
    RigidBodyState() { }
    template <typename T> RigidBodyState(const Eigen::MatrixBase<T>& state_vector)
    {
        *this = state_vector;
    }
    virtual ~RigidBodyState() {}

    template <typename T>
    void operator = (const Eigen::MatrixBase<T>& state_vector)
    {
        *((StateVector*)(this)) = state_vector;
    }
  

    /// accessors **************************************************************
    virtual Vector position() const
    {
        return this->middleRows<BLOCK_SIZE>(POSITION_INDEX);
    }
    virtual EulerVector euler_vector() const
    {
        return this->middleRows<BLOCK_SIZE>(EULER_VECTOR_INDEX);
    }
    virtual Vector linear_velocity() const
    {
        return this->middleRows<BLOCK_SIZE>(LINEAR_VELOCITY_INDEX);
    }
    virtual Vector angular_velocity() const
    {
        return this->middleRows<BLOCK_SIZE>(ANGULAR_VELOCITY_INDEX);
    }

    /// mutators ***************************************************************
    StateBlock position()
    {
      return StateBlock(this->derived(), POSITION_INDEX);
    }
    EulerStateBlock euler_vector()
    {
      return EulerStateBlock(this->derived(), EULER_VECTOR_INDEX);
    }
    StateBlock linear_velocity()
    {
      return StateBlock(this->derived(), LINEAR_VELOCITY_INDEX);
    }
    StateBlock angular_velocity()
    {
      return StateBlock(this->derived(), ANGULAR_VELOCITY_INDEX);
    }




//    // other representations
//    virtual void quaternion(const Quaternion& quaternion, )
//    {
//        AngleAxis angle_axis(quaternion.normalized());
//        euler_vector(body_index) = angle_axis.angle()*angle_axis.axis();
//    }
//    virtual void pose(const Affine& affine, )
//    {
//       quaternion(Quaternion(affine.rotation()), body_index);
//       position(body_index) = affine.translation();
//    }





//    // other representations
//    virtual Quaternion quaternion(const size_t& object_index = 0) const
//    {
//        Real angle = euler_vector(object_index).norm();
//        Vector axis = euler_vector(object_index).normalized();

//        if(std::isfinite(axis.norm()))
//        {
//            return Quaternion(AngleAxis(angle, axis));
//        }
//        return Quaternion::Identity();
//    }
//    virtual RotationMatrix rotation_matrix(const size_t& object_index = 0) const
//    {
//        return RotationMatrix(quaternion(object_index));
//    }
//    virtual HomogeneousMatrix homogeneous_matrix(const size_t& object_index = 0) const
//    {
//        HomogeneousMatrix homogeneous_matrix(HomogeneousMatrix::Identity());
//        homogeneous_matrix.topLeftCorner(3, 3) = rotation_matrix(object_index);
//        homogeneous_matrix.topRightCorner(3, 1) = position(object_index);

//        return homogeneous_matrix;
//    }

};

}

#endif

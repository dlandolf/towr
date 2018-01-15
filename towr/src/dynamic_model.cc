/**
 @file    dynamic_model.cc
 @author  Alexander W. Winkler (winklera@ethz.ch)
 @date    Sep 19, 2017
 @brief   Brief description
 */

#include <towr/models/dynamic_model.h>

namespace towr {

DynamicModel::DynamicModel(double mass)
{
  m_ = mass;
  g_ = 9.80665;
}

void
DynamicModel::SetCurrent (const ComPos& com_pos,
                          const AngVel& omega,
                          const EELoad& ee_force,
                          const EEPos& ee_pos)
{
  com_pos_  = com_pos;
  omega_    = omega;
  ee_force_ = ee_force;
  ee_pos_   = ee_pos;
}


} /* namespace towr */
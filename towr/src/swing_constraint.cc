/******************************************************************************
Copyright (c) 2018, Alexander W. Winkler. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

#include <towr/constraints/swing_constraint.h>
#include <towr/variables/cartesian_dimensions.h>
#include <iostream>
using namespace std;

namespace towr {

SwingConstraint::SwingConstraint (std::string ee_motion)
    :ConstraintSet(kSpecifyLater, "swing-" + ee_motion)
{
  ee_motion_id_ = ee_motion;
//  cout << "test if swing constraint active " << endl;
}

void
towr::SwingConstraint::InitVariableDependedQuantities (const VariablesPtr& x)
{
  ee_motion_ = x->GetComponent<NodesVariablesPhaseBased>(ee_motion_id_);

//  pure_swing_node_ids_ = ee_motion_->GetIndicesOfAllNodes();
  pure_swing_node_ids_ = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

  // constrain xy position and velocity of every swing node
//  int constraint_count =  pure_swing_node_ids_.size()*Node::n_derivatives*1;
  int constraint_count =  pure_swing_node_ids_.size();

  //only constrain drive nodes!:
//  int constraint_count =  16*Node::n_derivatives*k2D;

//  int constraint_count =  1;

  SetRows(constraint_count);
//  SetRows(0);
  cout << "swing constraints: " << constraint_count << endl;
}

Eigen::VectorXd
SwingConstraint::GetValues () const
{
  VectorXd g(GetRows());

  int row = 0;
  auto nodes = ee_motion_->GetNodes();
  for (int node_id : pure_swing_node_ids_) {
    // assumes two splines per swingphase and starting and ending in stance
    auto curr = nodes.at(node_id);
    int phase  = ee_motion_->GetPhase(node_id);

    Vector2d prev = nodes.at(node_id-1).p().topRows<k2D>();
    Vector2d next = nodes.at(node_id+1).p().topRows<k2D>();
//
    Vector2d distance_xy    = next - prev;
    Vector2d xy_center      = prev + 0.5*distance_xy;
    Vector2d des_vel_center = distance_xy/0.45;

//    if (phase == 0){
//    	des_vel_center = distance_xy/0.45; // linear interpolation not accurate
//    }
//    else {
//    	des_vel_center = distance_xy/0.25;
//    }

//    g(row++) = curr.p()(X) - xy_center(X);
//    g(row++) = curr.v()(X) - des_vel_center(X);
//    if (phase == 0){
    for (auto dim : {X}) {
//      g(row++) = curr.p()(dim) - xy_center(dim);
      g(row++) = curr.v()(dim) - des_vel_center(dim);
//    }
    }
//    g(row++) = curr.v()(X);

//    Vector2d distance_xy    = next - prev;
//    Vector2d xy_center      = prev + distance_xy;
//    Vector2d des_vel_center = distance_xy/(2.8/3); // linear interpolation not accurate
//    g(row++) = curr.v()(X) - des_vel_center(X);

  }

  return g;
}

SwingConstraint::VecBound
SwingConstraint::GetBounds () const
{
//  return VecBound(GetRows(), ifopt::BoundGreaterZero);
	return VecBound(GetRows(), ifopt::BoundZero);
}

void
SwingConstraint::FillJacobianBlock (std::string var_set,
                                    Jacobian& jac) const
{
  if (var_set == ee_motion_->GetName()) {
    int row = 0;
    for (int node_id : pure_swing_node_ids_) {
//      for (auto dim : {X,Y}) {
    	for (auto dim : {X}) {
        // position constraint
//        jac.coeffRef(row, ee_motion_->GetOptIndex(NodesVariables::NodeValueInfo(node_id,   kPos, dim))) =  1.0;  // current node
//        jac.coeffRef(row, ee_motion_->GetOptIndex(NodesVariables::NodeValueInfo(node_id+1, kPos, dim))) = -0.5;  // next node
//        jac.coeffRef(row, ee_motion_->GetOptIndex(NodesVariables::NodeValueInfo(node_id-1, kPos, dim))) = -0.5;  // previous node
//        row++;

        // velocity constraint
        jac.coeffRef(row, ee_motion_->GetOptIndex(NodesVariables::NodeValueInfo(node_id,   kVel, dim))) =  1.0;              // current node
        jac.coeffRef(row, ee_motion_->GetOptIndex(NodesVariables::NodeValueInfo(node_id+1, kPos, dim))) = -1.0/t_swing_avg_; // next node
        jac.coeffRef(row, ee_motion_->GetOptIndex(NodesVariables::NodeValueInfo(node_id-1, kPos, dim))) = +1.0/t_swing_avg_; // previous node
        row++;
      }
//    	jac.coeffRef(row, ee_motion_->GetOptIndex(NodesVariables::NodeValueInfo(node_id,   kVel, X))) =  1.0;
//    	row++;
    }
  }
}

} /* namespace towr */

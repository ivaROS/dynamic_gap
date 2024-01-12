// #include <ros/ros.h>
// #include <tf2_ros/transform_listener.h>
// #include <geometry_msgs/TransformStamped.h>
// #include <geometry_msgs/Twist.h>
// #include <geometry_msgs/Quaternion.h>
// #include <turtlesim/Spawn.h>
// #include <nav_msgs/Odometry.h>
// #include <sensor_msgs/Imu.h>
// #include <tf2_ros/buffer.h>
// #include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <dynamic_gap/gap_estimation/PerfectEstimator.h>
// #include <Eigen/Core>

// #include <sstream>
// #include <unsupported/Eigen/MatrixFunctions>
// #include "/home/masselmeier/Desktop/Research/vcpkg/installed/x64-linux/include/matplotlibcpp.h"
// namespace plt = matplotlibcpp;

namespace dynamic_gap 
{
    PerfectEstimator::PerfectEstimator() 
    {
        xFrozen_ << 0.0, 0.0, 0.0, 0.0;

    }

    void PerfectEstimator::initialize(const std::string & side, const int & modelID,
                                      const float & gapPtX, const float & gapPtY,
                                      const ros::Time & tUpdate, const geometry_msgs::TwistStamped & lastRbtVel,
                                      const geometry_msgs::TwistStamped & lastRbtAcc) 
    {
        this->side_ = side;
        this->modelID_ = modelID;
        ROS_INFO_STREAM_NAMED("GapEstimation", "    initialize model: " << modelID_);

        this->lastRbtVel_ = lastRbtVel;
        this->lastRbtAcc_ = lastRbtAcc;
        this->tLastUpdate_ = tUpdate;
    }

    // For transferring an existing model state to a new model
    void PerfectEstimator::transfer(const Estimator & incomingModel)
    {
        this->side_ = incomingModel.side_;
        this->modelID_ = incomingModel.modelID_;

        ROS_INFO_STREAM_NAMED("GapEstimation", "        transfer model: " << modelID_);

        // OBSERVATION MATRIX
        // this->H_ = model.H_;
        // this->H_transpose_ = model.H_transpose_;

        // this->Q_scalar = model.Q_scalar;
        // this->R_scalar = model.R_scalar;
        // this->Q_k_ = model.Q_k_;
        // this->R_k_ = model.R_k_;

        // COVARIANCE MATRIX
        // covariance/uncertainty of state variables (r_x, r_y, v_x, v_y)
        this->P_kmin1_plus_ = incomingModel.P_kmin1_plus_;
        this->P_k_minus_ = incomingModel.P_k_minus_;
        this->P_k_plus_ = incomingModel.P_k_plus_;

        this->lastRbtVel_ = incomingModel.lastRbtVel_;
        this->lastRbtAcc_ = incomingModel.lastRbtAcc_;
        this->tLastUpdate_ = incomingModel.tLastUpdate_;

        // this->xTilde_ = incomingModel.xTilde_;
        this->x_hat_kmin1_plus_ = incomingModel.x_hat_kmin1_plus_;
        this->x_hat_k_minus_ = incomingModel.x_hat_k_minus_;
        this->x_hat_k_plus_ = incomingModel.x_hat_k_plus_;

        this->G_k_ = incomingModel.G_k_;

        // this->A_ = model.A_;
        // this->STM_ = model.STM_;

        // this->eyes = model.eyes;

        return;
    }

    // void PerfectEstimator::processEgoRobotVelsAndAccs(const ros::Time & tUpdate)
    // {
    //     /*
    //     // Printing original dt values from intermediate odom measurements
    //     ROS_INFO_STREAM_NAMED("GapEstimation", "   t_0 - tLastUpdate_ difference:" << (intermediateRbtVels_[0].header.stamp - tLastUpdate_).toSec() << " sec");

    //     for (int i = 0; i < (intermediateRbtVels_.size() - 1); i++)
    //     {
    //         ROS_INFO_STREAM_NAMED("GapEstimation", "   t_" << (i+1) << " - t_" << i << " difference: " << (intermediateRbtVels_[i + 1].header.stamp - intermediateRbtVels_[i].header.stamp).toSec() << " sec");
    //     }

    //     ROS_INFO_STREAM_NAMED("GapEstimation", "   tUpdate" << " - t_" << (intermediateRbtVels_.size() - 1) << " difference:" << (tUpdate - intermediateRbtVels_[intermediateRbtVels_.size() - 1].header.stamp).toSec() << " sec");
    //     */

    //     // Tweaking ego robot velocities/acceleration to make sure that updates:
    //     //      1. Are never negative (backwards in time)
    //     //      2. Always start from time of last update
    //     //      3. Always at end of time of incoming laser scan measurement

    //     // Erasing odometry measurements that are from *before* the last update 
    //     while (!intermediateRbtVels_.empty() && tLastUpdate_ > intermediateRbtVels_[0].header.stamp)
    //     {
    //         intermediateRbtVels_.erase(intermediateRbtVels_.begin());
    //         intermediateRbtAccs_.erase(intermediateRbtAccs_.begin());
    //     }

    //     // Inserting placeholder odometry to represent the time of the last update
    //     intermediateRbtVels_.insert(intermediateRbtVels_.begin(), lastRbtVel_);
    //     intermediateRbtVels_[0].header.stamp = tLastUpdate_;

    //     intermediateRbtAccs_.insert(intermediateRbtAccs_.begin(), lastRbtAcc_);
    //     intermediateRbtAccs_[0].header.stamp = tLastUpdate_;

    //     // Erasing odometry measurements that occur *after* the incoming laser scan was received
    //     while (!intermediateRbtVels_.empty() && tUpdate < intermediateRbtVels_[intermediateRbtVels_.size() - 1].header.stamp)
    //     {
    //         intermediateRbtVels_.erase(intermediateRbtVels_.end() - 1);
    //         intermediateRbtAccs_.erase(intermediateRbtAccs_.end() - 1);
    //     }

    //     // Inserting placeholder odometry to represent the time that the incoming laser scan was received
    //     intermediateRbtVels_.push_back(intermediateRbtVels_.back());
    //     intermediateRbtVels_.back().header.stamp = tUpdate;

    //     for (int i = 0; i < (intermediateRbtVels_.size() - 1); i++)
    //     {
    //         float dt = (intermediateRbtVels_[i + 1].header.stamp - intermediateRbtVels_[i].header.stamp).toSec();
            
    //         ROS_INFO_STREAM_NAMED("GapEstimation", "   t_" << (i+1) << " - t_" << i << " difference: " << dt << " sec");
            
    //         ROS_INFO_STREAM_COND_NAMED(dt < 0, "GapEstimation", "   ERROR IN TIMESTEP CALCULATION, SHOULD NOT BE NEGATIVE");
    //     }
    // }

    // void PerfectEstimator::isolateGapDynamics() 
    // {
    //     Eigen::Vector4f cartesian_state = getState();
        
    //     // fixing position (otherwise can get bugs)
    //     cartesian_state[0] = xTilde_[0];
    //     cartesian_state[1] = xTilde_[1];

    //     // update cartesian
    //     cartesian_state[2] += lastRbtVel_.twist.linear.x;
    //     cartesian_state[3] += lastRbtVel_.twist.linear.y;
    //     xFrozen_ = cartesian_state;

    //     //std::cout << "modified cartesian state: " << xFrozen_[0] << ", " << xFrozen_[1] << ", " << xFrozen_[2] << ", " << xFrozen_[3] << std::endl;
    // }

    // void PerfectEstimator::setRewindState() 
    // {
    //     rewind_x = xFrozen_;
    // }

    // void PerfectEstimator::rewindPropagate(const float & rew_dt) 
    // {
    //     Eigen::Vector4f new_rewind_x;     
    //     new_rewind_x << 0.0, 0.0, 0.0, 0.0;

    //     Eigen::Vector2f frozen_linear_acc_ego(0.0, 0.0);

    //     Eigen::Vector2f frozen_linear_vel_ego(0.0, 0.0); 
    //     float frozen_ang_vel_ego = 0.0;

    //     float vdot_x_body = frozen_linear_acc_ego[0];
    //     float vdot_y_body = frozen_linear_acc_ego[1];

    //     // discrete euler update of state (ignoring rbt acceleration, set as 0)
    //     new_rewind_x[0] = rewind_x[0] + (rewind_x[2] + rewind_x[1]*frozen_ang_vel_ego)*rew_dt;
    //     new_rewind_x[1] = rewind_x[1] + (rewind_x[3] - rewind_x[0]*frozen_ang_vel_ego)*rew_dt;
    //     new_rewind_x[2] = rewind_x[2] + (rewind_x[3]*frozen_ang_vel_ego - vdot_x_body)*rew_dt;
    //     new_rewind_x[3] = rewind_x[3] + (-rewind_x[2]*frozen_ang_vel_ego - vdot_y_body)*rew_dt;
    //     rewind_x = new_rewind_x; 
    // }

    // void PerfectEstimator::gapStatePropagate(const float & froz_dt) 
    // {
    //     Eigen::Vector4f xFrozenProp_;     
    //     xFrozenProp_ << 0.0, 0.0, 0.0, 0.0;

    //     Eigen::Vector2f frozen_linear_acc_ego(0.0, 0.0);

    //     Eigen::Vector2f frozen_linear_vel_ego(0.0, 0.0); 
    //     float frozen_ang_vel_ego = 0.0;

    //     float vdot_x_body = frozen_linear_acc_ego[0];
    //     float vdot_y_body = frozen_linear_acc_ego[1];

    //     // discrete euler update of state (ignoring rbt acceleration, set as 0)
    //     xFrozenProp_[0] = xFrozen_[0] + (xFrozen_[2] + xFrozen_[1]*frozen_ang_vel_ego)*froz_dt;
    //     xFrozenProp_[1] = xFrozen_[1] + (xFrozen_[3] - xFrozen_[0]*frozen_ang_vel_ego)*froz_dt;
    //     xFrozenProp_[2] = xFrozen_[2] + (xFrozen_[3]*frozen_ang_vel_ego - vdot_x_body)*froz_dt;
    //     xFrozenProp_[3] = xFrozen_[3] + (-xFrozen_[2]*frozen_ang_vel_ego - vdot_y_body)*froz_dt;
    //     xFrozen_ = xFrozenProp_; 
    // }

    void PerfectEstimator::update(const Eigen::Vector2f & measurement, 
                                 const std::vector<geometry_msgs::TwistStamped> & intermediateRbtVels, 
                                 const std::vector<geometry_msgs::TwistStamped> & intermediateRbtAccs, 
                                 const std::vector<geometry_msgs::Pose> & agentPoses,
                                 const std::vector<geometry_msgs::Vector3Stamped> & agentVels,
                                 const ros::Time & tUpdate) 
    {
        
        agentPoses_ = agentPoses;
        agentVels_ = agentVels;

        // acceleration and velocity come in wrt robot frame
        intermediateRbtVels_ = intermediateRbtVels;
        intermediateRbtAccs_ = intermediateRbtAccs;
        // lastRbtVel_ = _current_rbt_vel;
        // lastRbtAcc_ = _current_rbt_acc;

        // dt = scan_dt;
        // life_time += dt;

        // inter_dt = (dt / intermediateRbtVels_.size());

        if (intermediateRbtVels_.size() == 0 || intermediateRbtAccs_.size() == 0)
        {
            ROS_WARN_STREAM_COND_NAMED(intermediateRbtVels_.size() == 0, "    GapEstimation", "intermediateRbtVels_ is empty, no update");
            ROS_WARN_STREAM_COND_NAMED(intermediateRbtAccs_.size() == 0, "    GapEstimation", "intermediateRbtAccs_ is empty, no update");
            return;
        }

        if (intermediateRbtVels_.size() != intermediateRbtAccs_.size())
        {
            ROS_INFO_STREAM_NAMED("GapEstimation", "intermediateRbtVels_ is of size " << intermediateRbtVels_.size() << " while intermediateRbtAccs_ is of size " << intermediateRbtAccs_.size());
            return;
        }

        ROS_INFO_STREAM_NAMED("GapEstimation", "update for model " << getID()); // << ", life_time: " << life_time << ", dt: " << dt << ", inter_dt: " << inter_dt);
        ROS_INFO_STREAM_NAMED("GapEstimation", "x_hat_kmin1_plus_: " << x_hat_kmin1_plus_[0] << ", " << x_hat_kmin1_plus_[1] << ", " << x_hat_kmin1_plus_[2] << ", " << x_hat_kmin1_plus_[3]);
        ROS_INFO_STREAM_NAMED("GapEstimation", "current_rbt_vel, x_lin: " << lastRbtVel_.twist.linear.x << ", y_lin: " << lastRbtVel_.twist.linear.y << ", z_ang: " << lastRbtVel_.twist.angular.z);

        processEgoRobotVelsAndAccs(tUpdate);

        xTilde_ = measurement;

        ROS_INFO_STREAM_NAMED("GapEstimation", "linear ego vel: " << lastRbtVel_.twist.linear.x << ", " << lastRbtVel_.twist.linear.y << ", angular ego vel: " << lastRbtVel_.twist.angular.z);
        ROS_INFO_STREAM_NAMED("GapEstimation", "linear ego acceleration: " << lastRbtAcc_.twist.linear.x << ", " << lastRbtAcc_.twist.linear.y << ", angular ego acc: " << lastRbtAcc_.twist.angular.z);

        x_hat_k_plus_ = updateStateFromEnv();

        x_hat_kmin1_plus_ = x_hat_k_plus_;
        P_kmin1_plus_ = P_k_plus_;
        tLastUpdate_ = tUpdate;
        
        if (intermediateRbtVels_.size() > 0)
            lastRbtVel_ = intermediateRbtVels_.back();
        
        if (intermediateRbtAccs_.size() > 0)
            lastRbtAcc_ = intermediateRbtAccs_.back();

        ROS_INFO_STREAM_NAMED("GapEstimation", "x_hat_k_plus_: " << x_hat_k_plus_[0] << ", " << x_hat_k_plus_[1] << ", " << x_hat_k_plus_[2] << ", " << x_hat_k_plus_[3]);       
        ROS_INFO_STREAM_NAMED("GapEstimation", "-----------");

        return;
    }    

    Eigen::Vector4f PerfectEstimator::updateStateFromEnv() 
    {
        // x state:
        // [r_x, r_y, v_x, v_y]
        Eigen::Vector4f return_x = x_hat_k_plus_;

        
        ROS_INFO_STREAM_NAMED("GapEstimation", "updating ground truth cartesian state");
        // ROS_INFO_STREAM_NAMED("GapEstimation", "xTilde_: " << xTilde_[0] << ", " << xTilde_[1]);
        

        return_x[0] = xTilde_[0];
        return_x[1] = xTilde_[1];
        
        float robot_i_odom_dist = 0.0;
        float min_dist = std::numeric_limits<float>::infinity();
        int min_idx = -1;
        for (int i = 0; i < agentPoses_.size(); i++) 
        {
            robot_i_odom_dist = sqrt(pow(agentPoses_[i].position.x - x_hat_kmin1_plus_[0], 2) + 
                                     pow(agentPoses_[i].position.y - x_hat_kmin1_plus_[1], 2));
            
            if (robot_i_odom_dist < min_dist) 
            {
                min_dist = robot_i_odom_dist;
                min_idx = i;
            }
        }
        
        ROS_INFO_STREAM_COND_NAMED(agentPoses_.size() > 0, "GapEstimation", "closest odom: " << agentPoses_[min_idx].position.x << ", " << agentPoses_[min_idx].position.y);
        
        float min_dist_thresh = 0.4;
        if (min_dist < min_dist_thresh) 
        {    
            ROS_INFO_STREAM_NAMED("GapEstimation", "attaching to odom");
            
            
            // xTilde_[0] = agentPoses_[min_idx].position.x;
            // xTilde_[1] = agentPoses_[min_idx].position.y;
            // return_x[0] = xTilde_[0];
            // return_x[1] = xTilde_[1];
            return_x[2] = agentVels_[min_idx].vector.x - lastRbtVel_.twist.linear.x;
            return_x[3] = agentVels_[min_idx].vector.y - lastRbtVel_.twist.linear.y;
        } else 
        {    
            ROS_INFO_STREAM_NAMED("GapEstimation", "attaching to nothing");
            
            return_x[2] = 0.0 - lastRbtVel_.twist.linear.x;
            return_x[3] = 0.0 - lastRbtVel_.twist.linear.y;         
        }

        return return_x;
    }


    // Eigen::Vector4f PerfectEstimator::getState() 
    // {
    //     // x state:
    //     // [r_x, r_y, v_x, v_y]

    //     return x_hat_k_plus_;
    // }

    // Eigen::Vector4f PerfectEstimator::getGapState() 
    // {
    //     // x state:
    //     // [r_x, r_y, v_x, v_y]
    //     return xFrozen_;
    // }

    // Eigen::Vector4f PerfectEstimator::getRewindGapState() 
    // {
    //     // x state:
    //     // [r_x, r_y, v_x, v_y]
    //     return rewind_x;
    // }

    // geometry_msgs::TwistStamped PerfectEstimator::getRobotVel() 
    // {
    //     return lastRbtVel_;
    // }

    // int PerfectEstimator::getID() 
    // {
    //     return modelID_;
    // }

    // Eigen::Vector2f PerfectEstimator::getXTilde() 
    // {
    //     return xTilde_;
    // }
}
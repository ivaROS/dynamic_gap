#pragma once

#include <ros/ros.h>
#include <boost/numeric/odeint.hpp>
#include <boost/shared_ptr.hpp>

// #include <ros/console.h>
// #include <traj_generator.h>
// #include <turtlebot_trajectory_generator/near_identity.h>
#include <math.h>
#include <dynamic_gap/utils/Gap.h>
#include <dynamic_gap/config/DynamicGapConfig.h>
#include <dynamic_gap/trajectory_generation/trajectory_synthesis_methods.h>
// #include <vector>
#include <geometry_msgs/PoseStamped.h>
// #include <geometry_msgs/Twist.h>
// #include <geometry_msgs/PoseArray.h>
#include <Eigen/Core>
#include <Eigen/Geometry>
// #include <tf/LinearMath/Matrix3x3.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_ros/transform_listener.h>
#include <tf2_ros/transform_broadcaster.h>
// #include "tf/transform_datatypes.h"
// #include <sensor_msgs/LaserScan.h>
#include "OsqpEigen/OsqpEigen.h"

namespace dynamic_gap {

    class GapTrajectoryGenerator
    {
        public:
            GapTrajectoryGenerator(ros::NodeHandle& nh, const dynamic_gap::DynamicGapConfig& cfg) {cfg_ = &cfg; };
            void updateTF(geometry_msgs::TransformStamped tf) {planning2odom = tf;};
            
            std::tuple<geometry_msgs::PoseArray, std::vector<float>> generateTrajectory(dynamic_gap::Gap&, const geometry_msgs::PoseStamped & , const geometry_msgs::TwistStamped &, bool);
            // std::vector<geometry_msgs::PoseArray> generateTrajectory(std::vector<dynamic_gap::Gap>);
            geometry_msgs::PoseArray transformLocalTrajectory(const geometry_msgs::PoseArray & path,
                                                              const geometry_msgs::TransformStamped & transform,
                                                              const std::string & sourceFrame,
                                                              const std::string & destFrame);
            std::tuple<geometry_msgs::PoseArray, std::vector<float>> processTrajectory(const std::tuple<geometry_msgs::PoseArray, std::vector<float>> & return_tuple);

        private: 
            void initializeSolver(OsqpEigen::Solver & solver, int Kplus1, const Eigen::MatrixXd & A);

            Eigen::VectorXd arclength_sample_bezier(Eigen::Vector2d pt_origin, Eigen::Vector2d pt_0, Eigen::Vector2d pt_1, float num_curve_points, float & des_dist_interval);        
            void buildBezierCurve(dynamic_gap::Gap& selectedGap, 
                                Eigen::MatrixXd & left_curve, Eigen::MatrixXd & right_curve, Eigen::MatrixXd & all_curve_pts, 
                                Eigen::MatrixXd & left_curve_vel, Eigen::MatrixXd & right_curve_vel,
                                Eigen::MatrixXd & left_curve_inward_norm, Eigen::MatrixXd & right_curve_inward_norm, 
                                Eigen::MatrixXd & all_inward_norms, Eigen::MatrixXd & left_right_centers, Eigen::MatrixXd & all_centers,
                                Eigen::Vector2d nonrel_left_vel, Eigen::Vector2d nonrel_right_vel, Eigen::Vector2d nom_vel,
                                Eigen::Vector2d left_pt_0, Eigen::Vector2d left_pt_1, Eigen::Vector2d right_pt_0, Eigen::Vector2d right_pt_1, 
                                Eigen::Vector2d gap_radial_extension, Eigen::Vector2d goal_pt_1, float & left_weight, float & right_weight, 
                                float num_curve_points, 
                                int & true_left_num_rge_points, int & true_right_num_rge_points, Eigen::Vector2d init_rbt_pos,
                                Eigen::Vector2d leftBezierOrigin_, Eigen::Vector2d right_bezier_origin);
            void setConstraintMatrix(Eigen::MatrixXd &A, 
                                     int N, 
                                     int Kplus1, 
                                     const Eigen::MatrixXd & all_curve_pts, 
                                     const Eigen::MatrixXd & all_inward_norms, 
                                     const Eigen::MatrixXd & all_centers);


            geometry_msgs::TransformStamped planning2odom;       
            int numCurvePts;
            const DynamicGapConfig* cfg_;

    };
}
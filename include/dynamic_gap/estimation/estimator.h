#include <ros/ros.h>

#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/TwistStamped.h>
#include <geometry_msgs/Vector3Stamped.h>

#include <Eigen/Core>
#include <Eigen/Dense>

namespace dynamic_gap 
{
    class Estimator 
    {
        protected:
            Eigen::Matrix<double, 2, 4> H; // observation matrix
            Eigen::Matrix<double, 4, 2> H_transpose;
            Eigen::Matrix2d R_k; // measurement noise matrix
            Eigen::Matrix4d Q_k; // covariance noise matrix
            Eigen::Matrix4d dQ; // discretized covariance noise matrix

            Eigen::Vector4d x_hat_kmin1_plus, x_hat_k_minus, x_hat_k_plus; // states before and after updates and integrations
            Eigen::Matrix4d P_kmin1_plus, P_k_minus, P_k_plus; // covariance matrices before and after updates and integrations
            
            Eigen::Matrix<double, 4, 2> G_k; // kalman gain
            Eigen::Vector2d x_tilde, innovation, residual;

            Eigen::Matrix4d A, STM;

            virtual void linearize(int idx);
            virtual void discretizeQ(int idx);
            virtual Eigen::Matrix<double, 4, 1> integrate();

        public:
            virtual void update(Eigen::Matrix<double, 2, 1> range_bearing_measurement, 
                                const std::vector<geometry_msgs::TwistStamped> & ego_rbt_vels_copied, 
                                const std::vector<geometry_msgs::TwistStamped> & ego_rbt_accs_copied, 
                                bool print,
                                std::vector<geometry_msgs::Pose> _agent_odoms,
                                std::vector<geometry_msgs::Vector3Stamped> _agent_vels,
                                const ros::Time & t_kf_update);
    };
}
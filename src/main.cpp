#include "ros/ros.h"
#include "controller_msgs/MotorSignal.h"
#include "protocol_msgs/Packet.h"

#include <iostream>
#include <inttypes.h>
#include <cmath>

using namespace std;

ros::Publisher pub;

void twistReceived(const controller_msgs::MotorSignal::ConstPtr &motorSignal)
{
    protocol_msgs::Packet packet;
    unsigned char command = 0x00;
    unsigned char motor1IsReversed = signbit(motorSignal->leftMotor) ? 1 : 0;
    unsigned char motor2IsReversed = signbit(motorSignal->rightMotor) ? 1 : 0;

    unsigned char motor1Speed = (unsigned char)abs(round(motorSignal->leftMotor * 255.0f));
    unsigned char motor2Speed = (unsigned char)abs(round(motorSignal->rightMotor * 255.0f));

    packet.data.push_back(command | (motor1IsReversed << 7) | (motor2IsReversed << 6));
    packet.data.push_back(motor1Speed);
    packet.data.push_back(motor2Speed);

    pub.publish(packet);
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "motor_to_protocol");
    ros::NodeHandle nodeHandle;

    pub = nodeHandle.advertise<protocol_msgs::Packet>("packet", 5);

    ros::Subscriber sub = nodeHandle.subscribe("motor_signal", 1, twistReceived);

    ros::spin();

    return 0;
}

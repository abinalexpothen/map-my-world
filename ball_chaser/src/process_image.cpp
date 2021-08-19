#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define global client to request service
ros::ServiceClient client;

void drive_robot(float lin_x, float ang_z)
{
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if (!client.call(srv))
    {
        ROS_ERROR("Failed to call service drive_bot");
    }
}

void process_image_callback(const sensor_msgs::Image img)
{
    int white_pixel = 255;

    double white_pixel_location = -1.0;

    for (int i=0; i < img.height * img.step; i++)
    {

        if (img.data[i] == white_pixel)
        {
            white_pixel_location = i % img.step;
            break;
        }
    }

    if (white_pixel_location == -1)
    {
        // stop
        drive_robot(0.0, 0.0);
    }
    else if (white_pixel_location < img.step/3.0)
    {
        // turn left
        drive_robot(0.0, 0.5);
    }
    else if (white_pixel_location > 2.0*img.step/3.0)
    {
        // turn right
        drive_robot(0.0, -0.5);
    }
    else
    {
        // move forward
        drive_robot(0.1, 0.0);
    }
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create handle
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // define a client service capable of requesting services from 
    // command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("ball_chaser/command_robot");

    // subscribe to /camera/rgb/image_raw topic
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    ros::spin();

    return 0;
}
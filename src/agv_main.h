
#include <iostream>
#include <JetsonGPIO.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <fcntl.h>
#include <unistd.h>
#include <sstream>
#include "ros/ros.h"
#include "std_msgs/Int32.h"
#include "std_msgs/String.h"
#include "std_msgs/Int32MultiArray.h"
#include <modbus/modbus.h>



void set_device(int file);
void classfication_color(int red, int green, int blue);
#include "/home/daedong/catkin_ws/src/daedong/daedong_agv/src/agv_main.h"

const unsigned short sub_addr =  0x10;
struct i2c_rdwr_ioctl_data data_i2c;

char data[8] = {0};
uint16_t mbbuf[10];

using namespace ros;

modbus_t *mb;


void set_device(int file)
{
	ioctl(file,I2C_SLAVE,0x29);

		char config[2] ={0};

		//Power on
		config[0] = 0x80;
		config[1] = 0x03;
		write(file,config,2);

		//AT Time
		config[0] = 0x81;
		config[1] = 0x00;
		write(file,config,2);

		config[0] = 0x83;
		config[1] = 0xFF;
		write(file,config,2);

		config[0] = 0x8F;
		config[1] = 0x00;
		write(file,config,2);

		char reg[1] ={0x94};
        write(file,reg,1);
}

int main(int argc, char **argv){
   
   int file;	
	
   char filename[20];
   char bus[20] = "/dev/i2c-0";
    
  if((file= open(bus,O_RDWR))<0)
   {
	  printf("Failed to open the bus \n");
   }

   mb = modbus_new_rtu("/dev/ttyUSB0", 115200, 'N', 8, 1);
  
   if (mb == NULL)
    {
     
      return -1;
    }
  
  modbus_set_debug(mb,TRUE);

  if (modbus_connect(mb) == -1)
   {
      
      modbus_free(mb);
      return -1;
      
   }
 modbus_set_slave(mb,1);
 modbus_write_register(mb,0x0078,0x0001);
 modbus_write_register(mb,0x0079,0x0000);
 //modbus_write_register(mb,0x0079,0x01F4); //500rpm

 modbus_set_slave(mb,3);
 modbus_write_register(mb,0x0078,0x0001);
 modbus_write_register(mb,0x0079,0x0000);
 
 //modbus_write_register(mb,0x0079,0xFE0C);//-500rpm

  set_device(file);


  init(argc, argv, "datasender");
	
  NodeHandle n;
  Publisher pub = n.advertise<std_msgs::String>("/message", 1000);
  Publisher sendcolordata = n.advertise<std_msgs::String>("/color", 1000);
  
  Rate loop_rate(10);

  int count = 0;   

  while(ok())
  {    

  //   memset(mbbuf,0,10*2);//Zero
  //  if(modbus_read_registers(mb,0,15,mbbuf)==1)
  //   {
	//    loop_rate.sleep();  
  //    ros::spinOnce();   
  //   }

    // modbus_close(mb);
    // modbus_free(mb);
    

    std_msgs::String msg;
    std_msgs::String colormsg;
    std_msgs::Int32 colorData;
    stringstream ss;

    read(file,data,8);
	  int cData =(data[1] * 256 + data[0]);
	  int red = (data[3] * 256 + data[2]);
	  int green = (data[5] * 256 + data[4]);
	  int blue =(data[7] * 256 + data[6]);

    if(red > green && red > blue)
    {
      
      ss <<"r";
      msg.data = ss.str();
      colormsg.data = ss.str();
      ROS_INFO("msg : %s", msg.data.c_str());
      ROS_INFO("color : %s", colormsg.data.c_str());

    }else if (blue > red && blue > green)
     {

      ss <<"b";
      msg.data = ss.str();
      colormsg.data = ss.str();
      ROS_INFO("msg : %s", msg.data.c_str());
      ROS_INFO("color : %s", colormsg.data.c_str());

     }else if (green > red && green > blue)
     {

      ss <<"g";
      msg.data = ss.str();
      colormsg.data = ss.str();
      ROS_INFO("msg : %s", msg.data.c_str());
      ROS_INFO("color : %s", colormsg.data.c_str());

     }
       
    // ss << "r" ;
    // msg.data = ss.str();
    // ROS_INFO("%s", msg.data.c_str());  


    //  colorData.data = red;
    //  ROS_INFO("red %d", colorData.data);
    // colordata.data = green;
    // ROS_INFO("green %d",colordata.data);
    // colordata.data = blue;   
    // ROS_INFO("blue %d",colordata.data);

    pub.publish(msg);   
    sendcolordata.publish(msg);
    spinOnce();        
    loop_rate.sleep();
    
  }
  
  return 0;
}
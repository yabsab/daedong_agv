#include "/home/daedong/catkin_ws/src/daedong/daedong_agv/src/agv_main.h"

const unsigned short sub_addr =  0x10;
struct i2c_rdwr_ioctl_data data_i2c;

char data[8] = {0};
uint16_t mbbuf[10];
int countYellow =0;

modbus_t *mb;

using namespace ros;

int set_motor()
{

 try
 {
    mb = modbus_new_rtu("/dev/ttyUSB0", 115200, 'N', 8, 1);
  
   if (mb == NULL)
    {
     
     //return -1;
    }
  
  modbus_set_debug(mb,TRUE);

   if (modbus_connect(mb) == -1)
    {
      
      modbus_free(mb);
     // return -1;
      
    }
 }

 catch(exception *e)
 {
    ROS_INFO("checking usb port");
    //return -1;
 }
 
  
  modbus_set_slave(mb,1);
  modbus_write_register(mb,0x0078,0x0001);

 //modbus_write_register(mb,0x0079,0x01F4); //500rpm

  modbus_set_slave(mb,3);
  modbus_write_register(mb,0x0078,0x0001);
 
 //modbus_write_register(mb,0x0079,0xFE0C);//-500rpm
}

void forwardAgv()
{
 
  modbus_set_slave(mb,1);
  modbus_write_register(mb,0x0079,0x00F4); //250rpm

  modbus_set_slave(mb,3);
  modbus_write_register(mb,0x0079,0xFF06);//-250rpm
 
}

void backwardAgv()
{

  modbus_set_slave(mb,3);
  modbus_write_register(mb,0x0079,0x01F4); //500rpm

  modbus_set_slave(mb,1);
  modbus_write_register(mb,0x0079,0xFE0C);//-500rpm

} 

void stopAgv()
{
  
  modbus_set_slave(mb,1);
  modbus_write_register(mb,0x0079,0x0000); 

  modbus_set_slave(mb,3);
  modbus_write_register(mb,0x0079,0x0000);

}

void lefgAgv()
{

  modbus_set_slave(mb,1);
  modbus_write_register(mb,0x0079,0x00FA);  //250 rpm

  modbus_set_slave(mb,3);
  modbus_write_register(mb,0x0079,0x0000);

}

void rightAgv()
{

  modbus_set_slave(mb,1);
  modbus_write_register(mb,0x0079,0x0000); 

  modbus_set_slave(mb,3);
  modbus_write_register(mb,0x0079,0x00FA);
  
}

void feedTheLivestock()
 {

  //  stopAgv();
  //  Rate loop_rate(4);
  //  forwardAgv();

 }

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

  set_device(file);
  set_motor();

  init(argc, argv, "datasender");
	
  NodeHandle n;
  Publisher pub = n.advertise<std_msgs::String>("/message", 1000);
  Publisher sendcolordata = n.advertise<std_msgs::String>("/color", 1000);
  Publisher colorRed = n.advertise<std_msgs::Int32>("/redColor",1000);
  Publisher colorGreen = n.advertise<std_msgs::Int32>("/greenColor",1000);
  Publisher colorBlue = n.advertise<std_msgs::Int32>("/blueColor",1000);

  Rate loop_rate(1000);

  while(ok())
   {    

    std_msgs::String msg;
    std_msgs::String colormsg;
    std_msgs::Int32 colorData;
    std_msgs::Int32 redColorData;
    std_msgs::Int32 greenColorData;
    std_msgs::Int32 blueColorData;

    stringstream ss;

    read(file,data,8);
	  int cData =(data[1] * 256 + data[0]);
	  int red = (data[3] * 256 + data[2]);
    ROS_INFO("red : %d", red);
    redColorData.data = red;

	  int green = (data[5] * 256 + data[4]);
    ROS_INFO("green : %d", green);
	  greenColorData.data = green;

    int blue =(data[7] * 256 + data[6]);
    ROS_INFO("blue : %d",blue);
    blueColorData.data = blue;
       
    if(red > 3200 && red < 4900)
     {
     
      forwardAgv();
      ROS_INFO("red red red");

     }
    else if(blue >2000 && blue <2900 && green > 4700 && green <6600)
     {
      
      for(int i=0; i <155; i++)
       {
         rightAgv();   
         ROS_INFO("yellow yellow yellow");
       }
      
     }
    else if (green >600 && green <800 && blue >500 && blue <800)
     {

      feedTheLivestock();
      ROS_INFO("black black black");

     }
    else if (red >610 && red < 800 && green > 1200 && green < 1800)
     {

      ROS_INFO("green green green");
     
     } 

    else
     {

      stopAgv();
       
     }

    pub.publish(msg);   
    sendcolordata.publish(msg);
    colorRed.publish(redColorData);
    colorGreen.publish(greenColorData);
    colorBlue.publish(blueColorData);
    spinOnce();        
    loop_rate.sleep();

   }
  
  return 0;
}
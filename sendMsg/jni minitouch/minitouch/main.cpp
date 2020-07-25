#include<stdlib.h>
#include<iostream>
int main(int argc, char* argv[])
{
  if(argc<1)
  {
   std::cout<<"agrc error"<<std::endl;
   return -1;
   }
  int res= system(argv[1]);
  if(res<0){
	std::cout<<"start "<<argv[1]<<" error"<<std::endl;
	}
   return 0;
}

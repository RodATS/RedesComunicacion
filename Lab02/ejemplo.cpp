#include <iostream>       // std::cout
#include <thread>         // std::thread, std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
 
void thread_read(int n) 
{
    char buffer[100];
    //loop
    do{
      read(socketC,buffer, 100);
      cout<<buffer;
    }
    while(strcmp(buffer,"bye") != 0);
}
 
int main() 
{
    string msg;
    char buffer[1000];
    int socketClient = connect = (...);
    
  std::thread (thread_read,socketClient).detach();
  
  do{
      getline(msg,...);
      write(socketClint,msg.to_string(),tamañops msg.size());
      
  }
  while(strcomp(buffer,"bye") != 0); //el write
  
  return 0;
}

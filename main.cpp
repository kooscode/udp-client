#include <cstdlib>
#include <iostream>
#include <cstring>
#include <unistd.h>

//socket stuff
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main( void )
{
    sockaddr_in si_me, si_other;
    int _socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (_socket_fd == -1)
    {
        perror ("socket error");
        return -1;
    }

    int port=52525;
    int broadcast=1;

//    setsockopt(s, SOL_SOCKET, SO_BROADCAST,
//                &broadcast, sizeof broadcast);

    memset(&si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(port);
//    si_me.sin_addr.s_addr = INADDR_ANY;

    //bind to specific NIC
    setsockopt(_socket_fd, SOL_SOCKET, SO_BINDTODEVICE, "lo",2);
    
    inet_aton("127.255.255.255", &si_me.sin_addr);
    
    //allow re-use of adddress
    bool enable = true;
    int retval = setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    if ( retval == -1 )   
        std::cerr << "setsockopt(SO_REUSEADDR) failed: " << errno;

   // #ifdef SO_REUSEPORT
    retval = setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int));
    if (retval == -1)
        std::cerr << "setsockopt(SO_REUSEPORT) failed: " << errno;
  // #endif        
    
    int ret = ::bind(_socket_fd, (sockaddr *)&si_me, sizeof(sockaddr));
    if (ret == -1)
    {
        perror ("bind error");
        return -1;
    }

    std::string prev_msg = "";
    while(1)
    {
        char buf[10000];
        memset(&buf, 0x00, sizeof(buf));
        unsigned slen=sizeof(sockaddr);
        recvfrom(_socket_fd, buf, sizeof(buf)-1, 0, (sockaddr *)&si_other, &slen);
        std::string msg(buf);
        
        if (prev_msg != msg)
        {
            std::cout << msg;
            prev_msg = msg;
        }
    }
    
    close(_socket_fd);

}
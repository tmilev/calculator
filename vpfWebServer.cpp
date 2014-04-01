//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include <unistd.h>
#include <string.h>
#include <netdb.h> //<-addrinfo and related data structures defined here
#include <arpa/inet.h> // <- inet_ntop declared here (ntop= network to presentation)
#include <sys/wait.h>
#include "vpfHeader4SystemFunctionsGlobalObjects.h"
ProjectInformationInstance projectInfoInstanceWebServer(__FILE__, "Web server.");


const char* PORT ="80";  // the port users will be connecting to
const int BACKLOG =10;     // how many pending connections queue will hold

void sigchld_handler(int s)
{ while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{ if (sa->sa_family == AF_INET)
    return &(((struct sockaddr_in*)sa)->sin_addr);
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

class Socket
{
public:
  int socketID;
  Socket(): socketID(-1){}
  ~Socket()
  { close(this->socketID);
    this->socketID=-1;
  }
};

Socket SocketUser;

void SendStringToSocket(const std::string& stringToOutput)
{ if (SocketUser.socketID==-1)
  { std::cout << stringToOutput;
    return;
  }
  send(SocketUser.socketID, stringToOutput.c_str(), stringToOutput.size(),0);
}

int serverMain()
{ MacroRegisterFunctionWithName("serverMain");
  int sockfd;  // listen on sock_fd, new connection on new_fd
  addrinfo hints, *servinfo, *p;
  sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size;
  struct sigaction sa;
  int yes=1;
  char userAddress[INET6_ADDRSTRLEN];
  int rv;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // use my IP
  if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
  { stOutput << "getaddrinfo: " << gai_strerror(rv);
    return 1;
  }
  // loop through all the results and bind to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next)
  { if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
    { stOutput << "Error: socket failed\n";
      continue;
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
      crash << "Error: setsockopt failed.\n" << crash;
    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
    { close(sockfd);
      stOutput << "Error: bind failed. \n";
      continue;
    }
    break;
  }
  if (p == NULL)
  { stOutput << "Failed to bind to port " << PORT << "\n";
    return 2;
  }
  freeaddrinfo(servinfo); // all done with this structure
  if (listen(sockfd, BACKLOG) == -1)
  { stOutput << "Listen function failed.";
    return 1;
  }
  sa.sa_handler = sigchld_handler; // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1)
    stOutput << "sigaction returned -1";
  stOutput << "server: waiting for connections...\n";
  while(1)
  { // main accept() loop
    sin_size = sizeof their_addr;
    SocketUser.socketID = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (SocketUser.socketID == -1)
    { stOutput << "Accept failed.";
      continue;
    }
    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), userAddress, sizeof userAddress);
    stOutput << "Server: got connection from " << userAddress;
    if (!fork())
    { // this is the child process
      close(sockfd); // child doesn't need the listener
      stOutput.theOutputFunction=SendStringToSocket;
      return 1;
    }
    close(SocketUser.socketID);  // parent doesn't need this
  }
  return 0;
}


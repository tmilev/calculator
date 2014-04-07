//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader6WebServer.h"

ProjectInformationInstance projectInfoInstanceWebServer(__FILE__, "Web server implementation.");

Socket ClientSocket;

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

std::string ClientMessage::ToString()const
{ std::stringstream out;
  if (requestType==this->requestTypeGet)
    out << "<br>\nGET  " << this->mainArgument;

  if (this->theStrings.size>0)
  { out << "<br>\nStrings extracted from message: ";
    for (int i =0; i<this->theStrings.size; i++)
      out << "<br>" << this->theStrings[i];

  }
  out << "\n<br>\nFull client message:\n" << this->theMessage;
  return out.str();
}

void ClientMessage::resetEverythingExceptMessageString()
{ this->mainArgument="";
  this->theStrings.SetSize(0);
  this->requestType=this->requestTypeNone;
}

void ClientMessage::ParseMessage()
{ MacroRegisterFunctionWithName("ClientMessage::ParseMessage");
  this->resetEverythingExceptMessageString();
  std::string buffer;
  buffer.reserve(this->theMessage.size());
  this->theStrings.SetExpectedSize(20);
  for (unsigned i =0; i<this->theMessage.size(); i++)
    if (theMessage[i]!=' ' && theMessage[i]!='\n')
      buffer.push_back(this->theMessage[i]);
    else
      if (buffer!="")
      { this->theStrings.AddOnTop(buffer);
        buffer="";
      }
  for (int i=0; i<this->theStrings.size; i++)
    if (this->theStrings[i]=="GET")
    { this->requestType=this->requestTypeGet;
      i++;
      if (i<this->theStrings.size)
        this->mainArgument=this->theStrings[i];
    } else if (this->theStrings[i]=="POST")
    { this->requestType=this->requestTypePost;
      i++;
      if (i<this->theStrings.size)
        this->mainArgument=this->theStrings[i];
    }
}

void SendStringToSocket(const std::string& stringToOutput)
{ if (ClientSocket.socketID==-1)
  { std::cout << stringToOutput;
    return;
  }
  std::string remainder=stringToOutput;
  std::cout << "\nSending " << remainder.size() << " bytes in chunks of: ";
//  remainder.push_back('\0');
  int numBytesToSend=remainder.size();
  int numBytesSent=0;
  while (numBytesSent<numBytesToSend)
  { numBytesSent+=send(ClientSocket.socketID, remainder.c_str(), remainder.size(),0);
    std::cout << numBytesSent;
    if (numBytesSent<numBytesToSend)
    { std::cout << ", ";
      remainder=remainder.substr(numBytesSent, std::string::npos);
    }
  }
}

bool Socket::Receive()
{ MacroRegisterFunctionWithName("Socket::Receive");
  int numBytesReceived=-1;
  char buffer[4096*2];
  numBytesReceived= recv(this->socketID, &buffer, 4096*2-1, 0);
  if (numBytesReceived>0)
  { numBytesReceived++;
    buffer[numBytesReceived]='\0';//null termination ensured
    this->lastMessageReceived.theMessage=buffer;
  } else
    this->lastMessageReceived.theMessage="";
  this->lastMessageReceived.ParseMessage();
  if (this->lastMessageReceived.requestType==this->lastMessageReceived.requestTypePost)
  {

  }
//  std::cout << "\nReceived from client: " << this->lastMessageReceived.ToString() << "\n";
  return true;
}

int main_HttpServer()
{ MacroRegisterFunctionWithName("main_HttpServer");
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
  { std::cout << "getaddrinfo: " << gai_strerror(rv);
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
      std::cout << "Error: bind failed. \n";
      continue;
    }
    break;
  }
  if (p == NULL)
  { std::cout << "Failed to bind to port " << PORT << "\n";
    return 2;
  }
  freeaddrinfo(servinfo); // all done with this structure
  if (listen(sockfd, BACKLOG) == -1)
  { std::cout << "Listen function failed.";
    return 1;
  }
  sa.sa_handler = sigchld_handler; // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1)
    std::cout << "sigaction returned -1";
  std::cout << "server: waiting for connections...\n";
  while(1)
  { // main accept() loop
    sin_size = sizeof their_addr;
    ClientSocket.socketID = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (ClientSocket.socketID == -1)
    { std::cout << "Accept failed.";
      continue;
    }
    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), userAddress, sizeof userAddress);
    std::cout << "Server: got connection from " << userAddress;
    if (!fork()) //creates an almost identical copy of this process, the original process is the parent, the almost identical copy is the child.
    { // this is the child process
      InitializeTimer();
      close(sockfd); // child doesn't need the listener

      ClientSocket.Receive();
      stOutput.theOutputFunction=SendStringToSocket;
      return 1;
    }
    close(ClientSocket.socketID);  // parent doesn't need this
  }
  return 0;
}


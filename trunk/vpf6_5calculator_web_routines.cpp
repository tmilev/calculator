//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader3Calculator2_InnerFunctions.h"
#include "vpfHeader6WebServer.h"
#include <netdb.h> //<-addrinfo and related data structures defined here
#include <arpa/inet.h> // <- inet_ntop declared here (ntop= network to presentation)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


ProjectInformationInstance ProjectInfoVpf6_5calculatorWebRoutines(__FILE__, "Calculator web routines. ");

bool CalculatorFunctionsGeneral::innerGoogleLogin(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerGoogleLogin");
/*  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  portno = 80;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    return output.MakeError("Couldn't open socket.", theCommands);*/
  std::stringstream reportStream;
  int status;
  struct addrinfo hints;
  struct addrinfo *servinfo;
  struct addrinfo *p;  // will point to the results

  memset(&hints, 0, sizeof hints); // make sure the struct is empty
  hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
  hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
  std::string theAddress="accounts.google.com";
  if ((status = getaddrinfo(theAddress.c_str(), "https", &hints, &servinfo)) != 0)
  { reportStream << "getaddrinfo error: " << gai_strerror(status);
    return output.MakeError(reportStream.str(), theCommands);
  }
  for(p = servinfo; p != 0; p = p->ai_next)
  { void *addr;
    // get the pointer to the address itself,
    // different fields in IPv4 and IPv6:
    if (p->ai_family == AF_INET)
    { // IPv4
      struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
      addr = &(ipv4->sin_addr);
      reportStream << "IPv4: ";
    } else
    { // IPv6
      struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
      addr = &(ipv6->sin6_addr);
      reportStream << "IPv6: ";
    }
    // convert the IP to a string and print it:
    char ipstr[INET6_ADDRSTRLEN];
    inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
//    std::string ipString()
    reportStream << theAddress << ": " << ipstr << "<br>";
    /*this->listeningSocketID= socket(p->ai_family, p->ai_socktype, p->ai_protocol);
      if (this->listeningSocketID == -1)
      { theLog << "Error: socket failed.\n";
        continue;
      }
      if (setsockopt(this->listeningSocketID, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        crash << "Error: setsockopt failed.\n" << crash;
      if (bind(this->listeningSocketID, p->ai_addr, p->ai_addrlen) == -1)
      { close(this->listeningSocketID);
        theLog << "Error: bind failed. " << this->ToStringLastErrorDescription() << logger::endL;
        continue;
      }
      break;*/
    }
  if (servinfo==0)
  { reportStream << "Servinfo is zero";
    return output.AssignValue(reportStream.str(), theCommands);
  }
  WebWorker theWorker;
  theWorker.connectedSocketID = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
  int connectionResult=connect(theWorker.connectedSocketID, servinfo->ai_addr, servinfo->ai_addrlen);
  if (connectionResult==-1)
    reportStream << "<br>failed to connect";
  else
  { reportStream << "<br>connected";
    reportStream << "<meta http-equiv=\"refresh\" "
    << "content=\"0; url=https://accounts.google.com/o/oauth2/auth?"
    << "redirect_uri=http://localhost:8080/vectorpartition/cgi-bin/calculator&"
    << "response_type=code&client_id=538605306594&"
    << "scope=&approval_prompt=force&access_type=offline\">";
  }
  close(theWorker.connectedSocketID);
  // servinfo now points to a linked list of 1 or more struct addrinfos

// ... do everything until you don't need servinfo anymore ....

  freeaddrinfo(servinfo); // free the linked-list

/*
  server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0)
         error("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0)
         error("ERROR reading from socket");
    printf("%s\n",buffer);
    close(sockfd);
    return 0;*/
  return output.AssignValue(reportStream.str(), theCommands);
}

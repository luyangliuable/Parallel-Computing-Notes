#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

char* get_cur_processs_ip_address() {
   char host[256];
   char *ip_address;
   int hostname;
   struct hostent *host_information;

   hostname = gethostname(host, sizeof(host));

   if (hostname == -1) {
     perror("Unable to get hostname.\n");
     exit(1);
   }

   host_information = gethostbyname(host);

   if (host_information == NULL) {
     perror("Unable to get host entry.\n");
     exit(1);
   }

   ip_address = inet_ntoa(*((struct in_addr*) host_information->h_addr_list[0])); //Convert into IP string

   return ip_address;
}

/* int main() { */
/*   char *res = get_cur_processs_ip_address(); */
/*   printf("%s\n", res); */
/* } */

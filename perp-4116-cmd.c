#include <stdio.h>
#include <getopt.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "vgpi-api.h"

char * ip_addr;
int port;
int button;
int value;

#define VGPI_TYPE_SET 2

static int parse_opt(int argc, char *argv[ ])
{
        int c;
        int errflg=0;

        while ((c = getopt(argc, argv, "")) != -1){
                switch(c){
                case '?':
                        fprintf(stderr,
                                "Unrecognized option: -%c\n", optopt);
                        errflg++;
                }
        }
        if (optind+4 > argc){
               fprintf(stderr, "Expected argument after options\n");
               errflg++;
               return errflg;
        }

        ip_addr = argv[optind];
        port = atoi(argv[optind+1]);
        button = atoi(argv[optind+2]);
        value = atoi(argv[optind+3]);

        if((button<1) || (button>4)){
                fprintf(stderr, "Invalid button\n");
                errflg++;
        }
        return errflg;
}

static void vgpi_set_packet_uint16(char * buf, int value)
{
    buf[HEADER_SIZE] = VGPI_FIELD_UINT16;
    buf[HEADER_SIZE + 1] = (value >> 8) & 0xff;
    buf[HEADER_SIZE + 2] =  value & 0xff;
}

static void send_cmd()
{
        char buf[HEADER_SIZE + VGPI_LEN_UINT16];
        char handshake[HANDSHAKE_SIZE];

        int sockfd;
        struct sockaddr_in server;

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        server.sin_addr.s_addr = inet_addr(ip_addr);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
        if (connect(sockfd , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		printf("connect error\n");
		return;
	}

        // handshake
        if(recv(sockfd, handshake , HANDSHAKE_SIZE , 0) < 0){
		printf("receive error\n");
		return;
	}
        if(!vgpi_check_handshake(handshake)){
                printf("handshake error\n");
		return;
        }
        send(sockfd, vgpi_handshake, HANDSHAKE_SIZE, 0);

        // hack
        usleep(10000);

        if((value == 0)||(value == 1)){
                // packet
                vgpi_set_packet_size(buf, sizeof(buf));
                vgpi_set_packet_type(buf, VGPI_TYPE_SET);
                vgpi_set_type_specific(buf, button);
                vgpi_set_packet_uint16(buf, value);
                send(sockfd, buf, sizeof(buf), 0);
        } else {
                // packet
                vgpi_set_packet_size(buf, sizeof(buf));
                vgpi_set_packet_type(buf, VGPI_TYPE_SET);
                vgpi_set_type_specific(buf, button);
                vgpi_set_packet_uint16(buf, 1);
                send(sockfd, buf, sizeof(buf), 0);

                usleep(value*1000);

                // packet
                vgpi_set_packet_size(buf, sizeof(buf));
                vgpi_set_packet_type(buf, VGPI_TYPE_SET);
                vgpi_set_type_specific(buf, button);
                vgpi_set_packet_uint16(buf, 0);
                send(sockfd, buf, sizeof(buf), 0);
        }
        

        close(sockfd);
}

int main(int argc, char *argv[])
{
        if (parse_opt(argc, argv))
                return 1;
        send_cmd();
        return 0;
}

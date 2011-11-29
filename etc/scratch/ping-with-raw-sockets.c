/*
 *  "ping.h" Version 1.0
 *
 *  Copyright (C) 2002 Michael Cowan.
 *  All Rights Reserved
 *
 *  Greetz: 
 *
 */


#ifndef _INC_PING
#define _INC_PING

#ifndef DEBUG_ALL
#define DEBUG_ALL 0
#endif

#pragma pack(4)

#ifndef WIN32

/* FreeBSD */
//#define _BSD_SOURCE

#define BYTE int
#define USHORT unsigned short
#define ULONG unsigned long
#define UCHAR unsigned char

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define SOCKET int

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/ioctl.h>
#ifdef __GLIBC__
#include <netinet/if_ether.h>	
#else
#include <linux/if_ether.h>	
#endif
#include <net/if_arp.h>
#include <net/if.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/time.h>
#else

/* Windows */
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#endif

/* Common */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ICMP_ECHO 8
#define ICMP_MIN 8

#define DEF_PACKET_SIZE 24
#define MAX_PACKET 64


/* The IP header */
typedef struct _iphdr {
  unsigned int h_len:4;          // length of the header
  unsigned int version:4;        // Version of IP
  unsigned char tos;             // Type of service
  unsigned short total_len;      // total length of the packet
  unsigned short ident;          // unique identifier
  unsigned short frag_and_flags; // flags
  unsigned char  ttl; 
  unsigned char proto;           // protocol (TCP, UDP etc)
  unsigned short checksum;       // IP checksum
  
  unsigned int sourceIP;
  unsigned int destIP;
  
}IpHeader;


/* The ICMP header */
typedef struct _ihdr {
  BYTE i_type;
  BYTE i_code; 
  USHORT i_cksum;
  USHORT i_id;
  USHORT i_seq;
  ULONG timestamp;
}IcmpHeader;


/* Linux version of Windows functions */
#ifndef WIN32

#define GetCurrentProcessId() getpid()

unsigned long GetTickCount(){
  struct timeval tv;
  gettimeofday(&tv, NULL); 
  return ((tv.tv_sec*1000000)+(tv.tv_usec))/1000; // msec
};
#endif


/* Function declerations (not for external use) */


/* Format for error messages */
void ping_error(const char *pMessage){
  fprintf(stderr, "[error]: <ping.h> %s\n", pMessage);
};


/* Helper function to fill in various stuff in our ICMP request. */
void fill_icmp_d(char * icmp_d, int datasize){
  
  IcmpHeader *icmp_hdr;
  char *datapart;
  
  icmp_hdr = (IcmpHeader*)icmp_d;
  
  icmp_hdr->i_type = ICMP_ECHO;
  icmp_hdr->i_code = 0;
  icmp_hdr->i_id = (USHORT)GetCurrentProcessId();
  icmp_hdr->i_cksum = 0;
  icmp_hdr->i_seq = 0;
  
  datapart = icmp_d + sizeof(IcmpHeader);

  //memset(datapart,'E', datasize - sizeof(IcmpHeader));
  strncpy(datapart, "Whee, i`m a fast packet.", 
	  datasize - sizeof(IcmpHeader));
};


/* Krappy checksum algorithm */
USHORT checksum(USHORT *buffer, int size) {
  
  unsigned long cksum=0;
  
  while(size >1) {
    cksum+=*buffer++;
    size -=sizeof(USHORT);
  }
  
  if(size ) {
    cksum += *(UCHAR*)buffer;
  }
  
  cksum = (cksum >> 16) + (cksum & 0xffff);
  cksum += (cksum >>16);
  return (USHORT)(~cksum);
};


/* Used to decode recvd buffer back into an IP header */
int decode_resp(char *buf, int bytes,struct sockaddr_in *from) {

  int DEBUG=0;

  IpHeader *iphdr;
  IcmpHeader *icmphdr;
  unsigned short iphdrlen;
  
  int nTime;

  if(DEBUG_ALL)DEBUG=1;
 
  nTime=GetTickCount();
  
  iphdr = (IpHeader *)buf;
  
  iphdrlen = iphdr->h_len * 4 ; // number of 32-bit words *4 = bytes
  
  if (bytes<iphdrlen+ICMP_MIN) 
    ping_error("decode_resp(), too few bytes recieved");
  
  icmphdr = (IcmpHeader*)(buf + iphdrlen);
  
  if (icmphdr->i_id != (USHORT)GetCurrentProcessId()) {
    //ping_error("someone elses packet");
    //printf("--->Packets ID is %i\n", icmphdr->i_id);
    return -1;
  }

  if(DEBUG) {
    printf("[debug]: <ping.h> %d bytes from %s:",
	   bytes, inet_ntoa(from->sin_addr));
    printf(" icmp_seq=%d",icmphdr->i_seq);
    printf(" time=%d ms\n",nTime-((int)icmphdr->timestamp));}

  return (nTime-(icmphdr->timestamp));
  
};


/*****************************************************************************/
/* DESCRIPTION: Sends a ping packet to *pDest, with nTimeout msec to live    */
/* RETURN VALUE: -1 on error (dead target), else time in ms                  */
/*****************************************************************************/
int ping(const char *pDest, unsigned int nTimeout){

  
#ifdef WIN32 
  WSADATA wsaData;
#endif
  
  SOCKET sockRaw;
  struct sockaddr_in dest,from;
  struct hostent * hp;
  int bread,datasize;
  int fromlen = sizeof(from);
  int bwrote;
  int next=1;
  int nTime=0;
  char *pToken, *pTarget;
  int nTargetLen;

  char *dest_ip;
  char *icmp_d;
  char *recvbuf;
  unsigned int addr=0;
  USHORT seq_no = 0;

  if(pDest==NULL) {
    ping_error("*pDest is NULL."); return -1;}

  pToken=strstr(pDest, ":");
  if(pToken==NULL) {
    nTargetLen=strlen(pDest);
    pTarget=(char*)malloc(nTargetLen+1); memset(pTarget, 0, nTargetLen+1);
    strcpy(pTarget, pDest);
  }
  else {
    nTargetLen=pToken-pDest;
    pTarget=(char*)malloc(nTargetLen+1); memset(pTarget, 0, nTargetLen+1);
    strncpy(pTarget, pDest, nTargetLen);
  }


#ifdef WIN32
  if (WSAStartup(MAKEWORD(2,1),&wsaData) != 0){
    ping_error("ping(), WSAStartup failed."); return -1;}

  sockRaw = WSASocket (AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0,0);
  if (sockRaw == INVALID_SOCKET) {
    ping_error("ping(), WSASocket() failed."); return -1; }

#else
  sockRaw = socket ( AF_INET, SOCK_RAW,IPPROTO_ICMP );
  if (sockRaw == INVALID_SOCKET) {
    ping_error("ping(), socket failed."); return -1; }
#endif

  memset(&dest,0,sizeof(dest));

  hp = gethostbyname(pTarget);
  if(!hp) addr=inet_addr(pTarget);
  if((!hp)  && (addr == INADDR_NONE) ) {
	ping_error("ping(), unable to resolve."); return -1; }
  if(hp != NULL) memcpy(&(dest.sin_addr),hp->h_addr,hp->h_length);
   else dest.sin_addr.s_addr = addr;
  if(hp) dest.sin_family = hp->h_addrtype;
  else dest.sin_family = AF_INET;
  
  dest_ip = inet_ntoa(dest.sin_addr);
  
  datasize = DEF_PACKET_SIZE; datasize += sizeof(IcmpHeader);  
  
  icmp_d = (char*) malloc(MAX_PACKET); memset(icmp_d, 0, MAX_PACKET);
  recvbuf = (char*) malloc(MAX_PACKET);
  
  if (!icmp_d){ ping_error("ping(), memory allocation failed."); return -1; }
  
  memset(icmp_d,0,MAX_PACKET); fill_icmp_d(icmp_d,datasize);
  
  while(1) {

    if(next>=0) {
      memset(recvbuf, 0, MAX_PACKET);
    
      ((IcmpHeader*)icmp_d)->i_cksum = 0;
      ((IcmpHeader*)icmp_d)->timestamp = GetTickCount();
      ((IcmpHeader*)icmp_d)->i_seq = seq_no++;
      ((IcmpHeader*)icmp_d)->i_cksum = checksum((USHORT*)icmp_d, datasize);

      nTime=GetTickCount();/*time(NULL);*/

      bwrote = sendto(sockRaw,icmp_d,datasize,0,(struct sockaddr*)&dest,
					sizeof(dest));
      if (bwrote == SOCKET_ERROR){
	ping_error("ping(), sendto failed."); return -1;}
      if (bwrote < datasize ) ping_error("ping(), couldn't write all bytes.");
    }

    do{
#ifndef WIN32
      
      fcntl(sockRaw, F_SETFL, O_NONBLOCK);

      bread = recvfrom(sockRaw,recvbuf,MAX_PACKET,0,
		       (struct sockaddr*)&from, (socklen_t*)&fromlen);
#else
      bread = recvfrom(sockRaw,recvbuf,MAX_PACKET,0,
		       (struct sockaddr*)&from, &fromlen);
#endif
       if(GetTickCount()/*time(NULL)*/-nTime>nTimeout) return -1;
      //if(bread>0) break;
    } while(bread<=0);

    if (bread == SOCKET_ERROR){
      ping_error("ping(), recvfrom failed."); return -1;}

    next=decode_resp(recvbuf,bread,&from);

    if(next>=0){
#ifdef WIN32
      /*Sleep(1000);*/ return next;
#else
      /*sleep(1);*/ return next;
#endif
    }
    
  }
  return 0;
};

#endif



// Copyright (c) by valixxx1 (2024)

#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <netdb.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t  i8;

u16 checksum(void *b, i32 len)
{
    u16 *buf = b;
    u32 sum = 0;

    for (;len > 1; len -= 2)
        sum += *(buf++);
    if (len == 1)
        sum += *(u8*) buf;

    return ~((sum >> 16) + (sum & 0xffff) + \
    (((sum >> 16) + (sum & 0xffff)) >> 16));
}

void send_pkt(int fd, void *buf, struct icmphdr *icmph, struct sockaddr_in dest)
{
  int sent = sendto(fd, buf, sizeof(*icmph), 0, (struct sockaddr*) &dest, sizeof(dest));
  if (sent == -1) {
    puts("Data wasn't sent!");
  } else {
    printf("Data was sent... ");
  }
}

char* getipbydom(char dom[])
{
  struct hostent *he = gethostbyname(dom);
  if (!he) {
    fputs("Domain doesn't exist!\n", stderr);
    exit(-1);
  }
  struct in_addr **addr_list = (struct in_addr**) he->h_addr_list;
  char *buf = (char*) malloc(16);
  if (!buf) {
    fputs("No memory!\n", stderr);
    exit(-1);
  }
  strcpy(buf, inet_ntoa(*addr_list[0]));
  return buf;
}

#define reset(buf) memset(&buf, 0, sizeof(buf))

void fill_sockaddr_in(struct sockaddr_in *addr, char *ip) {
  reset(*addr);
  addr->sin_family = AF_INET;
  int inet_ptoned = inet_pton(AF_INET, ip, &addr->sin_addr.s_addr);
  if (!inet_ptoned) {
    char *new_ip = getipbydom(ip);
    inet_pton(AF_INET, new_ip, &addr->sin_addr.s_addr);
    printf("PING sending to %s\n", new_ip);
  } else if (inet_ptoned == -1) {
    puts("IP isn't correct!");
    exit(-1);
  } else {
    printf("PING sending to %s\n", ip);
  }
}

#define fill_icmphdr(icmph)                            \
  do {                                                 \
    icmph->type = 8;                                   \
    icmph->code = 0;                                   \
    icmph->checksum = 0;                               \
    icmph->un.echo.sequence = rand();                  \
    icmph->un.echo.id = getpid();                      \
    icmph->checksum = checksum(icmph, sizeof(*icmph)); \
  } while (0)

int get_ttl(int fd)
{
  unsigned char buf[1024];
  struct sockaddr_in reply;
  socklen_t reply_len = sizeof(reply);
  ssize_t read_bytes = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *) &reply, &reply_len);
  if (read_bytes <= 0) {
    return -1;
  }

  struct iphdr *iph = (struct iphdr*) buf;
  return iph->ttl;
}

int main(int argc, char *argv[])
{
  if (argc < 2) {
    fputs("Too few arguments!\n", stderr);
    return -1;
  }

  int fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (fd == -1) {
    puts("Run with sudo!");
    return -1;
  }

  struct sockaddr_in dest;
  fill_sockaddr_in(&dest, argv[1]);

  char buf[64];
  reset(buf);

  struct icmphdr *icmph = (struct icmphdr*) buf;
  fill_icmphdr(icmph);

  int seq = 1, ttl;
  struct timeval start, end;
  long mtime, seconds, useconds;

  for (;;) {
    gettimeofday(&start, 0);

    send_pkt(fd, buf, icmph, dest);
    ttl = get_ttl(fd);

    gettimeofday(&end, 0);

    seconds = end.tv_sec - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;
    mtime = (seconds * 100 + useconds / 1000.0) + 0.5;

    printf("ttl = %d, seq = %d, time=%ldms\n", ttl, seq++, mtime);
    usleep(1000000); /* 1 second */
  }

  close(fd);
  return 0;
}

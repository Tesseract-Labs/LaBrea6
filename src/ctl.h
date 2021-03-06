/*
 * ctl.h
 *
 *  Created on: May 14, 2010
 *      Author: slezicz, slezicz@gmail.com
 */

#ifndef CTL_H_
#define CTL_H_

#include <signal.h>
#include <netinet/in.h>
#include <netinet/ip6.h>
/* Main control structure */
#define TCP_PORTS_COUNT 65535
#define DEFAULT_ADDRESSES_ARR_LENGHT 1024
struct ctl_s
{

  /* Controlling arrays and structures */
  uint8_t tcpport_filter;
  uint8_t port_array[TCP_PORTS_COUNT]; /* 1 byte / port to monitor */

  /* globals */
  //char cfg_file_name[BUFSIZE]; /* Configuration file name */
  int debuglevel; /* Level of debug output */

  /* capture performance */
  uint32_t throttlesize; /* Window size for incoming tcp sessions */
  uint32_t currentbytes; /* # bytes transmitted this minute */
  uint32_t sequence; /* initial seq no. */
  /* capture range */

  uint8_t ipaddr_filter;
  struct in6_addr *addresses_arr; /*  addr in subnet */
  uint64_t addresses; /* # addr in array */
  uint64_t addresses_arr_size; /* size of addresses array*/
  /* flags */

  uint16_t mode;
  /* -T */
#define FL_TESTMODE             0x0001
  /* -d */
#define FL_DONT_DETACH          0x0002

  volatile sig_atomic_t signals;
#define SIG_RESTART             0x0001
#define SIG_QUIT                0x0002
#define SIG_TIMER               0x0004
#define SIG_TOGGLE_LOGGING      0x0008
};

typedef struct ctl_s ctl_t;
extern ctl_t ctl;

/* Initialize ctl.addresses_arr and set ip/port filtering off */
void
init_ctr();

/* Opens the file with IPv6 addresses and parse them into the array ctl.addresses_arr */
void
parse_addr_file(char *filename);

/* Parse the port string and sets the PORT_MONITOR flag in the port_array the selected ports */
void
parse_ports(char *ports);

/* This function is used to decide if the address in the parametr is under
 * the observation of LaBrea6 or not.
 *
 * RETURN:
 * TRUE - in case the ip6 is address that we care about
 * FALSE - in case we ingnoring ip6 address
 */
int
filter_check_ip(const struct ip6_hdr *ip6);
/*
 * This funtion is used to decide on witch ports LaBrea6 reacts for communication.
 * If port in the ctl.port_array is set to PORT_MONITORED than it is port were listening at.
 *
 * RETURN:
 * TRUE - if ctl.port_array[port] == PORT_MONITORED
 * FALSE - in other cases
 */
int
filter_check_port(u_int16_t port);
#endif /* CTL_H_ */

/*
 * labrea6_init.c
 *
 *  Created on: May 13, 2010
 *      Author: slezicz, slezicz@gmail.com
 */
#include <getopt.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <err.h>
#include <unistd.h>
#include "labrea6.h"
#include "lbio.h"
#include "ctl.h"
#include "utils.h"

void
print_usage()
{
  warnx("%s", "\n\n\nUsage: LaBrea6 <options>\n"
    "--device (-d) interface : sets nondefault network interface\n"
    "--my-ip-addr (-I) ipaddr :  IP address of this system\n"
    "--ip-addrs (-a) filename : file with list of ipaddrs to capture\n"
    "--ports (-p) ports : list of ports listening to connection\n"
    "--syslog (-s) : enable logging to syslog\n"
    "--quiet (-q) : do not log\n"
    "--normal (-n) : do not report network info\n"
    "--verbose (-v) : default report level\n"
    "--very-verbose (-V) : very verbose report includes ND\n"
    "--help (-h), --usage (-u) : This message\n\n");
}

void
labrea_init(int argc, char **argv)
{
  u_char dev[] = "eth0"; /* default network device */
  u_char texpr[] = "ip6 and not pim"; /* default network bpf filter */

  int c = 0; /* Index for getopt */
  ctl.ipaddr_filter = FALSE;
  io.man_host_info = FALSE;
  struct in6_addr tmp_addr;

  /* decl for getopt_long */

  int option_index = 0; /* Option index */
  //n:m:i:j:I:E:qF:t:r:sXxhRHp:bPaflvoOVTdz?2:3D
  static struct option long_options[] =
    {
      { "device", required_argument, 0, 'i' },
      { "my-ip-addr", required_argument, 0, 'I' },
      { "ip-addrs-file", required_argument, 0, 'a' },
      { "ports", required_argument, 0, 'p' },
      { "syslog", no_argument, 0, 's' },
      { "quiet", no_argument, 0, 'q' },
      { "normal", no_argument, 0, 'n' },
      { "verbose", no_argument, 0, 'v' },
      { "very-verbose", no_argument, 0, 'V' },
      { "help", no_argument, 0, 'h' },
      { "usage", no_argument, 0, 'u' },
      { 0, 0, 0, 0 } };

  init_ctr();

  if (argc == 1)
    {
      print_usage();
      util_clean_exit(0);
    }

  while (TRUE)
    {
      c = getopt_long(argc, argv, "i:I:a:p:sqnvVvhu", long_options,
          &option_index);
#ifdef DEBUG
      warnx("Next option %c, %s", c, optarg);
#endif
      if (c == EOF) /* If at end of options, then stop */
        break;
      switch (c)
        {
      case 'i': /* setup the network device */
        strlcpy(dev, optarg, sizeof(dev));
        /* lbio_init will do further checking */
        break;

      case 'I': /* set MYIP that we can ignore trafic*/

        if ((inet_pton(AF_INET6, optarg, &tmp_addr) < 0))
          {
            warnx(
                "*** IP addr should be specified as xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx or xxxx::xxxx");
            warnx(
                "*** Labrea6 won't be able to filter trafic to the computer that is running on\n"
                  "It can cause duplicates in communication.");
            io.man_host_info = FALSE;
          }
        else
          {
            io.myip = tmp_addr;
            io.man_host_info = TRUE;
          }
        break;
      case 'a': /* file with ipadresses that we will monitor */
        parse_addr_file(optarg);
        ctl.ipaddr_filter = TRUE;
        break;
      case 'p': /* string with list of ports that we will listen for communication. */
        /* single ports or range of ports are separated by ','. The range is defined as <lower port no.>-<upper port no.>.
         * example: 23,25,30-41,55,90-120
         */
        parse_ports(optarg);
        ctl.tcpport_filter = TRUE;
        break;
      case 's': /* log to syslog */
        outp.syslog_open = TRUE;
        break;
      case 'q': /* quiet */
        outp.verbose = QUIET;
        break;
      case 'n': /* normal */
        outp.verbose = NORMAL;
        break;

      case 'v':
        /* verbose*/
        outp.verbose = VERBOSE;
        break;
      case 'V':
        /* very vebose*/
        outp.verbose = VERY_VERBOSE;
        break;
      case 'h':
      case 'u':
        print_usage();
        util_clean_exit(0);
        break;
      default:
        print_usage();
        util_clean_exit(0);
        break;
        }

    }
  lbio_init(dev, texpr); /* initialize pcap device */
}


/*
 * pkt.c
 *
 *  Created on: May 17, 2010
 *      Author: slezicz, slezicz@gmail.com
 */
#include <sys/types.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>
#include <linux/if_ether.h>
#include <netinet/ip6.h>

#ifndef UTILS_H_
#include "utils.h"
#endif

#ifndef LABREA6_H_
#include "labrea6.h"
#endif

#include "pkt.h"

/*
 * Create a new packet. Allocate mem.
 */

u_char*
new_packet(int len)
{
  u_char *new;

  if ((new = calloc(sizeof(u_char), len)) == NULL)
    {
      warnx("*** Error allocating new packet");
      util_clean_exit(1);
    }
  return new;
}

/*
 * Allocate the memory for the copy. Create a copy of the packet.
 */
u_char*
copy_packet(const u_char *packet, int len)
{
  u_char* new_pkt = new_packet(len);
  memcpy(new_pkt, packet, len);
  return new_pkt;
}

/*
 * Allocate the memory for the copy. Create a copy of the ETH and IP headers.
 *
 */
u_char*
copy_eth_ip_hdr(const u_char *packet, int len)
{
  u_char* new_pkt = new_packet(len);
  memcpy(new_pkt, packet, ETH_HLEN + IPV6_HLEN);
  return new_pkt;
}

/*
 * Append the next protocol to the new packet.
 */
u_char*
append_next_header(const u_char *header, u_char *packet_dst, int ofset,
    int len, int protocol_type)
{
  if (header == NULL)
    return NULL;
  struct ip6_hdr* ip6 = (struct ip6_hdr*) (packet_dst + ETH_HLEN);

  ip6->ip6_nxt = protocol_type;
  memcpy(packet_dst + ofset, header, len);
  return packet_dst + ofset;
}

/*
 * Skip ipv6 ext headers in packet.
 * returns pointer to next protocol header in case it is TCP or ICMPv6, otherwise NULL.
 * Returns number of the next protocol in protocol_type.
 *
 */

u_char*
next_proto_position(const u_char *packet, uint8_t *protocol_type)
{
  struct ip6_hdr* ip6 = (struct ip6_hdr*) (packet + ETH_HLEN);

  // point to first potential ext header
  struct ip6_ext* generic = (struct ip6_ext*) (packet + ETH_HLEN + IPV6_HLEN);

  int nxt = ip6->ip6_nxt;

  /* ipv6 ext header order: RFC2460
   *  hop by hop                0
   *  Destination Options       60
   *  Routing header            43
   *  Fragmentation header      44
   *  No next header            59
   */

  while (TRUE) /* go through the extension headers and skip them */
    {
      // hop-by-hop
      if ((nxt == IPV6_HOP_BY_HOP) || (nxt == IPV6_FRAGMENTATION_HDR) || (nxt
          == IPV6_ROUTING_HDR) || (nxt == IPV6_DESTINATION_HDR))
        {
#ifdef DEBUG
          warnx("before generic = 0x%x", generic);
#endif

          nxt = generic->ip6e_nxt;
          // lenght of the extension header is in octets
          generic = (struct ip6_ext*) (((char *)generic + ((generic->ip6e_len)
              + 1) * 8));
#ifdef DEBUG
          warnx("generic = 0x%x, len = %d", generic, ((generic->ip6e_len) + 1)
              * 4);
#endif
          continue;
        }

      break;
    }
  //If next headers are TCP or ICMPv6  return pointer to the header
  if (nxt == IPPROTO_TCP || nxt == IPPROTO_ICMPV6)
    {
      // let us know what kind of protocol we're pointing at
      *protocol_type = nxt;
      return (u_char*) generic;
    }
  *protocol_type = nxt;
  return NULL;
}

/*
 * Free mem.
 */
void
free_packet(u_char* packet)
{
  MYFREE(packet);
}

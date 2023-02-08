/*
  Pacman Arena
  Copyright (C) 2003 Nuno Subtil

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

/* $Id: net.h,v 1.1 2003/11/27 22:11:57 nsubtil Exp $ */

#ifndef _NET_H
#define _NET_H

extern int server_running;
extern int client_running;

/* new connection request */
#define PTYPE_REQ_NEWCONN 1
struct packet_newconn
{
};

/* connection accepted */
#define PTYPE_REPLY_CONNOK 2
struct packet_r_connok
{
	int player_id;
};

#define NET_PACMAN_PORT 1203
#define NET_MAX_PACKET_SIZE 10

void net_server_init(void);
void net_client_init(char *hostname);
void net_wait_packet(UDPsocket sock, UDPpacket *pack);
void net_send_packet(UDPsocket sock, Uint8 ptype, void *pdata, IPaddress dest);
void net_encode(UDPpacket *pack, Uint8 ptype, void *pdata);
int net_decode_ptype(UDPpacket *pack);
void net_decode_pdata(UDPpacket *pack, void *data);
void net_pack_byte(UDPpacket *pack, Uint8 value);
void net_pack_int32(UDPpacket *pack, Uint32 value);
void net_server_update(void);
void net_client_update(void);

#endif

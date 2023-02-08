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
#ifdef NETWORKING_GAME

static const char cvsid[] = 
  "$Id: net.c,v 1.4 2003/11/27 22:39:11 nsubtil Exp $";

#include "config.h"

#include <SDL_net.h>
#include <stdlib.h>

#include "audio.h"
#include "net.h"

static UDPsocket sock = NULL;
static IPaddress srv_addr;
static UDPpacket *packet = NULL;

int server_running = 0;
int client_running = 0;

/*
  starts a listener for the network server
*/
void net_server_init(void)
{
	if(sock == NULL)
		SDLNet_Init();

	sock = SDLNet_UDP_Open(NET_PACMAN_PORT);
	if(sock == NULL)
	{
		printf("net_server_init: SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		SDL_Quit();
		exit(1);
	}

	if(packet == NULL)
	{
		packet = SDLNet_AllocPacket(NET_MAX_PACKET_SIZE);
		if(packet == NULL)
		{
			printf("net_send_packet: SDLNet_AllocPacket: %s\n", SDLNet_GetError());
			SDL_Quit();
			exit(1);
		}
	}

	printf("net_server_init: listening on port %d\n", NET_PACMAN_PORT);
	server_running = 1;
}

/*
  connects to a server on a given host
*/
void net_client_init(char *hostname)
{
	struct packet_r_connok p;
	
	if(packet == NULL)
	{
		SDLNet_Init();
		packet = SDLNet_AllocPacket(NET_MAX_PACKET_SIZE);
		if(packet == NULL)
		{
			printf("net_send_packet: SDLNet_AllocPacket: %s\n", SDLNet_GetError());
			SDL_Quit();
			exit(1);
		}
	}

	sock = SDLNet_UDP_Open(0);
	if(sock == NULL)
	{
		printf("net_client_init: SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		SDL_Quit();
		exit(1);
	}

	if(SDLNet_ResolveHost(&srv_addr, hostname, NET_PACMAN_PORT) == -1)
	{
		printf("net_client_init: SDLNet_ResolveHost(%s): %s\n", hostname, SDLNet_GetError());
		SDL_Quit();
		exit(1);
	}

	if(SDLNet_UDP_Bind(sock, 0, &srv_addr) == -1)
	{
		printf("net_client_init: SDLNet_UDP_Bind: %s\n", SDLNet_GetError());
		SDL_Quit();
		exit(1);
	}

	printf("net_client_init: connecting to %s port %d...\n", hostname, NET_PACMAN_PORT);
	/* send a connection packet */
	net_send_packet(sock, PTYPE_REQ_NEWCONN, NULL, srv_addr);
	/* wait for reply */
	net_wait_packet(sock, packet);
	
	if(net_decode_ptype(packet) != PTYPE_REPLY_CONNOK)
	{
		printf("net_client_init: connection refused\n");
		SDL_Quit();
		exit(1);
	}
	
	net_decode_pdata(packet, &p);
	printf("net_client_init: connection established, id %d\n", p.player_id);
	
	client_running = 1;
}

/*
  waits for a packet
*/
void net_wait_packet(UDPsocket sock, UDPpacket *pack)
{
	while(SDLNet_UDP_Recv(sock, packet) != 1)
		SDL_Delay(1);
}

/*
  builds and sends a packet of a given type to a destination
*/
void net_send_packet(UDPsocket sock, Uint8 ptype, void *pdata, IPaddress dest)
{
	packet->len = 0;
	net_encode(packet, ptype, pdata);
	packet->address = dest;
	if(SDLNet_UDP_Send(sock, -1, packet) == 0)
	{
		printf("net_send_packet: SDLNet_UDP_Send: %s\n", SDLNet_GetError());
		SDL_Quit();
		exit(1);
	}
}

/*
  encodes a packet of a given type
*/
void net_encode(UDPpacket *pack, Uint8 ptype, void *pdata)
{
	/*
	  packet structure:
	  - packet type (1 byte)
	  - packet flags (1 byte)
	  - packet data (variable)
	*/

	/* packet type */
	net_pack_byte(pack, ptype);
	/* packet flags */
	net_pack_byte(pack, 0);

	switch(ptype)
	{
	case PTYPE_REQ_NEWCONN:
		/* nothing */
		break;

	case PTYPE_REPLY_CONNOK:
	{
		struct packet_r_connok *p;
		
		p = (struct packet_r_connok *)pdata;
		net_pack_int32(pack, p->player_id);
		
		break;
	}
	
	default:
		printf("net_send_packet: unknown ptype %d\n", ptype);
		SDL_Quit();
		exit(1);
	}
}

/*
  decodes the packet type
  returns the type of packet, or -1 on error
*/
int net_decode_ptype(UDPpacket *pack)
{
	switch(pack->data[0])
	{
	case PTYPE_REQ_NEWCONN:
		return PTYPE_REQ_NEWCONN;
		
	case PTYPE_REPLY_CONNOK:
		return PTYPE_REPLY_CONNOK;
		
	default:
		return -1;
	}
}

/*
  decodes the data in pack
*/
void net_decode_pdata(UDPpacket *pack, void *data)
{
	switch(pack->data[0])
	{
	case PTYPE_REQ_NEWCONN:
		return;
		
	case PTYPE_REPLY_CONNOK:
	{
		struct packet_r_connok *p;
		
		p = (struct packet_r_connok *)data;

		p->player_id = SDLNet_Read32(&pack->data[2]);
		
		return;
	}
	}
}

/*
  packs a byte on the end of a packet
*/
void net_pack_byte(UDPpacket *pack, Uint8 value)
{
	if(pack->len + 1 > pack->maxlen)
	{
		printf("net_pack_byte: packet too small (%d)\n", pack->maxlen);
		SDL_Quit();
		exit(1);
	}

	pack->data[pack->len] = value;
	pack->len++;
}

/*
  packs a 32-bit value on the end of a packet
*/
void net_pack_int32(UDPpacket *pack, Uint32 value)
{
	if(pack->len + 4 > pack->maxlen)
	{
		printf("net_pack_int32: packet too small (%d)\n", pack->maxlen);
		SDL_Quit();
		exit(1);
	}
	
	SDLNet_Write32(value, &pack->data[pack->len]);
	pack->len += 4;
}

/*
  updates the server
*/
void net_server_update(void)
{
	while(SDLNet_UDP_Recv(sock, packet) == 1)
	{
		switch(net_decode_ptype(packet))
		{
		case PTYPE_REQ_NEWCONN:
		{
			struct packet_r_connok p = {1};

			printf("net_server_update: new connection from %s\n", SDLNet_ResolveIP(&packet->address));
			audio_play_sample("sfx/ghost-return.wav");
			net_send_packet(sock, PTYPE_REPLY_CONNOK, &p, packet->address);
		}
		}
	}
}

/*
  updates the client
*/
void net_client_update(void)
{
}

#endif

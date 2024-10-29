/** @file t4u_addr.cpp Implements T4U address structure 
 *
 */

#include <netinet/ip.h>
#include <cstring>
#include <string>
#include "t4u_addr.h"
#include "t4u_errors.h"

T4U_Addr::T4U_Addr(uint32_t addr, uint16_t port)
{
    this->addr = addr;
    this->port = port;
    data = new unsigned char[MAX_PACKET_SIZE];
    read_len = 0;
    set_socket();
    return;
}

T4U_Addr::~T4U_Addr()
{
    delete[] data;
    return;
}

int32_t T4U_Addr::parse_addr(const std::string &addr_str)
{
    int curr_len;
    ssize_t total_len;
    uint32_t full_addr;
    uint32_t curr_num;
    uint32_t addr_bytes[4];
    ssize_t base_len;
    int read_count;
    
    const char *addr_c = addr_str.c_str();

    base_len = strlen(addr_c);
    total_len = 0;

    read_count = sscanf(addr_c, " %u.%u.%u.%u %n", &addr_bytes[0], &addr_bytes[1],
	   &addr_bytes[2], &addr_bytes[3], &curr_len);

    if (read_count != 4)
    {
	addr = 0;
	port = 0;
	return T4U_ERR_NARG;
    }

    total_len += curr_len;

    // Validate range
    addr = 0;
    for (uint32_t byte_idx = 0; byte_idx < 4; byte_idx++)
    {
	addr = addr << 8;
	if (addr_bytes[byte_idx] > 255)
	{
	    addr = 0;
	    port = 0;
	    return T4U_ERR_RANGE;
	}
	addr = addr + addr_bytes[byte_idx];
    }

    read_count = sscanf(addr_c + total_len, "%u", &curr_num);

    if (read_count != 1)
    {
	addr = 0;
	port = 0;
	return T4U_ERR_NARG;
    }

    if (curr_num > 65535)
    {
	addr = 0;
	port = 0;
	return T4U_ERR_RANGE;
    }

    port = curr_num;

    set_socket();
    return T4U_ERR_OK;
}

std::string T4U_Addr::get_line()
{
    std::string ret_line;
    uint32_t be_addr = htonl(addr);
    uint8_t *addr_byte = reinterpret_cast<uint8_t *>(&be_addr);;

    for (uint32_t byte_idx = 0; byte_idx < 4; byte_idx++)
    {
	ret_line.append(std::to_string(*(addr_byte+byte_idx)));
	if (byte_idx < 3)
	{
	    ret_line.append(".");
	}
    }

    ret_line.append(" ").append(std::to_string(port));
    return ret_line;
}
    

void T4U_Addr::set_socket()
{
    memset(&dest_socket, 0, sizeof(dest_socket));
    dest_socket.sin_family = AF_INET;
    dest_socket.sin_port = htons(port);
    dest_socket.sin_addr.s_addr = htonl((127<<24)+(1<<0)); // Destination is always localhost
    return;
}

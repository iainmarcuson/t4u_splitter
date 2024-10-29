/** @file t4u_splitter.cpp Main functionality for demulitplexing
 *        T4U data streams to multiple ports on localhost.
 */

#include "t4u_addr.h"
#include "t4u_errors.h"
#include <string>
#include <cstdint>
#include <list>
#include <iostream>
#include <netinet/ip.h>
#include <cstdio>
#include <cstring>
#include <vector>

int32_t read_addresses(std::vector<T4U_Addr> &addr_list, FILE *addr_file)
{
    char *line_text;
    size_t base_size = 256;
    T4U_Addr *curr_address;

    line_text = new char[base_size];
    if (line_text == nullptr)
    {
	return T4U_ERR_MALLOC;
    }

    while(!feof(addr_file))
    {
	int32_t ret;
	getline(&line_text, &base_size, addr_file);
	curr_address = new T4U_Addr(0,0); // Create a new address object
	if (curr_address == nullptr)
	{
	    return T4U_ERR_MALLOC;
	}
	
	ret = curr_address->parse_addr(std::string(line_text));
	if (ret < 0)		// Error parsing the line
	{
	    std::cout << "Invalid config line: " << std::string(line_text) << std::endl;
	    delete curr_address; // Clear the address and try again
	    curr_address = nullptr;
	    continue;
	}
	addr_list.push_back(*curr_address); // Successful, so add to the list
    }

    return T4U_ERR_OK;
}

int main(int argc, char *argv[])
{
    int udp_socket;
    FILE *address_file;
    int32_t ret;
    struct sockaddr_in recv_socket_addr;
    const uint16_t T4U_PORT = 10101;
    std::vector<T4U_Addr> t4u_list;
    const int32_t MAX_PACKET_SIZE = 16384;
    unsigned char *curr_packet_data;
    struct sockaddr_in t4u_socket_addr;
    
    if (argc != 2)
    {
	std::cout << "Usage: t4u_splitter <address file>" << std::endl;
	return 1;
    }

    curr_packet_data = new unsigned char[MAX_PACKET_SIZE];
    if (curr_packet_data == nullptr)
    {
	std::cout << "Error allocating packet buffer." << std::endl;
	return 1;
    }
    
    // Load the address list file
    address_file = fopen(argv[1], "r");
    ret = read_addresses(t4u_list, address_file);
    if (ret < 0)
    {
	std::cout << "Fatal error " << ret << " parsing config file." << std::endl;
	return 1;
    }
    fclose(address_file);

    // Zero the data address
    memset(&t4u_socket_addr, 0, sizeof(t4u_socket_addr));
    
    // Initialize the listening socket
    memset(&recv_socket_addr, 0, sizeof(recv_socket_addr));
    recv_socket_addr.sin_family = AF_INET;
    recv_socket_addr.sin_port = htons(T4U_PORT);
    recv_socket_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces.
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (ret == -1)
    {
	std::cout << "Error creating T4U socket." << std::endl;
	return 1;
    }

    ret = bind(udp_socket, reinterpret_cast<sockaddr *>(&recv_socket_addr), sizeof(recv_socket_addr));
    if (ret == -1)
    {
	std::cout << "Error binding T4U socket." << std::endl;
	return 1;
    }

    // Now start the loop of redirecting packets
    while (1)
    {
	socklen_t addr_len;
	uint32_t recv_addr_num;
	bool found_dest;
	uint32_t t4u_idx;
	ssize_t read_len;

	addr_len = sizeof(recv_socket_addr);
	read_len = recvfrom(udp_socket, curr_packet_data, MAX_PACKET_SIZE, 0,
			    reinterpret_cast<sockaddr *>(&recv_socket_addr), &addr_len);

	// Get the address as a number for comparison
	recv_addr_num = ntohl(recv_socket_addr.sin_addr.s_addr); // Stored in host-order

	found_dest = false;
	for (t4u_idx = 0; t4u_idx < t4u_list.size(); t4u_idx++)
	{
	    if (t4u_list.at(t4u_idx).get_addr() == recv_addr_num)
	    {
		found_dest = true;
		break;
	    }
	}

	if (!found_dest)	// No match
	{
	    continue;		// Start over with a new packet
	}

	sendto(udp_socket, curr_packet_data, read_len, 0, reinterpret_cast<sockaddr *>(t4u_list[t4u_idx].get_socket_addr()), sizeof(sockaddr));

    }

    return 0;
}

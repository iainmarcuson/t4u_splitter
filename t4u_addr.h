/** @file t4u_addr.h Includes and defines for T4U addresses.
 *
 */

#include <netinet/ip.h>
#include <cstdint>
#include <string>

class T4U_Addr
{
public:
    T4U_Addr(uint32_t addr, uint16_t port);
    ~T4U_Addr();
    int32_t parse_addr(const std::string &addr_str);
    const int32_t MAX_PACKET_SIZE=16384;
    uint32_t get_addr(){ return addr; };
    uint16_t get_port(){ return port; };
    unsigned char * get_data(){ return data; };
    int32_t get_len() { return read_len; };
    std::string get_line();
    struct sockaddr_in *get_socket_addr() { return &dest_socket;} ;
    
private:
    uint32_t addr;
    uint16_t port;
    unsigned char *data;
    int32_t read_len;
    struct sockaddr_in dest_socket;
    void set_socket();
    
};


/** @file Basic tests of the T4U address library
 *
 */

#include "t4u_addr.h"
#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
    T4U_Addr test_socket(0,0);

    std::cout << "Output line below should be all zeros." << std::endl;
    std::cout << test_socket.get_line() << std::endl;
    std::cout << "************************" << std::endl;
    
    std::cout << "The line should now show an address 4.2.2.2:53" << std::endl;
    test_socket.parse_addr(std::string("4.2.2.2 53"));
    std::cout << test_socket.get_line() << std::endl;
    return 0;
}

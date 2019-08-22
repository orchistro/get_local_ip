#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <string.h> 
#include <errno.h>

#include <string>
#include <stdexcept>
#include <iostream>

static void* get_addr_pointer(const struct addrinfo* aAddrinfo)
{
    if (aAddrinfo->ai_family == AF_INET)
    {
        return &(((struct sockaddr_in *)(aAddrinfo->ai_addr))->sin_addr);
    }
    else
    {
        return &(((struct sockaddr_in6 *)(aAddrinfo->ai_addr))->sin6_addr);
    }
}

class LocalIp
{
    public:
        LocalIp(void)
        {
            char hostname[255];

            if (gethostname(hostname, sizeof(hostname)) != 0)
            {
                int32_t err = errno;
                fprintf(stderr, "gethostname() fail. errno:%d(%s)", err, strerror(err));
                throw std::runtime_error("gethostname() fail");
            }

            struct addrinfo hints;
            memset(&hints, 0, sizeof(struct addrinfo));
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_flags = 0;
            hints.ai_protocol = 0;
            hints.ai_canonname = nullptr;
            hints.ai_addr = nullptr;
            hints.ai_next = nullptr;

            struct addrinfo* res;

            if (int32_t rc = getaddrinfo(hostname, nullptr, &hints, &res); rc != 0)
            {
                fprintf(stderr, "getaddrinfo() fail. errno:%d(%s)", rc, gai_strerror(rc));
                throw std::runtime_error("getaddrinfo() fail");
            }
            else
            {
                char buffer[255];

                /*
                 * Use the first addr. no need to continue to the next
                 */
                if (const char* addr = inet_ntop(res->ai_family,
                                                 get_addr_pointer(res),
                                                 buffer,
                                                 sizeof(buffer)); addr == nullptr)
                {
                    freeaddrinfo(res);

                    int32_t err = errno;
                    fprintf(stderr, "inet_ntop() fail. errno:%d(%s)", err, strerror(err));
                    throw std::runtime_error("inet_ntop() fail");
                }
                else
                {
                    mAddrString = addr;
                }
            }

            freeaddrinfo(res);
        }

    public:
        std::string mAddrString;
};

const std::string& get_local_ip(void)
{
    static LocalIp ip;

    return ip.mAddrString;
}

int32_t main(void)
{
    std::cout << get_local_ip() << "\n";
    return 0;
}

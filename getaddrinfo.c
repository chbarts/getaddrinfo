#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

static int v6cmp(unsigned char *left, unsigned char *right)
{
    uint64_t lol, hil, lor, hir;

    hil = *((uint64_t *) left);
    hir = *((uint64_t *) right);

    if (hil > hir) {
        return 1;
    } else if (hil < hir) {
        return -1;
    }

    lol = *((uint64_t *) (left + (64 / 8)));
    lor = *((uint64_t *) (right + (64 / 8)));

    if (lol > lor) {
        return 1;
    } else if (lol < lor) {
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    struct addrinfo *p, *host, hints;
    struct sockaddr_in *sai;
    struct sockaddr_in6 *sai6;
    unsigned long addr, laddr;
    unsigned char *v6addr, *lv6addr;
    int i, j, res;

    if (argc < 2) {
        puts("usage: getaddrinfo name [names..]");
        return 0;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_CANONNAME | AI_V4MAPPED | AI_ADDRCONFIG;
    hints.ai_family = AF_UNSPEC;

    for (i = 1; i < argc; i++) {
      again:
        if ((res = getaddrinfo(argv[i], NULL, &hints, &host)) != 0) {
            if (res == EAI_AGAIN)
                goto again;

            fprintf(stderr, "getaddrinfo: error on %s: %s\n", argv[i],
                    gai_strerror(res));
            continue;
        }

        puts(argv[i]);
        laddr = 0;
        lv6addr = NULL;

        for (p = host; p != NULL; p = p->ai_next) {
            switch (p->ai_addr->sa_family) {
            case AF_INET:      /* IPv4 */
                sai = (struct sockaddr_in *) (p->ai_addr);
                addr = ntohl(sai->sin_addr.s_addr);
                if (laddr == addr)
                    break;
                for (j = 3; j >= 0; j--)
                    printf("%lu%s", 0xff & (addr >> (j * 8)),
                           j > 0 ? "." : " ");
                printf("\t");
                laddr = addr;
                break;
            case AF_INET6:     /* IPv6 */
                sai6 = (struct sockaddr_in6 *) p->ai_addr;
                v6addr = sai6->sin6_addr.s6_addr;
                if (lv6addr && !v6cmp(lv6addr, v6addr))
                    break;
                for (j = 0; j < 16; j += 2)
                    printf("%02x%02x%s", v6addr[j], v6addr[j + 1],
                           j < 14 ? ":" : " ");
                printf("\t");
                lv6addr = v6addr;
                break;
            default:           /* What? */
                fprintf(stderr, "%s: bizarre sa_family %u on %s\n", argv[0],
                        (unsigned int) p->ai_addr->sa_family, argv[i]);
                break;
            }

            if (p->ai_canonname) {
                puts(p->ai_canonname);
            } else {
               puts("");
            }
        }

        puts("");
        freeaddrinfo(host);
    }

    return 0;
}

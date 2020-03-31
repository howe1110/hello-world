#include "tx_worker.h"
#include <ws2tcpip.h>

class comworker;
class listen_worker : public tx_worker_base
{
private:
    /* data */
    std::string _listen_port;
    struct sockaddr_in *_listen_addr;
    SOCKET _lsocket; //监听socket
private:
    comworker *_comworker;

public:
    listen_worker(/* args */);
    ~listen_worker();

public:
    void setcommwoker(comworker *cwk);
    void handleconnect(SOCKET st);

public:
    void proc();
    void stop();
};

#include "tx_worker.h"
#include <sys/socket.h>
class comworker;
class listen_worker : public tx_worker_base
{
private:
    /* data */
    std::string _listen_port;
    struct sockaddr_in *_listen_addr;
    int _lsocket; //监听socket
private:
    comworker *_comworker;

public:
    listen_worker(/* args */);
    ~listen_worker();

public:
    void setcommwoker(comworker *cwk);
    void handleconnect(int st);

public:
    void proc();
    void stop();
};

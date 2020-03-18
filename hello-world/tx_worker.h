#include <string>
class CNode;
class tx_worker
{
private:
    /* data */
    std::string _name;
    CNode *_owner;

public:
    tx_worker(std::string n, CNode *o);
    ~tx_worker();
    public:
    void operator()()
    {
        Proc();
    }
    virtual void Proc() = 0;
};

tx_worker::tx_worker(std::string n, CNode *o):_name(n),_owner(o)
{
}

tx_worker::~tx_worker()
{
}

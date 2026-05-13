#pragma once

#include <deque>
#include <string>
#include <windows.h>

class Server;
struct Data
{
    Server *server;
    SOCKET client;
    int index;
};
void worker(Data *);

class Thread
{
public:
    Thread() = default;
    Thread(Server* server, int index)
    {
        data.server = server;
        data.index = index;;
    }

    void create();
    bool available() const;
    void enter(SOCKET &Client);
    void setFree();
    bool isFree() const;
    bool isDestroyed() const;
    void CloseThread();
    HANDLE getHandle() const;
    SOCKET getClient() const;
    void endServer();

    // extra function:
    std::string getActivity();
    void closeClient();
 
    int Send(const std::string& str) const;
    int Send(const int& var) const;
    int Send(const double& var) const;
    int Rec(std::string& receivingString);

    // Test support: lets unit tests drive code without real sockets.
    void enableTestMode(bool enabled = true) { testMode = enabled; }
    void testPushRecv(const std::string& s) { inbox.push_back(s); }
    std::string testPopSent()
    {
        if (outbox.empty())
            return {};
        std::string v = outbox.front();
        outbox.pop_front();
        return v;
    }
    size_t testSentCount() const { return outbox.size(); }

private:
    HANDLE handle = nullptr;
    Data data;
    bool free = true;
    bool destroyed = false;

    bool testMode = false;
    std::deque<std::string> inbox;
    mutable std::deque<std::string> outbox;
};
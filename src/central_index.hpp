#ifndef CENTRAL_INDEX_H
#define CENTRAL_INDEX_H

#include <mutex>
#include <string>
#include <list>


typedef struct {
    std::string hostname;
    int upload_port;
} ConnectedClient;


typedef struct {
    int rfc;
    std::string title;
    ConnectedClient *peer;
} RFCHolder;


class CentralIndex {

private:
    static std::list<ConnectedClient> clients;
    static std::mutex clients_mut;
    static std::list<RFCHolder> rfcs;
    static std::mutex rfcs_mut;

public:
    static bool add_client(std::string hostname, int upload_port);
    static bool remove_client(std::string hostname, int upload_port);

    static bool add_rfc(std::string hostname, int upload_port, int rfc, std::string title);
    static std::list<ConnectedClient> find_rfcs(int rfc);

};


#endif


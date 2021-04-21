#ifndef CENTRAL_INDEX_H
#define CENTRAL_INDEX_H

#include <mutex>
#include <string>
#include <list>


/* Represents a client connected to the CI */
typedef struct {
    std::string hostname;
    int upload_port;
} ConnectedClient;


/* Represents an association between a client and an RFC.
 * As in, a client has a local copy of an RFC. */
typedef struct {
    int rfc;
    std::string title;
    ConnectedClient *peer;
} RFCHolder;


/* Stores all of the data the central index needs to keep track of */
class CentralIndex {

private:
    static std::list<ConnectedClient> clients;
    static std::mutex clients_mut;
    static std::list<RFCHolder> rfcs;
    static std::mutex rfcs_mut;

public:
    /* Represents a client connecting to the CI */
    static bool add_client(std::string hostname, int upload_port);
    
    /* Represents a client disconnecting from the CI */
    static bool remove_client(std::string hostname, int upload_port);

    
    /* Represents a client stating they have an RFC */
    static bool add_rfc(std::string hostname, int upload_port, int rfc, std::string title);

    /* Finds all the clients that have a local copy of an RFC */
    static std::list<ConnectedClient> find_rfcs(int rfc);

};


#endif


#ifndef CENTRAL_INDEX_H
#define CENTRAL_INDEX_H

#include "common.hpp"

#include <mutex>
#include <string>
#include <list>


/* Stores all of the data the central index needs to keep track of */
class CentralIndex {

private:
    static std::list<Client> clients;
    static std::mutex clients_mut;
    static std::list<RFCHolder> rfcs;
    static std::mutex rfcs_mut;

public:
    /* Represents a client connecting to the CI */
    static bool add_client(std::string hostname, int upload_port);
    
    /* Represents a client disconnecting from the CI */
    static bool remove_client(std::string hostname, int upload_port);

    
    /* Represents a client stating they have an RFC */
    static bool add_rfc(std::string hostname, int upload_port, int rfc);

    /* Finds all the clients that have a local copy of an RFC */
    static std::list<Client> find_rfcs(int rfc);

    /* Gets all RFCs in the index */
    static std::list<RFCHolder> all_rfcs();

};


#endif


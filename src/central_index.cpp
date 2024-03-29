#include "central_index.hpp"

#include <mutex>
using std::mutex;
#include <string>
using std::string;
#include <list>
using std::list;

#include <iostream>


list<Client> CentralIndex::clients;
mutex CentralIndex::clients_mut;
list<RFCHolder> CentralIndex::rfcs;
mutex CentralIndex::rfcs_mut;


bool compare_rfc_holders(const RFCHolder &first, const RFCHolder &second) {
    if (first.rfc != second.rfc)
        return first.rfc < second.rfc;
    if (first.peer.host != second.peer.host)
        return first.peer.host < second.peer.host;
    return first.peer.port < second.peer.port;
}


bool CentralIndex::add_client(string hostname, int upload_port) {
    
    clients_mut.lock();
    for (auto client : clients) {
        if (client.host == hostname && client.port == upload_port) {
            clients_mut.unlock();
            return false;
        }
    }

    Client client{ hostname, upload_port };
    clients.push_back(client);
    
    clients_mut.unlock();
    return true;

}


bool CentralIndex::remove_client(string hostname, int upload_port) {

    rfcs_mut.lock();
    for (auto it = rfcs.begin(); it != rfcs.end(); ) {
        if (it->peer.host == hostname && it->peer.port == upload_port) {
            it = rfcs.erase(it);
        } else
            ++it;
    }
    rfcs_mut.unlock();

    clients_mut.lock();
    for (auto it = clients.begin(); it != clients.end(); it++) {
        if (it->host == hostname && it->port == upload_port) {
            it = clients.erase(it);
            break;
        }
    }
    clients_mut.unlock();

    return false;

}


bool CentralIndex::add_rfc(string hostname, int upload_port, int rfc) {

    rfcs_mut.lock();
    for (auto r : rfcs) {
        if (r.rfc == rfc && r.peer.host == hostname &&
                r.peer.port == upload_port) {
            rfcs_mut.unlock();
            return false;
        }
    }

    clients_mut.lock();
    Client peer;
    bool found = false;
    for (auto it = clients.begin(); it != clients.end(); it++) {
        if (it->host == hostname && it->port == upload_port) {
            peer = *it;
            found = true;
            break;
        }
    }
    if (!found) {
        clients_mut.unlock();
        rfcs_mut.unlock();
        return false;
    }

    RFCHolder r{ rfc, peer };
    rfcs.push_back(r);
    rfcs.sort(compare_rfc_holders);

    clients_mut.unlock();
    rfcs_mut.unlock();
    return true;

}


list<Client> CentralIndex::find_rfcs(int rfc) {

    rfcs_mut.lock();
    list<Client> l;
    for (auto it = rfcs.begin(); it != rfcs.end(); it++) {
        if (it->rfc == rfc) {
            l.push_back(it->peer);
        }
    }
    rfcs_mut.unlock();

    return l;

}


std::list<RFCHolder> CentralIndex::all_rfcs() {

    rfcs_mut.lock();
    list<RFCHolder> l;
    for (auto it = rfcs.begin(); it != rfcs.end(); it++) {
        l.push_back(*it);
    }
    rfcs_mut.unlock();
    return l;

}


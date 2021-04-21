#include "central_index.hpp"

#include <mutex>
using std::mutex;
#include <string>
using std::string;
#include <list>
using std::list;

#include <iostream>


list<ConnectedClient> CentralIndex::clients;
mutex CentralIndex::clients_mut;
list<RFCHolder> CentralIndex::rfcs;
mutex CentralIndex::rfcs_mut;



bool CentralIndex::add_client(string hostname, int upload_port) {
    
    clients_mut.lock();
    for (auto client : clients) {
        if (client.hostname == hostname && client.upload_port == upload_port) {
            clients_mut.unlock();
            return false;
        }
    }

    ConnectedClient client{ hostname, upload_port };
    clients.push_back(client);
    
    clients_mut.unlock();
    return true;

}


bool CentralIndex::remove_client(string hostname, int upload_port) {

    rfcs_mut.lock();
    for (auto it = rfcs.begin(); it != rfcs.end(); ) {
        if (it->peer->hostname == hostname && it->peer->upload_port == upload_port) {
            it = rfcs.erase(it);
        } else
            ++it;
    }
    rfcs_mut.unlock();

    clients_mut.lock();
    for (auto it = clients.begin(); it != clients.end(); it++) {
        if (it->hostname == hostname && it->upload_port == upload_port) {
            it = clients.erase(it);
            break;
        }
    }
    clients_mut.unlock();

    return false;

}


bool CentralIndex::add_rfc(string hostname, int upload_port,
        int rfc, string title) {

    rfcs_mut.lock();
    for (auto r : rfcs) {
        if (r.rfc == rfc && r.title == title && r.peer->hostname == hostname &&
                r.peer->upload_port == upload_port) {
            rfcs_mut.unlock();
            return false;
        }
    }

    clients_mut.lock();
    ConnectedClient *peer = nullptr;
    for (auto it = clients.begin(); it != clients.end(); it++) {
        if (it->hostname == hostname && it->upload_port == upload_port) {
            peer = &(*it);
            break;
        }
    }
    if (peer == nullptr) {
        clients_mut.unlock();
        rfcs_mut.unlock();
        return false;
    }

    RFCHolder r{ rfc, title, peer };
    rfcs.push_back(r);

    clients_mut.unlock();
    rfcs_mut.unlock();
    return true;

}


list<ConnectedClient> CentralIndex::find_rfcs(int rfc) {

    rfcs_mut.lock();
    list<ConnectedClient> l;
    for (auto it = rfcs.begin(); it != rfcs.end(); it++) {
        if (it->rfc == rfc) {
            l.push_back(*(it->peer));
        }
    }
    rfcs_mut.unlock();

    return l;

}


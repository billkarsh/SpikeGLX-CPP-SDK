#ifndef NETCLIENT_H
#define NETCLIENT_H

/* ---------------------------------------------------------------- */
/* Includes ------------------------------------------------------- */
/* ---------------------------------------------------------------- */

#include "Socket.h"

#include <vector>
using namespace std;

/* ---------------------------------------------------------------- */
/* NetClient ------------------------------------------------------ */
/* ---------------------------------------------------------------- */

class NetClient : public Socket
{
private:
    vector<char>    vbuf;   // response buffer
    uint            read_timeout_secs;

public:
    NetClient(
        const string    &host               = "localhost",
        uint16          port                = 0,
        uint            read_timeout_secs   = 10 );

    virtual uint sendData( const void *src, uint srcBytes ) noexcept(false);

    virtual uint receiveData(
        void    *dst,
        uint    dstBytes ) noexcept(false);

    uint sendString( const string &s ) noexcept(false);

    void rcvLine( vector<char> &line ) noexcept(false);
    bool rcvLines( vector<vector<char> > &vlines ) noexcept(false);
};

#endif  // NETCLIENT_H



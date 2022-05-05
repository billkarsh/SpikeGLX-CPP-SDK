#ifndef COMM_H
#define COMM_H

/* ---------------------------------------------------------------- */
/* Includes ------------------------------------------------------- */
/* ---------------------------------------------------------------- */

#include "SglxApi.h"
#include "NetClient.h"

#include <map>

/* ---------------------------------------------------------------- */
/* Types ---------------------------------------------------------- */
/* ---------------------------------------------------------------- */

typedef map<int,NetClient*> NetClientMap;

/* ---------------------------------------------------------------- */
/* Comm ----------------------------------------------------------- */
/* ---------------------------------------------------------------- */

class Comm
{
private:
    static NetClientMap m_clientMap;
    static int          m_nextHandle;

public:
    bool checkConn( t_sglxconn &S ) noexcept(false);
    void close( t_sglxconn &S ) noexcept(false);

    bool doCommand( t_sglxconn &S, const string &cmd ) noexcept(false);
    bool do1LineQuery( string &resp, t_sglxconn &S, const string &cmd, bool srvside ) noexcept(false);
    bool doNLineQuery( vector<string> &vs, t_sglxconn &S, const string &cmd, bool srvside ) noexcept(false);

    bool sendString( t_sglxconn &S, const string &s ) noexcept(false);
    bool read_1s_srvside( string &s, t_sglxconn &S ) noexcept(false);
    bool read_vs( vector<string> &vs, t_sglxconn &S, bool srvside ) noexcept(false);

    bool receiveOK( t_sglxconn &S, const string &cmd ) noexcept(false);
    bool receiveREADY( t_sglxconn &S, const string &cmd ) noexcept(false);

    bool sendBinary( t_sglxconn &S, const void *data, int nBytes ) noexcept(false);
    bool readBinary( void *data, int nBytes, t_sglxconn &S ) noexcept(false);

private:
    bool error( const char *msg ) noexcept(false);

    NetClient *mapFind( int handle );
    void mapPut( NetClient *client );
    void mapDestroy( int handle ) noexcept(false);

    int create( t_sglxconn &S );
    void destroy( t_sglxconn &S ) noexcept(false);

    bool tryConnection( int handle ) noexcept(false);
    void closeSocket( int handle );
};

#endif  // COMM_H



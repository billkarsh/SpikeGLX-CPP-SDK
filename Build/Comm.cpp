
#include "Comm.h"

#include <string.h>

/* ---------------------------------------------------------------- */
/* Statics -------------------------------------------------------- */
/* ---------------------------------------------------------------- */

NetClientMap    Comm::m_clientMap;
int             Comm::m_nextHandle = 0;

/* ---------------------------------------------------------------- */
/* Comm ----------------------------------------------------------- */
/* ---------------------------------------------------------------- */

/* ---------------------------------------------------------------- */
/* Public --------------------------------------------------------- */
/* ---------------------------------------------------------------- */

bool Comm::checkConn( t_sglxconn &S ) noexcept(false)
{
    if( S.in_checkconn )
        return true;

    S.in_checkconn = true;
    S.cpp_zer_str( S.err );

    bool    ok = true;

    if( S.handle == -1 ) {

        S.handle = create( S );

        if( !tryConnection( S.handle ) )
            return error( "checkConn: Unable to connect to server." );

        ok = do1LineQuery( S.vers, S, "GETVERSION", false );
    }
    else {

        try {
            string  s;
            sendString( S, "NOOP\n" );
            read_1s_srvside( s, S );
        }
        catch( const exception &e ) {

            if( !tryConnection( S.handle ) ) {
                S.in_checkconn = false;
                ok = error( "checkConn: Still unable to connect to server." );
            }
        }
    }

    S.in_checkconn = false;
    return ok;
}


void Comm::close( t_sglxconn &S ) noexcept(false)
{
    closeSocket( S.handle );
    destroy( S );
}


bool Comm::doCommand( t_sglxconn &S, const string &cmd ) noexcept(false)
{
    if( !checkConn( S ) )
        return false;

    sendString( S, cmd + "\n" );

    return receiveOK( S, cmd );
}


bool Comm::do1LineQuery( string &resp, t_sglxconn &S, const string &cmd, bool srvside ) noexcept(false)
{
    if( srvside )
        resp.clear();
    else
        S.cpp_zer_str( resp );

    if( !checkConn( S ) )
        return false;

    sendString( S, cmd + "\n" );

    vector<string>  vs;
    read_vs( vs, S, true );

    if( vs.size() ) {
        if( srvside )
            resp = vs[0];
        else
            S.cpp_ins_str( resp, vs[0].c_str() );
    }

    return true;
}


bool Comm::doNLineQuery( vector<string> &vs, t_sglxconn &S, const string &cmd, bool srvside ) noexcept(false)
{
    if( srvside )
        vs.clear();
    else
        S.cpp_zer_vstr( vs );

    if( !checkConn( S ) )
        return false;

    sendString( S, cmd + "\n" );

    return read_vs( vs, S, srvside );
}


bool Comm::sendString( t_sglxconn &S, const string &s ) noexcept(false)
{
    NetClient   *nc = mapFind( S.handle );

    if( !nc )
        return error( "sendString: Invalid handle." );

    uint    nsent = nc->sendString( s );

    if( !nsent )
        return error( "sendString: No chars sent." );

    return true;
}


// Fetch next line, excluding ERROR lines.
//
bool Comm::read_1s_srvside( string &s, t_sglxconn &S ) noexcept(false)
{
    NetClient   *nc = mapFind( S.handle );

    s.clear();

    if( !nc )
        return error( "read1Line: Invalid handle." );

    vector<char>    line;
    nc->rcvLine( line );
    s = &line[0];

    if( 0 == s.find( "ERROR", 0 ) )
        return error( &line[0] );

    return true;
}


// Fetch one or more lines, excluding {OK, ERROR} lines.
//
bool Comm::read_vs( vector<string> &vs, t_sglxconn &S, bool srvside ) noexcept(false)
{
    NetClient   *nc = mapFind( S.handle );

    if( srvside )
        vs.clear();
    else
        S.cpp_zer_vstr( vs );

    if( !nc )
        return error( "readNLines: Invalid handle." );

    vector<char>    line;

    for(;;) {

        nc->rcvLine( line );

        if( line.size() ) {

            const char  *s = &line[0];

            if( !strcmp( s, "OK" ) )
                break;

            if( !strncmp( s, "ERROR", 5 ) )
                return error( s );

            if( srvside )
                vs.push_back( s );
            else
                S.cpp_ins_vstr( vs, s );
        }
    }

    return true;
}


bool Comm::receiveOK( t_sglxconn &S, const string &cmd ) noexcept(false)
{
    string  s;
    read_1s_srvside( s, S );

    if( strcmp( s.c_str(), "OK" ) ) {
        s = "receiveOK: Missing OK following [" + cmd + "].";
        return error( s.c_str() );
    }

    return true;
}


bool Comm::receiveREADY( t_sglxconn &S, const string &cmd ) noexcept(false)
{
    string  s;
    read_1s_srvside( s, S );

    if( strcmp( s.c_str(), "READY" ) ) {
        s = "receiveREADY: Missing READY following [" + cmd + "].";
        return error( s.c_str() );
    }

    return true;
}


bool Comm::sendBinary( t_sglxconn &S, const void *data, int nBytes ) noexcept(false)
{
    NetClient   *nc = mapFind( S.handle );

    if( !nc )
        return error( "sendBinary: Invalid handle." );

    uint    nsent = nc->sendData( data, nBytes );

    if( !nsent )
        return error( "sendBinary: No bytes sent." );

    return true;
}


bool Comm::readBinary( void *data, int nBytes, t_sglxconn &S ) noexcept(false)
{
    NetClient   *nc = mapFind( S.handle );

    if( !nc )
        return error( "readBinary: Invalid handle." );

    uint nrcv = nc->receiveData( data, nBytes );

    if( !nrcv )
        return error( "readBinary: No bytes received." );

    return true;
}

/* ---------------------------------------------------------------- */
/* Private -------------------------------------------------------- */
/* ---------------------------------------------------------------- */

bool Comm::error( const char *msg ) noexcept(false)
{
    throw std::runtime_error( msg );
    return false;
}


NetClient *Comm::mapFind( int handle )
{
    NetClientMap::iterator  it = m_clientMap.find( handle );

    if( it == m_clientMap.end() )
        return 0;

    return it->second;
}


void Comm::mapPut( NetClient *client )
{
    NetClient   *old = mapFind( ++m_nextHandle );

    if( old )
        delete old;

    m_clientMap[m_nextHandle] = client;
}


void Comm::mapDestroy( int handle ) noexcept(false)
{
    NetClientMap::iterator  it = m_clientMap.find( handle );

    if( it != m_clientMap.end() ) {

        delete it->second;
        m_clientMap.erase( it );
    }
    else
        error( "MapDestroy: Invalid or unknown handle." );
}


int Comm::create( t_sglxconn &S )
{
    mapPut( new NetClient( S.cpp_get_str(S.host), S.port ) );
    return m_nextHandle;
}


void Comm::destroy( t_sglxconn &S ) noexcept(false)
{
    mapDestroy( S.handle );
    S.handle = -1;
}


bool Comm::tryConnection( int handle ) noexcept(false)
{
    NetClient   *nc = mapFind( handle );

    if( !nc )
        return error( "tryConnection: Invalid handle." );

    return nc->tcpConnect();
}


void Comm::closeSocket( int handle )
{
    NetClient   *nc = mapFind( handle );

    if( nc )
        nc->tcpDisconnect();
}



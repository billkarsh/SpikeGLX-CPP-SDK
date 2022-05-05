
#include "NetClient.h"

#include <string.h>




NetClient::NetClient(
    const string    &host,
    uint16          port,
    uint            read_timeout_secs )
    :   Socket(TCP), read_timeout_secs(read_timeout_secs)
{
    setHost( host );
    setPort( port );
}


uint NetClient::sendData( const void *src, uint srcBytes ) noexcept(false)
{
    static const uint maxsend = 2*1024*1024;

    vbuf.clear();   // clear response

    if( !srcBytes )
        return 0;

    uint        sent = 0;
    const char  *buf = static_cast<const char*>(src);

    while( sent < srcBytes ) {

        uint    tosend = srcBytes - sent;

        if( tosend > maxsend )
            tosend = maxsend;

        sent += Socket::sendData( buf + sent, tosend );
    }

    return sent;
}


uint NetClient::receiveData( void *dst, uint dstBytes ) noexcept(false)
{
    static const uint maxrecv = 2*1024*1024;

    if( !dstBytes )
        return 0;

    ulong   recvd   = 0,
            nB      = ulong(vbuf.size());
    char    *buf    = static_cast<char*>(dst);

    if( nB ) {

        recvd = nB;

        if( recvd > dstBytes )
            recvd = dstBytes;

        memcpy( buf, &vbuf[0], recvd );

        if( (nB -= recvd) > 0 )
            memcpy( &vbuf[0], &vbuf[recvd], nB );

        vbuf.resize( nB );
    }

    while( recvd < dstBytes ) {

        uint    retval,
                nR = nReadyForRead();

        if( nR ) {

            if( nR > dstBytes - recvd )
                nR = dstBytes - recvd;

            if( nR > maxrecv )
                nR = maxrecv;

            retval = Socket::receiveData( buf + recvd, nR );
            recvd += retval;

            if( retval == 0 )
                break;
        }
        else if( !waitData( 1000 * read_timeout_secs ) )
            throw std::runtime_error("receiveData: Receive timed out.");
        else if( !nReadyForRead() ) {

            // In this failure mode...
            // flush out garbage byte for next time
            char    dump[4];
            Socket::receiveData( dump, 1 );
            throw std::runtime_error("receiveData: Data transfer interrupted.");
        }
    }

    return recvd;
}


uint NetClient::sendString( const string &s ) noexcept(false)
{
    vbuf.clear();   // clear response

    return Socket::sendData( s.data(), uint(s.length()) );
}


// Return one line; newline stripped, null-terminated.
//
void NetClient::rcvLine( vector<char> &line ) noexcept(false)
{
    line.clear();

    for(;;) {

        char    *v0, *vN;
        uint    nB = uint(vbuf.size());

        // ------------------
        // Is '\n' in buffer?
        // ------------------

        if( nB && (vN = (char*)memchr( v0 = &vbuf[0], '\n', nB )) ) {

            // -------------
            // Copy data out
            // -------------

            *vN++ = 0;  // convert '\n' to null

            uint    nL = uint(vN - v0);

            line.resize( nL );
            memcpy( &line[0], v0, nL );

            // -----------------------
            // Remove line from buffer
            // -----------------------

            if( (nB -= nL) > 0 )
                memcpy( v0, vN, nB );

            vbuf.resize( nB );
            break;
        }

        // -----------------------
        // Else, buffer more chars
        // -----------------------

        uint    nR = nReadyForRead();

        if( nR ) {

            vbuf.resize( nB + nR );
            Socket::receiveData( &vbuf[nB], nR );
        }
        else if( !waitData( 1000 * read_timeout_secs ) ) {

            // nothing to read - quit.
            line.push_back( 0 );
            throw std::runtime_error("rcvLine: Receive timed out.");
        }
        else {

            // should be something to read after the wait

            uint    nR = nReadyForRead();

            if( nR ) {
                vbuf.resize( nB + nR );
                Socket::receiveData( &vbuf[nB], nR );
            }
            else {
                // In this failure mode...
                // flush out garbage character for next time
                vbuf.resize( nB + 1 );
                Socket::receiveData( &vbuf[nB], 1 );
                throw std::runtime_error("rcvLine: Data transfer interrupted.");
            }
        }
    }
}


// Return true if OK received, data excludes OK.
// Return false if ERROR received, ERROR line is thrown.
//
bool NetClient::rcvLines( vector<vector<char> > &vlines ) noexcept(false)
{
    vlines.clear();

    vector<char>    line;

    for(;;) {

        rcvLine( line );

        if( line.size() ) {

            const char  *s = &line[0];

            if( !strcmp( s, "OK" ) )
                return true;

            if( !strncmp( s, "ERROR", 5 ) ) {
                throw std::runtime_error( (string("rcvLines: ") + s).c_str() );
                return false;
            }

            vlines.push_back( line );
        }
    }
}



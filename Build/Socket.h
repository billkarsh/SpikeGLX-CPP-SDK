#ifndef SOCKET_H
#define SOCKET_H

/* ---------------------------------------------------------------- */
/* Includes ------------------------------------------------------- */
/* ---------------------------------------------------------------- */

#include <stdexcept>
#include <string>

struct sockaddr_in;

/* ---------------------------------------------------------------- */
/* Types ---------------------------------------------------------- */
/* ---------------------------------------------------------------- */

typedef unsigned short  uint16;
typedef unsigned int    uint;
typedef unsigned long   ulong;

/* ---------------------------------------------------------------- */
/* Socket --------------------------------------------------------- */
/* ---------------------------------------------------------------- */

// NB: UDP is untested and probably doesn't work
class Socket
{
private:
    typedef int Sock_t;

    struct sockaddr_in  *m_addr;
    std::string         m_host,
                        m_error;
    Sock_t              m_sock;
    int                 m_type;
    uint16              m_port;
    bool                m_tcpNDelay,
                        m_reuseAddr;

public:
    enum SocketType { TCP, UDP };
    enum SocketOption { TCPNoDelay, ReuseAddr };

    Socket( int type = TCP );
    virtual ~Socket();

    void setHost( const std::string &host ) {m_host = host;}
    const std::string &host() const         {return m_host;}

    void setPort( uint16 port )             {m_port = port;}
    uint16 port() const                     {return m_port;}

    bool tcpConnect( const std::string &host = "", uint16 port = 0 ) noexcept(false);
    void tcpDisconnect();

    bool isValid() const                    {return m_sock > -1;}
    std::string errorReason() const         {return m_error;}

    void setSocketOption( int option, bool enable );
    bool bind( const std::string &iface = "0.0.0.0", uint16 port = 0 ) noexcept(false);

    virtual uint sendData( const void *src, uint srcBytes ) noexcept(false);
    virtual uint receiveData( void *dst, uint dstBytes ) noexcept(false);

    bool waitData( uint waitMS = 10 ) noexcept(false);
    uint nReadyForRead() noexcept(false);

private:
    void resolveHostAddr();
    static void resolveHostAddr(
        struct sockaddr_in  &addr,
        const std::string   &host,
        uint16              port );
    void setTCPNDelay() const;
    void setReuseAddr() const;
};

#endif  // SOCKET_H



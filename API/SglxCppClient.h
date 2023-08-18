#ifndef SGLXCPPCLIENT_H
#define SGLXCPPCLIENT_H

#include "SglxApi.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------- */
/* Client-side container callbacks -------------------------------- */
/* ---------------------------------------------------------------- */

// clear client container
void SGLX_CALL cppClient_zer_str( std::string &str );

// insert into client container
void SGLX_CALL cppClient_set_str( std::string &str, const char *data );

// get C-data from client container
const char* SGLX_CALL cppClient_get_str( const std::string &str );

/* ---------------------------------------------------------------- */
/* T_sglx_get_strs ------------------------------------------------ */
/* ---------------------------------------------------------------- */

struct cppClient_sglx_get_strs : public T_sglx_get_strs {
    std::vector<std::string>    vstr;
    virtual void set_str( const char *s );
};

struct cClient_sglx_get_strs : public T_sglx_get_strs {
    std::vector<std::string>    &vstr;
    cClient_sglx_get_strs( std::vector<std::string> &vstr )
        : vstr(vstr)    {}
    virtual void set_str( const char *s );
};

/* ---------------------------------------------------------------- */
/* T_sglx_get_keyvals --------------------------------------------- */
/* ---------------------------------------------------------------- */

struct cppClient_sglx_get_keyvals : public T_sglx_get_strs {
    std::map<std::string,std::string>   mstrstr;
    virtual void set_str( const char *s );
};

/* ---------------------------------------------------------------- */
/* T_sglx_get_ints ------------------------------------------------ */
/* ---------------------------------------------------------------- */

struct cppClient_sglx_get_ints : public T_sglx_get_ints {
    std::vector<int>    vint;
    virtual void set_val( int val, int flag );
};

struct cClient_sglx_get_ints : public T_sglx_get_ints {
    std::vector<int>    &vint;
    cClient_sglx_get_ints( std::vector<int> &vint )
        : vint(vint)    {}
    virtual void set_val( int val, int flag );
};

/* ---------------------------------------------------------------- */
/* T_sglx_get_dbls ------------------------------------------------ */
/* ---------------------------------------------------------------- */

struct cppClient_sglx_get_dbls : public T_sglx_get_dbls {
    std::vector<double> vdbl;
    virtual void set_val( double val, int flag );
};

struct cClient_sglx_get_dbls : public T_sglx_get_dbls {
    std::vector<double> &vdbl;
    cClient_sglx_get_dbls( std::vector<double> &vdbl )
        : vdbl(vdbl)    {}
    virtual void set_val( double val, int flag );
};

/* ---------------------------------------------------------------- */
/* T_sglx_set_ints ------------------------------------------------ */
/* ---------------------------------------------------------------- */

struct cppClient_sglx_set_ints : public T_sglx_set_ints {
    std::vector<int>    vint;
    virtual bool get_int( int &val, int j );
};

struct cClient_sglx_set_ints : public T_sglx_set_ints {
    const int   *vint;
    int         n;
    cClient_sglx_set_ints( const int *vint, int n )
        : vint(vint), n(n)  {}
    virtual bool get_int( int &val, int j );
};

/* ---------------------------------------------------------------- */
/* T_sglx_set_keyvals --------------------------------------------- */
/* ---------------------------------------------------------------- */

struct cppClient_sglx_set_keyvals : public T_sglx_set_keyvals {
    std::map<std::string,std::string>                   mstrstr;
    std::map<std::string,std::string>::const_iterator   it, end;
    virtual bool get_pair( const char* &key, const char* &val );
};

struct cClient_sglx_set_keyvals : public T_sglx_set_keyvals {
    const std::map<std::string,std::string>             &mstrstr;
    std::map<std::string,std::string>::const_iterator   it, end;
    cClient_sglx_set_keyvals( const std::map<std::string,std::string> &mstrstr )
        : mstrstr(mstrstr)  {}
    virtual bool get_pair( const char* &key, const char* &val );
};

/* ---------------------------------------------------------------- */
/* T_sglx_fetch --------------------------------------------------- */
/* ---------------------------------------------------------------- */

struct cppClient_sglx_fetch : public T_sglx_fetch {
    std::vector<short>  data;
    std::vector<int>    chans;
    virtual short* base_addr( int nshort );
};

struct cClient_sglx_fetch : public T_sglx_fetch {
    std::vector<short>  &data;
    cClient_sglx_fetch( std::vector<short> &data ) : data(data) {}
    virtual short* base_addr( int nshort );
};

/* ---------------------------------------------------------------- */
/* t_sglxshankmap ------------------------------------------------- */
/* ---------------------------------------------------------------- */

struct cppClient_sglxshankmap : public t_sglxshankmap {
    std::vector<t_entry>    e;
    virtual t_entry* base_addr();
};

/* ---------------------------------------------------------------- */
/* Standard createHandle ------------------------------------------ */
/* ---------------------------------------------------------------- */

// set standard callbacks
void* SGLX_CALL sglx_createHandle_std();

#ifdef __cplusplus
}
#endif

#endif // SGLXCPPCLIENT_H




// ---------------------------------------------------------
// THIS FILE MUST BE COMPILED/LINKED IN THE CLIENT PROJECT!!
// ---------------------------------------------------------

//#include "stdafx.h" // enable if using Visual Studio precompiled headers

#include "SglxCppClient.h"




/* ---------------------------------------------------------------- */
/* Client-side container callbacks -------------------------------- */
/* ---------------------------------------------------------------- */

void SGLX_CALL cppClient_zer_str( std::string &str )
{
    str.clear();
}

void SGLX_CALL cppClient_set_str( std::string &str, const char *data )
{
    str = data;
}

const char* SGLX_CALL cppClient_get_str( const std::string &str )
{
    return str.c_str();
}

/* ---------------------------------------------------------------- */
/* T_sglx_get_strs ------------------------------------------------ */
/* ---------------------------------------------------------------- */

void cppClient_sglx_get_strs::set_str( const char *s )
{
    if( s[0] )
        vstr.push_back( s );
    else
        vstr.clear();
}

void cClient_sglx_get_strs::set_str( const char *s )
{
    if( s[0] )
        vstr.push_back( s );
    else
        vstr.clear();
}

void SGLX_CALL T_sglx_get_strs::dispatch( T_sglx_get_strs &u, const char *s )
{
    u.set_str( s );
}

T_sglx_get_strs::T_sglx_get_strs()
{
    _dispatch = T_sglx_get_strs::dispatch;
}

/* ---------------------------------------------------------------- */
/* T_sglx_get_keyvals --------------------------------------------- */
/* ---------------------------------------------------------------- */

void cppClient_sglx_get_keyvals::set_str( const char *s )
{
    if( s[0] ) {
        std::string kv(s);
        int         eq = kv.find( '=' );
        mstrstr[kv.substr( 0, eq )] = kv.substr( eq + 1 );
    }
    else
        mstrstr.clear();
}

/* ---------------------------------------------------------------- */
/* T_sglx_get_ints ------------------------------------------------ */
/* ---------------------------------------------------------------- */

void cppClient_sglx_get_ints::set_val( int val, int flag )
{
    if( flag )
        vint.push_back( val );
    else
        vint.clear();
}

void cClient_sglx_get_ints::set_val( int val, int flag )
{
    if( flag )
        vint.push_back( val );
    else
        vint.clear();
}

void SGLX_CALL T_sglx_get_ints::dispatch( T_sglx_get_ints &u, int val, int flag )
{
    u.set_val( val, flag );
}

T_sglx_get_ints::T_sglx_get_ints()
{
    _dispatch = T_sglx_get_ints::dispatch;
}

/* ---------------------------------------------------------------- */
/* T_sglx_get_dbls ------------------------------------------------ */
/* ---------------------------------------------------------------- */

void cppClient_sglx_get_dbls::set_val( double val, int flag )
{
    if( flag )
        vdbl.push_back( val );
    else
        vdbl.clear();
}

void cClient_sglx_get_dbls::set_val( double val, int flag )
{
    if( flag )
        vdbl.push_back( val );
    else
        vdbl.clear();
}

void SGLX_CALL T_sglx_get_dbls::dispatch( T_sglx_get_dbls &u, double val, int flag )
{
    u.set_val( val, flag );
}

T_sglx_get_dbls::T_sglx_get_dbls()
{
    _dispatch = T_sglx_get_dbls::dispatch;
}

/* ---------------------------------------------------------------- */
/* T_sglx_set_ints ------------------------------------------------ */
/* ---------------------------------------------------------------- */

bool cppClient_sglx_set_ints::get_int( int &val, int j )
{
    if( j < vint.size() ) {
        val = vint[j];
        return true;
    }
    return false;
}

bool cClient_sglx_set_ints::get_int( int &val, int j )
{
    if( j < n ) {
        val = vint[j];
        return true;
    }
    return false;
}

bool SGLX_CALL T_sglx_set_ints::dispatch( T_sglx_set_ints &u, int &val, int j )
{
    return u.get_int( val, j );
}

T_sglx_set_ints::T_sglx_set_ints()
{
    _dispatch = T_sglx_set_ints::dispatch;
}

/* ---------------------------------------------------------------- */
/* T_sglx_set_keyvals --------------------------------------------- */
/* ---------------------------------------------------------------- */

bool cppClient_sglx_set_keyvals::get_pair( const char* &key, const char* &val )
{
    if( !key ) {
        it  = mstrstr.begin();
        end = mstrstr.end();
    }
    if( it != end ) {
        key = it->first.c_str();
        val = it->second.c_str();
        ++it;
        return true;
    }
    return false;
}

bool cClient_sglx_set_keyvals::get_pair( const char* &key, const char* &val )
{
    if( !key ) {
        it  = mstrstr.begin();
        end = mstrstr.end();
    }
    if( it != end ) {
        key = it->first.c_str();
        val = it->second.c_str();
        ++it;
        return true;
    }
    return false;
}

bool SGLX_CALL T_sglx_set_keyvals::dispatch( T_sglx_set_keyvals &u, const char* &key, const char* &val )
{
    return u.get_pair( key, val );
}

T_sglx_set_keyvals::T_sglx_set_keyvals()
{
    _dispatch = T_sglx_set_keyvals::dispatch;
}

/* ---------------------------------------------------------------- */
/* T_sglx_fetch --------------------------------------------------- */
/* ---------------------------------------------------------------- */

short* cppClient_sglx_fetch::base_addr( int nshort )
{
    if( data.size() != nshort )
        data.resize( nshort );

    return &data[0];
}

short* cClient_sglx_fetch::base_addr( int nshort )
{
    if( data.size() != nshort )
        data.resize( nshort );

    return &data[0];
}

short* SGLX_CALL T_sglx_fetch::dispatch( T_sglx_fetch &u, int nshort )
{
    return u.base_addr( nshort );
}

T_sglx_fetch::T_sglx_fetch()
{
    _dispatch = T_sglx_fetch::dispatch;
}

/* ---------------------------------------------------------------- */
/* t_sglxshankmap ------------------------------------------------- */
/* ---------------------------------------------------------------- */

t_sglxshankmap::t_entry* cppClient_sglxshankmap::base_addr()
{
    e.resize( ne );
    return (ne ? &e[0] : 0);
}

t_sglxshankmap::t_entry* SGLX_CALL t_sglxshankmap::dispatch( t_sglxshankmap &u )
{
    return u.base_addr();
}

t_sglxshankmap::t_sglxshankmap()
{
    _dispatch = t_sglxshankmap::dispatch;
}

/* ---------------------------------------------------------------- */
/* Standard createHandle ------------------------------------------ */
/* ---------------------------------------------------------------- */

void* SGLX_CALL sglx_createHandle_std()
{
    return
    sglx_createHandle(
        cppClient_zer_str,
        cppClient_set_str,
        cppClient_get_str );
}



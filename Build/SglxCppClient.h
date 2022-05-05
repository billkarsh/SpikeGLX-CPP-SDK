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
void SGLX_CALL cppClient_zer_vi32( std::vector<int> &vi32 );
void SGLX_CALL cppClient_zer_vdbl( std::vector<double> &vdbl );
void SGLX_CALL cppClient_zer_vstr( std::vector<std::string> &vstr );
void SGLX_CALL cppClient_zer_mstrstr( std::map<std::string,std::string> &mstrstr );

// insert into client container
void SGLX_CALL cppClient_ins_str( std::string &str, const char *data );
void SGLX_CALL cppClient_ins_vi32( std::vector<int> &vi32, int data );
void SGLX_CALL cppClient_ins_vdbl( std::vector<double> &vdbl, double data );
void SGLX_CALL cppClient_ins_vstr( std::vector<std::string> &vstr, const char *data );
void SGLX_CALL cppClient_ins_mstrstr( std::map<std::string,std::string> &mstrstr, const char *key, const char *val );

// resize client container
void SGLX_CALL cppClient_siz_vi16( std::vector<short> &vi16, int n );

// get C-data from client container
const char* SGLX_CALL cppClient_get_str( const std::string &str );

// Iterate through vector:
// Server sets opaque iterator to zero on first call;
// itr then managed by this function thereafter.
// Returns true if another val available.
bool SGLX_CALL cppClient_itr_vi32( int &val, const std::vector<int> &vi32, void* &itr );

// Iterate through map:
// Server sets opaque iterator to zero on first call;
// itr then managed by this function thereafter.
// Returns true if another (key,val) pair available.
bool SGLX_CALL cppClient_itr_mstrstr(
    const char*                             &key,
    const char*                             &val,
    const std::map<std::string,std::string> &mstrstr,
    void*                                   &itr );

// set standard callbacks
bool SGLX_CALL sglx_connect_std(
    t_sglxconn          &S,
    const std::string   &host   = "localhost",
    int                 port    = 4142 );

#ifdef __cplusplus
}
#endif

#endif // SGLXCPPCLIENT_H



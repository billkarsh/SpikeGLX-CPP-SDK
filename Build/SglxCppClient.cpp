
// ---------------------------------------------------------
// THIS FILE MUST BE COMPILED/LINKED IN THE CLIENT PROJECT!!
// ---------------------------------------------------------

//#include "stdafx.h" // enable if using Visual Studio precompiled headers

#include "SglxCppClient.h"




void SGLX_CALL cppClient_zer_str( std::string &str )
{
    str.clear();
}

void SGLX_CALL cppClient_zer_vi32( std::vector<int> &vi32 )
{
    vi32.clear();
}

void SGLX_CALL cppClient_zer_vdbl( std::vector<double> &vdbl )
{
    vdbl.clear();
}

void SGLX_CALL cppClient_zer_vstr( std::vector<std::string> &vstr )
{
    vstr.clear();
}

void SGLX_CALL cppClient_zer_mstrstr( std::map<std::string,std::string> &mstrstr )
{
    mstrstr.clear();
}

void SGLX_CALL cppClient_ins_str( std::string &str, const char *data )
{
    str = data;
}

void SGLX_CALL cppClient_ins_vi32( std::vector<int> &vi32, int data )
{
    vi32.push_back( data );
}

void SGLX_CALL cppClient_ins_vdbl( std::vector<double> &vdbl, double data )
{
    vdbl.push_back( data );
}

void SGLX_CALL cppClient_ins_vstr( std::vector<std::string> &vstr, const char *data )
{
    vstr.push_back( data );
}

void SGLX_CALL cppClient_ins_mstrstr( std::map<std::string,std::string> &mstrstr, const char *key, const char *val )
{
    mstrstr[key] = val;
}

void SGLX_CALL cppClient_siz_vi16( std::vector<short> &vi16, int n )
{
    vi16.resize( n );
}

const char* SGLX_CALL cppClient_get_str( const std::string &str )
{
    return str.c_str();
}

bool SGLX_CALL cppClient_itr_vi32( int &val, const std::vector<int> &vi32, void* &itr )
{
    std::vector<int>::const_iterator    it;

    if( itr )
        it = *reinterpret_cast<std::vector<int>::const_iterator*>(&itr);
    else
        it = vi32.begin();

    if( it != vi32.end() ) {
        val = *it;
        itr = *reinterpret_cast<void**>(&++it);
        return true;
    }
    else {
        itr = 0;
        return false;
    }
}

bool SGLX_CALL cppClient_itr_mstrstr(
    const char*                             &key,
    const char*                             &val,
    const std::map<std::string,std::string> &mstrstr,
    void*                                   &itr )
{
    std::map<std::string,std::string>::const_iterator   it;

    if( itr )
        it = *reinterpret_cast<std::map<std::string,std::string>::const_iterator*>(&itr);
    else
        it = mstrstr.begin();

    if( it != mstrstr.end() ) {
        key = it->first.c_str();
        val = it->second.c_str();
        itr = *reinterpret_cast<void**>(&++it);
        return true;
    }
    else {
        itr = 0;
        return false;
    }
}


bool SGLX_CALL sglx_connect_std(
    t_sglxconn          &S,
    const std::string   &host,
    int                 port )
{
    return
    sglx_connect(
        S,
        cppClient_zer_str,
        cppClient_zer_vi32,
        cppClient_zer_vdbl,
        cppClient_zer_vstr,
        cppClient_zer_mstrstr,
        cppClient_ins_str,
        cppClient_ins_vi32,
        cppClient_ins_vdbl,
        cppClient_ins_vstr,
        cppClient_ins_mstrstr,
        cppClient_siz_vi16,
        cppClient_get_str,
        cppClient_itr_vi32,
        cppClient_itr_mstrstr,
        host, port );
}



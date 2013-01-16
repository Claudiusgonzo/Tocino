/* -*- Mode:C++; c-file-style:"microsoft"; indent-tabs-mode:nil; -*- */
#ifndef __TOCINO_ARBITER_H__
#define __TOCINO_ARBITER_H__   

#include "ns3/object.h"
#include "ns3/ptr.h"

#include "tocino-misc.h"

namespace ns3
{

class TocinoNetDevice;
class TocinoTx;

// ISSUE-REVIEW: consider moving this to its own file
struct TocinoArbiterAllocation
{
    TocinoInputPort inputPort;
    TocinoOutputVC outputVC;
    
    TocinoArbiterAllocation( TocinoInputPort port, TocinoOutputVC vc ) 
        : inputPort( port )
        , outputVC( vc )
    {}
    
    bool operator==( const TocinoArbiterAllocation& other ) const
    {
        if( other.inputPort != inputPort )
            return false;

        if( other.outputVC != outputVC )
            return false;

        return true;
    }

    bool operator!=( const TocinoArbiterAllocation& other ) const
    {
        return !( *this == other );
    }
};

struct TocinoArbiter : public Object
{
    static TypeId GetTypeId( void );

    virtual TocinoArbiterAllocation Arbitrate() = 0;

    virtual void Initialize( Ptr<TocinoNetDevice>, const TocinoTx* ) = 0;

    virtual TocinoArbiterAllocation GetVCOwner( const TocinoOutputVC ) const = 0;

    static const TocinoArbiterAllocation DO_NOTHING;
};

}
#endif //__TOCINO_ARBITER_H__

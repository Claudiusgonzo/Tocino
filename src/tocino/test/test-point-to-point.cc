/* -*- Mode:C++; c-file-style:"microsoft"; indent-tabs-mode:nil; -*- */

#include <stdint.h>
#include <vector>

#include "ns3/log.h"
#include "ns3/uinteger.h"
#include "ns3/config.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/test.h"
#include "ns3/tocino-net-device.h"
#include "ns3/tocino-flit-header.h"
#include "ns3/tocino-channel.h"
#include "ns3/simulator.h"
#include "ns3/node.h"

#include "test-point-to-point.h"

using namespace ns3;

TestPointToPoint::TestPointToPoint()
  : TestCase( "Send between two directly-connected net devices" )
{
}

TestPointToPoint::~TestPointToPoint() {}

namespace
{
    const TocinoAddress ADDR_ONE(0,0,0);
    const TocinoAddress ADDR_TWO(1,0,0);

    unsigned totalCount;
    unsigned totalBytes;
    
    void Reset()
    {
        totalCount = 0;
        totalBytes = 0;
    }

    bool AcceptPacket( Ptr<NetDevice>, Ptr<const Packet> p, uint16_t, const Address& )
    {
        totalCount++;
        totalBytes += p->GetSize();
        
        return true;
    }
}

Ptr<TocinoNetDevice> TestPointToPoint::CreateNetDeviceHelper( const TocinoAddress& a )
{
    Ptr<TocinoNetDevice> netDevice = CreateObject<TocinoNetDevice>();
    
    netDevice->Initialize();
    netDevice->SetAddress( a );
    netDevice->SetReceiveCallback( MakeCallback( AcceptPacket ) );

    // HACK: The Nodes are required to avoid
    // SIGSEGV in TocinoChannel::TransmitEnd()
    Ptr<Node> node = CreateObject<Node>();
    netDevice->SetNode( node );

    return netDevice;
}

void TestPointToPoint::CreateChannelHelper( Ptr<TocinoNetDevice> tnda, uint32_t pa, Ptr<TocinoNetDevice> tndb, uint32_t pb )
{
    Ptr<TocinoChannel> c = CreateObject<TocinoChannel>();

    tnda->SetTxChannel( c, pa );
    c->SetTransmitter( tnda->GetTransmitter( pa ) );

    tndb->SetRxChannel( c, pb );
    c->SetReceiver( tndb->GetReceiver( pb ) );
}

void TestPointToPoint::TestHelper( const unsigned COUNT, const unsigned BYTES )
{
    Ptr<Packet> p = Create<Packet>( BYTES );
    
    Ptr<TocinoNetDevice> netDeviceOne = CreateNetDeviceHelper( ADDR_ONE );
    Ptr<TocinoNetDevice> netDeviceTwo = CreateNetDeviceHelper( ADDR_TWO );

    CreateChannelHelper( netDeviceOne, 0, netDeviceTwo, 1 );
    CreateChannelHelper( netDeviceTwo, 1, netDeviceOne, 0 );

    // 1 -> 2 

    Reset();

    for( unsigned i = 0; i < COUNT; ++i )
    {
        netDeviceOne->Send( p, ADDR_TWO, 0 );
    }

    Simulator::Run();

    NS_TEST_ASSERT_MSG_EQ( totalCount, COUNT, "1->2: Got unexpected total packet count" );
    NS_TEST_ASSERT_MSG_EQ( totalBytes, BYTES*COUNT, "1->2: Got unexpected total packet bytes" );

    // 2 -> 1 

    Reset();

    for( unsigned i = 0; i < COUNT; ++i )
    {
        netDeviceTwo->Send( p, ADDR_ONE, 0 );
    }

    Simulator::Run();

    NS_TEST_ASSERT_MSG_EQ( totalCount, COUNT, "2->1: Got unexpected total packet count" );
    NS_TEST_ASSERT_MSG_EQ( totalBytes, BYTES*COUNT, "2->1: Got unexpected total packet bytes" );
    
    Simulator::Destroy();
}

void
TestPointToPoint::DoRun (void)
{
    TestHelper( 1, 20 );
    TestHelper( 1, 123 );
    TestHelper( 2, 32 );
}
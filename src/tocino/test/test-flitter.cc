/* -*- Mode:C++; c-file-style:"stroustrup"; indent-tabs-mode:nil; -*- */

#include "ns3/test.h"
#include "ns3/ptr.h"
#include "ns3/packet.h"

#include "ns3/tocino-net-device.h"
#include "ns3/tocino-flit-header.h"

#include "test-flitter.h"

using namespace ns3;

namespace 
{
    const TocinoAddress TEST_SRC(0);
    const TocinoAddress TEST_DST(1);
    const TocinoFlitHeader::Type TEST_TYPE( TocinoFlitHeader::MAX_TYPE );
}

TestFlitter::TestFlitter()
    : TestCase( "Tocino Flitter Tests" )
{}

TestFlitter::~TestFlitter()
{}

void TestFlitter::TestEmpty()
{
    Ptr<Packet> p = Create<Packet>( 0 );
    std::deque< Ptr<Packet> > flits;

    flits = TocinoNetDevice::Flitter( p, TEST_SRC, TEST_DST, TEST_TYPE );

    NS_TEST_ASSERT_MSG_EQ( flits.size(), 1, "Empty packet should result in one flit" );
}

void TestFlitter::TestOneFlit( const unsigned LEN )
{
    Ptr<Packet> p = Create<Packet>( LEN );
    std::deque< Ptr<Packet> > flits;
    
    flits = TocinoNetDevice::Flitter( p, TEST_SRC, TEST_DST, TEST_TYPE );

    NS_TEST_ASSERT_MSG_EQ( flits.size(), 1, "Incorrect number of flits" );

    TocinoFlitHeader h;
    flits[0]->PeekHeader( h );

    NS_TEST_ASSERT_MSG_EQ( h.GetLength(), LEN, "Flit has wrong length" );
    NS_TEST_ASSERT_MSG_EQ( h.IsHead(), true, "Head flit missing head flag?" );
    NS_TEST_ASSERT_MSG_EQ( h.IsTail(), true, "Tail flit missing tail flag?" );
    NS_TEST_ASSERT_MSG_EQ( h.GetSource(), TEST_SRC, "Flit has incorrect source" );
    NS_TEST_ASSERT_MSG_EQ( h.GetDestination(), TEST_DST, "Flit has incorrect destination" );
    NS_TEST_ASSERT_MSG_EQ( h.GetType(), TEST_TYPE, "Flit has incorrect type" );
}

void TestFlitter::TestTwoFlits( const unsigned TAIL_LEN )
{
    const unsigned HEAD_LEN = TocinoFlitHeader::MAX_PAYLOAD_HEAD;
    const unsigned LEN = HEAD_LEN + TAIL_LEN;
    
    Ptr<Packet> p = Create<Packet>( LEN );
    std::deque< Ptr<Packet> > flits;
    
    flits = TocinoNetDevice::Flitter( p, TEST_SRC, TEST_DST, TEST_TYPE );

    NS_TEST_ASSERT_MSG_EQ( flits.size(), 2, "Incorrect number of flits" );

    TocinoFlitHeader h;

    // interrogate head flit
    flits[0]->PeekHeader( h );

    NS_TEST_ASSERT_MSG_EQ( h.GetLength(), HEAD_LEN, "Head flit has wrong length" );
    NS_TEST_ASSERT_MSG_EQ( h.IsHead(), true, "Head flit missing head flag?" );
    NS_TEST_ASSERT_MSG_EQ( h.IsTail(), false, "Head flit has tail flag?" );
    NS_TEST_ASSERT_MSG_EQ( h.GetSource(), TEST_SRC, "Flit has incorrect source" );
    NS_TEST_ASSERT_MSG_EQ( h.GetDestination(), TEST_DST, "Flit has incorrect destination" );
    NS_TEST_ASSERT_MSG_EQ( h.GetType(), TEST_TYPE, "Flit has incorrect type" );
    
    // interrogate tail flit 
    flits[1]->PeekHeader( h );
    
    NS_TEST_ASSERT_MSG_EQ( h.GetLength(), TAIL_LEN, "Tail flit has wrong length" );
    NS_TEST_ASSERT_MSG_EQ( h.IsHead(), false, "Tail flit has head flag?" );
    NS_TEST_ASSERT_MSG_EQ( h.IsTail(), true, "Tail flit missing tail flag?" );
}

void TestFlitter::TestThreeFlits( const unsigned TAIL_LEN )
{
    const unsigned HEAD_LEN = TocinoFlitHeader::MAX_PAYLOAD_HEAD;
    const unsigned BODY_LEN = TocinoFlitHeader::MAX_PAYLOAD_OTHER;
    const unsigned LEN = HEAD_LEN + BODY_LEN + TAIL_LEN;

    Ptr<Packet> p = Create<Packet>( LEN );
    std::deque< Ptr<Packet> > flits;

    flits = TocinoNetDevice::Flitter( p, TEST_SRC, TEST_DST, TEST_TYPE );

    NS_TEST_ASSERT_MSG_EQ( flits.size(), 3, "Incorrect number of flits" );

    TocinoFlitHeader h;

    // interrogate head flit
    flits[0]->PeekHeader( h );

    NS_TEST_ASSERT_MSG_EQ( h.GetLength(), HEAD_LEN, "Head flit has wrong length" );
    NS_TEST_ASSERT_MSG_EQ( h.IsHead(), true, "Head flit missing head flag?" );
    NS_TEST_ASSERT_MSG_EQ( h.IsTail(), false, "Head flit has tail flag?" );
    NS_TEST_ASSERT_MSG_EQ( h.GetSource(), TEST_SRC, "Flit has incorrect source" );
    NS_TEST_ASSERT_MSG_EQ( h.GetDestination(), TEST_DST, "Flit has incorrect destination" );
    NS_TEST_ASSERT_MSG_EQ( h.GetType(), TEST_TYPE, "Flit has incorrect type" );
    
    // interrogate body flit 
    flits[1]->PeekHeader( h );
    
    NS_TEST_ASSERT_MSG_EQ( h.GetLength(), BODY_LEN, "Body flit has wrong length" );
    NS_TEST_ASSERT_MSG_EQ( h.IsHead(), false, "Body flit has head flag?" );
    NS_TEST_ASSERT_MSG_EQ( h.IsTail(), false, "Body flit has tail flag?" );
    
    // interrogate tail flit 
    flits[2]->PeekHeader( h );
    
    NS_TEST_ASSERT_MSG_EQ( h.GetLength(), TAIL_LEN, "Tail flit has wrong length" );
    NS_TEST_ASSERT_MSG_EQ( h.IsHead(), false, "Tail flit has head flag?" );
    NS_TEST_ASSERT_MSG_EQ( h.IsTail(), true, "Tail flit missing tail flag?" );
}

void TestFlitter::DoRun( void )
{
    TestEmpty();

    TestOneFlit( 0 ); // similar to TestEmpty()
    TestOneFlit( 1 );
    TestOneFlit( TocinoFlitHeader::MAX_PAYLOAD_HEAD - 1 );
    
    TestTwoFlits( 1 );
    TestTwoFlits( TocinoFlitHeader::MAX_PAYLOAD_HEAD - 1 );

    TestThreeFlits( 1 );
    TestThreeFlits( TocinoFlitHeader::MAX_PAYLOAD_HEAD - 1 );
}
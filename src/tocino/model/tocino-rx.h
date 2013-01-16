/* -*- Mode:C++; c-file-style:"microsoft"; indent-tabs-mode:nil; -*- */
#ifndef __TOCINO_RX_H__
#define __TOCINO_RX_H__

#include <stdint.h>
#include <vector>

#include "ns3/ptr.h"

#include "tocino-flow-control.h"

namespace ns3
{

class NetDevice;
class TocinoNetDevice;
class Packet;
class CallbackQueue;
class TocinoRouter;
class TocinoTx;

class TocinoRx
{
    public:

    TocinoRx( const uint32_t, Ptr<TocinoNetDevice>, Ptr<TocinoRouter> );
    ~TocinoRx();

    uint32_t GetPortNumber() const;

    Ptr<NetDevice> GetNetDevice();
    
    bool IsVCBlocked( const TocinoInputVC ) const;

    Ptr<const Packet>
        PeekNextFlit( const TocinoInputVC ) const;

    void Receive(Ptr<Packet> p);
    
    void TryRouteFlit();

    void SetReserveFlits( uint32_t );
    
    bool AllQuiet() const;
    void DumpState() const;

    private:
    
    Ptr<CallbackQueue> GetInputQueue( const TocinoInputVC ) const;
    void SetInputQueue( const TocinoInputVC, const Ptr<CallbackQueue> );
  
    bool CanRouteFrom( const TocinoInputVC ) const;
    
    void RewriteFlitHeaderVC( Ptr<Packet>, const TocinoOutputVC ) const;
    
    bool EnqueueHelper( Ptr<Packet>, const TocinoInputVC );
    
    // FIXME: Out parameters are ugly. Replace this with
    // std::tuple and std::tie ASAP so we can return
    // both the packet and the boolean at once.
    Ptr<Packet> DequeueHelper( const TocinoInputVC, bool& );

    const uint32_t m_inputPortNumber;

    // link to owning TocinoNetDevice
    const Ptr<TocinoNetDevice> m_tnd;

    // corresponding transmitter
    TocinoTx * const m_tx;
    
    Ptr<TocinoRouter> m_router;

    // This nested class controls access to our
    // primary state variable
    class
    {
        private:
        
        // The input queues are per input virtual channel.
        // These are used mainly for correctness: to
        // tolerate XOFF delay.
        std::vector< Ptr<CallbackQueue> > vec;

        public:
     
        // If you're thinking about adding another 
        // friend function here, you're wrong. -MAS
        
        friend 
            TocinoRx::TocinoRx( const uint32_t,
                Ptr<TocinoNetDevice>, Ptr<TocinoRouter> );
        
        friend Ptr<CallbackQueue>
            TocinoRx::GetInputQueue(
                const TocinoInputVC ) const;

        friend void 
            TocinoRx::SetInputQueue( const TocinoInputVC,
                const Ptr<CallbackQueue> );
    } 
    m_inputQueues;

};

} // namespace ns3

#endif // __TOCINO_RX_H__

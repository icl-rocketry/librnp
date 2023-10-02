#pragma once

#include <queue>
#include <utility>

template<typename ELEMENT_T>
class Rnp_PacketBufferInterface
{
    public:

        /**
         * @brief Construct a new Rnp_PacketBufferInterface object
         * 
         * @param underlyingQueue reference to underlying queue
         * @param queueMaxSize maxmiuym allowable queue size, if zero, queue is unbounded
         */
        Rnp_PacketBufferInterface(std::queue<ELEMENT_T>& underlyingQueue,const size_t queueMaxSize):
        _underlyingQueue(underlyingQueue),
        _queueMaxSize(queueMaxSize)
        {};

        /**
         * @brief Push a new element to the packet buffer with perfect forwarding. Returns false on an error.
         * 
         * @param arg 
         * @return true 
         * @return false 
         */
        template<typename T>
        bool push(T&& arg)
        {
            if (_queueMaxSize)
            {
                if (_underlyingQueue.size() == _queueMaxSize)
                {
                    return false;
                }
            }
            _underlyingQueue.push(std::forward<T>(arg));
            return true;
        };


    private:
        std::queue<ELEMENT_T>& _underlyingQueue;

        const size_t _queueMaxSize;



};
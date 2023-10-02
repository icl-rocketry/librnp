#pragma once

#include <queue>
#include <utility>

template<typename T>
class Rnp_PacketBufferInterface
{
    public:

        Rnp_PacketBufferInterface(std::queue<T>& underlyingQueue,size_t queueMaxSize):
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
        bool push(T&& arg)
        {
            if (_underlyingQueue.size() == _queueMaxSize)
            {
                return false;
            }
            _underlyingQueue.push(std::forward<T>(arg));
            return true;
        };

    private:
        std::queue<T>& _underlyingQueue;

        const size_t _queueMaxSize;



};
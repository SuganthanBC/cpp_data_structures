#pragma once

#include "Array.h"
#include <atomic>

namespace EA
{
/*
 * A lock free fifo meant to share big objects between threads
 *
 * The idea is push() can be called from any thread, but pull() provides a safe
 * pointer to use from a specific thread that remains untouched
 * until the newxt pull() call.
 *
 * This FIFO is meant for cases where we want the processor to have a version of the shared object 
 * independent from the GUI processing/writes and pushes. At the same time, if a push from the GUI is available,
 * the processor will know it by calling shouldUpdate(). After which, depending on the shouldUpdate() result, 
 * the processor will prepare and then updates its version of the shared object by calling pull()
 *
 * This is good for something like a big vector that the processor needs to know about
 * constantly and also if any pushes from GUI is available, it has to note off the current sounding midi notes,
 * get the pointer to the recently pushed big vector, retrigger those notes
 *
 * For message passing between threads where you absolutely need every message read,
 * probably something else is useful
 */
template <typename T, int size = 5>
class Fifo
{
public:
    void push(const T& object)
    {
        auto loc = writePos.load();
        array[loc] = object;
        
        /* After writing the object, the position is saved for future realtime reads */        
        futureReadPos.store(loc);

        do
        {
            getNextLocation(loc);
        } while (loc == currentReadPosition.load());

        writePos.store(loc);
    }

    T& pull()
    {
        /*   
              
        No calls to pull() means the processor's reading data will be intact
        a single call to pull() = guaranteed latest data position that has been pushed by the GUI 

        */
        
        auto readPos = futureReadPos.load() ;
        currentReadPosition.store(readPos);
        return array[readPos];
    }

private:
    void getNextLocation(int& prevLocation)
    {
        prevLocation++;

        if (prevLocation == size)
            prevLocation = 0;
    }

    std::atomic<int> writePos {1};
    std::atomic<int> currentReadPosition {0};
    std::atomic<int> futureReadPos {0};

    Array<T, size> array;
};

} // namespace EA

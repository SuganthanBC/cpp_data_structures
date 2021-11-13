#pragma once

#include "Fifo.h"

namespace EA
{
/* This is a useful object for classic cases of sharing random data
 * that gets edited in the GUI but read from the processor and 
 * given the processor has to prepare(do some processing to let go of the old version of shared data)
 * inorder to get the updates from the GUI
 *
 * To use, create one SharedWithRealTime<T> in your processor. You can freely pass
 * a reference to it, but remember that you must call push() at the end of the GUI processing for the updates
 * to register in the processor
 *
 * On the processor side, you need to call shouldUpdate() at the start of the block. 
 * If it is true, do the preparations to get the latest GUI data push, and then call getRealTime()
 * And from then on the rt object will be safe to use with no thread contentions

Example:
========

void MainProcessor::processBlock()
{
    if (sharedState.shouldUpdate()) //Is there any updates from the GUI?
    {
        handleNoteOff(MidiMessage::noteOff(1, currentNote));  //Turn the current notes off

        rtData = &sharedState.getRealTime();  //Update RT data 

        handleNoteOn(MidiMessage::noteOn(1, currentNote, velocity)); //Trigger the same notes again
    }  

    ...  
}

*/
template <typename T, int fifoSize = 5>
class SharedWithRealTime
{
public:
    SharedWithRealTime() { push(); }

    //Call at the start of the block in the processor to decide whether to prepare and pull or not
    bool shouldUpdate() 
    { 
        return shouldProcessorUpdate.load();
    }

    //Call this from the GUI thread to update the data
    void push() 
    { 
        fifo.push(data); 
        shouldProcessorUpdate.store(true);
    }

    //Useful operators to call from the GUI only to reach the shared object
    T* operator->() { return &data; }
    T& operator*() { return data; }

    //Another way to access the realtime object. Only valid during processBlock.
    T& getRealTime()
    {
        shouldProcessorUpdate.store(false);
        rt = &fifo.pull();        
        return *rt;
    }

    //Safe to use pointer, but only during the process block.
    T* rt = nullptr;

private:
    //GUI read and GUI writes
    T data;

    //RT pulls and GUI pushes
    Fifo<T, fifoSize> fifo;

    std::atomic<bool> shouldProcessorUpdate;
};

//Similar to SharedWithRealTime, but a non-owning version of the GUI data
template <typename T, int fifoSize = 5>
class GUIToRealTime
{
public:
    explicit GUIToRealTime(T& dataToUse)
        : data(dataToUse)
    {
        push();
    }

    //Call at the start of the block. This will make the RT pointer safe to use
    void blockStarted() { rt = &fifo.pull(); }

    //Call this from the GUI thread to update the data
    void push() { fifo.push(data); }

    //Useful operators to call from the GUI only to reach the shared object
    T* operator->() { return &data; }
    T& operator*() { return data; }

    //Another way to access the realtime object. Only valid during processBlock.
    T& getRealTime()
    {
        blockStarted();
        return *rt;
    }

    //Safe to use pointer, but only during the process block. See blockStarted()
    T* rt = nullptr;

private:
    T& data;
    Fifo<T, fifoSize> fifo;
};

} // namespace EA

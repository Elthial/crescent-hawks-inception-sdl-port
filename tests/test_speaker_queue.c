/* Pure SDL backend scanout checks: no device callback can race this fixture. */
#include "backend.h"
#include <stdio.h>
#include <stdlib.h>
static int16_t wave[AudioQueueCapacity],output[AudioQueueCapacity];
static void check(int ok)
{ if(!ok) { fputs("Retained speaker audio mismatch\n",stderr); exit(1); } }
int main(void)
{
    size_t index;
    for(index=0;index<AudioQueueCapacity;++index)
        wave[index]=(int16_t)((int)(index%20001)-10000);
    SDLBackend_SpeakerOff();
    check(!SDLBackend_QueueAudio(NULL,1));
    check(SDLBackend_QueueAudio(NULL,0));
    check(!SDLBackend_QueueAudio(wave,AudioQueueCapacity+1));
    check(SDLBackend_QueueAudio(wave,24));
    SDLBackend_RenderSpeaker(output,32);
    for(index=0;index<24;++index) check(output[index]==wave[index]);
    for(;index<32;++index) check(output[index]==0);
    /* Full queue rejection must not mutate data; partial consumption then
     * enqueue exercises a wrap across the retained ring's physical end. */
    check(SDLBackend_QueueAudio(wave,AudioQueueCapacity));
    check(!SDLBackend_QueueAudio(wave,1));
    SDLBackend_RenderSpeaker(output,17);
    for(index=0;index<17;++index) check(output[index]==wave[index]);
    check(SDLBackend_QueueAudio(wave,17));
    SDLBackend_RenderSpeaker(output,AudioQueueCapacity);
    for(index=0;index<AudioQueueCapacity-17;++index) check(output[index]==wave[index+17]);
    for(;index<AudioQueueCapacity;++index) check(output[index]==wave[index-(AudioQueueCapacity-17)]);
    /* Queued samples survive a later port-state change. Live PIT scanout
     * resumes only after the retained sequence has played. */
    SDLBackend_ConfigureSpeaker();
    check(SDLBackend_QueueAudio(wave,2)); SDLBackend_SpeakerOn(1200);
    SDLBackend_RenderSpeaker(output,4);
    check(output[0]==wave[0] && output[1]==wave[1]);
    check(output[2]==4096 && output[3]==4096);
    SDLBackend_SpeakerOff(); check(SDLBackend_QueueAudio(wave,2));
    SDLBackend_Close(); SDLBackend_RenderSpeaker(output,4);
    for(index=0;index<4;++index) check(output[index]==0);
    /* Three explicit millisecond spans retain silence/tone/silence even
     * though the hardware gate is already off when playback finally begins. */
    check(SDLBackend_QueueSpeakerSpan(1000000));
    SDLBackend_ConfigureSpeaker(); SDLBackend_SpeakerOn(1200);
    check(SDLBackend_QueueSpeakerSpan(1000000)); SDLBackend_SpeakerOff();
    check(SDLBackend_QueueSpeakerSpan(1000000));
    SDLBackend_RenderSpeaker(output,144);
    for(index=0;index<48;++index) check(output[index]==0);
    for(;index<96;++index) check(output[index]==(index-48<=24?4096:-4096));
    for(;index<144;++index) check(output[index]==0);
    /* Subsample spans accumulate rather than each rounding down to silence. */
    check(SDLBackend_QueueSpeakerSpan(10000));
    check(SDLBackend_QueueSpeakerSpan(10000));
    check(SDLBackend_QueueSpeakerSpan(1000));
    SDLBackend_SpeakerOn(1200); SDLBackend_RenderSpeaker(output,2);
    check(output[0]==0 && output[1]==4096);
    SDLBackend_SpeakerOff(); SDLBackend_Close();
    check(SDLBackend_QueueAudio(wave,AudioQueueCapacity));
    check(!SDLBackend_QueueSpeakerSpan(33333));
    check(!SDLBackend_QueueSpeakerSpan(UINT64_MAX));
    SDLBackend_RenderSpeaker(output,AudioQueueCapacity);
    for(index=0;index<AudioQueueCapacity;++index) check(output[index]==wave[index]);
    /* Failed span must not carry a fractional sample into the next request. */
    check(SDLBackend_QueueSpeakerSpan(10000));
    SDLBackend_SpeakerOn(1200); SDLBackend_RenderSpeaker(output,1);
    check(output[0]==4096); SDLBackend_SpeakerOff(); SDLBackend_Close();
    /* Music rest/high-bit note keeps PIT divisor14 but must not alias its
     * ultrasonic oscillator into a constant audible ~10.8kHz whistle. */
    SDLBackend_SpeakerOn(14); SDLBackend_RenderSpeaker(output,480);
    for(index=0;index<480;++index) check(output[index]==0);
    check(SDLBackend_QueueSpeakerSpan(1000000)); SDLBackend_SpeakerOff();
    SDLBackend_RenderSpeaker(output,48);
    for(index=0;index<48;++index) check(output[index]==0);
    SDLBackend_SpeakerOn(1200); SDLBackend_RenderSpeaker(output,480);
    for(index=0;index<480;++index) check(output[index]==4096 || output[index]==-4096);
    SDLBackend_SpeakerOff(); SDLBackend_Close();
    puts("Retained PCM ordering, overflow rejection, wrap, ultrasonic rejection and teardown passed.");
    return 0;
}

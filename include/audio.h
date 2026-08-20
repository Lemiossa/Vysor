#ifndef AUDIO_H
#define AUDIO_H
#include <globals.h>
#include <pthread.h>

extern float samples[FRAMES];
extern pthread_mutex_t samples_mutex;

// Initializes vysor audio system
// Start the audio read thread
int audio_init(char *device);
// Destroys de audio
void audio_destroy(void);

#endif // AUDIO_H

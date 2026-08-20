#ifndef AUDIO_H
#define AUDIO_H
#include <globals.h>

extern float samples[FRAMES];
int audio_init(void);
void audio_destroy(void);

#endif // AUDIO_H

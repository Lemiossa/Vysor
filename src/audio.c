/*
 * audio.c
 * Created by Matheus Leme da Silva
 * */
#include <globals.h>
#include <stdio.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <pthread.h>
#include <stddef.h>

float samples[FRAMES];
static pthread_t audio_id;
static int running = 0;

void *audio_thread(void *arg)
{
    if (!arg)
        return NULL;

    pa_simple *s = (pa_simple *)arg;

    while (running != 0)
    {
        pa_simple_read(s, samples, sizeof(samples), NULL);
    }

    return s;
}

// Initializes vysor audio system
// Start the audio read thread
int audio_init(void)
{
    pa_sample_spec ss = {
        .format = PA_SAMPLE_FLOAT32LE,
        .rate = 44100,
        .channels = 1
    };

    pa_simple *s = pa_simple_new(NULL, "Vysor", PA_STREAM_RECORD, NULL, "monitor", &ss, NULL, NULL, NULL);
    
    running = 1;
    if (pthread_create(&audio_id, NULL, audio_thread, (void *)s))
    {
        fprintf(stderr, "Failed to initialize audio thread\r\n");
        return 1;
    }

    return 0;
}

void audio_destroy(void)
{
    printf("Destroying audio thread...\r\n");
    running = 0;
    pa_simple *s;
    pthread_join(audio_id, (void **)&s);
    if (s)
        pa_simple_free(s);
}

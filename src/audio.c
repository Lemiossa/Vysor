/*
 * audio.c
 * Created by Matheus Leme da Silva
 * */
#include <globals.h>
#include <stdint.h>
#include <stdio.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <pthread.h>
#include <stddef.h>

int16_t buffer[FRAMES];
float samples[FRAMES];
pthread_mutex_t samples_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t audio_id;
static int running = 0;

void *audio_thread(void *arg)
{
    if (!arg)
        return NULL;

    pa_simple *s = (pa_simple *)arg;

    while (running != 0)
    {
        pa_simple_read(s, buffer, sizeof(buffer), NULL);

        pthread_mutex_lock(&samples_mutex);
        for (int i = 0; i < FRAMES; i++)
            samples[i] = ((float)buffer[i] / 32768.0);
        pthread_mutex_unlock(&samples_mutex);
    }

    return s;
}

// Initializes vysor audio system
// Start the audio read thread
int audio_init(char *device)
{
    pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 44100,
        .channels = 1
    };
    pa_buffer_attr attr;
    attr.maxlength = (uint32_t)-1;
    attr.tlength   = (uint32_t)-1;
    attr.prebuf    = (uint32_t)-1;
    attr.minreq    = (uint32_t)-1;
    attr.fragsize  = sizeof(int16_t) * FRAMES;
    int error;
    pa_simple *s = pa_simple_new(
        NULL, 
        "Vysor", 
        PA_STREAM_RECORD, 
        device != NULL ? device : DEFAULT_MONITOR, 
        "monitor", 
        &ss, 
        NULL, 
        &attr, 
        &error
    );

    if (!s)
    {
        fprintf(stderr, "pa_simple_new() failed: %s\n", pa_strerror(error));
        return 1;
    }

    running = 1;
    if (pthread_create(&audio_id, NULL, audio_thread, (void *)s))
    {
        fprintf(stderr, "Failed to initialize audio thread\r\n");
        pa_simple_free(s);
        return 1;
    }

    return 0;
}

// Destroys de audio
void audio_destroy(void)
{
    printf("Destroying audio thread...\r\n");
    running = 0;
    pa_simple *s;
    pthread_join(audio_id, (void **)&s);
    if (s)
        pa_simple_free(s);
}

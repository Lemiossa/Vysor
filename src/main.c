/*
 * main.c
 * Created by Matheus Leme da Silva
 * */
#include <globals.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dft.h>
#include <audio.h>
#include <curses.h>

int running = 1;

pthread_mutex_t m_shared_mutex = PTHREAD_MUTEX_INITIALIZER;
float m_shared[FRAMES/2];
float max_mag_shared = 0.0f;

void *dft_thread(void *arg)
{
    if (!arg)
        return NULL;

    DFTContext *dft_ctx = (DFTContext *)arg;

    float complex X[FRAMES];

    float samples_copy[FRAMES];
    while (running)
    {
        pthread_mutex_lock(&samples_mutex);
        memcpy(samples_copy, samples, sizeof(float) * FRAMES);
        pthread_mutex_unlock(&samples_mutex);

        dft_execute(dft_ctx, samples_copy, X);

        float m[FRAMES/2];
        float max_mag = 0.0f;
        for (int k = 0; k < FRAMES/2; k++)
        {
            m[k] = cabsf(X[k]);
            if (m[k] > max_mag)
                max_mag = m[k];
        }

        pthread_mutex_lock(&m_shared_mutex);
        for (int k = 0; k < FRAMES/2; k++)
            m_shared[k] = m[k];
        max_mag_shared = max_mag;
        pthread_mutex_unlock(&m_shared_mutex);
    }

    return dft_ctx;
}

pthread_t dft_thread_id;

// Initializes DFT thread
int dft_init_thread(void)
{
    DFTContext *dft_ctx = dft_create(FRAMES);
    if (!dft_ctx)
    {
        fprintf(stderr, "Failed to create DFT context\r\n");
        return 1;
    }

    if (pthread_create(&dft_thread_id, NULL, dft_thread, (void *)dft_ctx))
    {
        fprintf(stderr, "Failed to create DFT thread\r\n");
        dft_destroy(dft_ctx);
        return 1;
    }

    return 0;
}

// Destroys DFT thread
void dft_destroy_thread(void)
{
    DFTContext *dft_ctx;
    pthread_join(dft_thread_id, (void **)&dft_ctx);
    if (dft_ctx)
        dft_destroy(dft_ctx);
}

int main(void)
{
    printf("Initializing...\r\n");

    if (audio_init(NULL))
        return 1;

    if (dft_init_thread())
    {
        audio_destroy();
        return 1;
    }

    initscr();
    noecho();
    nodelay(stdscr, TRUE);
    curs_set(0);

    start_color();
    use_default_colors();
    init_pair(1, COLOR_GREEN, COLOR_GREEN);

    int width, height;
    getmaxyx(stdscr, height, width);
    int num_bars = width; 

    float smooth_bars[FRAMES / 2] = {0};

    while (running)
    {
        erase();

        float current_m[FRAMES / 2];
        float max_mag = 1.0f;

        pthread_mutex_lock(&m_shared_mutex);
        memcpy(current_m, m_shared, sizeof(float) * (FRAMES / 2));
        max_mag = max_mag_shared;
        pthread_mutex_unlock(&m_shared_mutex);

        for (int i = 0; i < num_bars; i++) 
        {
            int idx = (i * (FRAMES / 2)) / num_bars;
            
            float target = 0.0f;
            if (max_mag > 0.0f)
                target = current_m[idx] / max_mag;

            float alpha_up = 0.4f;
            float alpha_down = 0.12f;

            if (target > smooth_bars[i])
                smooth_bars[i] += alpha_up * (target - smooth_bars[i]);
            else
                smooth_bars[i] += alpha_down * (target - smooth_bars[i]);

            int bar_height = (int)(smooth_bars[i] * height);

            for (int y = 0; y < bar_height; y++)
                mvaddch(height - 1 - y, i, ' ' | COLOR_PAIR(1) | A_REVERSE);
        }

        refresh();

        int ch = getch();
        if (ch == 'q')
            running = 0;

        usleep(16000); 
    }

    endwin();
    dft_destroy_thread();
    audio_destroy();

    return 0;
}

/*
 * main.c
 * Created by Matheus Leme da Silva
 * */
#include <globals.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fft.h>
#include <audio.h>
#include <curses.h>

int running = 1;

pthread_mutex_t m_shared_mutex = PTHREAD_MUTEX_INITIALIZER;
float m_shared[FRAMES / 2] = {0};
float max_mag_shared = 0.0f;
pthread_t fft_thread_id;

void *fft_worker(void *arg)
{
    (void)arg;
    float samples_copy[FRAMES];
    float complex X[FRAMES];
    float m_local[FRAMES / 2];

    while (running)
    {
        pthread_mutex_lock(&samples_mutex);
        memcpy(samples_copy, samples, sizeof(float) * FRAMES);
        pthread_mutex_unlock(&samples_mutex);

        fft(samples_copy, X, FRAMES);

        float max_mag = 0.0f;
        for (int k = 0; k < FRAMES / 2; k++)
        {
            m_local[k] = cabsf(X[k]);
            if (m_local[k] > max_mag)
                max_mag = m_local[k];
        }

        pthread_mutex_lock(&m_shared_mutex);
        memcpy(m_shared, m_local, sizeof(float) * (FRAMES / 2));
        max_mag_shared = max_mag;
        pthread_mutex_unlock(&m_shared_mutex);

        usleep(2000); 
    }

    return NULL;
}

int main(void)
{
    printf("Initializing...\r\n");

    if (audio_init(NULL))
        return 1;

    if (pthread_create(&fft_thread_id, NULL, fft_worker, NULL) != 0)
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

    float smooth_bars[1024] = {0}; 

    while (running)
    {
        erase();

        int height, width;
        getmaxyx(stdscr, height, width);
        int num_bars = width;

        if (num_bars > 1024) num_bars = 1024;

        float current_m[FRAMES / 2];
        float max_mag;

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
    pthread_join(fft_thread_id, NULL);
    audio_destroy();

    return 0;
}

/*
 * main.c
 * Created by Matheus Leme da Silva
 * */
#include <globals.h>
#include <stdio.h>
#include <dft.h>
#include <audio.h>
#include <curses.h>

int running = 1;

int main(void)
{
    if (audio_init())
        return 1;

    printf("Initializing...\r\n");
    initscr();
    noecho();
    nodelay(stdscr, TRUE);
    curs_set(0);

    int width, height;
    getmaxyx(stdscr, height, width);

    while (running)
    {
        float complex X[FRAMES];

        dft(samples, X, FRAMES);

        float m[FRAMES/2];
        float max_mag = 0.0;
        for (int k = 0; k < FRAMES/2; k++)
        {
            m[k] = cabs(X[k]);
            if (m[k] > max_mag)
                max_mag = m[k];
        }

        int num_bars = width; 
        int samples_per_bar = (FRAMES / 2) / num_bars;

        if (samples_per_bar < 1) samples_per_bar = 1;

        clear();

        for (int i = 0; i < num_bars; i++) 
        {
            float sum = 0.0;
            for (int j = 0; j < samples_per_bar; j++) 
            {
                int idx = i * samples_per_bar + j;
                if (idx < FRAMES / 2)
                    sum += m[idx];
            }
            float avg = sum / samples_per_bar;

            int bar_height = 0;
            if (max_mag > 0) 
                bar_height = (int)((avg / max_mag) * height);

            for (int y = 0; y < bar_height; y++)
                mvaddch(height - 1 - y, i, '-');
        }
        
        refresh();

        int ch = getch();
        if (ch == 'q')
            running = 0;
    }

    endwin();
    audio_destroy();
    return 0;
}

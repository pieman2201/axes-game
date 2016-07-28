#include "SDL.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

unsigned int LastTicks;
int PlayGrid[32][32];

void frame_limit(int limit) {
    float minTicks, diffTicks;
    unsigned int sinceTicks;

    sinceTicks = SDL_GetTicks() - LastTicks;
    minTicks = 1000.0 / (float) limit;
    diffTicks = floor(minTicks - (float) sinceTicks + 0.5);
    
    if (diffTicks > 0) {
        SDL_Delay((int) diffTicks);
    }
    LastTicks = SDL_GetTicks();
}

void draw_grid(SDL_Surface *s) {
    for (int x = 0; x < 32; x++) {
        for (int y = 0; y < 32; y++) {
            if (PlayGrid[x][y] > 0) {
                SDL_Rect targetRect;
                targetRect.x = x * 24;
                targetRect.y = 768 - 24 - (y * 24);
                targetRect.w = 24;
                targetRect.h = 24;
                SDL_FillRect(s, &targetRect, 0xff0000);
            }
        }
    }
}
                            
bool move_down() {
    for (int x = 0; x < 32; x++) {
        for (int y = 0; y < 32; y++) {
            if (PlayGrid[x][y] > 0 && y == 0) {
                return false;
            }
        }
    }
    for (int x = 0; x < 32; x++) {
        for (int y = 0; y < 32; y++) {
            if (y > 0) {
                PlayGrid[x][y-1] = PlayGrid[x][y];
                PlayGrid[x][y] = 0;
            }
        }
    }
    return true;
}

void add_targets() {
    int x;
    srand(time(NULL));
    x = rand() % 32;
    PlayGrid[x][31] = 1;
}

int main() {
    SDL_Window *window;

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
            ".-.",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            768,
            768,
            SDL_WINDOW_SHOWN
            );

    SDL_Surface *s;
    s = SDL_GetWindowSurface(window);
    SDL_Rect fullScreen;
    fullScreen.x = 0;
    fullScreen.y = 0;
    fullScreen.w = 768;
    fullScreen.h = 768;
    SDL_FillRect(s, &fullScreen, 0x000000);
    SDL_UpdateWindowSurface(window);

    bool run, triggered, hasShotX, hasShotY;
    int trigPos, xC, yC, score, xTrigPos;
    int step = 8;
    int trigDir = step;

    LastTicks = SDL_GetTicks();
    unsigned int lastUpdate = LastTicks;

    run = true;
    while (run) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                run = false;
            } else if (event.type == SDL_KEYDOWN) {
                SDL_Keysym key;
                key = event.key.keysym;
                if (key.sym == SDLK_SPACE) {
                    triggered = true;
                }
            }
        }
        if (hasShotX && hasShotY) {
            hasShotX = false;
            hasShotY = false;
            if (PlayGrid[xC / 24][(yC - 24) / 24] > 0) {
                PlayGrid[xC / 24][(yC - 24) / 24]--;
                score++;
            }
        } else {
            SDL_FillRect(s, &fullScreen, 0x000000);
            draw_grid(s);
            if (!hasShotX) {
                SDL_Rect markRect;
                markRect.x = trigPos;
                markRect.y = 0;
                markRect.w = 2;
                markRect.h = 768;
                if (triggered) {
                    if (trigPos > 768 - 2) {
                        trigPos = 768 - 2;
                    }
                    xTrigPos = trigPos;
                    xC = trigPos - (trigPos % 24);
                    hasShotX = true;
                    trigPos = 2 - step;
                    triggered = false;
                }
                if (trigPos <= 0) {
                    trigDir = step;
                } else if (trigPos >= 768 - 2) {
                    trigDir = -step;
                }
                SDL_FillRect(s, &markRect, 0xffffff);
            } else if (!hasShotY) {
                SDL_Rect markRect;
                markRect.x = 0;
                markRect.y = 768 - trigPos;
                markRect.w = 768;
                markRect.h = 2;
                if (triggered) {
                    if (trigPos < 2) {
                        trigPos = 2;
                    }
                    yC = trigPos + (24 - (trigPos % 24));
                    hasShotY = true;
                    trigPos = 0 - 2 * step;
                    triggered = false;
                } 
                if (trigPos <= 2) {
                    trigDir = step;
                } else if (trigPos >= 768) {
                    trigDir = -step;
                }
                SDL_FillRect(s, &markRect, 0xffffff);
                markRect.x = xTrigPos;
                markRect.y = 0;
                markRect.w = 2;
                markRect.h = 768;
                SDL_FillRect(s, &markRect, 0x00ff00);
            }
            SDL_UpdateWindowSurface(window);
        }
        if (SDL_GetTicks() - lastUpdate > 2000) {
            run = move_down();
            add_targets();
            lastUpdate = SDL_GetTicks();
            if (!run) {
                printf("Score: %d\n", score);
            }
        }
        trigPos += trigDir;
        frame_limit(60);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

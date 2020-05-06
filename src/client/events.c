#include <stdio.h>
#include <SDL.h>

#include "game.h"
#include "player.h"
#include "gamenet.h"

void HandleEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT) 
        {
            Game* game = GetGame();

            GameNetDisconnect();

            game->running = 0;
            SDL_Log("Program quit after %i ticks", event.quit.timestamp);
            break;
        }
        HandlePlayerEvents(&event); 

    }
    
    
}


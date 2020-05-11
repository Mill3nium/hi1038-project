#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_FontCache.h>
#include <time.h> 

#define HASHTABLE_IMPLEMENTATION
#include "shared/hashtable.h"
#undef HASHTABLE_IMPLEMENTATION

#include "memory.h"
#include "collision.h"
#include "events.h"
#include "player.h"
#include "game.h"
#include "audio.h"
#include "fonts.h"
#include "timer.h"
#include "netplayer.h"
#include "gamenet.h"

#include "shared/log.h"

void NetworkDisconnected(TCPsocket socket)
{
    LogInfo("DISCONNECTED");

    GameSetState(CR_STATE_MENU);

    SDL_ShowSimpleMessageBox(
        SDL_MESSAGEBOX_INFORMATION,
        "Connection",
        "Disconnected from server",
        NULL
    );
}

int main(int argc, const char *argv[])
{
    printf("Corona Royale\n");

    Game* game = GetGame();
    Fonts* fonts = GetFonts();

    GameSetState(CR_STATE_MENU);
    playMusic();
    while (game->running)
    {
        Uint32 frameStart = GameStartFrame();
        //// FRAME START
        
        { /////////// STATE UPDATES PHASE BEGIN ///////////
            HandleEvents();
            if(GameGetState() == CR_STATE_CONNECTED)
            {
                OnPlayerUpdate(&game->player);
            }
        } /////////// STATE UPDATES PHASE END /////////////

        { /////////// RENDERING PHASE BEGIN /////////
            SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
            SDL_RenderClear(game->renderer);

            
            if(GameGetState() == CR_STATE_MENU)
            {
                RenderMenu();
            }
            else if(GameGetState() == CR_STATE_CONNECTED)
            {
                // Draw background
                SDL_RenderCopy(game->renderer, game->background, &game->player.camera.cameraRect, NULL);

                // Render all net players
                if(GameGetState())
                {
                    NetPlayer* players = GameGetAllPlayers();
                    for (size_t i = 0; i < GameGetPlayerCount(); i++)
                    {
                        RenderNetPlayer(&players[i]);
                    }
                }

                OnPlayerRender(&game->player);
                // { // Draw CORONA ROYALE text
                //     static Uint8 r = 0;
                //     static float theta = 0.f; theta+=0.03f;
                //     r = ((sin(theta)+1)/2)*255;
                //     FC_DrawColor(fonts->openSans, game->renderer, 200, 50, FC_MakeColor(r, 20, 20, 255), "CORONA\n%s", "ROYALE");
                // }
                RendererTimer(&game->timer);
            }

            SDL_RenderPresent(game->renderer);
        } /////////// RENDERING PHASE END ///////////
        
        //// FRAME END
        GameEndFrame(frameStart);
    }

    DisposeGame();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    StopAudio();
    return 0;
}


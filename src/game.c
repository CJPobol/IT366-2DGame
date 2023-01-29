#include <SDL.h>
#include "gfc_shape.h"
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "simple_logger.h"
#include "entity.h"

void player_think(Entity* self);

int main(int argc, char * argv[])
{
    /*variable declarations*/
    int done = 0;
    const Uint8 * keys;
    Sprite *sprite;
    
    int mx,my;
    float mf = 0;
    Sprite *mouse;
    Color mouseColor = gfc_color8(255,100,255,200);
    
    /*program initializtion*/
    init_logger("gf2d.log", 1);
    slog("---==== BEGIN ====---");
    gf2d_graphics_initialize(
        "gf2d",
        1200,
        720,
        1200,
        720,
        vector4d(0,0,0,255),
        0);
    gf2d_graphics_set_frame_delay(16);
    gf2d_sprite_init(1024);
    entity_manager_init(1024);
    SDL_ShowCursor(SDL_DISABLE);
    
    /*demo setup*/
    sprite = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");
    mouse = gf2d_sprite_load_all("images/pointer.png",32,32,16,0);

    Entity* player = entity_new();
    if (player)
    {
        player->up = gf2d_sprite_load_image("images/playerUp.png");
        player->down = gf2d_sprite_load_image("images/playerDown.png");
        player->right = gf2d_sprite_load_image("images/playerRight.png");
        player->left = gf2d_sprite_load_image("images/playerLeft.png");

        player->currentSprite = player->down;
        player->position = vector2d(500, 300);

        

        player->think = player_think;
    }

    Entity* walls[8];
    for (int i = 0; i < 4; i++)
    {
        walls[i] = entity_new();
        walls[i]->currentSprite = gf2d_sprite_load_image("images/tallWall.png");
    }
    for (int i = 4; i < 8; i++)
    {
        walls[i] = entity_new();
        walls[i]->currentSprite = gf2d_sprite_load_image("images/wideWall.png");
    }
    walls[0]->position = vector2d(0, 0);
    walls[1]->position = vector2d(0, 500);
    walls[2]->position = vector2d(1150, 0);
    walls[3]->position = vector2d(1150, 500);
    walls[4]->position = vector2d(0, 0);
    walls[5]->position = vector2d(750, 0);
    walls[6]->position = vector2d(0, 670);
    walls[7]->position = vector2d(750, 670);

    for (int i = 0; i < 4; i++)
    {
        walls[i]->bounds = gfc_rect(walls[i]->position.x, walls[i]->position.y, 50, 250);
    }
    for (int i = 4; i < 8; i++)
    {
        walls[i]->bounds = gfc_rect(walls[i]->position.x, walls[i]->position.y, 450, 50);
    }

    /*main game loop*/
    while(!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/
        SDL_GetMouseState(&mx,&my);
        mf+=0.1;
        if (mf >= 16.0)mf = 0;

        entity_think_all();
        entity_update_all();
        
        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
            gf2d_sprite_draw_image(sprite,vector2d(0,0));
            
            entity_draw_all();

            //UI elements last
            gf2d_sprite_draw(
                mouse,
                vector2d(mx,my),
                NULL,
                NULL,
                NULL,
                NULL,
                &mouseColor,
                (int)mf);

        gf2d_graphics_next_frame();// render current draw frame and skip to the next frame


        //----------BASIC CONTROLS-----------//
        if (keys[SDL_SCANCODE_W])
        {
            if (!gfc_rect_overlap(player->bounds, walls[4]->bounds) && !gfc_rect_overlap(player->bounds, walls[5]->bounds))
            {
                player->currentSprite = player->up;
                player->position.y += -2;
            }
        }
        if (keys[SDL_SCANCODE_A])
        {
            if (!gfc_rect_overlap(player->bounds, walls[0]->bounds) && !gfc_rect_overlap(player->bounds, walls[1]->bounds))
            {
                player->currentSprite = player->left;
                player->position.x += -2;
            }
            
        }
        if (keys[SDL_SCANCODE_S])
        {
            if (!gfc_rect_overlap(player->bounds, walls[6]->bounds) && !gfc_rect_overlap(player->bounds, walls[7]->bounds))
            {
                player->currentSprite = player->down;
                player->position.y += 2;
            }
            
        }
        if (keys[SDL_SCANCODE_D])
        {
            if (!gfc_rect_overlap(player->bounds, walls[2]->bounds) && !gfc_rect_overlap(player->bounds, walls[3]->bounds))
            {
                player->currentSprite = player->right;
                player->position.x += 2;
            }
            
        }
        //-----------------------------------//

        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
        slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    slog("---==== END ====---");
    return 0;
}

void player_think(Entity* self)
{
    self->bounds = gfc_rect(self->position.x, self->position.y, 100, 150);
}

/*eol@eof*/

#include <SDL.h>
#include "gfc_shape.h"
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "simple_logger.h"
#include "entity.h"

void player_think(Entity* self);
void bullet_think(Entity* self);
void removeNodes(Entity* resources[4]);

void mainLevel(Entity* walls[8], Entity *self);

void resourceLevel(Entity* walls[8], Entity* self);

void combatLevel(Entity* walls[8], Entity* self);

void shopLevel(Entity* walls[8], Entity* self);

void menuLevel(Entity* walls[8], Entity* self);

void fireBullet(Vector2D velocity, Entity* player);

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
    sprite = gf2d_sprite_load_image("images/backgrounds/background.png");
    mouse = gf2d_sprite_load_all("images/pointer.png",32,32,16,0);

    Entity* resourceNodes[4];

    resourceNodes[0] = entity_new();
    resourceNodes[0]->currentSprite = gf2d_sprite_load_image("images/redResource.png");



    resourceNodes[1] = entity_new();
    resourceNodes[1]->currentSprite = gf2d_sprite_load_image("images/greenResource.png");



    resourceNodes[2] = entity_new();
    resourceNodes[2]->currentSprite = gf2d_sprite_load_image("images/blueResource.png");


    resourceNodes[3] = entity_new();
    resourceNodes[3]->currentSprite = gf2d_sprite_load_image("images/whiteResource.png");

    Entity* player = entity_new();
    if (player)
    {
        player->up = gf2d_sprite_load_image("images/playerUp.png");
        player->down = gf2d_sprite_load_image("images/playerDown.png");
        player->right = gf2d_sprite_load_image("images/playerRight.png");
        player->left = gf2d_sprite_load_image("images/playerLeft.png");

        player->currentSprite = player->down;
        player->position = vector2d(550, 300);

        player->cooldown = 100;

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

    player->level = 1;

    float lastBullet = player->cooldown; //how long ago the last bullet was fired


    

    /*main game loop*/
    while(!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/
        SDL_GetMouseState(&mx,&my);
        mf+=0.1;
        if (mf >= 16.0)mf = 0;
        lastBullet += 1;

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

        if (gfc_rect_overlap(player->bounds, resourceNodes[0]->bounds))
        {
            slog("Red");
        }
        if (gfc_rect_overlap(player->bounds, resourceNodes[1]->bounds))
        {
            slog("Green");
        }
        if (gfc_rect_overlap(player->bounds, resourceNodes[2]->bounds))
        {
            slog("Blue");
        }
        if (gfc_rect_overlap(player->bounds, resourceNodes[3]->bounds))
        {
            slog("White");
        }



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
        if (keys[SDL_SCANCODE_UP] && lastBullet >= player->cooldown)
        {
                fireBullet(vector2d(0, -5), player);
                lastBullet = 0;
        }
        if (keys[SDL_SCANCODE_DOWN] && lastBullet >= player->cooldown)
        {
            fireBullet(vector2d(0, 5), player);
            lastBullet = 0;
        }
        if (keys[SDL_SCANCODE_LEFT] && lastBullet >= player->cooldown)
        {
            fireBullet(vector2d(-5, 0), player);
            lastBullet = 0;
        }
        if (keys[SDL_SCANCODE_RIGHT] && lastBullet >= player->cooldown)
        {
            fireBullet(vector2d(5, 0), player);
            lastBullet = 0;
        }
        //-----------------------------------//

        if (player->level == 0)
        {
            menuLevel(walls, player);
            removeNodes(resourceNodes);
        }
            
        if (player->level == 1)
        {
            mainLevel(walls, player);
            removeNodes(resourceNodes);
        }

        if (player->level == 2)
        {
            resourceLevel(walls, player, resourceNodes);
        }

        if (player->level == 3)
        {
            shopLevel(walls, player);
            removeNodes(resourceNodes);
        }

        if (player->level == 4)
        {
            combatLevel(walls, player);
            removeNodes(resourceNodes);
        }


        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
        //slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    slog("---==== END ====---");
    return 0;
}



void player_think(Entity* self)
{
    self->bounds = gfc_rect(self->position.x, self->position.y, 100, 150);

    if (self->position.y <= 0) //top exit
    {
        self->position = vector2d(550, 710);
        if (self->level == 1)
            self->level = 0;
        if (self->level == 4)
            self->level = 1;
    }
    if (self->position.y >= 720) //bottom exit
    {
        self->position = vector2d(550, 10);
        if (self->level == 1)
            self->level = 4;
        if (self->level == 0)
            self->level = 1;
    }
    if (self->position.x <= 0) //left exit
    {
        self->position = vector2d(1190, 300);
        if (self->level == 1)
            self->level = 2;
        if (self->level == 3)
            self->level = 1;
    }
    if (self->position.x >= 1200) //right exit
    {
        self->position = vector2d(10, 300);
        if (self->level == 1)
            self->level = 3;
        if (self->level == 2)
            self->level = 1;

    }

}

void removeNodes(Entity* resources[4])
{
    for (int i = 0; i < 4; i++)
    {
        resources[i]->position = vector2d(-200, -200);
    }
}

void mainLevel(Entity* walls[8], Entity* self)
{
    
    walls[0]->position = vector2d(0, -100);
    walls[1]->position = vector2d(0, 460);
    walls[2]->position = vector2d(1150, -100);
    walls[3]->position = vector2d(1150, 460);
    walls[4]->position = vector2d(-100, 0);
    walls[5]->position = vector2d(700, 0);
    walls[6]->position = vector2d(-100, 670);
    walls[7]->position = vector2d(700, 670);

    for (int i = 0; i < 4; i++)
    {
        walls[i]->bounds = gfc_rect(walls[i]->position.x, walls[i]->position.y, 50, 360);
    }
    for (int i = 4; i < 8; i++)
    {
        walls[i]->bounds = gfc_rect(walls[i]->position.x, walls[i]->position.y, 600, 50);
    }
}

void menuLevel(Entity* walls[8], Entity* self)
{
    walls[0]->position = vector2d(0, 0);
    walls[1]->position = vector2d(0, 360);
    walls[2]->position = vector2d(1150, 0);
    walls[3]->position = vector2d(1150, 360);
    walls[4]->position = vector2d(0, 0);
    walls[5]->position = vector2d(600, 0);
    walls[6]->position = vector2d(-100, 670);
    walls[7]->position = vector2d(700, 670);

    for (int i = 0; i < 4; i++)
    {
        walls[i]->bounds = gfc_rect(walls[i]->position.x, walls[i]->position.y, 50, 360);
    }
    for (int i = 4; i < 8; i++)
    {
        walls[i]->bounds = gfc_rect(walls[i]->position.x, walls[i]->position.y, 600, 50);
    }
}

void resourceLevel(Entity* walls[8], Entity* self, Entity* resources[4])
{
    walls[0]->position = vector2d(0, 0);
    walls[1]->position = vector2d(0, 360);
    walls[2]->position = vector2d(1150, -100);
    walls[3]->position = vector2d(1150, 460);
    walls[4]->position = vector2d(0, 0);
    walls[5]->position = vector2d(600, 0);
    walls[6]->position = vector2d(0, 670);
    walls[7]->position = vector2d(600, 670);

    for (int i = 0; i < 4; i++)
    {
        walls[i]->bounds = gfc_rect(walls[i]->position.x, walls[i]->position.y, 50, 360);
    }
    for (int i = 4; i < 8; i++)
    {
        walls[i]->bounds = gfc_rect(walls[i]->position.x, walls[i]->position.y, 600, 50);
    }

    resources[0]->position = vector2d(100, 100);
    resources[1]->position = vector2d(700, 100);
    resources[2]->position = vector2d(100, 500);
    resources[3]->position = vector2d(700, 500);

    for (int i = 0; i < 4; i++)
    {
        resources[i]->bounds = gfc_rect(resources[i]->position.x, resources[i]->position.y, 300, 150);
    }

}

void shopLevel(Entity* walls[8], Entity* self) 
{
    walls[0]->position = vector2d(0, -100);
    walls[1]->position = vector2d(0, 460);
    walls[2]->position = vector2d(1150, 0);
    walls[3]->position = vector2d(1150, 360);
    walls[4]->position = vector2d(0, 0);
    walls[5]->position = vector2d(600, 0);
    walls[6]->position = vector2d(0, 670);
    walls[7]->position = vector2d(600, 670);

    for (int i = 0; i < 4; i++)
    {
        walls[i]->bounds = gfc_rect(walls[i]->position.x, walls[i]->position.y, 50, 360);
    }
    for (int i = 4; i < 8; i++)
    {
        walls[i]->bounds = gfc_rect(walls[i]->position.x, walls[i]->position.y, 600, 50);
    }
}

void combatLevel(Entity* walls[8], Entity* self)
{
    walls[0]->position = vector2d(0, 0);
    walls[1]->position = vector2d(0, 360);
    walls[2]->position = vector2d(1150, 0);
    walls[3]->position = vector2d(1150, 360);
    walls[4]->position = vector2d(-100, 0);
    walls[5]->position = vector2d(700, 0);
    walls[6]->position = vector2d(0, 670);
    walls[7]->position = vector2d(600, 670);

    for (int i = 0; i < 4; i++)
    {
        walls[i]->bounds = gfc_rect(walls[i]->position.x, walls[i]->position.y, 50, 360);
    }
    for (int i = 4; i < 8; i++)
    {
        walls[i]->bounds = gfc_rect(walls[i]->position.x, walls[i]->position.y, 600, 50);
    }
}

void fireBullet(Vector2D velocity, Entity* player)
{
    Entity* bullet = entity_new();
    vector2d_add(bullet->position, player->position, vector2d(50, 75));
    bullet->currentSprite = gf2d_sprite_load_image("images/bullet.png");
    bullet->bounds = gfc_rect(bullet->position.x, bullet->position.y, 10, 10);
    bullet->think = bullet_think;
    bullet->velocity = velocity;
}

void bullet_think(Entity* self)
{
    if (self->position.x >= 0 && self->position.x <= 1200 && self->position.y >= 0 && self->position.y >= 720)
    {
        vector2d_add(self->position, self->position, self->velocity);
        
    }

}

/*eol@eof*/

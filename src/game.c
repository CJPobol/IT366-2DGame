#include <SDL.h>
#include <stdlib.h>
#include "gfc_shape.h"
#include "gfc_input.h"
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "gf2d_draw.h"
#include "simple_logger.h"
#include "entity.h"

void powerUp(Entity* powerup, Vector2D position);
void player_think(Entity* self);
void bullet_think(Entity* self, Entity* monster1, Entity* monster2, Entity* monster3, Entity* monster4, Entity* monster5);
void enemy_think(Entity* self, Entity* player);
void removeNodes(Entity* resources[4]);

void mainLevel(Entity* walls[8], Entity *self);

void resourceLevel(Entity* walls[8], Entity* self, Entity* resources[4]);

void spawnEnemy(Entity* player, int enemiesSpawned);

void combatLevel(Entity* walls[8], Entity* self, int time, int enemiesSpawned);

void shopLevel(Entity* walls[8], Entity* self, Entity* shop);

void menuLevel(Entity* walls[8], Entity* self);

Entity* fireBullet(Vector2D velocity, Entity* player);

int menuOn = 1;
int editor = 0;

int main(int argc, char * argv[])
{
    /*variable declarations*/
    srand(time());

    int done = 0;
    int enemiesSpawned = 0;
    int upgradeTiers[5] = { 0,0,0,0,0 };
    const Uint8 * keys;
    Sprite *sprite;
    
    int mx,my;
    float mf = 0;
    Sprite *mouse;
    Color mouseColor = gfc_color8(255,100,255,200);
    
    /*program initializtion*/
    init_logger("gf2d.log");
    gfc_input_init("config/input.cfg");
    slog("---==== BEGIN ====---");
    slog_sync();
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
    Sprite* menu = gf2d_sprite_load_image("images/mainmenu.png");

    Entity* resourceNodes[4];

    resourceNodes[0] = entity_new();
    resourceNodes[0]->currentSprite = gf2d_sprite_load_image("images/redResource.png");



    resourceNodes[1] = entity_new();
    resourceNodes[1]->currentSprite = gf2d_sprite_load_image("images/greenResource.png");



    resourceNodes[2] = entity_new();
    resourceNodes[2]->currentSprite = gf2d_sprite_load_image("images/blueResource.png");


    resourceNodes[3] = entity_new();
    resourceNodes[3]->currentSprite = gf2d_sprite_load_image("images/whiteResource.png");


    Entity* shop = entity_new();
    shop->currentSprite = gf2d_sprite_load_image("images/shop.png");
    shop->position = vector2d(-200, -200);
    shop->bounds = gfc_rect(shop->position.x, shop->position.y, 250, 100);


    Entity* player = entity_new();
    if (player)
    {
        player->up = gf2d_sprite_load_image("images/playerUp.png");
        player->down = gf2d_sprite_load_image("images/playerDown.png");
        player->right = gf2d_sprite_load_image("images/playerRight.png");
        player->left = gf2d_sprite_load_image("images/playerLeft.png");

        player->currentSprite = player->down;
        player->position = vector2d(550, 300);
        player->velocity = vector2d(2, 2);

        player->collectionRate = 300;
        player->cooldown = 100;
        player->totalHealth = 100;
        player->currentHealth = 100;
        player->damage = 10;

        player->red = 0;
        player->green = 0;
        player->blue = 0;
        player->white = 0;
        player->coins = 0;
        player->usingPower = 5;

        player->shopping = 0;

        player->think = player_think;
        slog("player initialized");
    }

    Entity* monster1 = entity_new();
    monster1->currentSprite = gf2d_sprite_load_image("images/monster1.png");
    monster1->currentHealth = 10;
    monster1->damage = 0.5;
    monster1->bounds = gfc_rect(1000, 500, 100, 150);
    monster1->think = enemy_think;
    monster1->killed = false;

    Entity* monster2 = entity_new();
    monster2->currentSprite = gf2d_sprite_load_image("images/monster2.png");
    monster2->currentHealth = 30;  
    monster2->damage = 0.6;
    monster2->bounds = gfc_rect(100, 200, 100, 120);
    monster2->think = enemy_think;
    monster2->killed = false;

    Entity* monster3 = entity_new();
    monster3->currentSprite = gf2d_sprite_load_image("images/monster3.png");
    monster3->currentHealth = 70;   
    monster3->damage = 0.3;
    monster3->bounds = gfc_rect(550, 500, 100, 100);
    monster3->think = enemy_think;
    monster3->killed = false;

    Entity* monster4 = entity_new();
    monster4->currentSprite = gf2d_sprite_load_image("images/monster4.png");
    monster4->currentHealth = 20;   
    monster4->damage = 0.2;
    monster4->bounds = gfc_rect(100, 500, 100, 150);
    monster4->think = enemy_think;
    monster4->killed = false;

    Entity* monster5 = entity_new();
    monster5->currentSprite = gf2d_sprite_load_image("images/monster5.png");
    monster5->currentHealth = 50;   
    monster5->damage = 0.4;
    monster5->bounds = gfc_rect(1000, 200, 100, 150);
    monster5->think = enemy_think;
    monster5->killed = false;

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

    Entity* powerup = entity_new();


    Sprite* shopMenu = gf2d_sprite_load_image("images/upgradeList.png");

    player->level = 1;

    float lastBullet = player->cooldown; //how long ago the last bullet was fired

    Sprite* collectResourcePrompt = gf2d_sprite_load_image("images/collectResourcePrompt.png");
    
    float lastCollect = player->collectionRate;
    int lastEnemy = 0;

    int bluePrice = 5;
    int redPrice = 5;
    int greenPrice = 5;
    int whitePrice = 5;
    int coinPrice = 5;

    //REAL PRICES (LOWERED FOR IN-CLASS TESTING)
    /*int bluePrice = 10;
    int redPrice = 10;
    int greenPrice = 10;
    int whitePrice = 10;
    int coinPrice = 10;*/

    Entity* currentBullet = entity_new();
    
    int powerUpTracker = 0;
    /*main game loop*/
    while(!done)
    {
        gfc_input_update();
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/
        SDL_GetMouseState(&mx,&my);
        mf+=0.1;
        if (mf >= 16.0)mf = 0;
        lastBullet += 1;
        lastCollect += 1;
        lastEnemy += 1;
        if (player->usingPower > 0) powerUpTracker += 1;
        if (player->usingPower == 1)
        {
            player->velocity = vector2d(upgradeTiers[0] + 3, upgradeTiers[0] + 3);
        }
        if (player->usingPower == 2)
        {
            player->damage = 10 + upgradeTiers[2]*10 + 10;
        }
        if (player->usingPower == 4)
        {
            player->cooldown = (100 - (upgradeTiers[1] * 20)) - 40;
        }
        if (player->usingPower == 5)
        {
            player->totalHealth = (100 + (50 * upgradeTiers[3])) + 50;
            if (player->currentHealth < player->totalHealth)player->currentHealth += 0.2;
        }

        if (powerUpTracker >= 1000)
        {
            if (player->usingPower == 1) vector2d_sub(player->velocity, player->velocity, vector2d(1, 1));
            if (player->usingPower == 2) player->damage -= 10;
            if (player->usingPower == 4) player->cooldown += 40;
            if (player->usingPower == 5)
            {
                player->totalHealth -= 50;
                if (player->currentHealth > player->totalHealth)
                    player->currentHealth = player->totalHealth;
            }
            player->usingPower = 0;
            powerUpTracker = 0;
        }

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

            if (player->level == 4) //combat
            {
                gf2d_draw_rect_filled(gfc_rect(0, 0, player->totalHealth*2, 25), gfc_color(0,0,0,1));
                gf2d_draw_rect_filled(gfc_rect(0, 0, player->currentHealth*2, 25), gfc_color(1, 0, 0, 1));
            }
            if (player->shopping)
            {
                gf2d_sprite_draw(shopMenu,
                    vector2d(600, 0),
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    0);
                gf2d_draw_rect_filled(gfc_rect(0, 50, 250, 50), gfc_color(0, 0, 0, 1));
                gf2d_draw_rect_filled(gfc_rect(0, 50, upgradeTiers[0] * 50, 50), gfc_color(0, 1, 0, 1));

                gf2d_draw_rect_filled(gfc_rect(0, 110, 250, 50), gfc_color(0, 0, 0, 1));
                gf2d_draw_rect_filled(gfc_rect(0, 110, upgradeTiers[1] * 50, 50), gfc_color(0, 1, 0, 1));

                gf2d_draw_rect_filled(gfc_rect(0, 170, 250, 50), gfc_color(0, 0, 0, 1));
                gf2d_draw_rect_filled(gfc_rect(0, 170, upgradeTiers[2] * 50, 50), gfc_color(0, 1, 0, 1));

                gf2d_draw_rect_filled(gfc_rect(0, 230, 250, 50), gfc_color(0, 0, 0, 1));
                gf2d_draw_rect_filled(gfc_rect(0, 230, upgradeTiers[3] * 50, 50), gfc_color(0, 1, 0, 1));

                gf2d_draw_rect_filled(gfc_rect(0, 290, 250, 50), gfc_color(0, 0, 0, 1));
                gf2d_draw_rect_filled(gfc_rect(0, 290, upgradeTiers[4] * 50, 50), gfc_color(0, 1, 0, 1));
            }

            if (gfc_rect_overlap(player->bounds, resourceNodes[0]->bounds) || gfc_rect_overlap(player->bounds, resourceNodes[1]->bounds)
                || gfc_rect_overlap(player->bounds, resourceNodes[2]->bounds) || gfc_rect_overlap(player->bounds, resourceNodes[3]->bounds))
            {
                gf2d_sprite_draw(collectResourcePrompt,
                    vector2d(0, 670),
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    0);
            }
            if (menuOn == 1)
            {
                gf2d_sprite_draw(menu, vector2d(0, 0), NULL, NULL, NULL, NULL, NULL, 0);
            }

        gf2d_graphics_next_frame();// render current draw frame and skip to the next frame



        //----------BASIC CONTROLS-----------//
        if (keys[SDL_SCANCODE_W])
        {
            if (!gfc_rect_overlap(player->bounds, walls[4]->bounds) && !gfc_rect_overlap(player->bounds, walls[5]->bounds))
            {
                player->currentSprite = player->up;
                player->position.y -= player->velocity.y;
            }
        }
        if (keys[SDL_SCANCODE_A])
        {
            if (!gfc_rect_overlap(player->bounds, walls[0]->bounds) && !gfc_rect_overlap(player->bounds, walls[1]->bounds))
            {
                player->currentSprite = player->left;
                player->position.x -= player->velocity.x;
            }
            
        }
        if (keys[SDL_SCANCODE_S])
        {
            if (!gfc_rect_overlap(player->bounds, walls[6]->bounds) && !gfc_rect_overlap(player->bounds, walls[7]->bounds))
            {
                player->currentSprite = player->down;
                player->position.y += player->velocity.y;
            }
        }
        if (keys[SDL_SCANCODE_D])
        {
            if (!gfc_rect_overlap(player->bounds, walls[2]->bounds) && !gfc_rect_overlap(player->bounds, walls[3]->bounds))
            {
                player->currentSprite = player->right;
                player->position.x += player->velocity.x;
            }
        }
        if (gfc_input_command_pressed("interact"))
        {
            if (gfc_rect_overlap(player->bounds, resourceNodes[0]->bounds) && lastCollect >= player->collectionRate) //red resource
            {
                player->red++;
                slog("Red Resource: %i", player->red);
                lastCollect = 0;
            }
            if (gfc_rect_overlap(player->bounds, resourceNodes[1]->bounds) && lastCollect >= player->collectionRate) //green resource
            {
                player->green++;
                lastCollect = 0;
                slog("Green Resource: %i", player->green);
            }
            if (gfc_rect_overlap(player->bounds, resourceNodes[2]->bounds) && lastCollect >= player->collectionRate) //blue resource
            {
                player->blue++;
                lastCollect = 0;
                slog("Blue Resource: %i", player->blue);
            }
            if (gfc_rect_overlap(player->bounds, resourceNodes[3]->bounds) && lastCollect >= player->collectionRate) //white resource
            {
                player->white++;
                lastCollect = 0;
                slog("White Resource: %i", player->white);
            }
            if (gfc_rect_overlap(player->bounds, shop->bounds)) //shop
            {
                player->shopping = !player->shopping;
            }
        }
        //NOTE: PRICES ADJUSTED FOR EASIER SHOWCASING IN CLASS; COMMENTED PRICES ARE THE BALANCED ADJUSTMENTS
        if (gfc_input_command_pressed("1") && player->shopping)
        {
            if (player->blue >= bluePrice && upgradeTiers[0] < 5)
            {
                vector2d_add(player->velocity, player->velocity, vector2d(1, 1));
                player->blue -= bluePrice;
                //bluePrice *= 2;
                bluePrice += 5;
                upgradeTiers[0]++;
                slog("Blue Resource: %i", player->blue);
            }
        }
        if (gfc_input_command_pressed("2") && player->shopping)
        {
            if (player->green >= greenPrice && upgradeTiers[1] < 5)
            {
                player->cooldown -= 20;
                player->currentHealth = player->totalHealth;
                player->green -= greenPrice;
                //greenPrice *= 2;
                greenPrice += 5;
                upgradeTiers[1]++;
                slog("Green Resource: %i", player->green);
            }
        }
        if (gfc_input_command_pressed("3") && player->shopping)
        {
            if (player->red >= redPrice && upgradeTiers[2] < 5)
            {
                player->damage += 10;
                player->red -= redPrice;
                //redPrice *= 2;
                redPrice += 5;
                upgradeTiers[2]++;
                slog("Red Resource: %i", player->red);
            }
        }
        if (gfc_input_command_pressed("4") && player->shopping)
        {
            if (player->white >= whitePrice && upgradeTiers[3] < 5)
            {
                player->totalHealth += 50;
                player->white -= whitePrice;
                //whitePrice *= 2;
                whitePrice += 5;
                upgradeTiers[3]++;
                slog("White Resource: %i", player->white);
            }
        }
        if (gfc_input_command_pressed("5") && player->shopping)
        {
            if (player->coins >= coinPrice && upgradeTiers[4] < 5)
            {
                player->collectionRate -= 50;
                player->coins -= coinPrice;
                //coinPrice *= 2;
                coinPrice += 5;
                upgradeTiers[4]++;
                slog("Coins: %i", player->coins);
            }
        }

        if (keys[SDL_SCANCODE_UP] && lastBullet >= player->cooldown)
        {
            currentBullet = fireBullet(vector2d(0, -5), player);
            lastBullet = 0;
        }
        if (keys[SDL_SCANCODE_DOWN] && lastBullet >= player->cooldown)
        {
            currentBullet = fireBullet(vector2d(0, 5), player);
            lastBullet = 0;
        }
        if (keys[SDL_SCANCODE_LEFT] && lastBullet >= player->cooldown)
        {
            currentBullet = fireBullet(vector2d(-5, 0), player);
            lastBullet = 0;
        }
        if (keys[SDL_SCANCODE_RIGHT] && lastBullet >= player->cooldown)
        {
            currentBullet = fireBullet(vector2d(5, 0), player);
            lastBullet = 0;
        }
        if (keys[SDL_SCANCODE_RETURN])
        {
            editor = 0;
            player->level = 0;
            player->position = vector2d(550, 300);
            menuOn = 0;
        }
        if (keys[SDL_SCANCODE_ESCAPE])
        {
            menuOn = 1;
        }
        if (keys[SDL_SCANCODE_L])
        {
            editor = 1;
            menuOn = 0;
            player->level = 0;
            player->position = vector2d(-200, -200);
        }
        //-----------------------------------//

        if (player->level == 0)
        {
            menuLevel(walls, player);
            removeNodes(resourceNodes);

            monster1->position = vector2d(-200, -200);
            monster2->position = vector2d(-200, -200);
            monster3->position = vector2d(-200, -200);
            monster4->position = vector2d(-200, -200);
            monster5->position = vector2d(-200, -200);
            powerup->position = vector2d(-200, -200);
        }
            
        if (player->level == 1)
        {
            mainLevel(walls, player);
            removeNodes(resourceNodes);
            shop->position = vector2d(-200, -200);
            shop->bounds = gfc_rect(shop->position.x, shop->position.y, 250, 100);

            monster1->position = vector2d(-200, -200);
            monster2->position = vector2d(-200, -200);
            monster3->position = vector2d(-200, -200);
            monster4->position = vector2d(-200, -200);
            monster5->position = vector2d(-200, -200);
            powerup->position = vector2d(-200, -200);
        }

        if (player->level == 2)
        {
            resourceLevel(walls, player, resourceNodes);
            shop->position = vector2d(-200, -200);
            shop->bounds = gfc_rect(shop->position.x, shop->position.y, 250, 100);
        }

        if (player->level == 3)
        {
            shopLevel(walls, player, shop);
            removeNodes(resourceNodes);
            shop->position = vector2d(600, 200);
            shop->bounds = gfc_rect(shop->position.x, shop->position.y, 250, 100);
        }

        if (player->level == 4)
        {
            combatLevel(walls, player, lastEnemy, enemiesSpawned);
            removeNodes(resourceNodes);
            shop->position = vector2d(-200, -200);
            shop->bounds = gfc_rect(shop->position.x, shop->position.y, 250, 100);

            if (!monster1->killed)monster1->position = vector2d(1000, 500);
            if (!monster2->killed)monster2->position = vector2d(100, 200);
            if (!monster3->killed)monster3->position = vector2d(550, 500);
            if (!monster4->killed)monster4->position = vector2d(100, 500);
            if (!monster5->killed)monster5->position = vector2d(1000, 200);

            if (gfc_rect_overlap(player->bounds, powerup->bounds))
            {
                powerup->position = vector2d(-200, -200);
                player->usingPower = powerup->usingPower;
            }
            if (gfc_rect_overlap(monster1->bounds, player->bounds))
            {
                if (player->usingPower != 3)
                    player->currentHealth -= monster1->damage;
            }
            if (gfc_rect_overlap(monster2->bounds, player->bounds))
            {
                if (player->usingPower != 3)
                    player->currentHealth -= monster2->damage;
            }
            if (gfc_rect_overlap(monster3->bounds, player->bounds))
            {
                if (player->usingPower != 3)
                    player->currentHealth -= monster3->damage;
            }
            if (gfc_rect_overlap(monster4->bounds, player->bounds))
            {
                if (player->usingPower != 3)
                    player->currentHealth -= monster4->damage;
            }
            if (gfc_rect_overlap(monster5->bounds, player->bounds))
            {
                if (player->usingPower != 3)
                    player->currentHealth -= monster5->damage;
            }
            if (gfc_rect_overlap(monster1->bounds, currentBullet->bounds))
            {
                monster1->currentHealth -= currentBullet->damage;
                entity_free(currentBullet);
            }
            if (gfc_rect_overlap(monster2->bounds, currentBullet->bounds))
            {
                monster2->currentHealth -= currentBullet->damage;
                entity_free(currentBullet);
            }
            if (gfc_rect_overlap(monster3->bounds, currentBullet->bounds))
            {
                monster3->currentHealth -= currentBullet->damage;
                entity_free(currentBullet);
            }
            if (gfc_rect_overlap(monster4->bounds, currentBullet->bounds))
            {
                monster4->currentHealth -= currentBullet->damage;
                entity_free(currentBullet);
            }
            if (gfc_rect_overlap(monster5->bounds, currentBullet->bounds))
            {
                monster5->currentHealth -= currentBullet->damage;
                entity_free(currentBullet);
            }
            if (monster1->currentHealth <= 0 && !monster1->killed)
            {
                powerUp(powerup, monster1->position);
                monster1->killed = true;
                monster1->position = vector2d(-200, -200);
                player->coins++;
                slog("Coins: %i", player->coins);
            }
            if (monster2->currentHealth <= 0 && !monster2->killed)
            {
                powerUp(powerup, monster2->position);
                monster2->killed = true;
                monster2->position = vector2d(-200, -200);
                player->coins += 3;
                slog("Coins: %i", player->coins);
            }
            if (monster3->currentHealth <= 0 && !monster3->killed)
            {
                powerUp(powerup, monster3->position);
                monster3->killed = true;
                monster3->position = vector2d(-200, -200);
                player->coins += 2;
                slog("Coins: %i", player->coins);
            }
            if (monster4->currentHealth <= 0 && !monster4->killed)
            {
                powerUp(powerup, monster4->position);
                monster4->killed = true;
                monster4->position = vector2d(-200, -200);
                player->coins += 3;
                slog("Coins: %i", player->coins);
            }
            if (monster5->currentHealth <= 0 && !monster5->killed)
            {
                powerUp(powerup, monster5->position);
                monster5->killed = true;
                monster5->position = vector2d(-200, -200);
                player->coins += 4;
                slog("Coins: %i", player->coins);
            }

        }

        if (keys[SDL_SCANCODE_0])done = 1; // exit condition
        //if (!player->shopping)
            //slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    slog("---==== END ====---");
    return 0;
}

void powerUp(Entity* powerup, Vector2D position)
{
    if (gfc_random() <= 0.4) //40% chance
    {
        powerup->currentSprite = gf2d_sprite_load_image("images/powerUp.png");
        powerup->usingPower = (rand() % 5) + 1;
        powerup->position = position;
        powerup->bounds = gfc_rect(powerup->position.x, powerup->position.y, 30, 30);
    }
}

void player_think(Entity* self)
{
    self->bounds = gfc_rect(self->position.x, self->position.y, 100, 150);

    if (self->position.y <= 0 && editor == 0) //top exit
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
    if (self->position.x <= 0 && editor == 0) //left exit
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

    if (self->currentHealth <= 0)
    {
        menuOn = 1;
        self->level = 0;
        self->position = vector2d(550, 300);
        self->currentHealth = self->totalHealth;
        self->coins -= 5;
        if (self->coins < 0)
            self->coins = 0;
    }

}

void removeNodes(Entity* resources[4])
{
    for (int i = 0; i < 4; i++)
    {
        resources[i]->position = vector2d(-200, -200);
        resources[i]->bounds = gfc_rect(resources[i]->position.x, resources[i]->position.y, 300, 150);
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

void shopLevel(Entity* walls[8], Entity* self, Entity* shop) 
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

    shop->position = vector2d(600, 200);
    shop->bounds = gfc_rect(shop->position.x, shop->position.y, 250, 100);
}

void combatLevel(Entity* walls[8], Entity* self, int time, int enemiesSpawned)
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

void spawnEnemy(Entity* player, int enemiesSpawned)
{
    //RANDOM ENEMY SPAWNING NOT WORKING YET; EVENTUALLY USE THIS INSTEAD OF WHATS ABOVE
    /*Entity* enemy = entity_new();
    
    int enemyType = rand() % 5;
    float width = 100, height = 100;
    Rect enemyspawn;
    int x, y;
    do
    {
        x = 50 + rand() % 1000;
        y = 50 + rand() % 480;
        slog("determined position: %i, %i", x, y);
        
        
        enemyspawn = gfc_rect(x, y, width, height);
    } while (gfc_rect_overlap(enemyspawn, player->bounds)); //to make sure enemies don't spawn on top of you

    //enemy->position = vector2d(x, y);
    enemy->position.x = 400;
    enemy->position.y = 500;
    
    enemy->bounds = enemyspawn;


    enemy->think = enemy_think;
    switch (enemyType) {
    case 0:
        enemy->currentSprite = gf2d_sprite_load_image("images/monster1.png");
        enemy->currentHealth = 10;
        enemy->velocity = vector2d(2,2);
        enemy->damage = 1;
        width = 100;
        height = 150;
        break;

    case 1:
        enemy->currentSprite = gf2d_sprite_load_image("images/monster2.png");
        enemy->currentHealth = 30;
        enemy->velocity = vector2d(1, 1);
        enemy->damage = 2;
        width = 100;
        height = 120;
        break;

    case 2:
        enemy->currentSprite = gf2d_sprite_load_image("images/monster3.png");
        enemy->currentHealth = 70;
        enemy->velocity = vector2d(0.5, 0.5);
        enemy->damage = 1;
        width = 100;
        height = 100;
        break;

    case 3:
        enemy->currentSprite = gf2d_sprite_load_image("images/monster4.png");
        enemy->currentHealth = 20;
        enemy->velocity = vector2d(3, 3);
        enemy->damage = 0.5;
        width = 100;
        height = 150;
        break;

    case 4:
        enemy->currentSprite = gf2d_sprite_load_image("images/monster5.png");
        enemy->currentHealth = 50;
        enemy->velocity = vector2d(4, 4);
        enemy->damage = 1;
        width = 100;
        height = 150;

    }

    
    */
}

void enemy_think(Entity* self, Entity* player)
{
    self->bounds = gfc_rect(self->position.x, self->position.y, 100, 100);
    /*if (self->position.x >= 50 && self->position.x <= 1050 && self->position.y >= 50 && self->position.y <= 570)
    {
        if (player->position.x < self->position.x)
        {
            if (player->position.y < self->position.y)
            {
                self->velocity = player->velocity;
                vector2d_add(self->position, self->position, self->velocity);
            }
            else
                vector2d_add(self->position, self->position, vector2d(-self->velocity.x, self->velocity.y));
        }
        else if (player->position.y < self->position.y)
        {
            vector2d_add(self->position, self->position, vector2d(self->velocity.x, -self->velocity.y));
        }
        else
            vector2d_add(self->position, self->position, self->velocity);

        self->bounds = gfc_rect(self->position.x, self->position.y, 100, 100);
        
    }*/
    
    
    

}

Entity* fireBullet(Vector2D velocity, Entity* player)
{
    Entity* bullet = entity_new();
    vector2d_add(bullet->position, player->position, vector2d(50, 75));
    bullet->currentSprite = gf2d_sprite_load_image("images/bullet.png");
    bullet->bounds = gfc_rect(bullet->position.x, bullet->position.y, 10, 10);
    bullet->think = bullet_think;
    bullet->velocity = velocity;
    bullet->damage = player->damage;
    return bullet;
}

void bullet_think(Entity* self, Entity* monster1, Entity* monster2, Entity* monster3, Entity* monster4, Entity* monster5)
{
    if (self->position.x >= 0 && self->position.x <= 1200 && self->position.y >= 0 && self->position.y <= 720)
    {
        vector2d_add(self->position, self->position, self->velocity);
        self->bounds = gfc_rect(self->position.x, self->position.y, 10, 10);
        if (gfc_rect_overlap(self->bounds, monster1->bounds))
        {
            monster1->currentHealth -= self->damage;
            entity_free(self);
        }
            
    }
    else
        entity_free(self);

}

/*eol@eof*/

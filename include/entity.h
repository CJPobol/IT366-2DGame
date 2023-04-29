#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gfc_types.h"
#include "gfc_shape.h"
#include "gf2d_sprite.h"

typedef struct Entity_S
{
	Bool		_inuse;

	Sprite*		down;
	Sprite*		up;
	Sprite*		right;
	Sprite*		left;

	Sprite*		currentSprite;

	float		frame;
	Vector2D	position;
	Vector2D	savedPosition;
	Vector2D	velocity;			//PLAYER UPGRADE LEVELS: 2, 3, 4, 5, 6
	Vector2D	acceleration;

	int			level;				//0 = menu; 1 = main; 2 = resource; 3 = shop; 4 = combat
	
	float		cooldown;			//PLAYER UPGRADE LEVELS: 100, 80, 60, 40, 20
	float		damage;				//PLAYER UPGRADE LEVELS: 10, 20, 30, 40, 50
	int			totalHealth;		//PLAYER UPGRADE LEVELS: 100, 150, 200, 250, 300
	int			currentHealth;		
	float		collectionRate;		//PLAYER UPGRADE LEVELS: 300, 250, 200, 150, 100

	int			red;
	int			green;
	int			blue;
	int			white;
	int			coins;

	Bool		shopping;

	Rect		bounds;

	void(*think)(struct Entity_S *self);

}Entity;

/**
* @brief initialize the internal manager for the entity system
* @note this will automatically queue up the close function for program exit
* @param max: this is the maximum number of supported entities at a given time
*/
void entity_manager_init(Uint32 max);

/**
* @brief allocate and initialize a new entity
* @return NULL if there are no entities left, an empty entity otherwise
*/
Entity* entity_new();

/**
* @brief free a previously allocated entity
* @param the entity to be freed
*/
void entity_free(Entity* ent);

/**
* @brief free all entities
*/
void entity_free_all();

void entity_draw_all();

void entity_think_all();

void entity_update_all();

#endif

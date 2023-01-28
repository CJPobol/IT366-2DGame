#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gfc_types.h"
#include "gf2d_sprite.h"

typedef struct Entity_S
{
	Bool		_inuse;
	Sprite*		sprite;
	float		frame;
	Vector2D	position;
	Vector2D	velocity;
	Vector2D	acceleration;

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

void entity_update_all();

#endif

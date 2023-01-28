#include "simple_logger.h"
#include "entity.h"

typedef struct
{
	Uint32 entity_max;
	Entity* entity_list;
}EntityManager;

static EntityManager entity_manager = { 0 };

void entity_manager_close()
{
	if (entity_manager.entity_list)free(entity_manager.entity_list);
}


void entity_manager_init(Uint32 max)
{
	if (max <= 0)
	{
		slog("cannot initialize entity system: zero entities specified!");
		return;
	}
	entity_manager.entity_list = gfc_allocate_array(sizeof(Entity), max);
	if (!entity_manager.entity_list)
	{
		slog("failed to initialize entity system");
		return;
	}
	entity_manager.entity_max = max;
	atexit(entity_manager_close);

}

Entity* entity_new()
{
	int i;
	for (i = 0; i < entity_manager.entity_max; i++)
	{
		if (entity_manager.entity_list[i]._inuse)continue;
		entity_manager.entity_list[i]._inuse = 1;
		return &entity_manager.entity_list[i];
	}
	return NULL;
}

void entity_free_all()
{
	int i;
	for (i = 0; i < entity_manager.entity_max; i++)
	{
		if (!entity_manager.entity_list[i]._inuse)continue;
		entity_free(&entity_manager.entity_list[i]);
	}
	return NULL;
}

void entity_free(Entity* ent)
{
	if (!ent)
	{
		slog("no entity provided");
		return;
	}
	if (ent->sprite)gf2d_sprite_free(ent->sprite);
	memset(ent, 0, sizeof(Entity));
}

void entity_draw(Entity *ent)
{
	if (!ent) return;
	if (ent->sprite)
	{
		gf2d_sprite_draw(
			ent->sprite,
			ent->position,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			(int)ent->frame);
	}
}

void entity_draw_all()
{
	int i;
	for (i = 0; i < entity_manager.entity_max; i++)
	{
		if (!entity_manager.entity_list[i]._inuse)continue;
		entity_draw(&entity_manager.entity_list[i]);
	}
}

void entity_think(Entity* ent)
{
	if (!ent) return;
}

void entity_think_all()
{

};

void entity_update()
{

};

void entity_update_all()
{

};
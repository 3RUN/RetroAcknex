
// register new lens flare for the given entity with the given offset
LIGHT_SOURCE *lens_flare_register(ENTITY *ent, VECTOR *offset_)
{
	if(!ent)
	{
		error("ERROR! Can't register lens flare! Entity doesn't exist!");
		return NULL;
	}
	
	ent->OBJ_TYPE = TYPE_LIGHT;
	
	LIGHT_SOURCE *light = sys_malloc(sizeof(LIGHT_SOURCE));
	ent->OBJ_STRUCT = light;
	ent->OBJ_ID = total_light_sources;
	
	vec_set(&light->offset, offset_);
	vec_set(&light->world_pos, &light->offset);
	vec_rotate(&light->world_pos, &ent->pan);
	vec_add(&light->world_pos, &ent->x);
	
	light->faded = true;
	light->owner_ent = ent;
	light->aura_ent = ent_create(lens_flare_b_tga, &light->world_pos, NULL);
	set(light->aura_ent, TRANSLUCENT | BRIGHT | PASSABLE | LIGHT | UNLIT | NOFOG);
	vec_set(&light->aura_ent->blue, &ent->blue);
	light->aura_ent->material = mtl_lens_flare_;
	
	light->aura_ent->ambient = 100;
	light->aura_ent->albedo = -100;
	light->aura_ent->alpha = 0;
	light->aura_ent->scale_x = LIGHT_AURA_SIZE_X;
	light->aura_ent->scale_y = LIGHT_AURA_SIZE_Y;
	
	lens_flare_create_all(light);
	
	total_light_sources++;
	return light;
}

// get structure from the lens flare entity
LIGHT_SOURCE *get_lens_flare(ENTITY *ent)
{
	if(!ent)
	{
		error("ERROR! Can't get lens flare! Entity doesn't exist!");
		return NULL;
	}
	
	LIGHT_SOURCE *light = ent->OBJ_STRUCT;
	return light;
}

// remove lens flare structure from the given entity
var remove_lens_flare(ENTITY *ent)
{
	if(!ent)
	{
		error("ERROR! Can't remove lens flare! Entity doesn't exist!");
		return NULL;
	}
	
	LIGHT_SOURCE *light = get_lens_flare(ent);
	light->owner_ent = NULL;
	
	ptr_remove(light->aura_ent);
	light->aura_ent = NULL;
	
	lens_flare_remove_all(light);
	
	sys_free(light);
	light = NULL;
	ent->OBJ_STRUCT = NULL;
}
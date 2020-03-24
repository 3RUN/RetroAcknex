#ifndef _LENS_FLARE_H_
	#define _LENS_FLARE_H_
	
	#define LENS_TRACE_FLAGS (IGNORE_PASSABLE + IGNORE_PASSENTS + IGNORE_MAPS + IGNORE_SPRITES + IGNORE_CONTENT)

	#define LIGHT_AURA_ALPHA 15
	#define LIGHT_AURA_SIZE_X 0.05
	#define LIGHT_AURA_SIZE_Y 0.05

	#define MAX_FLARE_PER_LIGHT 3
	#define MAX_LENS_ALPHA 50
	#define LENS_FADING_SPEED 40
	#define LENS_FLARE_DISTANCE 1024

	var total_light_sources = 0;

	STRING *lens_flare_a_tga = "LF_sunA.tga";
	STRING *lens_flare_b_tga = "LF_sunB.tga";
	STRING *lens_flare_c_tga = "LF_sunC.tga";

	typedef struct LENS_FLARE
	{
		ENTITY *ent;
		
		var pivot_dist;
		var alpha;
		
		VECTOR world_pos;
		VECTOR offset;
	}LENS_FLARE;
	
	typedef struct LIGHT_SOURCE
	{
		ENTITY *owner_ent;
		ENTITY *aura_ent;
		
		VECTOR offset;
		VECTOR world_pos;
		VECTOR screen_pos;
		
		var on_screen;
		var is_visible;
		var alpha;
		var faded;
		
		LENS_FLARE flare[MAX_FLARE_PER_LIGHT];
	}LIGHT_SOURCE;
	
	// register new lens flare for the given entity with the given offset
	LIGHT_SOURCE *lens_flare_register(ENTITY *ent, VECTOR *offset_);

	// get structure from the lens flare entity
	LIGHT_SOURCE *get_lens_flare(ENTITY *ent);

	// remove lens flare structure from the given entity
	var remove_lens_flare(ENTITY *ent);
	
	#include "sun_lens_flare.h"
	#include "lens_flare_logic.h"
	#include "lens_flare.c"
#endif
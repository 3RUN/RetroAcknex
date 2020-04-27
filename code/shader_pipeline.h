#ifndef _SHADER_PIPELINE_H_
	#define _SHADER_PIPELINE_H_
	
	// nice trick made by txesmi, without this you
	// won't be able (f.e.) to have proper ambient on terrains
	#define CUSTOM_AMBIENT
	
	// add NES color palette postprocessing shader !
	#define NES_COLOR_PALETTE

	#define MIRRORS_MAX 9 // only 9 mirror floor per level, if you want more, increase it here
	#define MIRRORS_RESOLUTION 512 // reflection resolution, increase for better quality
	#define MIRRORS_FLAGS (NOCULL | PORTALCLIP | NOFLAG1)

	#define SKYCUBE_TGA "skycube+6.tga"

	ENTITY *skycube_ent = NULL;

	BMAP *mtl_envmap_tga = "skycube+6.tga";
	BMAP *mtl_chrome_tga = "chrome.tga";
	BMAP *render_target_tga;

	VIEW *pp_view = { layer = -1; flags = PROCESS_TARGET; }

	float mat_scales[16] = { 16, 16, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

	var level_ambient = 0.75; // smaller value = brighter level ambient
	var vertex_snapping = 160; // good range is from 50-200 (smaller value = less snapping)
	var cutoff_distance = 256; // distance for polygon cutoff (in quants)
	var light_brightness = 1; // dynamic light brightness factor ! (smaller value = weaker lights)
	var sky_fogginess = 0.9; // fog factor for sky (smaller value - less fog)
	var decal_shadow_darkness = 65; // decal shadow transparency (default 50)
	var total_mirrors = 0; // total amount of mirrors created per level

	typedef struct MIRROR
	{
		ENTITY *mirror_ent;
		BMAP *mirror_target_bmp;
		VIEW *view_mirror;
		var is_visible;
	} MIRROR;

	MIRROR mirror[MIRRORS_MAX];
	
	MATERIAL *mtl_world_ =
	{
		effect = "mtl_world_.fx";
		flags = PASS_SOLID | AUTORELOAD;
	}

	MATERIAL *mtl_solid_ =
	{
		effect = "mtl_solid_.fx";
		flags = PASS_SOLID | AUTORELOAD;
	}
	
	MATERIAL *mtl_solid_zfunc =
	{
		effect = "mtl_solid_zfunc.fx";
		flags = PASS_SOLID | AUTORELOAD;
	}

	MATERIAL *mtl_terrain_ =
	{
		effect = "mtl_terrain_.fx";
		flags = PASS_SOLID | AUTORELOAD;
	}

	MATERIAL *mtl_translucent_ =
	{
		effect = "mtl_translucent_.fx";
		flags = TRANSLUCENT | AUTORELOAD;
	}

	MATERIAL *mtl_anim_sprite_ =
	{
		effect = "mtl_anim_sprite_.fx";
		flags = TRANSLUCENT | AUTORELOAD;
	}

	MATERIAL *mtl_water_ =
	{
		effect = "mtl_water_.fx";
		flags = TRANSLUCENT | AUTORELOAD;
	}

	MATERIAL *mtl_lens_flare_ =
	{
		effect = "mtl_lens_flare_.fx";
		flags = TRANSLUCENT | AUTORELOAD;
	}
	
	MATERIAL *mtl_pp_screen_ = 
	{
		effect = "mtl_pp_screen_.fx";
		flags = AUTORELOAD;
	}

	void mtl_envmap_view();

	void mtl_envmap_init();

	MATERIAL *mtl_environment_ =
	{
		effect = "mtl_environment_.fx";
		event = mtl_envmap_init;
		flags = PASS_SOLID | AUTORELOAD | ENABLE_VIEW;
	}

	MATERIAL *mtl_chrome_ = 
	{
		effect = "mtl_chrome_.fx";
		skin1 = mtl_chrome_tga;
		flags = PASS_SOLID | AUTORELOAD;
		
		specular_blue = 128;
		specular_green = 128;
		specular_red = 128;
		
		power = 25; // play with 25 (default was 50)
	}
	
	// get ambient from the terrain
	void get_ambient_c_trace(ENTITY *ent);
	
	void shader_pipeline_create_sky();

	void shader_pipeline_remove_sky();

	void shader_pipeline_update_sky();

	void shader_pipeline_create_mirror(ENTITY *ent, var reflection_power);
	
	void shader_pipeline_remove_mirrors();
	
	void shader_pipeline_update_mirrors();

	void shader_pipeline_init();
	
	void shader_pipeline_remove();

	void shader_pipeline_update();

	#include "shader_pipeline.c"
#endif
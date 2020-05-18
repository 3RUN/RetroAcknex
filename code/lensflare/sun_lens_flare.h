#ifndef _SUN_LENS_FLARE_H_
	#define _SUN_LENS_FLARE_H_

	#define MAX_FLARE_PER_SUN 3
	#define MAX_SUN_ALPHA 90
	#define SUN_ALPHA_FACTOR 1

	STRING *sun_lens_a_tga = "LF_sunA.tga";
	STRING *sun_lens_b_tga = "LF_sunB.tga";
	STRING *sun_lens_c_tga = "LF_sunC.tga";

	VECTOR sun_pos3d;

	var sun_lens_flare_active = true; // used to toggle sun lens flare ON/OFF
	var total_sun_lens_flare = 0; // total amount of lens flare created for sun

	typedef struct SUN
	{
		ENTITY *sun_ent;
		ENTITY *aura_ent;
		
		var temp_alpha[3];
		var center_alpha[3];
		
		LENS_FLARE flare[MAX_FLARE_PER_SUN];
		
		var temp_var[0];
		var temp_var2[0];
		var scale;
		var diff;
	}SUN;

	SUN sun_lens;
	
	// create lens flare for the sun with the given parameters
	void sun_lens_flare_init(STRING *file_, var layer_, var size_x_, var size_y_, var size_z_, var dist_, var alpha_, VECTOR *offset_);

	// create all sun lens flare
	void sun_lens_flare_create_all();
	
	// custom sun position
	// faced some troubles with the native sun_pos (clipping, fog etc)
	void sun_lens_flare_pos_update();
	
	// check if sun is visible from the camera position
	// true - if visible, otherwise - false
	var is_sun_visible();
	
	// place given lens flare with offset at the given position
	void sun_lens_flare_place(LENS_FLARE *flare, VECTOR *pos_);
	
	// update all sun lens flare
	void lens_flare_sun_update();

	#include "sun_lens_flare.c"
#endif
#ifndef _LENS_FLARE_LOGIC_H_
	#define _LENS_FLARE_LOGIC_H_
	
	// create lens flare with the given parameters
	void lens_flare_init(LENS_FLARE *flare, STRING *file_, var layer_, var size_x_, var size_y_, var size_z_, var dist_, var alpha_, VECTOR *offset_);
	
	// create all lens flare
	void lens_flare_create_all(LIGHT_SOURCE *light);

	// remove all lens flare
	void lens_flare_remove_all(LIGHT_SOURCE *light);
	
	// perform trace to check if light source is visible or not
	var lens_flare_trace(VECTOR *pos);
	
	// check if light source is visible or not
	void is_lens_flare_visible(LIGHT_SOURCE *light);
	
	// place given lens flare with offset at the given position
	void lens_flare_place(LENS_FLARE *flare, VECTOR *pos_);
	
	// update given lens flare
	void update_lens_flare(ENTITY *ent);
	
	#include "lens_flare_logic.c"
#endif
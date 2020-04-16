
#include <acknex.h>
#include <default.c>
#include <windows.h>

#define PRAGMA_POINTER

#define PRAGMA_PATH "code"
#define PRAGMA_PATH "code\\fx"
#define PRAGMA_PATH "code\\lensflare"
#define PRAGMA_PATH "resources"
#define PRAGMA_PATH "resources\\lensflare"

#define TRACE_FLAGS (IGNORE_ME + IGNORE_PASSABLE + IGNORE_PASSENTS + IGNORE_MAPS + IGNORE_SPRITES + IGNORE_CONTENT)
#define MOVE_FLAGS (IGNORE_ME + IGNORE_PASSABLE + IGNORE_PASSENTS + IGNORE_MAPS + IGNORE_SPRITES + IGNORE_CONTENT)

#define OBJ_ID skill50
#define OBJ_STRUCT skill51
#define OBJ_TYPE skill52
#define TYPE_NONE 0
#define TYPE_LIGHT 1

#include "shader_pipeline.h"
#include "lens_flare.h"

action light_point()
{
	set(my, UNLIT | BRIGHT | PASSABLE);
	my->ambient = 100;
	my->lightrange = 512;
	vec_set(&my->blue, COLOR_WHITE);
	vec_set(&my->skill1, &my->x);

	lens_flare_register(my, vector(0, 0, 0));
	
	while(my)
	{
		my->x = my->skill1 + fsin(total_ticks * 8, 64);
		my->y = my->skill2 + fcos(total_ticks * 8, 64);
		update_lens_flare(my);
		wait(1);
	}
}

action light_spot()
{
	set(my, POLYGON | UNLIT | BRIGHT | SPOTLIGHT);
	my->ambient = 100;
	my->lightrange = 512;
	vec_set(&my->blue, COLOR_WHITE);
	
	LIGHT_SOURCE *lens = lens_flare_register(my, vector(9, 0, 0));
	
	while(my)
	{
		my->pan += 5 * time_step;
		update_lens_flare(my);
		wait(1);
	}
}

action terrain()
{
	my->material = mtl_terrain_;
}

action translucent()
{
	set(my, POLYGON | TRANSLUCENT);
	my->alpha = 50;
	my->material = mtl_translucent_;
	
	#ifdef CUSTOM_AMBIENT
		get_ambient_c_trace(my);
	#endif
	
	while(my)
	{
		while(my->alpha > 0)
		{
			my->alpha -= 5 * time_step;
			wait(1);
		}
		my->alpha = 0;
		
		while(my->alpha < 100)
		{
			my->alpha += 5 * time_step;
			wait(1);
		}
		my->alpha = 100;
	}
}

action object()
{
	set(my, POLYGON | SHADOW | CAST);
	my->material = mtl_solid_;
	
	#ifdef CUSTOM_AMBIENT
		get_ambient_c_trace(my);
	#endif
	
	while(my)
	{
		my->pan += 5 * time_step;
		wait(1);
	}
}

action npc()
{
	c_setminmax(my);
	vec_set(&my->min_x, vector(-16, -16, -32));
	vec_set(&my->max_x, vector(16, 16, 32));
	set(my, POLYGON | SHADOW | CAST);
	my->eflags |= ANIMATE;
	my->material = mtl_solid_;
	
	#ifdef CUSTOM_AMBIENT
		get_ambient_c_trace(my);
	#endif
	
	while(my)
	{
		my->pan += 10 * time_step;
		wait(1);
	}
}

action npc_through_walls()
{
	my->material = mtl_solid_zfunc;
	my->skill41 = floatv(0.5); // red (0...1)
	my->skill42 = floatv(0.5); // green (0...1)
	my->skill43 = floatv(1); // blue (0...1)
	set(my, POLYGON | SHADOW | CAST);
	my->eflags |= ANIMATE;
}

action npc_on_path()
{
	vec_fill(&my->scale_x, 5);
	wait(1);
	c_setminmax(my);
	vec_set(&my->min_x, vector(-64, -64, -128));
	vec_set(&my->max_x, vector(64, 64, 128));
	set(my, POLYGON | SHADOW | CAST);
	my->material = mtl_solid_;
	
	var node_id = 0, node_dir = 1;
	var movement_speed = 15, rotation_factor = 0.5;
	
	VECTOR dir_vec;
	VECTOR target_node;
	
	vec_fill(&dir_vec, 0);
	vec_fill(&target_node, 0);
	
	ANGLE target_angle;
	vec_fill(&target_angle, 0);
	
	node_id = path_scan(my, &my->x, &my->pan, vector(360, 0, 128));
	path_getnode(my, node_id, &my->x, NULL);
	
	while(my)
	{
		if(path_getnode(my, node_id, &target_node, NULL) == 0)
		{
			node_dir *= -1;
			node_id += node_dir;
			path_getnode(my, node_id, &target_node, NULL);
		}
		
		if(vec_dist(&my->x, &target_node) < 25)
		{
			node_id += node_dir;
		}
		
		vec_diff(&dir_vec, &target_node, &my->x);
		
		vec_normalize(&dir_vec, 1);
		vec_scale(&dir_vec, movement_speed * time_step);
		vec_add(&my->x, &dir_vec);
		
		vec_to_angle(&target_angle, &dir_vec);
		my->pan += ang(target_angle.pan - my->pan) * rotation_factor * time_step;
		my->tilt += ang(target_angle.tilt - my->tilt) * rotation_factor * time_step;
		
		my->skill1 += 6 * time_step;
		ent_animate(my, "run", my->skill1, ANM_CYCLE);
		
		// in this case, to get proper ambient on terrains
		// we need to call this function each frame
		// this needed for all moving objects, to get
		// proper ambient on terrains
		#ifdef CUSTOM_AMBIENT
			get_ambient_c_trace(my);
		#endif
		wait(1);
	}
}

#define frame_count_x skill1
#define frame_count_y skill2
// uses: frame_count_x, frame_count_y
action animated_sprite()
{
	set(my, PASSABLE);
	
	my->material = mtl_anim_sprite_;
	my->skill41 = floatv(my->frame_count_x);
	my->skill42 = floatv(my->frame_count_y);
	
	#ifdef CUSTOM_AMBIENT
		get_ambient_c_trace(my);
	#endif
	
	while(my)
	{
		my->frame = cycle(my->frame + time_step, 0, my->frame_count_x);
		my->skill43 = floatd(floor(my->frame), my->frame_count_x);
		my->skill44 = floatd(0, my->frame_count_y);
		wait(1);
	}
}

#define amplitude skill1
#define speed skill2
#define modulation skill3
// uses: amplitude, speed, modulation
action water()
{
	set(my, PASSABLE | TRANSLUCENT);
	my->alpha = 75;
	my->material = mtl_water_;
	
	if(my->amplitude <= 0){ my->amplitude = 5; }
	if(my->speed <= 0){ my->speed = 1; }
	if(my->modulation <= 0){ my->modulation = 25; }
	
	my->skill41 = floatv(my->amplitude);
	my->skill42 = floatv(my->speed);
	my->skill43 = floatv(my->modulation);
}

// mirror floor
action mirror_floor()
{
	set(my, PASSABLE);
	
	// make sure to place mirror floor about 2-4 quants (depends on psx_vertex_snapping)
	// above the ground, if you don't you'll end up seeing surface below the mirror floor model
	// on distance.. this is because of vertex snapping becoming more noticeable on distance
	shader_pipeline_create_mirror(my, 100); // 100 - reflection power (less number - less reflection)
}

action environment()
{
	set(my, POLYGON | SHADOW | CAST);
	my->material = mtl_environment_;
	my->skill41 = floatv(25); // texture power (less number - more reflection)
	
	#ifdef CUSTOM_AMBIENT
		get_ambient_c_trace(my);
	#endif
	
	while(my)
	{
		var env_power = 50 + fsin(total_ticks * 8, 50);
		my->skill41 = floatv(env_power);
		my->pan += 5 * time_step;
		wait(1);
	}
}

action chrome()
{
	set(my, POLYGON | SHADOW | CAST);
	my->material = mtl_chrome_;
	my->skill41 = floatv(100); // chrome texture scale (noticeable from 15 to 500)
	my->skill42 = floatv(20); // reflection power (noticeable from 15 to 50)
	
	// we change color of reflection map
	// skill45 - red (range 0 ... 1)
	// skill46 - green (range 0 ... 1)
	// skill47 - blue (range 0 ... 1)
	vec_set(&my->skill45, vector(floatv(1), floatv(0.75), floatv(0.5)));
	
	#ifdef CUSTOM_AMBIENT
		get_ambient_c_trace(my);
	#endif
	
	while(my)
	{
		var ref_power = 25 + fsin(total_ticks * 8, 25);
		my->skill42 = floatv(ref_power);
		my->pan += 5 * time_step;
		wait(1);
	}
}

void mouse_lock()
{
	static var autolock_mouse_locked = false;
	if(!autolock_mouse_locked && window_focus)
	{
		autolock_mouse_locked = true;
		RECT rect;
		GetClientRect(hWnd, &rect);
		ClientToScreen(hWnd, &rect);
		ClientToScreen(hWnd, &rect.right);
		SetCursorPos(rect.left, rect.top);
		ClipCursor(&rect);
	}
	
	if(autolock_mouse_locked && !window_focus)
	{
		autolock_mouse_locked = false;
		ClipCursor(NULL);
	}
}

void on_exit_event()
{
}

void on_ent_remove_event(ENTITY *ent)
{
	if(ent->OBJ_TYPE == TYPE_LIGHT && ent->OBJ_STRUCT != 0)
	{
		remove_lens_flare(ent);
	}
}

void on_frame_event()
{
	shader_pipeline_update();
	lens_flare_sun_update();
	mouse_lock();
}

void main()
{
	on_exit = on_exit_event;
	on_ent_remove = on_ent_remove_event;
	on_frame = on_frame_event;
	
	tex_cycles = 5;
	warn_level = 6;
	fps_max = 60;
	
	mouse_pointer = 0;
	
	video_mode = 10;
	
	level_load("scene.wmb");
	wait(3);
	
	camera->arc = 90;
	camera->clip_far = 2048;
	camera->clip_near = 0.1;
	
	camera->fog_start = 128;
	camera->fog_end = camera->clip_far * 0.9;
	sun_angle.roll = camera->clip_far * 0.8; // sun distance
	
	fog_color = 4;
	vec_set(&d3d_fogcolor4.blue, vector(128, 128, 128));
	vec_set(&sky_color.blue, &d3d_fogcolor4.blue);
	
	vec_set(&camera->x, vector(-23, 437, 330));
	vec_set(&camera->pan, vector(308, -33, 0));
	
	sun_light = 50;
	vec_set(&sun_color, vector(128, 128, 128));
	vec_set(&ambient_color, vector(0, 0, 0));
	
	// should be called after level was loaded
	// because it's gonna create a sky
	shader_pipeline_init();
	
	// create sun lens flare
	sun_lens_flare_create_all();
	
	VECTOR force, speed, dist;
	ANGLE aforce, aspeed;
	
	vec_fill(&force, 0);
	vec_fill(&speed, 0);
	vec_fill(&dist, 0);
	
	vec_fill(&aforce, 0);
	vec_fill(&aspeed, 0);
	
	while(!key_esc)
	{
		sun_angle.pan += 5 * (key_cul - key_cur) * time_step; 
		sun_angle.tilt += 5 * (key_cuu - key_cud) * time_step; 
		
		aforce.pan = -5 * (mouse_right * mouse_force.x);
		aforce.tilt = 5 * (mouse_right * mouse_force.y);
		aforce.roll = 0;
		vec_add(&camera->pan, vec_accelerate(&dist, &aspeed, &aforce, 0.8));
		
		force.x = 7 * (key_w - key_s);
		force.y = 3 * (key_a - key_d);
		force.z = 3 * (key_q - key_e);
		vec_accelerate(&dist, &speed, &force, 0.5);
		vec_add(&camera->x, vec_rotate(&dist, &camera->pan));
		wait(1);
	}
}
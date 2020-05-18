
// create lens flare with the given parameters
void lens_flare_init(LENS_FLARE *flare, STRING *file_, var layer_, var size_x_, var size_y_, var size_z_, var dist_, var alpha_, VECTOR *offset_)
{
	flare->ent = ent_createlayer(file_, 0, layer_);
	set(flare->ent, TRANSLUCENT | BRIGHT | LIGHT | UNLIT | NOFOG);
	
	flare->ent->material = mtl_lens_flare_;
	flare->ent->ambient = 100;
	flare->ent->albedo = -100;
	flare->ent->alpha = 0;
	
	flare->ent->scale_x = size_x_;
	flare->ent->scale_y = size_y_;
	flare->ent->scale_z = size_z_;
	
	flare->pivot_dist = dist_;
	flare->alpha = alpha_;
	vec_set(&flare->offset, offset_);
}

// create all lens flare
void lens_flare_create_all(LIGHT_SOURCE *light)
{
	lens_flare_init(&light->flare[0], lens_flare_a_tga, 1, 0.5, 0.5, 0.5, 1, 15.0, nullvector);
	lens_flare_init(&light->flare[1], lens_flare_b_tga, 2, 1, 1, 1, 1, 10.0, nullvector);
	lens_flare_init(&light->flare[2], lens_flare_c_tga, 3, 0.5, 0.5, 0.5, 1, 15.0, nullvector);
}

// remove all lens flare
void lens_flare_remove_all(LIGHT_SOURCE *light)
{
	int i = 0;
	for(i = 0; i < MAX_FLARE_PER_LIGHT; i++)
	{
		ptr_remove(light->flare[i].ent);
		light->flare[i].ent = NULL;
	}
}

// perform trace to check if light source is visible or not
var lens_flare_trace(VECTOR *pos)
{
	var is_visible = true;
	if(c_trace(&camera->x, pos, LENS_TRACE_FLAGS | IGNORE_YOU) > 0)
	{
		is_visible = false;
	}
	return is_visible;
}

// check if light source is visible or not
void is_lens_flare_visible(LIGHT_SOURCE *light)
{
	VECTOR temp_pos;
	vec_set(&temp_pos, &light->world_pos);
	light->on_screen = false;
	if(vec_to_screen(&temp_pos, camera) != NULL)
	{
		light->on_screen = true;
	}
	
	// on screen ?
	if(light->on_screen == true)
	{
		// check if light source is visible from time to time
		if((total_frames % 20) == true)
		{
			if(lens_flare_trace(&light->world_pos) == true)
			{
				light->is_visible = true;
			}
			else
			{
				light->is_visible = false;
			}
		}
	}
	else
	{
		light->is_visible = false;
	}
}

// place given lens flare with offset at the given position
void lens_flare_place(LENS_FLARE *flare, VECTOR *pos_)
{
	vec_set(&flare->world_pos, pos_);
	set(flare->ent, TRANSLUCENT | BRIGHT);
	flare->ent->flags2 |= SHOW;
	
	flare->ent->ambient = 100;
	flare->ent->albedo = -100;
	
	flare->ent->x = flare->world_pos.x * flare->pivot_dist + 0.5 * screen_size.x + (flare->world_pos.x * flare->offset.x);
	flare->ent->y = flare->world_pos.y * flare->pivot_dist + 0.5 * screen_size.y + (flare->world_pos.y * flare->offset.y);
	flare->ent->z = 750;
	rel_for_screen(&flare->ent->x, camera);
}

// update given lens flare
void update_lens_flare(ENTITY *ent)
{
	if(!ent)
	{
		error("ERROR! Can't update lens flare! Entity doesn't exist!");
		return;
	}
	
	LIGHT_SOURCE *light = get_lens_flare(ent);
	
	if(!light)
	{
		error("ERROR! Can't update lens flare! Structure doesn't exist!");
		return;
	}
	
	// update lens flare position
	vec_set(&light->world_pos, &light->offset);
	vec_rotate(&light->world_pos, &ent->pan);
	vec_add(&light->world_pos, &ent->x);
	
	// convert world position to screen position
	vec_set(&light->screen_pos, &light->world_pos);
	vec_to_screen(&light->screen_pos, camera);
	
	// move + rotate aura to camera
	vec_set(&light->aura_ent->x, &light->world_pos);
	vec_to_angle(&light->aura_ent->pan, vec_diff(NULL, &light->aura_ent->x, &camera->x));
	
	// close enough ?
	if(vec_dist(&light->world_pos, &camera->x) < LENS_FLARE_DISTANCE)
	{
		// check if this light source is visible
		is_lens_flare_visible(light);
		
		// show lens ?
		if(light->is_visible == true)
		{
			// fade in
			if(light->alpha < MAX_LENS_ALPHA && light->faded == true)
			{
				light->alpha += LENS_FADING_SPEED * time_step;
				if(light->alpha > (MAX_LENS_ALPHA - 1))
				{
					light->alpha = MAX_LENS_ALPHA;
					light->faded = false;
				}
			}
			
			light->screen_pos.x -= 0.5 * screen_size.x;
			light->screen_pos.y -= 0.5 * screen_size.y;
			
			int i = 0;
			for(i = 0; i < MAX_FLARE_PER_LIGHT; i++)
			{
				// fade lens flare
				var alpha_ = light->flare[i].alpha;
				light->flare[i].ent->alpha = (alpha_ / 100.0 * light->alpha) + random((0.02) * light->alpha);
				
				// change their color with the light source
				vec_set(&light->flare[i].ent->blue, &ent->blue);
				
				// place them correctly on the screen
				lens_flare_place(&light->flare[i], &light->screen_pos);
			}
			
			// rotata very first lens
			light->flare[0].ent->roll += 0.03 * time_step;
		}
		else // hide 
		{
			if(light->alpha > 0)
			{
				light->alpha -= LENS_FADING_SPEED * time_step;
				light->faded = true;
			}
			
			int i = 0;
			for(i = 0; i < MAX_FLARE_PER_LIGHT; i++)
			{
				// fade lens flare
				var alpha_ = light->flare[i].alpha;
				light->flare[i].ent->alpha = (alpha_ / 100.0 * light->alpha) + random((0.02) * light->alpha);
			}
		}
		
		// handle aura fading and color
		light->aura_ent->alpha -= (light->aura_ent->alpha - LIGHT_AURA_ALPHA) * time_step;
		vec_set(&light->aura_ent->blue, &ent->blue);
	}
	else
	{
		// hide all lens flare
		int i = 0;
		for(i = 0; i < MAX_FLARE_PER_LIGHT; i++)
		{
			light->flare[i].ent->alpha = 0;
		}
		
		// fade away aura
		light->aura_ent->alpha -= (light->aura_ent->alpha - 0) * 0.25 * time_step;
		vec_set(&light->aura_ent->blue, &ent->blue);
		
		// reset all alpha stuff
		light->faded = true;
		light->alpha = 0;
	}
}
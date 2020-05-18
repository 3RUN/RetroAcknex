
// get ambient from the terrain
void get_ambient_c_trace(ENTITY *ent)
{
	if(!ent){ return; }
	
	vec_set(&ent->blue, COLOR_WHITE);
	
	VECTOR temp_pos;
	vec_set(&temp_pos, &ent->x);
	temp_pos.z -= 300 - ent->min_z;
	
	you = NULL;
	
	var mode = IGNORE_ME + IGNORE_PASSABLE + IGNORE_PASSENTS + IGNORE_MAPS + IGNORE_MODELS + IGNORE_SPRITES + IGNORE_CONTENT;
	c_trace(&ent->x, &temp_pos, mode + SCAN_TEXTURE);
	
	if(HIT_TARGET)
	{
		if(you) // if you exits, is a terrain for sure
		{
			if(you->lightmap)
			{
				BMAP *bmp_skin1 = ent_getskin(you, 1);
				var scale = bmap_width(you->lightmap) / bmap_width(bmp_skin1);
				var format = bmap_lock(you->lightmap, 0);
				var c_x = hit->u1 * scale;
				var c_y = hit->v1 * scale;
				var pixel = pixel_for_bmap(you->lightmap, c_x, c_y);
				
				COLOR col;
				pixel_to_vec(&col, NULL, format, pixel);
				vec_set(&ent->blue, &col);
				bmap_unlock(you->lightmap);
				set(ent, LIGHT | UNLIT);
			}
		}
		else // hit a block
		{
			reset(ent, LIGHT | UNLIT);
		}
	}
}

void mtl_envmap_view()
{
	mat_set(mtl->matrix, matViewInv);
	
	mtl->matrix41 = 0;
	mtl->matrix42 = 0;
	mtl->matrix43 = 0;
}

void mtl_envmap_init()
{
	bmap_to_cubemap(mtl_envmap_tga);
	mtl->skin1 = mtl_envmap_tga;
	mtl->event = mtl_envmap_view;
}

void shader_pipeline_create_sky()
{
	if(skycube_ent){ return; }
	
	skycube_ent = ent_createlayer(SPHERE_MDL, SKY | SCENE | SHOW, 1);
	vec_set(&skycube_ent->x, &camera->x);
	
	vec_fill(&skycube_ent->scale_x, (camera->clip_far * 0.99) / 8);
	skycube_ent->material = mtl_create();
	skycube_ent->material->skin1 = bmap_create(SKYCUBE_TGA);
	bmap_to_cubemap(skycube_ent->material->skin1);
	effect_load(skycube_ent->material, "mtl_sky_.fx");
	skycube_ent->material->skill1 = floatv(sky_fogginess);
	skycube_ent->material->skill2 = floatv(vertex_snapping);
}

void shader_pipeline_remove_sky()
{
	if(!skycube_ent){ return; }
	
	if(skycube_ent->material->skin1)
	{
		ptr_remove(skycube_ent->material->skin1);
		skycube_ent->material->skin1 = NULL;
	}
	
	if(skycube_ent->material)
	{
		ptr_remove(skycube_ent->material);
		skycube_ent->material = NULL;
	}
	
	ptr_remove(skycube_ent);
	skycube_ent = NULL;
}

void shader_pipeline_update_sky()
{
	if(!skycube_ent){ return; }
	
	vec_set(&skycube_ent->x, &camera->x);
	
	skycube_ent->material->skill1 = floatv(sky_fogginess);
	skycube_ent->material->skill2 = floatv(vertex_snapping);
}

void shader_pipeline_create_mirror(ENTITY *ent, var reflection_power)
{
	if(!ent)
	{
		error("ERROR! Can't create mirror effect, entity doesn't exist!");
		return;
	}
	
	// not setup yet ?
	if(mirror[total_mirrors].mirror_ent == NULL)
	{
		ent->albedo = reflection_power;
		mirror[total_mirrors].mirror_ent = ent;
		
		mirror[total_mirrors].view_mirror = view_create(-1);
		mirror[total_mirrors].mirror_target_bmp = bmap_createblack(MIRRORS_RESOLUTION, MIRRORS_RESOLUTION, 888);
		
		ent->material = mtl_create();
		ent->material->skin2 = mirror[total_mirrors].mirror_target_bmp;
		ent->material->flags |= PASS_SOLID | AUTORELOAD;
		effect_load(ent->material, "mtl_mirror_floor_.fx");
		
		mirror[total_mirrors].view_mirror->bmap = mirror[total_mirrors].mirror_target_bmp;
		mirror[total_mirrors].view_mirror->size_x = bmap_width(mirror[total_mirrors].view_mirror->bmap);
		mirror[total_mirrors].view_mirror->size_y = bmap_height(mirror[total_mirrors].view_mirror->bmap);
		
		vec_set(&mirror[total_mirrors].view_mirror->pnormal_x, vector(0, 0, 1)); // reflect upwards
		set(mirror[total_mirrors].view_mirror, MIRRORS_FLAGS);
		
		mirror[total_mirrors].is_visible = false;
		total_mirrors++;
	}
}

void shader_pipeline_remove_mirrors()
{
	int i = 0;
	for(i = 0; i < total_mirrors; i++)
	{
		if(mirror[i].mirror_ent)
		{
			if(mirror[i].mirror_ent->material)
			{
				ptr_remove(mirror[i].mirror_ent->material);
				mirror[i].mirror_ent->material = NULL;
			}
			
			mirror[i].mirror_ent = NULL;
		}
		
		if(mirror[i].view_mirror)
		{
			if(mirror[i].view_mirror->bmap)
			{
				mirror[i].view_mirror->bmap = NULL;
			}
			ptr_remove(mirror[i].view_mirror);
			mirror[i].view_mirror = NULL;
		}
		
		if(mirror[i].mirror_target_bmp)
		{
			ptr_remove(mirror[i].mirror_target_bmp);
			mirror[i].mirror_target_bmp = NULL;
		}
	}
	
	total_mirrors = 0;
}

void shader_pipeline_update_mirrors()
{
	int i = 0;
	for(i = 0; i < total_mirrors; i++)
	{
		if(mirror[i].mirror_ent)
		{
			if(mirror[i].is_visible) 
			{ 
				set(mirror[i].view_mirror, SHOW);
				mirror[i].view_mirror->genius = camera->genius;
				mirror[i].view_mirror->aspect = (screen_size.x / screen_size.y) * camera->aspect;
				mirror[i].view_mirror->arc = camera->arc;
				mirror[i].view_mirror->fog_start = camera->fog_start;
				mirror[i].view_mirror->fog_end = camera->fog_end;
				mirror[i].view_mirror->clip_far = camera->clip_far * 1.5; // make it bigger so we can see sky
				mirror[i].view_mirror->clip_near = camera->clip_near * 2;
				mirror[i].view_mirror->x = camera->x;
				mirror[i].view_mirror->y = camera->y;
				mirror[i].view_mirror->z = 2 * mirror[i].view_mirror->portal_z - camera->z;
				mirror[i].view_mirror->pan = camera->pan;
				mirror[i].view_mirror->tilt = -camera->tilt;
				mirror[i].view_mirror->roll = -camera->roll;
			}
			else
			{ 
				// switch rendering off when all mirror objects are outside the frustum
				reset(mirror[i].view_mirror, SHOW); 
			}
			
			mirror[i].is_visible = false;
			
			if(mirror[i].mirror_ent)
			{
				if(!is(mirror[i].mirror_ent, CLIPPED)) 
				{
					vec_for_max(&mirror[i].view_mirror->portal_x, mirror[i].mirror_ent);
					vec_add(&mirror[i].view_mirror->portal_x, &mirror[i].mirror_ent->x);
					mirror[i].is_visible = true;
				}
				
				mirror[i].mirror_ent->skill41 = floatv(level_ambient);
				mirror[i].mirror_ent->skill42 = floatv(vertex_snapping);
				mirror[i].mirror_ent->skill43 = floatv(cutoff_distance);
				mirror[i].mirror_ent->skill44 = floatv(light_brightness);
			}	
		}
	}
}

// this function has to be called each time you change resolution!
void shader_pipeline_reset()
{
	// create bmap with the largest resolution we can afford
	if(!render_target_tga)
	{
		render_target_tga = bmap_createblack(2048, 2048, 32);
	}
	
	// generate/adapt render target z buffer
	bmap_zbuffer(render_target_tga);
	
	// reset all view sorting
	int i = 0;
	for(i = 0; i < total_mirrors; i++)
	{
		mirror[i].view_mirror->stage = NULL;
	}
	
	// if you add any pp shaders into the view rendering chain, remove them all here too!
	#ifdef PSX_DITHER
		pp_view->material = NULL;	
		camera->stage = NULL;
	#endif
	
	// now we start linking from the mirrors, due to the acknex BUG !
	// mirrors (and any other square rendertargets) need to be rendered first !
	// if you don't to this, you will get weird render target bug
	for(i = 0; i < total_mirrors; i++)
	{
		if(mirror[i + 1].view_mirror != NULL)
		{
			mirror[i].view_mirror->stage = mirror[i + 1].view_mirror;
		}
		else
		{
			mirror[i].view_mirror->stage = camera;
		}
	}
	
	// if you have any pp shaders, add them into view rendering chain here!
	#ifdef PSX_DITHER
		pp_view->material = mtl_pp_screen_;	
		camera->stage = pp_view;
	#endif
}

void shader_pipeline_init()
{
	mat_effect1 = mat_scales;
	
	set(camera, NOFLAG1); // don't render anything with FLAG1 set
	
	shadow_stencil = 0;
	shadow_mode |= IGNORE_MODELS + IGNORE_SPRITES + IGNORE_PASSENTS + IGNORE_PASSABLE;
	mtl_shadow->alpha = decal_shadow_darkness;
	
	cutoff_distance = camera->fog_end;
	
	shader_pipeline_create_sky();
	shader_pipeline_reset();
}

void shader_pipeline_remove()
{
	if(render_target_tga)
	{
		bmap_remove(render_target_tga);
		render_target_tga = NULL;
	}
	
	shader_pipeline_remove_sky();
	shader_pipeline_remove_mirrors();
	
	#ifdef PSX_DITHER
		pp_view->material = NULL;	
		camera->stage = NULL;
	#endif
}

void shader_pipeline_update()
{
	mtl_world_->skill1 = floatv(level_ambient);
	mtl_world_->skill2 = floatv(vertex_snapping);
	mtl_world_->skill3 = floatv(cutoff_distance);
	mtl_world_->skill4 = floatv(light_brightness);
	
	mtl_solid_->skill1 = floatv(level_ambient);
	mtl_solid_->skill2 = floatv(vertex_snapping);
	mtl_solid_->skill3 = floatv(cutoff_distance);
	mtl_solid_->skill4 = floatv(light_brightness);
	
	mtl_glow_solid_->skill1 = floatv(level_ambient);
	mtl_glow_solid_->skill2 = floatv(vertex_snapping);
	mtl_glow_solid_->skill3 = floatv(cutoff_distance);
	mtl_glow_solid_->skill4 = floatv(light_brightness);
	
	mtl_glow_translucent_->skill1 = floatv(level_ambient);
	mtl_glow_translucent_->skill2 = floatv(vertex_snapping);
	mtl_glow_translucent_->skill3 = floatv(cutoff_distance);
	mtl_glow_translucent_->skill4 = floatv(light_brightness);
	
	mtl_solid_zfunc->skill1 = floatv(level_ambient);
	mtl_solid_zfunc->skill2 = floatv(vertex_snapping);
	mtl_solid_zfunc->skill3 = floatv(cutoff_distance);
	mtl_solid_zfunc->skill4 = floatv(light_brightness);
	
	mtl_terrain_->skill1 = floatv(level_ambient);
	mtl_terrain_->skill2 = floatv(vertex_snapping);
	mtl_terrain_->skill3 = floatv(cutoff_distance);
	mtl_terrain_->skill4 = floatv(light_brightness);
	
	mtl_translucent_->skill1 = floatv(level_ambient);
	mtl_translucent_->skill2 = floatv(vertex_snapping);
	mtl_translucent_->skill3 = floatv(cutoff_distance);
	mtl_translucent_->skill4 = floatv(light_brightness);
	
	mtl_anim_sprite_->skill1 = floatv(level_ambient);
	mtl_anim_sprite_->skill2 = floatv(vertex_snapping);
	mtl_anim_sprite_->skill3 = floatv(cutoff_distance);
	mtl_anim_sprite_->skill4 = floatv(light_brightness);
	
	mtl_water_->skill1 = floatv(level_ambient);
	mtl_water_->skill2 = floatv(vertex_snapping);
	mtl_water_->skill3 = floatv(cutoff_distance);
	mtl_water_->skill4 = floatv(light_brightness);
	
	mtl_environment_->skill1 = floatv(level_ambient);
	mtl_environment_->skill2 = floatv(vertex_snapping);
	mtl_environment_->skill3 = floatv(cutoff_distance);
	mtl_environment_->skill4 = floatv(light_brightness);
	
	mtl_chrome_->skill1 = floatv(level_ambient);
	mtl_chrome_->skill2 = floatv(vertex_snapping);
	mtl_chrome_->skill3 = floatv(cutoff_distance);
	mtl_chrome_->skill4 = floatv(light_brightness);
	
	mtl_lens_flare_->skill1 = floatv(vertex_snapping / 2);
	
	shader_pipeline_update_sky();
	shader_pipeline_update_mirrors();
}
// This file was automatically generated by `ncgen.rb' from
// `pism_state.cdl'.  If you edit it, your changes will be overwritten
// on the next invocation of `ncgen.rb'.

   int  stat;			/* return status */
   int  ncid;			/* netCDF id */

   /* dimension ids */
   int x_dim;
   int y_dim;
   int z_dim;
   int zb_dim;
   int t_dim;

   /* dimension lengths */
   size_t x_len = grid.p->Mx;
   size_t y_len = grid.p->My;
   size_t z_len = grid.p->Mz;
   size_t zb_len = grid.p->Mbz;
   size_t t_len = NC_UNLIMITED;

   /* variable ids */
   int polar_stereographic_id;
   int x_id;
   int y_id;
   int z_id;
   int zb_id;
   int t_id;
   int lon_id;
   int lat_id;
   int mask_id;
   int h_id;
   int H_id;
   int Hmelt_id;
   int b_id;
   int dbdt_id;
   int T_id;
   int Tb_id;
   int age_id;
   int Ts_id;
   int ghf_id;
   int accum_id;

   /* rank (number of dimensions) for each variable */
#  define RANK_polar_stereographic 0
#  define RANK_x 1
#  define RANK_y 1
#  define RANK_z 1
#  define RANK_zb 1
#  define RANK_t 1
#  define RANK_lon 2
#  define RANK_lat 2
#  define RANK_mask 3
#  define RANK_h 3
#  define RANK_H 3
#  define RANK_Hmelt 3
#  define RANK_b 3
#  define RANK_dbdt 3
#  define RANK_T 4
#  define RANK_Tb 4
#  define RANK_age 4
#  define RANK_Ts 3
#  define RANK_ghf 3
#  define RANK_accum 3

   /* variable shapes */
   int x_dims[RANK_x];
   int y_dims[RANK_y];
   int z_dims[RANK_z];
   int zb_dims[RANK_zb];
   int t_dims[RANK_t];
   int lon_dims[RANK_lon];
   int lat_dims[RANK_lat];
   int mask_dims[RANK_mask];
   int h_dims[RANK_h];
   int H_dims[RANK_H];
   int Hmelt_dims[RANK_Hmelt];
   int b_dims[RANK_b];
   int dbdt_dims[RANK_dbdt];
   int T_dims[RANK_T];
   int Tb_dims[RANK_Tb];
   int age_dims[RANK_age];
   int Ts_dims[RANK_Ts];
   int ghf_dims[RANK_ghf];
   int accum_dims[RANK_accum];

   int polar_stereographic_straight_vertical_longitude_from_pole[1];
   int polar_stereographic_latitude_of_projection_origin[1];
   int polar_stereographic_standard_parallel[1];

   /* enter define mode */
if (grid.rank == 0) {
   stat = nc_create(fname, NC_CLOBBER|NC_64BIT_OFFSET, &ncid);
   check_err(stat,__LINE__,__FILE__);

   /* define dimensions */
   stat = nc_def_dim(ncid, "x", x_len, &x_dim);
   check_err(stat,__LINE__,__FILE__);
   stat = nc_def_dim(ncid, "y", y_len, &y_dim);
   check_err(stat,__LINE__,__FILE__);
   stat = nc_def_dim(ncid, "z", z_len, &z_dim);
   check_err(stat,__LINE__,__FILE__);
   stat = nc_def_dim(ncid, "zb", zb_len, &zb_dim);
   check_err(stat,__LINE__,__FILE__);
   stat = nc_def_dim(ncid, "t", t_len, &t_dim);
   check_err(stat,__LINE__,__FILE__);

   /* define variables */

   stat = nc_def_var(ncid, "polar_stereographic", NC_INT, RANK_polar_stereographic, 0, &polar_stereographic_id);
   check_err(stat,__LINE__,__FILE__);

   x_dims[0] = x_dim;
   stat = nc_def_var(ncid, "x", NC_FLOAT, RANK_x, x_dims, &x_id);
   check_err(stat,__LINE__,__FILE__);

   y_dims[0] = y_dim;
   stat = nc_def_var(ncid, "y", NC_FLOAT, RANK_y, y_dims, &y_id);
   check_err(stat,__LINE__,__FILE__);

   z_dims[0] = z_dim;
   stat = nc_def_var(ncid, "z", NC_FLOAT, RANK_z, z_dims, &z_id);
   check_err(stat,__LINE__,__FILE__);

   zb_dims[0] = zb_dim;
   stat = nc_def_var(ncid, "zb", NC_FLOAT, RANK_zb, zb_dims, &zb_id);
   check_err(stat,__LINE__,__FILE__);

   t_dims[0] = t_dim;
   stat = nc_def_var(ncid, "t", NC_DOUBLE, RANK_t, t_dims, &t_id);
   check_err(stat,__LINE__,__FILE__);

   lon_dims[0] = x_dim;
   lon_dims[1] = y_dim;
   stat = nc_def_var(ncid, "lon", NC_FLOAT, RANK_lon, lon_dims, &lon_id);
   check_err(stat,__LINE__,__FILE__);

   lat_dims[0] = x_dim;
   lat_dims[1] = y_dim;
   stat = nc_def_var(ncid, "lat", NC_FLOAT, RANK_lat, lat_dims, &lat_id);
   check_err(stat,__LINE__,__FILE__);

   mask_dims[0] = t_dim;
   mask_dims[1] = x_dim;
   mask_dims[2] = y_dim;
   stat = nc_def_var(ncid, "mask", NC_BYTE, RANK_mask, mask_dims, &mask_id);
   check_err(stat,__LINE__,__FILE__);

   h_dims[0] = t_dim;
   h_dims[1] = x_dim;
   h_dims[2] = y_dim;
   stat = nc_def_var(ncid, "h", NC_FLOAT, RANK_h, h_dims, &h_id);
   check_err(stat,__LINE__,__FILE__);

   H_dims[0] = t_dim;
   H_dims[1] = x_dim;
   H_dims[2] = y_dim;
   stat = nc_def_var(ncid, "H", NC_FLOAT, RANK_H, H_dims, &H_id);
   check_err(stat,__LINE__,__FILE__);

   Hmelt_dims[0] = t_dim;
   Hmelt_dims[1] = x_dim;
   Hmelt_dims[2] = y_dim;
   stat = nc_def_var(ncid, "Hmelt", NC_FLOAT, RANK_Hmelt, Hmelt_dims, &Hmelt_id);
   check_err(stat,__LINE__,__FILE__);

   b_dims[0] = t_dim;
   b_dims[1] = x_dim;
   b_dims[2] = y_dim;
   stat = nc_def_var(ncid, "b", NC_FLOAT, RANK_b, b_dims, &b_id);
   check_err(stat,__LINE__,__FILE__);

   dbdt_dims[0] = t_dim;
   dbdt_dims[1] = x_dim;
   dbdt_dims[2] = y_dim;
   stat = nc_def_var(ncid, "dbdt", NC_FLOAT, RANK_dbdt, dbdt_dims, &dbdt_id);
   check_err(stat,__LINE__,__FILE__);

   T_dims[0] = t_dim;
   T_dims[1] = x_dim;
   T_dims[2] = y_dim;
   T_dims[3] = z_dim;
   stat = nc_def_var(ncid, "T", NC_FLOAT, RANK_T, T_dims, &T_id);
   check_err(stat,__LINE__,__FILE__);

   Tb_dims[0] = t_dim;
   Tb_dims[1] = x_dim;
   Tb_dims[2] = y_dim;
   Tb_dims[3] = zb_dim;
   stat = nc_def_var(ncid, "Tb", NC_FLOAT, RANK_Tb, Tb_dims, &Tb_id);
   check_err(stat,__LINE__,__FILE__);

   age_dims[0] = t_dim;
   age_dims[1] = x_dim;
   age_dims[2] = y_dim;
   age_dims[3] = z_dim;
   stat = nc_def_var(ncid, "age", NC_FLOAT, RANK_age, age_dims, &age_id);
   check_err(stat,__LINE__,__FILE__);

   Ts_dims[0] = t_dim;
   Ts_dims[1] = x_dim;
   Ts_dims[2] = y_dim;
   stat = nc_def_var(ncid, "Ts", NC_FLOAT, RANK_Ts, Ts_dims, &Ts_id);
   check_err(stat,__LINE__,__FILE__);

   ghf_dims[0] = t_dim;
   ghf_dims[1] = x_dim;
   ghf_dims[2] = y_dim;
   stat = nc_def_var(ncid, "ghf", NC_FLOAT, RANK_ghf, ghf_dims, &ghf_id);
   check_err(stat,__LINE__,__FILE__);

   accum_dims[0] = t_dim;
   accum_dims[1] = x_dim;
   accum_dims[2] = y_dim;
   stat = nc_def_var(ncid, "accum", NC_FLOAT, RANK_accum, accum_dims, &accum_id);
   check_err(stat,__LINE__,__FILE__);

   /* assign attributes */
   stat = nc_put_att_text(ncid, polar_stereographic_id, "grid_mapping_name", 19, "polar_stereographic");
   check_err(stat,__LINE__,__FILE__);
   polar_stereographic_straight_vertical_longitude_from_pole[0] = 0;
   stat = nc_put_att_int(ncid, polar_stereographic_id, "straight_vertical_longitude_from_pole", NC_INT, 1, polar_stereographic_straight_vertical_longitude_from_pole);
   check_err(stat,__LINE__,__FILE__);
   polar_stereographic_latitude_of_projection_origin[0] = 90;
   stat = nc_put_att_int(ncid, polar_stereographic_id, "latitude_of_projection_origin", NC_INT, 1, polar_stereographic_latitude_of_projection_origin);
   check_err(stat,__LINE__,__FILE__);
   polar_stereographic_standard_parallel[0] = -71;
   stat = nc_put_att_int(ncid, polar_stereographic_id, "standard_parallel", NC_INT, 1, polar_stereographic_standard_parallel);
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, x_id, "axis", 1, "X");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, x_id, "long_name", 32, "x-coordinate in Cartesian system");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, x_id, "standard_name", 23, "projection_x_coordinate");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, x_id, "units", 1, "m");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, x_id, "pism_intent", 11, "model_state");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, y_id, "axis", 1, "Y");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, y_id, "long_name", 32, "y-coordinate in Cartesian system");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, y_id, "standard_name", 23, "projection_y_coordinate");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, y_id, "units", 1, "m");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, y_id, "pism_intent", 11, "model_state");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, z_id, "axis", 1, "Z");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, z_id, "long_name", 32, "z-coordinate in Cartesian system");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, z_id, "standard_name", 23, "projection_z_coordinate");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, z_id, "units", 1, "m");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, z_id, "positive", 2, "up");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, z_id, "pism_intent", 11, "model_state");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, zb_id, "long_name", 23, "z-coordinate in bedrock");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, zb_id, "standard_name", 34, "projection_z_coordinate_in_bedrock");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, zb_id, "units", 1, "m");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, zb_id, "positive", 2, "up");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, zb_id, "pism_intent", 11, "model_state");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, t_id, "long_name", 4, "time");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, t_id, "units", 33, "seconds since 2007-01-01 00:00:00");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, t_id, "calendar", 4, "none");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, t_id, "axis", 1, "T");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, t_id, "pism_intent", 11, "model_state");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, lon_id, "long_name", 9, "longitude");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, lon_id, "standard_name", 9, "longitude");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, lon_id, "units", 12, "degrees_east");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, lat_id, "long_name", 8, "latitude");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, lat_id, "standard_name", 8, "latitude");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, lat_id, "units", 13, "degrees_north");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, mask_id, "long_name", 39, "grounded_dragging_floating_integer_mask");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, mask_id, "pism_intent", 11, "model_state");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, h_id, "long_name", 16, "surface_altitude");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, h_id, "standard_name", 16, "surface_altitude");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, h_id, "units", 1, "m");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, h_id, "pism_intent", 10, "diagnostic");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, H_id, "long_name", 18, "land_ice_thickness");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, H_id, "standard_name", 18, "land_ice_thickness");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, H_id, "units", 1, "m");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, H_id, "pism_intent", 11, "model_state");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, Hmelt_id, "long_name", 34, "thickness_of_subglacial_melt_water");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, Hmelt_id, "units", 1, "m");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, Hmelt_id, "pism_intent", 11, "model_state");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, b_id, "long_name", 16, "bedrock_altitude");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, b_id, "standard_name", 16, "bedrock_altitude");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, b_id, "units", 1, "m");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, b_id, "pism_intent", 11, "model_state");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, dbdt_id, "long_name", 11, "uplift_rate");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, dbdt_id, "standard_name", 28, "tendency_of_bedrock_altitude");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, dbdt_id, "units", 5, "m s-1");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, T_id, "long_name", 20, "land_ice_temperature");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, T_id, "standard_name", 20, "land_ice_temperature");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, T_id, "units", 1, "K");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, T_id, "pism_intent", 11, "model_state");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, Tb_id, "long_name", 19, "bedrock_temperature");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, Tb_id, "standard_name", 19, "bedrock_temperature");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, Tb_id, "units", 1, "K");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, Tb_id, "pism_intent", 11, "model_state");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, age_id, "long_name", 12, "land_ice_age");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, age_id, "standard_name", 12, "land_ice_age");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, age_id, "units", 1, "s");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, Ts_id, "long_name", 19, "surface_temperature");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, Ts_id, "standard_name", 19, "surface_temperature");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, Ts_id, "units", 1, "K");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, Ts_id, "pism_intent", 14, "climate_steady");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, ghf_id, "long_name", 27, "upward_geothermal_heat_flux");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, ghf_id, "units", 5, "W m-2");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, ghf_id, "pism_intent", 14, "climate_steady");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, accum_id, "long_name", 37, "mean ice equivalent accumulation rate");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, accum_id, "standard_name", 38, "land_ice_surface_specific_mass_balance");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, accum_id, "units", 5, "m s-1");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, accum_id, "pism_intent", 14, "climate_steady");
   check_err(stat,__LINE__,__FILE__);
   stat = nc_put_att_text(ncid, NC_GLOBAL, "Conventions", 6, "CF-1.0");
   check_err(stat,__LINE__,__FILE__);

} // end if (grid.rank == 0)

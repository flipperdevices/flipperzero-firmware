/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.5
 *
 * Configuration saving / restoring
***/

/*** Includes ***/
#include <storage/storage.h>
#include <gui/view.h>

#include "noptel_lrf_sampler.h"
#include "config_view.h"



/*** Routines ***/

/** Load saved configuration options
    Silently fail **/
void load_configuration(App *app) {

  SamplerModel *sampler_model = view_get_model(app->sample_view);
  TestLaserModel *testlaser_model = view_get_model(app->testlaser_view);
  TestPointerModel *testpointer_model = view_get_model(app->testpointer_view);
  Storage* storage;
  File* file;
  Config read_config;
  bool file_read = false;
  uint16_t bytes_read = 0;
  uint8_t mode_idx, buf_idx, beep_idx, smm_pfx_idx;
  uint8_t i;

  /* Open storage and allocate space for the file*/
  storage = furi_record_open(RECORD_STORAGE);
  file = storage_file_alloc(storage);

  /* Attempt to open the file */
  if(storage_file_open(file, config_file, FSAM_READ, FSOM_OPEN_EXISTING)) {

    /* Read the file */
    bytes_read = storage_file_read(file, &read_config, sizeof(Config));

    /* Close the file */
    storage_file_close(file);

    file_read = true;
  }
  else
    FURI_LOG_I(TAG, "Could not read config file %s", config_file);

  /* Free the file and close storage */
  storage_file_free(file);
  furi_record_close(RECORD_STORAGE);

  /* If we couldn't read the file, give up */
  if(!file_read)
    return;

  /* If we didn't read enough bytes, give up */
  if(bytes_read < sizeof(Config)) {
    FURI_LOG_I(TAG, "Read %d bytes from config file %s but %d expected",
			bytes_read, config_file, sizeof(Config));
    return;
  }

  /* Do we have a SMM prefix sequence? */
  if(read_config.smm_pfx_sequence[0]) {

    /* Do we have a valid SMM prefix configuration label? */
    for(i = 0; i < sizeof(read_config.config_smm_pfx_label) &&
		read_config.config_smm_pfx_label[i] >= 32 &&
		read_config.config_smm_pfx_label[i] < 127; i++);

    if(i > 0 && !read_config.config_smm_pfx_label[i]) {

      /* Do we have a valid first SMM prefix configuration choice name? */
      for(i = 0; i < sizeof(read_config.config_smm_pfx_names[0]) &&
			read_config.config_smm_pfx_names[0][i] >= 32 &&
			read_config.config_smm_pfx_names[0][i] < 127;
			i++);

      if(i > 0 && !read_config.config_smm_pfx_names[0][i]) {

        /* Do we have a valid second SMM prefix configuration choice name? */
        for(i = 0; i < sizeof(read_config.config_smm_pfx_names[1]) &&
			read_config.config_smm_pfx_names[1][i] >= 32 &&
			read_config.config_smm_pfx_names[1][i] < 127; i++);

        if(i > 0 && !read_config.config_smm_pfx_names[1][i]) {

          /* Store the SMM prefix sequence in the sampler model */
          memcpy(sampler_model->config.smm_pfx_sequence,
			read_config.smm_pfx_sequence,
			sizeof(read_config.smm_pfx_sequence));

          /* Store the configuration menu item string in the sampler model */
          memcpy(sampler_model->config.config_smm_pfx_label,
			read_config.config_smm_pfx_label,
			sizeof(read_config.config_smm_pfx_label));

          /* Store the configuration choice names in the sampler model */
          memcpy(sampler_model->config.config_smm_pfx_names[0],
			read_config.config_smm_pfx_names[0],
			sizeof(read_config.config_smm_pfx_names[0]));

          memcpy(sampler_model->config.config_smm_pfx_names[1],
			read_config.config_smm_pfx_names[1],
			sizeof(read_config.config_smm_pfx_names[1]));

          /* Add the item to the configuration menu */
          app->item_smm_pfx =
		variable_item_list_add(
				app->config_list,
				sampler_model->config.config_smm_pfx_label,
				nb_config_smm_pfx_values,
				config_smm_pfx_change, app);

          /* Check that the SMM prefix option exists */
          for(smm_pfx_idx = 0; smm_pfx_idx < nb_config_smm_pfx_values &&
		read_config.smm_pfx != config_smm_pfx_values[smm_pfx_idx];
		smm_pfx_idx++);

          if(smm_pfx_idx >= nb_config_smm_pfx_values) {
            FURI_LOG_I(TAG, "Invalid SMM prefix option %d in config file %s",
				read_config.smm_pfx, config_file);
            return;
          }

          /* Configure the SMM prefix option from the read value */
          sampler_model->config.smm_pfx = read_config.smm_pfx;
          variable_item_set_current_value_index(app->item_smm_pfx, smm_pfx_idx);
          variable_item_set_current_value_text(
				app->item_smm_pfx,
				read_config.config_smm_pfx_names[smm_pfx_idx]);

          FURI_LOG_I(TAG, "Restored config %s %s",
				read_config.config_smm_pfx_label,
				read_config.config_smm_pfx_names[smm_pfx_idx]);
        }
      }
    }
  }

  /* Check that the sampling mode setting exists */
  for(mode_idx = 0; mode_idx < nb_config_mode_values &&
			read_config.mode != config_mode_values[mode_idx];
	mode_idx++);

  if(mode_idx >= nb_config_mode_values) {
    FURI_LOG_I(TAG, "Invalid sampling mode value %d in config file %s",
			read_config.mode, config_file);
    return;
  }

  /* Check that the buffering setting exists */
  for(buf_idx = 0; buf_idx < nb_config_buf_values &&
			read_config.buf != config_buf_values[buf_idx];
	buf_idx++);

  if(buf_idx >= nb_config_buf_values) {
    FURI_LOG_I(TAG, "Invalid buffering value %d in config file %s",
			read_config.buf, config_file);
    return;
  }

  /* Check that the beep option exists */
  for(beep_idx = 0; beep_idx < nb_config_beep_values &&
			read_config.beep != config_beep_values[beep_idx];
	beep_idx++);

  if(beep_idx >= nb_config_beep_values) {
    FURI_LOG_I(TAG, "Invalid beep option %d in config file %s",
			read_config.beep, config_file);
    return;
  }

  /* Check that the submenu item selection exists */
  if(read_config.sitem >= total_submenu_items) {
    FURI_LOG_I(TAG, "Invalid submenu item %d in config file %s",
			read_config.sitem, config_file);
    return;
  }

  /* Configure the sampling mode setting from the read value */
  sampler_model->config.mode = read_config.mode;
  variable_item_set_current_value_index(app->item_mode, mode_idx);
  variable_item_set_current_value_text(app->item_mode,
					config_mode_names[mode_idx]);

  /* Configure the buffering setting from the read value */
  sampler_model->config.buf = read_config.buf;
  variable_item_set_current_value_index(app->item_buf, buf_idx);
  variable_item_set_current_value_text(app->item_buf,
					config_buf_names[buf_idx]);

  /* Configure the beep option from the read value */
  sampler_model->config.beep = read_config.beep;
  testlaser_model->beep = read_config.beep;
  testpointer_model->beep = read_config.beep;
  variable_item_set_current_value_index(app->item_beep, beep_idx);
  variable_item_set_current_value_text(app->item_beep,
					config_beep_names[beep_idx]);

  /* Restore the saved last selected submenu item */
  sampler_model->config.sitem = read_config.sitem;
  submenu_set_selected_item(app->submenu, read_config.sitem);

  FURI_LOG_I(TAG, "Restored config sampling mode %s, buffering %s, beep %s, "
			"selected submenu item %d",
		config_mode_names[mode_idx], config_buf_names[buf_idx],
		config_beep_names[beep_idx], read_config.sitem);
}



/** Save configuration options
    Silently fail **/
void save_configuration(App *app) {

  SamplerModel *sampler_model = view_get_model(app->sample_view);
  Storage* storage;
  File* file;
  bool file_written = false;
  uint16_t bytes_written = 0;

  /* Open storage and allocate space for the file*/
  storage = furi_record_open(RECORD_STORAGE);
  file = storage_file_alloc(storage);

  /* Attempt to open the file */
  if(storage_file_open(file, config_file, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {

    /* Write the file */
    bytes_written = storage_file_write(file, &sampler_model->config,
					sizeof(Config));
    /* Close the file */
    storage_file_close(file);

    file_written = true;
  }
  else
    FURI_LOG_I(TAG, "Could not open config file %s for writing", config_file);

  /* Free the file and close storage */
  storage_file_free(file);
  furi_record_close(RECORD_STORAGE);

  /* If we couldn't write the file, stop now */
  if(!file_written)
    return;

  /* If we didn't write the correct number of bytes, log the error */
  if(bytes_written != sizeof(Config))
    FURI_LOG_I(TAG, "Wrote %d bytes to config file %s but %d expected",
			bytes_written, config_file, sizeof(Config));

  FURI_LOG_I(TAG, "Config saved in file %s", config_file);
}

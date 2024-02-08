##########################################################################################
# Python script to replace formely used template sensor, which causes havoc in HA 115
# https://community.home-assistant.io/t/develop-and-rebuild-template-sensor-to-python-script/228699
# thanks to @VDRainer for providing most of the code here, and persevering my thoughts and
# questions
# 23 sep 2020 @mariusthvdb
##########################################################################################

####################
#ENTITIES EXCLUSION#
####################

# excludes based on certain conditions
# if hass.states.get('binary_sensor.outside_daylight_sensor').state == 'on':
#     exclude_conditional_entities = ['light.parking_light']
# else:
#    exclude_conditional_entities = []

exclude_conditional_entities = []

if hass.states.get('sensor.weather_control_status').state == 'unavailable':
    exclude_weather_station_entities = [
                    'sensor.chance_of_precip',
                    'sensor.condition',
                    'sensor.humidex',
                    'sensor.icon_code',
                    'camera.dolbeau_radar',
                    'sensor.rain_count',
                    'sensor.raining_timer',
                    'sensor.precipitation_yesterday',
                    'sensor.visibility',
                    'sensor.wind_chill',
                    'sensor.wind_gust',
                    'sensor.uv_index_2',
                    'sensor.dolbeau_aqhi',
                    'sensor.outdoor_humidity_bme',
                    'sensor.outdoor_humidity_dht',
                    'sensor.outdoor_pressure_raw',
                    'sensor.outdoor_rain_sensor',
                    'sensor.outdoor_temperature_bme',
                    'sensor.outdoor_temperature_dht',
                    'sensor.outdoor_temperature_ntc',
                    'sensor.rain_level_toggle',
                    'sensor.rain_sensor',
                    'sensor.uv_sensor',
                    'sensor.aqi_sensor',
                    'sensor.weather_control_5v_voltage',
                    'sensor.weather_control_box_humidity',
                    'sensor.weather_control_box_temperature',
                    'sensor.weather_control_input_voltage',
                    'sensor.wind_direction_filter',
                    'sensor.wind_direction_raw',
                    'sensor.wind_direction_stats',
                    'sensor.wind_direction_average',
                    'sensor.wind_speed_gust',
                    'sensor.wind_speed_raw',
                    'sensor.wind_speed_average'
                    ]
else:
    exclude_weather_station_entities = [
                    'sensor.chance_of_precip',
                    'sensor.condition',
                    'sensor.humidex',
                    'sensor.icon_code',
                    'camera.dolbeau_radar',
                    'sensor.rain_count',
                    'sensor.raining_timer',
                    'sensor.precipitation_yesterday',
                    'sensor.visibility',
                    'sensor.wind_chill',
                    'sensor.wind_gust',
                    'sensor.uv_index_2',
                    'sensor.dolbeau_aqhi',
                    'sensor.wind_bearing',
                    'sensor.wind_direction_2',
                    ]

if hass.states.get('binary_sensor.garage_access_point').state == 'off':
    exclude_garage_entities = [
                    'binary_sensor.garage_air_valve_closed',
                    'binary_sensor.garage_attic_light',
                    'binary_sensor.garage_bd_locked',
                    'binary_sensor.garage_bd_opened',
                    'binary_sensor.garage_cd_locked',
                    'binary_sensor.garage_cd_opened',
                    'binary_sensor.garage_fd_bell',
                    'binary_sensor.garage_fd_opened',
                    'binary_sensor.garage_front_heater_ac',
                    'binary_sensor.garage_rear_heater_ac',
                    'binary_sensor.garagefrontheater_error',
                    'binary_sensor.garagefrontlight_status',
                    'binary_sensor.garagefrontpower_status',
                    'binary_sensor.garagerearheater_error',
                    'binary_sensor.house_air_valve_closed',
                    'climate.garage_front_hvac',
                    'climate.garage_rear_hvac',
                    'cover.garage_cd_opener',
                    'lock.garage_fd_lock',
                    'lock.shed_fd_lock',
                    'sensor.air_compressor_pressure',
                    'sensor.air_compressor_pressure_raw',
                    'sensor.garage_air_valve',
                    'sensor.garage_bd_status',
                    'sensor.garage_cd_raw',
                    'sensor.garage_cd_status',
                    'sensor.garage_fd_status',
                    'sensor.garage_front_hum',
                    'sensor.garage_front_temp',
                    'sensor.garage_front_temp_raw',
                    'sensor.garage_main_12v_voltage',
                    'sensor.garage_main_5v_voltage',
                    'sensor.garage_main_box_humidity',
                    'sensor.garage_main_box_temperature',
                    'sensor.garage_mean_temp',
                    'sensor.garage_rear_hum',
                    'sensor.garage_rear_temp',
                    'sensor.garage_rear_temp_raw',
                    'sensor.garage_safety_front_temp_raw',
                    'sensor.garage_safety_front_temperature',
                    'sensor.garage_safety_rear_temp_raw',
                    'sensor.garage_safety_rear_temperature',
                    'sensor.garage_slave_5v_voltage',
                    'sensor.garage_slave_box_humidity',
                    'sensor.garage_slave_box_temperature',
                    'sensor.house_air_valve',
                    'switch.air_compressor_purge',
                    'switch.garage_air_compressor',
                    'switch.garage_air_valve',
                    'switch.garage_air_valve_open',
                    'switch.garage_front_heater',
                    'switch.garage_front_light',
                    'switch.garage_front_power',
                    'switch.garage_rear_heater',
                    'switch.house_air_valve',
                    'switch.house_air_valve_open'
                    ]
else:
   exclude_garage_entities = []

if hass.states.get('sensor.shed_control_status').state == 'unavailable':
    exclude_shed_entities = [
                    'sensor.shed_12v_voltage',
                    'sensor.shed_5v_voltage',
                    'sensor.shed_box_humidity',
                    'sensor.shed_box_temperature',
                    'switch.shed_door_light',
                    'switch.shed_inside_light',
                    'switch.shed_pool_light',
                    'binary_sensor.shed_fd_opened',
                    'binary_sensor.shed_ld_opened',
                    'binary_sensor.shed_rd_opened'
                    ]
else:
   exclude_shed_entities = []
   
if hass.states.get('binary_sensor.wavlink_access_point').state == 'unavailable':
    exclude_wavlink_entities = [
                    'binary_sensor.pool_td_opened',
                    'binary_sensor.shed_fd_opened',
                    'sensor.pool_toilet_3v3_voltage',
                    'sensor.pool_toilet_humidity',
                    'sensor.pool_toilet_temperature',
                    'sensor.shed_fd_status',
                    'sensor.pool_td_status'
                    ]
else:
   exclude_wavlink_entities = []

#Other winterized devices
if hass.states.get('input_boolean.global_winterization').state == 'on':
    exclude_winterized_entities = [
                    'automation.shed_inside_light',
                    'binary_sensor.glasshouse_fd_opened',
                    'binary_sensor.shed_fd_opened',
                    'lock.glasshouse_fd_lock',
                    'lock.shed_fd_lock'
                    ]
else:
   exclude_winterized_entities = []

exclude_entities = [
                    'automation.restart_hass',
                    'binary_sensor.octoprint_printing',
                    'binary_sensor.octoprint_printing_error',
                    'button.octoprint_pause_job',
                    'button.octoprint_resume_job',
                    'button.octoprint_stop_job',
                    'sensor.octoprint_actual_bed_temp',
                    'sensor.octoprint_actual_tool0_temp',
                    'sensor.octoprint_current_state',
                    'sensor.octoprint_estimated_finish_time',
                    'sensor.octoprint_job_percentage',
                    'sensor.octoprint_start_time',
                    'sensor.octoprint_target_bed_temp',
                    'sensor.octoprint_target_tool0_temp',
                    'sensor.octoprint_time_elapsed',
                    'sensor.octoprint_time_remaining',
                    'sensor.ted5000_mtu0_energy_daily_cost',
                    'sensor.ted5000_mtu1_energy_daily_cost',
                    'sensor.ted5000_mtu2_energy_daily_cost',
                    'sensor.ted5000_mtu3_energy_daily_cost',
                    'sensor.iphone_de_jean_activity',
                    'sensor.iphone_de_cindy_activity',
                    'sensor.spacex_latest_launch_payload',
                    'sensor.spacex_next_launch_payload',
                    'sensor.ipad_de_cindy_activity',
                    'sensor.ipad_de_cindy_battery_level',
                    'sensor.ipad_de_cindy_battery_state',
                    'sensor.ipad_de_cindy_bssid',
                    'sensor.ipad_de_cindy_connection_type',
                    'sensor.ipad_de_cindy_last_update_trigger',
                    'sensor.ipad_de_cindy_ssid',
                    'sensor.ipad_de_cindy_storage',
                    'binary_sensor.remote_ui',
                    'sensor.home_assistant_mariadb',
                    'sensor.mariadb_addon_update_available',
                    'sensor.jgau_pc_network_local_area_connection',
                    'sensor.ted5000_mtu0_energy_daily_raw_cost',
                    'sensor.ted5000_mtu2_energy_daily_raw_cost',
                    'sensor.ted5000_mtu3_energy_daily_raw_cost',
                    'input_text.pool_clock_text_top',
                    'input_text.pool_clock_text_bottom',
                    'input_text.garage_clock_text',
                    'sensor.jgau_pc_audio_default_input_device',
                    'sensor.jgau_pc_audio_default_input_device_muted',
                    'sensor.jgau_pc_audio_default_input_device_state',
                    'sensor.jgau_pc_audio_default_input_device_volume',
                    'sensor.jgau_pc_network_ethernet_2',
                    'sensor.jgau_pc_network_wi_fi',
                    'automation.notification_cindy_magasinage'
                    ]
                    
#Entities not ready or not used yet
exclude_entities_blacklist = [
                    'automation.garage_heaters_safety_notif',
                    'automation.shed_inside_light',
                    'binary_sensor.house_sd_bell',
                    'binary_sensor.house_sd_opened',
                    'binary_sensor.glasshouse_sd_opened',
                    'light.massage_clock_rgb',
                    'light.pool_water_rgb',
                    'lock.house_sd_lock',
                    'sensor.house_sd_status',
                    'sensor.glasshouse_sd_status',
                    'sensor.massage_room_humidity',
                    'sensor.massage_room_temp',
                    'sensor.glasshouse_12v_voltage',
                    'sensor.glasshouse_5v_voltage',
                    'sensor.glasshouse_box_humidity',
                    'sensor.glasshouse_box_temperature',
                    'sensor.glasshouse_control_status',
                    'sensor.living_room_hum',
                    'sensor.living_room_temp',
                    'sensor.pond_area_humidity',
                    'sensor.pond_area_temperature',
                    'sensor.pond_control_12v_voltage',
                    'sensor.pond_control_5v_voltage',
                    'sensor.pond_control_3v3_voltage',
                    'sensor.pond_control_status',
                    'sensor.pond_temperature',
                    'sensor.pond_temperature_raw',
                    'sensor.pond_temperature_stats',
                    'binary_sensor.pond_level_high',
                    'binary_sensor.pond_level_low',
                    'automation.pond_filler_force_start',
                    'sensor.pond_control_box_humidity',
                    'sensor.pond_control_box_temperature',
                    'sensor.pool_ph',
                    'automation.control_room_door_open_immediate',
                    'automation.control_room_door_open_repetitive',
                    'automation.publish_postal_arrival',
                    'automation.publish_postal_removal',
                    'sensor.recroom_hvac_energy',
                    'sensor.recroom_hvac_energy_daily',
                    'sensor.recroom_hvac_power',
                    'sensor.recroom_hvac_temperature',
                    'automation.garage_temperature_error',
                    'switch.camera_ir_poolarea',
                    'switch.camera_pool_area'
                    ]
                    
excludes = exclude_conditional_entities + exclude_entities + exclude_entities_blacklist + exclude_weather_station_entities + exclude_garage_entities + exclude_shed_entities + exclude_wavlink_entities + exclude_winterized_entities

####################
# GROUPS EXCLUSION #
####################

exclude_group = []#'group.entity_blacklist'] # configured in HA group outside this python script

if hass.states.get('input_boolean.pool_running').state.lower() == 'off':
    exclude_group_pool = ['group.pool_sensors']
else:
    exclude_group_pool = []
    
if hass.states.get('input_boolean.pond_running').state.lower() == 'off':
    exclude_group_pond = ['group.pond_sensors']
else:
    exclude_group_pond = []

exclude_groups = exclude_group + exclude_group_pool + exclude_group_pond
exclude_domains = ['media_player','automation','geo_location','group','button']
    
# some variables
count_unfiltered = 0
count_all = 0
count_none = 0
count_unavailable = 0
count_unknown = 0
count_sensors = 0
count_automations = 0
count_blacklisted = 0

list_unfiltered = []
list_all = []
list_none = []
list_unavailable = []
list_unknown = []
list_sensors = []
list_automations = []
list_blacklisted = []


attributes = {}

# create unfiltered list to fine-tune filtering
for entity_id in hass.states.entity_ids():
    state = hass.states.get(entity_id).state.lower()
    domain = entity_id.split('.')[0]
    if state in ['none', 'unavailable', 'unknown']:
        list_unfiltered.append(entity_id)
        count_unfiltered = count_unfiltered + 1
    if entity_id in exclude_entities_blacklist:
        if (state in ['none', 'unavailable', 'unknown']) or (domain == 'automation' and state == 'off') :
            count_blacklisted = count_blacklisted + 1
            list_blacklisted.append(entity_id)
          

# add groups entities to exclude_entities list
for group in exclude_groups:
    for entity_id in hass.states.get(group).attributes['entity_id']:
        excludes.append(entity_id)

# iterate over all entities
for entity_id in hass.states.entity_ids():
    domain = entity_id.split('.')[0]
    if entity_id not in excludes and domain not in exclude_domains:
        state = hass.states.get(entity_id).state.lower()
        if state in ['none','unavailable','unknown']:
            count_all = count_all + 1
            list_all.append(entity_id)
            if state == 'none':
                count_none = count_none + 1
                list_none.append(entity_id)
            if state == 'unavailable':
                count_unavailable = count_unavailable + 1
                list_unavailable.append(entity_id)
            if state == 'unknown':
                count_unknown = count_unknown + 1
                list_unknown.append(entity_id)
            if domain == 'sensor':
               count_sensors = count_sensors + 1
               list_sensors.append(entity_id)
    if domain == 'automation' and entity_id not in excludes:
        state = hass.states.get(entity_id).state.lower()
        if state == 'off':
            count_all = count_all + 1
            list_all.append(entity_id)
            count_automations = count_automations + 1
            list_automations.append(entity_id)


# Build the attributes
attributes['all_count'] = count_all
attributes['none_count'] = count_none
attributes['unavailable_count'] = count_unavailable
attributes['unknown_count'] = count_unknown
attributes['sensors_count'] = count_sensors
attributes['automations_count'] = count_automations
attributes['blacklisted_count'] = count_blacklisted
attributes['unfiltered_count'] = count_unfiltered


attributes['all_entities'] = '\n'.join(sorted(list_all)) if count_all>0 else 'All entities OK!'
attributes['none_entities'] = '\n'.join(sorted(list_none)) if count_none>0 else 'No None entities'
attributes['unavailable_entities'] = '\n'.join(sorted(list_unavailable)) if count_unavailable>0 else 'No Unavailable entities'
attributes['unknown_entities'] = '\n'.join(sorted(list_unknown)) if count_unknown>0 else 'No Unknown entities'
attributes['sensors_entities'] = '\n'.join(sorted(list_sensors)) if count_sensors>0 else 'All sensors Ok'
attributes['automations_entities'] = '\n'.join(sorted(list_automations)) if count_automations>0 else 'All automations Ok'
attributes['blacklisted_entities'] = '\n'.join(sorted(list_blacklisted)) if count_blacklisted>0 else 'All blacklisted Ok'
attributes['unfiltered_entities'] = '\n'.join(sorted(list_unfiltered)) if count_unfiltered>0 else 'All and Unfiltered Ok'


attributes['friendly_name'] = 'Entities Issues'
attributes['icon'] = 'mdi:thumb-down' if count_all>0 else 'mdi:thumb-up'
attributes['icon_color'] = 'red' if count_all>0 else 'green'

# set the sensor
hass.states.set('sensor.uun_ordered', count_all, attributes)

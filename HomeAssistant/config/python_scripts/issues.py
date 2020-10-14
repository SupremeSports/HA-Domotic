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

exclude_entities = [
                    'sensor.chance_of_precip', 
                    'sensor.condition', 
                    'sensor.humidex', 
                    'sensor.icon_code', 
                    'sensor.rain_count', 
                    'sensor.precipitation_yesterday',
                    'sensor.visibility', 
                    'sensor.wind_chill', 
                    'sensor.wind_gust',
                    'sensor.octoprint_current_state', 
                    'sensor.octoprint_time_elapsed', 
                    'sensor.octoprint_time_remaining'
                    ]
                    
#Entities not ready or not used yet
exclude_entities_blacklist = [
                    'binary_sensor.house_bd_bell',
                    'binary_sensor.house_bd_opened',
                    'binary_sensor.house_sd_bell',
                    'binary_sensor.house_sd_opened',
                    'binary_sensor.shed_fd_opened',
                    'light.massage_clock_rgb',
                    'lock.house_bd_lock',
                    'lock.house_sd_lock',
                    'lock.shed_fd_lock',
                    'sensor.house_bd_status',
                    'sensor.house_sd_status',
                    'sensor.massage_room_humidity',
                    'sensor.massage_room_temp',
                    'sensor.postal_status_json',
                    'sensor.shed_fd_status',
                    'switch.postal_status',
                    'switch.postal_status_activate_delay',
                    'sensor.pond_area_humidity',
                    'sensor.pond_area_temperature',
                    'sensor.pond_control_12v_voltage',
                    'sensor.pond_control_5v_voltage',
                    'sensor.pond_control_box_humidity',
                    'sensor.pond_control_box_temperature',
                    'sensor.pond_control_status',
                    'sensor.pool_ph'
                    ]
                    
excludes = exclude_conditional_entities + exclude_entities + exclude_entities_blacklist

####################
# GROUPS EXCLUSION #
####################

exclude_group = []#'group.entity_blacklist'] # configured in HA group outside this python script

if hass.states.get('input_boolean.pool_running').state == 'off':
    exclude_group_pool = ['group.pool_sensors']
else:
    exclude_group_pool = []
    
if hass.states.get('input_boolean.pond_running').state == 'off':
    exclude_group_pond = ['group.pond_sensors']
else:
    exclude_group_pond = []

exclude_groups = exclude_group + exclude_group_pool + exclude_group_pond
exclude_domains = ['media_player','automation','geo_location']
    
# some variables
count_unfiltered = 0
count_all = 0
count_none = 0
count_unavailable = 0
count_unknown = 0
count_sensors = 0
count_automations = 0

list_unfiltered = []
list_all = []
list_none = []
list_unavailable = []
list_unknown = []
list_sensors = []
list_automations = []


attributes = {}

# create unfiltered list to fine-tune filtering
for entity_id in hass.states.entity_ids():
    state = hass.states.get(entity_id).state
    if state in ['none', 'unavailable', 'unknown']:
         list_unfiltered.append(entity_id)
         count_unfiltered = count_unfiltered + 1

# add groups entities to exclude_entities list
for group in exclude_groups:
    for entity_id in hass.states.get(group).attributes['entity_id']:
        excludes.append(entity_id)

# iterate over all entities
for entity_id in hass.states.entity_ids():
    domain = entity_id.split('.')[0]
    if entity_id not in excludes and domain not in exclude_domains:
        state = hass.states.get(entity_id).state
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
        state = hass.states.get(entity_id).state
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
attributes['unfiltered_count'] = count_unfiltered


if count_all > 0:
    icon = 'mdi:thumb-down'
    icon_color = 'red'
    attributes['all_entities'] = '\n'.join(sorted(list_all))
else:
    icon = 'mdi:thumb-up'
    icon_color = 'green'
    attributes['all_entities'] = 'All entities OK!'

if count_none > 0:
    attributes['none_entities'] = '\n'.join(sorted(list_none))
else:
    attributes['none_entities'] = 'No None entities'

if count_unavailable > 0:
    attributes['unavailable_entities'] = '\n'.join(sorted(list_unavailable))
else:
    attributes['unavailable_entities'] = 'No Unavailable entities'

if count_unknown > 0:
    attributes['unknown_entities'] = '\n'.join(sorted(list_unknown))
else:
    attributes['unknown_entities'] = 'No Unknown entities'

if count_sensors > 0:
    attributes['sensors_entities'] = '\n'.join(sorted(list_sensors))
else:
    attributes['sensors_entities'] = 'All sensors Ok'
    
if count_automations > 0:
    attributes['automations_entities'] = '\n'.join(sorted(list_automations))
else:
    attributes['automations_entities'] = 'All automations Ok'

if count_unfiltered > 0:
    attributes['unfiltered_entities'] = '\n'.join(sorted(list_unfiltered))
else:
    attributes['unfiltered_entities'] = 'All and Unfiltered Ok'
    
attributes['friendly_name'] = 'Entities Issues'
attributes['icon'] = icon
attributes['icon_color'] = icon_color

# set the sensor
hass.states.set('sensor.uun_ordered', count_all, attributes)

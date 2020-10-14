##########################################################################################
# Python script to count sensors by domain
# 2 oct 2020 @supremesports
##########################################################################################
# some variables

count_all = 0
domains = []
attributes = {}

for entity_id in hass.states.entity_ids():
    count_all = count_all + 1
    entity_domain = entity_id.split('.')[0]
    if entity_domain not in domains:
        domains.append(entity_domain)

attributes['Entities'] = count_all
attributes['Domains'] = len(domains)
attributes['--------------'] = '-------'
for domain in sorted(domains):
    attributes[domain] = len(hass.states.entity_ids(domain))

attributes['friendly_name'] = 'Entities'
attributes['icon'] = 'mdi:script-text'
#attributes['icon_color'] = icon_color

# set the sensor
hass.states.set('sensor.entities', count_all, attributes)
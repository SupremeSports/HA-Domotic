# Mqtt template switch

Based on standard mqtt switch component with templateble properties:

```yaml
payload_on
payload_off
```
Each of them can contains template

# Example:

```yaml
switch:
  - platform: mqtt_template
    name: laundry-fan
    state_topic: "home/bathroom/switch/laundry-fan/state"
    command_topic: "home/bathroom/switch/laundry-fan"
    payload_on: '{"action":"hold","duration": {{(states.input_number.fan_duration.state
        | int)*1000}}}'
    payload_off: '{"action":"off"}'
    state_on: 1
    state_off: 0
```
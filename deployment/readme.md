# Deployment playbook
## Priority neighborhood
1. Add suggested sensor locations to the ["**Sensor inbox**"](https://docs.google.com/spreadsheets/d/1PHg1eIUd0UxAiTazykVBoy64rBAW5rRA5NoZ6fUNqqc/edit#gid=1479948156) sheet, ensuring you've filled in all the required columns marked with an asterisk.
2. Identify the next priority neighborhood (this process is TBD) and use the filter on the "**Neighborhood**" column to isolate the suggested sensors located in that neighborhood.

## Gateway scouting and outreach
1. Using the [TTN Mapper](https://ttnmapper.org/heatmap), locate any blue icon-ed Things Network gateways within ~0.5 miles of the proposed deployment neighborhood and add them to the ["**Gateway inbox**"](https://docs.google.com/spreadsheets/d/1PHg1eIUd0UxAiTazykVBoy64rBAW5rRA5NoZ6fUNqqc/edit#gid=1479948156) sheet.
2. Using the [NYCHA building map](https://www.arcgis.com/apps/webappviewer/index.html?id=41c6ff5e73ec459092e982060b7cf1a1), [City facilities map](https://cmydlarz.carto.com/builder/e06a8892-1836-4241-9514-2b02cae1ac7b/embed), and [Google Maps](https://www.google.com/maps) - locate buildings that could potentially host a gateway and add them to the ["**Gateway inbox**"](https://docs.google.com/spreadsheets/d/1PHg1eIUd0UxAiTazykVBoy64rBAW5rRA5NoZ6fUNqqc/edit#gid=1479948156) sheet, prioritizing the following criteria:
   * Tall with un-occluded views of the area where sensors will be deployed
   * Central to and within ~1 mile of potential sensor locations
   * City owned preferably by: DEP, DOT, NYCHA, DCAS
3. Start outreach to each agency/building owner for blanket gateway mounting permissions.
4. Identify building managers and facilities personnel who can provide roof access.
5. Update the ["**Gateway inbox**"](https://docs.google.com/spreadsheets/d/1PHg1eIUd0UxAiTazykVBoy64rBAW5rRA5NoZ6fUNqqc/edit#gid=1479948156) sheet.

## Sensor scouting and deployment
1. To virtually scout sensor locations refer to [Flood Hotspot Identification Strategy](https://github.com/floodnet-nyc/flood-sensor/blob/main/deployment/flood-hotspots-identification.md) and follow the below steps:
   * Create new sheet for neighborhood by duplicating the ["**Template**"](https://docs.google.com/spreadsheets/d/1PHg1eIUd0UxAiTazykVBoy64rBAW5rRA5NoZ6fUNqqc/edit#gid=130763079) sheet and renaming
   * Complete all columns with as much information as possible.
   * Update the ["**Sensor Locations Planner**"](https://www.google.com/maps/d/u/3/edit?mid=1njszfj9XP9E2616GYRWTTzQ7gLllMuxZ&ll=40.72909711386875%2C-73.9801152&z=12) map with proposed sensor locations.
2. Delete rows from ["**Sensor inbox**"](https://docs.google.com/spreadsheets/d/1PHg1eIUd0UxAiTazykVBoy64rBAW5rRA5NoZ6fUNqqc/edit#gid=1479948156) sheet for the sensors you have already migrated to their own neighborhood sheet.
3. Plan for deployment using [this guide for sensors](https://github.com/floodnet-nyc/flood-sensor/blob/main/deployment/flood-sensor-deployment-manual.md) and [this guide for gateways](https://github.com/floodnet-nyc/floodnet-gateway).
4. Deploy gateway and sensors!
5. Move deployed gateway row from ["**Gateway inbox**"](https://docs.google.com/spreadsheets/d/1PHg1eIUd0UxAiTazykVBoy64rBAW5rRA5NoZ6fUNqqc/edit#gid=1479948156) sheet to neighborhood sheet.
6. Update deployed date and any extra fields in the neighborhood sheet for deployed sensors.
7. Update the [Sensor Locations Directory](https://docs.google.com/spreadsheets/d/1NVvIKUWnidkvHxfQ3tJmftphjNo6dKeGr0YioP82oGE/edit#gid=0) with the newly deployed sensors.
8. Update the [Sensor Locations Directory - DOT](https://docs.google.com/spreadsheets/d/1ZReNvlH-R3MdetIZDS5ga3bWxmXC1i-LLoB1pRAIp6k/edit#gid=0) sheet and [Sensor Locations Directory - DOT](https://www.google.com/maps/d/edit?mid=1hVbk4Q3k9M8H7DdSSQZ9QwjQ_xt-dHmK&usp=sharing) map with the newly deployed sensors.
9. Migrate the newly deployed sensors from the "**Potential Sensors**" to the "**Deployed Sensors**" layer, adding the sensor's device ID to the "**name**" field.

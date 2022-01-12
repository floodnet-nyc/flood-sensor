# Flood Hotspots Identification
Several parameters are relevant when determining flood sensor locations. This manual describes those parameters, and how they factor into the selection process. Once a location is selected, we deploy our sensor through a process described in the [Flood Sensor Deployment Manual](https://github.com/floodnet-nyc/flood-sensor/blob/main/deployment/manuals/flood-sensor-deployment-manual.md).

**Table of Contents:**
- [Flood Hotspots Identificantion](#flood-hotspots-identification)
  - [1. Community Engagement](#1-community-engagement)
  - [2. Verification of Flooding](#2-verification-of-flooding)
  - [3. Gateway Location](#3-gateway-location)

## 1. Community Engagement
We want to make sure that those who are most impacted by severe flooding have a say in determining where sensors will be mounted. This [Google Form](https://forms.gle/4kJpujo9pDt7hZmRA) allows NYC residents to suggest locations for deployment.

We are partnered with NY Sea Grant and the Science and Resilience Institute of Jamaica Bay; these organizations, along with the NYC Mayor’s Office of Climate Resiliency and other partners, co-founded a [Community Flood Watch Project](https://www.srijb.org/jbfloodwatch/) that has helped to inform our outreach. We are continuing to seek out partnerships with other community organizations that can inform our hotspot identification process.

The Mayor’s Office of Climate and Resiliency has also provided us with a list of flood-prone neighborhoods, to help us ensure that we are distributing our sensors throughout all five boroughs.

## 2. Verification of Flooding
Once a location is proposed, we consult the [NYC Stormwater Flood Maps](https://experience.arcgis.com/experience/4b290961cac34643a49b9002f165fad8/) to confirm that this location is likely to be impacted by flooding in a Moderate Stormwater Event.

## 3. Gateway Location
Once we have a potential sensor location, we need to ensure that it is proximate to a LoRaWAN gateway. In this [Sensor Locations Planner](https://www.google.com/maps/d/edit?mid=1njszfj9XP9E2616GYRWTTzQ7gLllMuxZ&usp=sharing), we have mapped out existing locations of gateways that we have installed, or that are visible on this [TTN Mapper](https://ttnmapper.org/heatmap/).

If our potential sensor location is not close enough to one of these gateways, it will slow down the process a bit. In order to mount, we will have to seek out a location where we can mount a gateway. Typically, we can identify these locations by building relationships with organizations in the area. Our [Google Form](https://forms.gle/4kJpujo9pDt7hZmRA) provides an opportunity for community members to identify potential gateway sites. We mount gateways according to this [Floodnet Gateway setup](https://github.com/floodnet-nyc/floodnet-gateway) guide.

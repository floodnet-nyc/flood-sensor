# Flood Hotspots Identification
This manual describes the parameters that inform sensor location selection. Once a location is selected, we deploy our sensor through a process described in the [Flood Sensor Deployment Manual](https://github.com/floodnet-nyc/flood-sensor/blob/main/deployment/manuals/flood-sensor-deployment-manual.md).

**Table of Contents:**
- [Flood Hotspots Identificantion](#flood-hotspots-identification)
  - [1. Community Engagement](#1-community-engagement)
    - [1.1. Sensor Suggestion Form](#11-sensor-suggestion-form)
    - [1.2. Stakeholder Relationships](#12-stakeholder-relationships)
    - [1.3. Fair Distribution of Sensors](#13-fair-distribution-of-sensors)
  - [2. Verification of Flooding](#2-verification-of-flooding)
  - [3. Identification of Gateway Location](#3-identification-of-gateway-location)
    - [3.1. Proximity to Existing Gateway](#31-proximity-to-existing-gateway)
    - [3.2. New Gateway Deployment](#32-new-gateway-deployment)
  - [4. Identification of Mounting Infrastructure](#4-identification-of-mounting-infrastructure)
    - [4.1. Acceptable DOT Infrastructure](#41-acceptable-dot-infrastructure)
    - [4.2. Ideal Sign Post Location](#42-ideal-sign-post-location)

## 1. Community Engagement
We want to make sure that those who are most impacted by severe flooding have a say in determining where sensors will be mounted.
### 1.1. Sensor Suggestion Form
This [Google Form](https://forms.gle/4kJpujo9pDt7hZmRA) allows NYC residents to suggest locations for deployment.
### 1.2. Stakeholder Relationships
We are partnered with NY Sea Grant and the Science and Resilience Institute of Jamaica Bay; these organizations, along with the NYC Mayor’s Office of Climate Resiliency and other partners, co-founded a [Community Flood Watch Project](https://www.srijb.org/jbfloodwatch/) that has helped to inform our outreach. We are continuing to seek out partnerships with other community organizations that can inform our hotspot identification process.
### 1.3. Fair Distribution of Sensors
The Mayor’s Office of Climate and Resiliency has also provided us with a list of flood-prone neighborhoods, to help us ensure that we are distributing our sensors throughout all five boroughs.

## 2. Verification of Flooding
Once a location is proposed, we consult the [NYC Stormwater Flood Maps](https://experience.arcgis.com/experience/4b290961cac34643a49b9002f165fad8/) to confirm that this location is likely to be impacted by flooding in a Moderate Stormwater Event.

## 3. Identification of Gateway Location
Once we have a potential sensor location, we need to ensure that it is proximate to a LoRaWAN gateway.
### 3.1. Proximity to Existing Gateway
In this [Sensor Locations Planner](https://www.google.com/maps/d/edit?mid=1njszfj9XP9E2616GYRWTTzQ7gLllMuxZ&usp=sharing), we have mapped out existing locations of gateways that we have installed, or that are visible on this [TTN Mapper](https://ttnmapper.org/heatmap/).
### 3.2. New Gateway Deployment
If our potential sensor location is not close enough to one of these gateways, it will slow down the process a bit. In order to mount, we will have to seek out a location where we can mount a gateway. Typically, we can identify these locations by building relationships with organizations in the area. Our [Google Form](https://forms.gle/4kJpujo9pDt7hZmRA) provides an opportunity for community members to identify potential gateway sites. We mount gateways according to this [Floodnet Gateway setup](https://github.com/floodnet-nyc/floodnet-gateway) guide.

## 4. Identification of Mounting Infrastructure
After confirming that a location is flood-prone and proximate to a gateway, we need to determine where we will mount the sensors.
### 4.1. Acceptable DOT Infrastructure
Our sensors are designed to be mounted on U-Channel sign posts. We have permission from DOT to mount sensors on these poles, provided that we notify DOT whenever we install a new sensor. We have been instructed to avoid mounting on stop signs when possible
### 4.2. Ideal Sign Post Location
We use the street view feature in Google Maps to locate sign posts in the region we have chosen, and cross-reference these locations with the [NYC Stormwater Flood Maps](https://experience.arcgis.com/experience/4b290961cac34643a49b9002f165fad8/) to verify that the specific mounting location we have chosen is flood-prone. If there are tall buildings on the south side of the street, we try to select mounting locations on the north side. This ensures that our sensors will have plenty of sun exposure. It is also important to consider the distance from the sensor to the nearest gateway. Ensure that this distance is less than 1 mile. Once these conditions are satisfied, we compile street view and map images in a Google Sheet, which our technical team can reference in the field. This sheet could include more than one potential sign post that satisfies the above conditions in each area.  
Deployed sensors on ideal sign post locations:  
![hoyt 5th](https://user-images.githubusercontent.com/105950235/175569789-9e65056a-1ca5-41c5-b0f1-3fcdd978370e.jpg) ![126th 7th](https://user-images.githubusercontent.com/105950235/175575464-7dc1b4c6-d0c3-47ff-9cde-ca8c4f0bdc40.jpg)

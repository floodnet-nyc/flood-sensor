# Build Quality
The following practices shall be followed while building the sensor to ensure a good build quality.

## :white_check_mark: Waterproofing

Wire insulation is quite resistant to water. However, electrical connection is not. Every electrical connection that is outside the sensor housing shall be water proofed.

## :white_check_mark: No Loose Wirings

##### Two or more wires in the same Screw Terminal/ Connection

A common problem faced when making electrical connections is two or more wires held under a single screw terminal. This is an electrical hazard and possibly damages the circuitry. Building this sensor doesn't require two or more wires in a same connection and must be avoided.

##### Exposed Wires

It is quite common mistake to leave the wires exposed. When a wire is exposed too much, the wire has too much exposed copper wire which potentially leads to short circuits.

## :white_check_mark: No Short-circuits

##### Faulty wire insulation

Wire isulation is what protects the wire from being exposed to other wires. When this insulation is damaged due to applying excess heat/ incorrect stripping of the wire, this can lead to short-circuits.

##### Loose Wire Connections

Connections sometimes loosen/break allowing GND(Neutral) and VCC(Positive) wires to touch causing a short-circuit.

##### Faulty wiring and *Human error*

It is pretty humane to make mistakes, however double checking before making every single connection mitigates the chances of making errors.





## :white_check_mark: Good Soldering practices

### Soldering basics

:warning:_Caution:_ Never touch the tip of the solder iron, or the iron stand!! (often referred to as a cradle)

:heavy_exclamation_mark:_Potential Hazard:_ Leaving the solder iron unattended is a potential hazard: it could burn you or even burn your desk and start a fire. Always dock the solder iron in its stand and turn off after use.

First, plug and turn on the soldering iron to warm up. While the solder is warming up wet the sponge.

Apply a small amount of solder to the tip and then clean the tip using the wet sponge. You should have a shiny clean tip with a layer of molten solder on the tip of your iron.

Solder is a "joint" and to make a good electrical connection make sure that the through holes are clean. Oxidation and dirt can decrease the quality of the joints. Use a little isopropyl alcohol with a tissue/paper towel to clean the board.

Heat the through hole by placing the tip so that it is in contact with both the metal contact and the header pin.

:warning:_Caution:_ Be careful not to apply lot of heat to the board as it can damage the board.

Carefully put the solder on the joint either on the pin or the metal through hole, and it should melt into the through hole making a connection.

A good quality connection looks like the picture below:

### Common soldering problems

The following are some common problems encountered while soldering that makes an electric connection low quality. These shall be identified and rectified through out the sensor's hardware build procedure.

#### :x: Too much solder!
Applying too much solder onto a pin causes excessive buildup that looks like a rounded ball or a raised shape.

#### :x: Not enough solder/ Insufficient wetting (Pad)
Sometimes the solder wets the leads nicely but does not form a good connection with the pad. This can be caused by failing to apply heat to the pad as well as the pin while making the connection.

#### :x: Solder balling / Solder splashes
If not careful, excess solder while soldering

#### :x: Cold joint
Cold joints are the ones where the solder did not melt completely. These joints often are lumpy and look like they are falling off. These type of joints are unreliable over time.

#### :x: Overheated joint
Applying excessive heat at the joint may lift the pad or burn the board.

#### :x: Solder bridge
Excess solder on the pins or bent leads can cause unintended connections like shown below

## :white_check_mark: Good wire connection practices

##### No wire Connections Made With Electrical Tape

Electric tape is an unreliable wire connection method and shall be avoided. Instead soldering and heatshrinking(like shown below) the joint is a much better approach for a good electrical connection.

##### Soldering and Heat-shrinking the joints

All the solder joints shall be covered with heat shrink so that there is no exposed wire. Make sure the solder joint is not too big/sharp so that it cannot damage the heat shrink. The goal of using a heat shrink is used to prevent the exposure of a solder joint. 

##### No melted wires

It is pretty common mistake to over heat the wires while joing two wires using solder. Try not to apply too much heat. Tan them individually before joining. The melted wires can short-circuit and potentially damage the circuitry.

## References

1. Wire connection problems and solutions: https://www.thespruce.com/wire-connection-problems-and-solutions-1152877
2. Good Soldering Practices: https://learn.adafruit.com/adafruit-guide-excellent-soldering?view=all
3. How to Solder: Through-hole Soldering: https://learn.sparkfun.com/tutorials/how-to-solder-through-hole-soldering/all

# TG Robot Car
This car was the first one created.\
It was calibrated to work on the lab I created it, under the circunstances offered by the lab (same and even lighting, solid lines made with tape, etc).

The mainTG.sh file was used to start the car logic as soon as the Raspberry Pi started.
The mainTG.py contains most of the car's logic. It contains the camera setup and vision process using OpenCV. It also contains the engine parameters to turn both the left and right engines.
Finally, the robotAI.py contains the fuzzy logic used to control the engine power on both sides to maneuver the car throughout the track.

I'll update this project with further and better commented details of it.

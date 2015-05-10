speedometer
============

//Random calculation stuff. This probably ties in with DEBOUNCE
To make sure that the program does not miss magnet digitalRead data, loop() has to execute within a certain amount of time. Thus loop() has a "time limit".

Assuming wheel diamater is 27 inches, circumference = diamter*pi = (27 in)(pi)(0.0254 m/in) ~= 2.15 m 
Therefore the wheel (and the whole bike) travels 2.15 m per magnet contact (2.15 m/contact).
Assume max velocity is 45 km/h (overestimate a bit to be safe). 45 km/h(1000 m/km)(1 h/3600s) = 12.5 m/s
Number of magnet contacts at max velocity = 12.5 m/s / 2.15 m/contact = 5.81 ~= 6 contacts/s 

Thus the magnet will make 6 contacts with the sensor in 1 second while travelling at max velocity. 
Therefore loop() excecution time has to be less than 1/6 ~= 0.17 s = 170 ms in order for the max velocity to be accurate.
(Note: velocities greater than 45 km/h can easily be obtainable while going downhill, and so the "time limit" for the program will be even smaller if a significant portion of the bike trip involves going downhill.)

Hypothesis: the velocity displayed will have a maximum value which is inversely proportional to loop() excecution time. 
The reason is that if the program takes a long time to run, it will "miss" some of the magnet contacts and thus calculate a velocity lower than the actual velocity. This can be verified while travelling downhill at very fast speeds while monitoring the displayed velocity (using a nearby speed radar can be a useful reference).

Based on this hypothesis, the accuracy of the spedometer may be limited to a threshold velocity.

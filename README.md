ReeferMonPipeline
===================

[ReeferMon] (http://reefermon.com) monitors the transit of sensitive cargo, 
measuring GPS location, temperature, humidity, etc. using custom sensors. 
By placing various sensors throughout the refrigerated truck (reefer),
we get a more accurate description of the environment to which produce, art, wine, etc.
is subject.

A camera is constantly recording but only keeping 60 seconds worth of audio and video at any time.
An accelerometer event, however, may indicate an accident has occurred and triggers the system
to commit the previous 60 seconds of A/V to non-volatile memory

At any time, 100,000 or more systems are sending their sensor data to the central servers,
producing 10Mbytes per second of incoming data.

# The Data Pipeline

![Alt Text](https://github.com/jsvirzi/reefer_mon_pipeline/blob/master/images/pipeline.png "Data Pipeline")

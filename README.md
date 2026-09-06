# Low Power Solar Grid
A project to make a low power solar setup with a hybrid switcher
The general idea is that most small scale solar chargers and controllers you can buy, takes a lot of base load power.
Say, if you have 300W of solar capacity, its a bit wasteful for the charge controller to be constantly eating 5-10W all day long.

So the general idea is to make a very low power setup that can work with little to no base load.

For this, there are 2 main components, the Low Power Solar Charger and the Low Power Power Switch.
The Charger part adds an extra controller before the charge controller step, which has a Arduino in a low power setup that switches power on and off the 
controller depending on how much light comes in from a light sensor outside. The switches are bistatic relay, so they effectively dont use any base load.
The controller itself uses less than a watt (i cant measure any lower).

The Power Switch part is to have a continuous supply of 12V power to the systems that use it.
It measures the voltage on the battery, and when the voltage is sufficiently high, draws power from the battery. If its too low, it instead draws power
from a 230V to 12V charger.

<img width="442" height="362" alt="lpsolargrid" src="https://github.com/user-attachments/assets/59ef5f9e-0358-480e-aae0-17fe75e6d101" />


<img width="500" alt="image" src="https://github.com/user-attachments/assets/063526ec-b3cf-402a-a35c-614e907c5780" />


Everything in the 12V range is connected by Anderson connectors to make a unified connector for everything.

Keep in mind that this is a low power setup, so dont expect to draw more than 200-300W from this setup.

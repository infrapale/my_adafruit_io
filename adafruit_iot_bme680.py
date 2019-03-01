"""
'adafruit_iot_bme680'
==================================
Testing Adafruit IO together with a BME680 sensor
values to an Adafruit IO feed.
Author(s): Tom Höglund,  original: Brent Rubell
Tutorial Link: Tutorial Link: https://learn.adafruit.com/adafruit-io-basics-temperature-and-humidity
Dependencies:
    - Adafruit IO Python Client
        (https://github.com/adafruit/io-client-python)
    - Adafruit_Python_DHT
        (https://github.com/adafruit/Adafruit_Python_DHT)
"""

# import standard python modules.
import time
import bme680

# import Adafruit IO REST client.
from Adafruit_IO import Client, Feed
from my_account import account_dict

print(account_dict['ada_io_username'])
print(account_dict['ada_io_key'])

print("""Display Temperature, Pressure, Humidity and Gas
Press Ctrl+C to exit
""")

try:
    sensor = bme680.BME680(bme680.I2C_ADDR_PRIMARY)
except IOError:
    sensor = bme680.BME680(bme680.I2C_ADDR_SECONDARY)

# Delay in-between sensor readings, in seconds.
DELAY_BETWEEN_SEND = 60
sensor.set_humidity_oversample(bme680.OS_2X)
sensor.set_pressure_oversample(bme680.OS_4X)
sensor.set_temperature_oversample(bme680.OS_8X)
sensor.set_filter(bme680.FILTER_SIZE_3)
sensor.set_gas_status(bme680.ENABLE_GAS_MEAS)

print('\n\nInitial reading:')
for name in dir(sensor.data):
    value = getattr(sensor.data, name)

    if not name.startswith('_'):
        print('{}: {}'.format(name, value))

sensor.set_gas_heater_temperature(320)
sensor.set_gas_heater_duration(150)
sensor.select_gas_heater_profile(0)

# Up to 10 heater profiles can be configured, each
# with their own temperature and duration.
# sensor.set_gas_heater_profile(200, 150, nb_profile=1)
# sensor.select_gas_heater_profile(1)



# Set to your Adafruit IO key.
# Remember, your key is a secret,
# so make sure not to publish it when you publish this code!

ADAFRUIT_IO_USERNAME = account_dict['ada_io_username']
ADAFRUIT_IO_KEY = account_dict['ada_io_key']


# Set to your Adafruit IO username.
# (go to https://accounts.adafruit.com to find your username).

# Create an instance of the REST client.
aio = Client(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)
print(aio.base_url)

# Set up Adafruit IO Feeds.
temperature_feed = aio.feeds('home-tampere.indoor-temp')
humidity_feed = aio.feeds('home-tampere.indoor-humidity')
print(temperature_feed)
while True:
    try:
        if sensor.get_sensor_data():
            output = '{0:.2f} C,{1:.2f} hPa,{2:.2f} %RH'.format(
                sensor.data.temperature,
                sensor.data.pressure,
                sensor.data.humidity)
			
            aio.send(temperature_feed.key, str(sensor.data.temperature))
            aio.send(humidity_feed.key, str(sensor.data.humidity))
 
            if sensor.data.heat_stable:
                print('{0},{1} Ohms'.format(
                    output,
                    sensor.data.gas_resistance))
            else:
                print(output)

        time.sleep(30)

    except KeyboardInterrupt:
        pass
    # Timeout to avoid flooding Adafruit IO
    time.sleep(DELAY_BETWEEN_SEND)


#IPK
#Projekt 1 - varianta 2
#Adrian Boros [xboros03@stud.fit.vutbr.cz]
#Datum: 17.03.2019

import socket
import json
import sys

server = 'api.openweathermap.org'
port = 80
# Nacitanie parametrov
api_key = sys.argv[1]
city = str.lower(sys.argv[2])
if (sys.argv[1] == '' or sys.argv[2] == ''):
    print('ERROR: Invalid number of arguments', file=sys.stderr)
    sys.exit(0)
# Vytvorenie requestu
request = 'GET /data/2.5/weather?q={}&APPID={}&units=metric HTTP/1.1\r\nHost: {}\r\n\r\n'.format(city,api_key,server)
request_bytes = str.encode(request)

# Vytvorenie socketu
try:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
except socket.error as err:
    print ('ERROR: Socket creation failed with error %s'%(err), file=sys.stderr)
    sys.exit(0)

# Pripojenie sa na server
try:
    s.connect((server, port))
except socket.error as err:
    print('ERROR %s: Cannot connect to server'%(err), file=sys.stderr)
    sys.exit(0)

# Poslanie requestu a ziskanie navratoveho kodu
s.sendall(request_bytes)
received_data = s.recv(2048)
received_data = received_data.decode()
received_data = received_data.splitlines()
message = received_data[0].split(' ',2)
if (int)(message[1]) != 200:
    if (int)(message[1]) == 404:
        print("ERROR {}: City {}".format(message[1],message[2]), file=sys.stderr)
        sys.exit(0)
    elif (int)(message[1]) == 401:
        print("ERROR {}: {} - Invalid API key".format(message[1],message[2]), file=sys.stderr)
        sys.exit(0)
    else:
        print("ERROR {}: {}".format(message[1],message[2]), file=sys.stderr)
        sys.exit(0)
received_data = ' '.join(str(x) for x in received_data)
json_data = received_data[received_data.find('{'):]
result = json.loads(json_data)
s.close()

# Vypis potrebnych udajov
if (int)(result['cod']) == 200:
    print(result['name'] + ', ' + result['sys']['country'])
    print(result['weather'][0]['description'])
    print('Temp: {}°C'.format(result['main']['temp']))
    print('Humidity: {}%'.format(result['main']['humidity']))
    print('Pressure: {} hPa'.format(result['main']['pressure']))
    print('Wind speed: {0:.2f} km/h'.format(result['wind']['speed']*3.6))
    wind = result['wind']
    if 'deg' in wind:
        print('Wind deg: {}'.format(result['wind']['deg']))
    else:
        print('Wind deg: n/a')

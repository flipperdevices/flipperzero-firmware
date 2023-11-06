import serial

# Especifica los puertos seriales y velocidades de baudios
port_path_serial = '/dev/tty.usbmodemflip_Acereadn1'  # Reemplaza con el puerto de tu Arduino para Serial
baud_rate_serial = 115200  # Velocidad de baudios para Serial

# Abre la conexión serial para Serial
ser = serial.Serial(port=port_path_serial, baudrate=baud_rate_serial, timeout=None)



print("Connected to Serial and Serial1.")

# Comandos que deseas enviar a los puertos seriales
comando_serial =  "gbcartridge -i\n"
# comando_serial1 = "Comando para Serial1\n"

# Codifica las cadenas a bytes y envíalas a los puertos seriales
ser.write(comando_serial.encode())
# ser1.write(comando_serial1.encode())

name = "rom_dump.gb"
print(ser.read_until(b'}'))

# b'JSON:{"type":"info","gb_type":"gbc","gb_sgb":true,"logo":[206,237,102,102,204,13,0,11,3,115,0,131,0,12,0,13,0,8,17,31,136,137,0,14,220,204,110,230,221,221,217,153,187,187,103,99,110,14,236,204,221,220,153,159,187,185,51,62],"logo_correct":1,"success":1,"message":"","checksum":"04C7","title":"POKEMON YEL","ramBanks":4,"ramEndAddress":49151,"romBanks":64,"romEndAddress":32767,"serial":"APSS","rev":0,"mapper":"MBC5","ROMSize":"1 MB","RAMSize":"32 KB"}'

# with open(name, 'wb') as f:
#     comando_serial =  "gbcartridge -d -o\n"
#     ser.write(comando_serial.encode())
#     ser.read()
#     ser.read()
#     while True:
#         f.write(ser.read())
with open(name, 'wb') as f:
    comando_serial = "gbcartridge -d -o\n"
    ser.write(comando_serial.encode())
    ser.read()
    ser.read()

    data_to_read = 64 * (16 * 1024)  # Cantidad deseada de datos a leer
    bytes_read = 0  # Inicializamos la cantidad de bytes leídos

    while bytes_read < data_to_read:
        data = ser.read(min(data_to_read - bytes_read, 512))  # Leer hasta 4096 bytes a la vez o menos si se alcanza el objetivo
        f.write(data)
        bytes_read += len(data)
f.close()
    # Salir del bucle cuando se alcanza la cantidad deseada
ser.close()
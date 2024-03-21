import json
import gmplot

with open("data.json", "r") as file:
    data = json.load(file)

sorted_data = []
for device_id, positions in data.items():
    for pos in positions:
        sorted_data.append(pos)
sorted_data.sort(key=lambda x: x["decrypted_payload"]["timestamp"])

latitudes = [pos["decrypted_payload"]["lat"] for pos in sorted_data]
longitudes = [pos["decrypted_payload"]["lon"] for pos in sorted_data]

if latitudes and longitudes:
    gmap = gmplot.GoogleMapPlotter(latitudes[0], longitudes[0], 13)
    gmap.plot(latitudes, longitudes, color='red', edge_width=2.5)
    for lat, lon in zip(latitudes, longitudes):
        gmap.marker(lat, lon, color='red')

    gmap.draw("map.html")

    print("Map generated! Open 'map.html' in a web browser to view.")
else:
    print("No data available to plot.")

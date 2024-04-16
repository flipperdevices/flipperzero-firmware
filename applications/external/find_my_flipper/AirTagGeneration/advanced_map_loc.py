import json
import pandas as pd
import folium
from folium.plugins import AntPath
from datetime import datetime
import os


def format_time(seconds):
    hours = seconds // 3600
    minutes = (seconds % 3600) // 60
    seconds = seconds % 60
    return f"{int(hours)}h {int(minutes)}m {int(seconds)}s"


with open("data.json", "r") as file:
    data = json.load(file)


sorted_data = sorted(data, key=lambda x: x["timestamp"])

df = pd.DataFrame(sorted_data)

df["datetime"] = pd.to_datetime(df["isodatetime"])
df["time_diff"] = df["datetime"].diff().dt.total_seconds()
average_time_diff = df["time_diff"][1:].mean()
time_diff_total = (df.iloc[-1]["datetime"] - df.iloc[0]["datetime"]).total_seconds()

formatted_total_time = format_time(time_diff_total)
formatted_avg_time = format_time(average_time_diff)

start_timestamp = df.iloc[0]["datetime"].strftime("%Y-%m-%d %H:%M:%S")
simple_start_timestamp = df.iloc[0]["datetime"].strftime("%m-%d-%y")
end_timestamp = df.iloc[-1]["datetime"].strftime("%Y-%m-%d %H:%M:%S")

ping_count = df.shape[0]

# sanity check before plotting
if not df.empty:
    map_center = [df.iloc[0]["lat"], df.iloc[0]["lon"]]
    m = folium.Map(location=map_center, zoom_start=13)

    latlon_pairs = list(zip(df["lat"], df["lon"]))
    ant_path = AntPath(
        locations=latlon_pairs,
        dash_array=[10, 20],
        delay=1000,
        color="red",
        weight=5,
        pulse_color="black",
    )
    m.add_child(ant_path)

    # Location markers look good, click to see timestamp
    for index, row in df.iterrows():
        if index == 0:  # First marker
            folium.Marker(
                [row["lat"], row["lon"]],
                popup=f"Timestamp: {row['isodatetime']} Start Point",
                tooltip=f"Start Point",
                icon=folium.Icon(color="green"),
            ).add_to(m)
        elif index == len(df) - 1:  # Last marker
            folium.Marker(
                [row["lat"], row["lon"]],
                popup=f"Timestamp: {row['isodatetime']} End Point",
                tooltip=f"End Point",
                icon=folium.Icon(color="red"),
            ).add_to(m)
        else:  # Other markers
            folium.Marker(
                [row["lat"], row["lon"]],
                popup=f"Timestamp: {row['isodatetime']}",
                tooltip=f"Point {index+1}",
            ).add_to(m)

    title_and_info_html = f"""
     <h3 align="center" style="font-size:20px; margin-top:10px;"><b>FindMy Flipper Location Mapper</b></h3>
     <div style="position: fixed; bottom: 50px; left: 50px; width: 300px; height: 160px; z-index:9999; font-size:14px; background-color: white; padding: 10px; border-radius: 10px; box-shadow: 0 0 5px rgba(0,0,0,0.5);">
     <b>Location Summary</b><br>
     Start: {start_timestamp}<br>
     End: {end_timestamp}<br>
     Number of Location Pings: {ping_count}<br>
     Total Time: {formatted_total_time}<br>
     Average Time Between Pings: {formatted_avg_time}<br>
     Created by Matthew KuKanich and luu176<br>
     </div>
     """
    m.get_root().html.add_child(folium.Element(title_and_info_html))

    base_filename = f"LocationMap_{simple_start_timestamp}"
    extension = "html"
    counter = 1
    filename = f"{base_filename}.{extension}"
    while os.path.exists(filename):
        filename = f"{base_filename}_{counter}.{extension}"
        counter += 1

    m.save(filename)
    print(f"map info generated! Open '{filename}' in a web browser to view.")

else:
    print("No data available to plot.")

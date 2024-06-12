

import sys
import requests
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout
from PyQt5.QtWebEngineWidgets import QWebEngineView
from PyQt5.QtCore import QTimer
import folium

# Google Apps Script URL
GOOGLE_SHEET_SCRIPT_URL = 'https://script.google.com/macros/s/AKfycbyunycYBoQpZpjpDp0JS0VZpkxUYK5ILhEJki4f5dd9RRdWEWlbn8VUQ8dqB4giur2vHA/exec'

class MapWidget(QWidget):
    def __init__(self, parent=None):
        super(MapWidget, self).__init__(parent)

        # Create a QWebEngineView object to display the map
        self.webview = QWebEngineView()

        # Create a layout and add the webview to it
        layout = QVBoxLayout(self)
        layout.addWidget(self.webview)

        # Initialize the map
        self.update_map()

        # Set a timer to update the map every 5 seconds
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.update_map)
        self.timer.start(5000)

    def update_map(self):
        # Fetch the latest latitude and longitude from Google Sheets
        response = requests.get(GOOGLE_SHEET_SCRIPT_URL)
        if response.status_code == 200:
            data = response.json()
            if data:
                latest_location = data[-1]
                self.latitude = latest_location['latitude']
                self.longitude = latest_location['longitude']

                # Create a Folium map object centered around the new coordinates
                self.m = folium.Map(location=[self.latitude, self.longitude], zoom_start=18)  # Increase zoom level to 18

                # Add a marker to the map
                folium.Marker(location=[self.latitude, self.longitude], popup="Your Location").add_to(self.m)

                # Set the HTML content of the webview to the updated map
                self.webview.setHtml(self.m.get_root().render())
            else:
                print("No data received")
        else:
            print(f"Failed to fetch data: {response.status_code}")

if __name__ == '__main__':
    app = QApplication(sys.argv)
    widget = MapWidget()
    widget.show()
    sys.exit(app.exec_())

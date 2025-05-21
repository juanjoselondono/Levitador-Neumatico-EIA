import pandas as pd 
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

CSV_FILENAME = 'serial_data.csv'
plt.style.use('fivethirtyeight')

# Create figure and axes BEFORE animation
fig, ax = plt.subplots()

def animate(i):
        data = pd.read_csv(CSV_FILENAME)
        if data.empty:
            return  # Skip if CSV has no data yet

        x = data['Timestamp']
        rpm = data['RPM']
        distance = data['Distance_cm']

        ax.clear()
        ax.plot(x, distance, label='Distance (cm)', color='blue', linewidth = 0.2)
        #ax.plot(x, rpm, label='RPM', color='red')
        ax.set_xlabel('Time')
        ax.set_ylabel('Values')
        ax.legend(loc='upper left')
        plt.tight_layout()


ani = FuncAnimation(fig, animate, interval=500)
plt.show()

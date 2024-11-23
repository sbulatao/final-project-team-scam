from fastapi import APIRouter
from models import SensorData
from firebase.firebaseClient import get_db_reference

import io
from fastapi.responses import StreamingResponse
import matplotlib.pyplot as plt
from datetime import datetime


router = APIRouter()

# Create test data
data = {
    "entry1": {
        "timestamp": "2024-11-22 10:30:00",
        "value": 7.5
    },
    "entry2": {
        "timestamp": "2024-11-22 11:00:00",
        "value": 6.8
    },
    "entry3": {
        "timestamp": "2024-11-22 11:30:00",
        "value": 7.2
    }
}

# Push test data only if "pHSensors" node is empty
ref = get_db_reference("pHSensors")
if not ref.get():
    ref.set(data)
    print("Test data added to Firebase.")


@router.post("/", summary="Add pH Sensor Data", tags=["pH Sensor"])
async def add_sensor_data(data: SensorData):
    """
    Adds pH sensor data to Firebase.
    """
    ref = get_db_reference("pHSensors")
    ref.push(data.dict())
    return {"message": "pH Sensor Data added successfully"}


@router.get("/", summary="Get pH Sensor Data", tags=["pH Sensor"])
async def get_sensor_data():
    """
    Retrieves all pH sensor data from Firebase.
    """
    ref = get_db_reference("pHSensors")
    return ref.get()


@router.get("/plot", tags=["pH Sensor"])
async def plot_ph_data():
    """
    Generate and return a plot of pH values over time.
    """
    # Fetch data from Firebase
    ref = get_db_reference("pHSensors")
    data = ref.get()  # Assuming data is a dictionary {key: {timestamp, value}}

    if not data:  # Handle missing or empty data
        return {"error": "No data available under 'pHSensors' in Firebase."}

    # Extract timestamps and values
    timestamps = []
    values = []
    for key, entry in data.items():
        timestamps.append(datetime.strptime(entry['timestamp'], "%Y-%m-%d %H:%M:%S"))  # Convert string to datetime
        values.append(entry['value'])

    # Sort data by timestamps
    timestamps, values = zip(*sorted(zip(timestamps, values)))

    # Create the plot
    plt.figure(figsize=(10, 6))
    plt.plot(timestamps, values, marker='o', linestyle='-')
    plt.title("pH Levels Over Time")
    plt.xlabel("Time (Month/Day/Year HH:MM:SS)")
    plt.ylabel("pH Levels")
    plt.xticks(rotation=45)
    plt.grid()

    # Save the plot to a BytesIO object
    buf = io.BytesIO()
    plt.savefig(buf, format="png")
    buf.seek(0)
    plt.close()

    return StreamingResponse(buf, media_type="image/png")

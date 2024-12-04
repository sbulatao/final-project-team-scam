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
        "value": 6.2
    },
    "entry2": {
        "timestamp": "2024-11-22 10:35:00",
        "value": 6.4
    },
    "entry3": {
        "timestamp": "2024-11-22 10:40:00",
        "value": 6.5
    },
    "entry4": {
        "timestamp": "2024-11-22 10:45:00",
        "value": 6.7
    },
    "entry5": {
        "timestamp": "2024-11-22 10:50:00",
        "value": 7
    },
    "entry6": {
        "timestamp": "2024-11-22 10:55:00",
        "value": 7.7
    },
    "entry7": {
        "timestamp": "2024-11-22 11:00:00",
        "value": 7.3
    },
    "entry8": {
        "timestamp": "2024-11-22 11:05:00",
        "value": 6.7
    },
    "entry9": {
        "timestamp": "2024-11-22 11:10:00",
        "value": 6.6
    },
        "entry10": {
        "timestamp": "2024-11-22 11:15:00",
        "value": 6.5
    },
    "entry11": {
        "timestamp": "2024-11-22 11:20:00",
        "value": 6.4
    },
    "entry12": {
        "timestamp": "2024-11-22 11:25:00",
        "value": 6.5
    },
    "entry13": {
        "timestamp": "2024-11-22 11:30:00",
        "value": 6.7
    },
    "entry14": {
        "timestamp": "2024-11-22 11:35:00",
        "value": 7
    },
    "entry15": {
        "timestamp": "2024-11-22 11:40:00",
        "value": 7.1
    },
    "entry16": {
        "timestamp": "2024-11-22 11:45:00",
        "value": 7
    },
    "entry17": {
        "timestamp": "2024-11-22 11:50:00",
        "value": 6.9
    },
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
    ref = get_db_reference("pHSensors")
    data = ref.get()
    print(f"Retrieved data: {data}")  # Debugging line
    if not data:
        return {"message": "No data found under 'pHSensors' in Firebase."}
    return data
# @router.get("/", summary="Get pH Sensor Data", tags=["pH Sensor"])
# async def get_sensor_data():
#     """
#     Retrieves all pH sensor data from Firebase.
#     """
#     ref = get_db_reference("pHSensors")
#     data = ref.get()
#     if not data:
#         return {"message": "No data found under 'pHSensors' in Firebase."}
#     return data


@router.get("/plot", tags=["pH Sensor"])
async def plot_ph_data():
    """
    Generate and return a plot of pH values over time.
    """
    ref = get_db_reference("pHSensors")
    data = ref.get()  # Fetch data from Firebase

    if not data:
        # return {"error": "No data available under 'pHSensors' in Firebase."}
        print("No data found in Firebase. Using test data")
        data = {
            "entry1": {"timestamp": "2024-11-22 10:30:00", "value": 6.2},
            "entry2": {"timestamp": "2024-11-22 10:35:00", "value": 6.4},
            "entry3": {"timestamp": "2024-11-22 10:40:00", "value": 6.5},
            "entry4": {"timestamp": "2024-11-22 10:45:00", "value": 6.7},
            "entry5": {"timestamp": "2024-11-22 10:50:00", "value": 7},
            "entry6": {"timestamp": "2024-11-22 10:55:00", "value": 7.7},
            "entry7": {"timestamp": "2024-11-22 11:00:00", "value": 7.3},
            "entry8": {"timestamp": "2024-11-22 11:05:00", "value": 6.7},
            "entry9": {"timestamp": "2024-11-22 11:10:00", "value": 6.6},
            "entry10": {"timestamp": "2024-11-22 11:15:00", "value": 6.5},
        }

    # Validate data structure and extract entries
    try:
        entries = [
            (key, value)
            for key, value in data.items()
            if isinstance(value, dict) and "timestamp" in value and "pH" in value
        ]

        # Sort entries by timestamp
        entries = sorted(
            entries, key=lambda x: datetime.strptime(x[1]["timestamp"], "%Y-%m-%d %H:%M:%S")
        )
    except Exception as e:
        return {"error": f"Data parsing error: {e}"}

    if not entries:
        return {"error": "No valid entries found in 'pHSensors'."}

    # Extract timestamps and pH values
    timestamps = [datetime.strptime(entry[1]["timestamp"], "%Y-%m-%d %H:%M:%S") for entry in entries]
    values = [entry[1]["pH"] for entry in entries]

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
from pydantic import BaseModel

class SensorData(BaseModel):
    sensor_id: str
    pH_value: float
    timestamp: str

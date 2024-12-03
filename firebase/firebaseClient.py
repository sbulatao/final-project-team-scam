import os
from firebase_admin import credentials, initialize_app, db

# print("Current working directory:", os.getcwd())

# Initialize Firebase Admin SDK
cred = credentials.Certificate("serviceAccountKey")
firebase_app = initialize_app(cred, {
    "databaseURL": "https://ece-196scamproject-default-rtdb.firebaseio.com/"
})

def get_db_reference(ref_path: str):
    """
    Utility to get a Firebase DB reference.
    """
    return db.reference(ref_path)

// Import the functions you need from the SDKs you need
import { initializeApp } from "firebase/app";
import { getAnalytics } from "firebase/analytics";
import { getDatabase, ref, set, push, onValue } from "firebase/database";

// Your web app's Firebase configuration
const firebaseConfig = {
  apiKey: "AIzaSyCiGlf_kzK1KlWMj4m4A3mAozRCmw8gUHs",
  authDomain: "ece-196scamproject.firebaseapp.com",
  databaseURL: "https://ece-196scamproject-default-rtdb.firebaseio.com",
  projectId: "ece-196scamproject",
  storageBucket: "ece-196scamproject.firebasestorage.app",
  messagingSenderId: "846518697241",
  appId: "1:846518697241:web:d9d4768efd306d8a871988",
  measurementId: "G-YGRGRSB1QX"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const analytics = getAnalytics(app);
const database = getDatabase(app); // Initialize Realtime Database

// Example: Write Data to Firebase Realtime Database
function writeData(pHValue, timestamp) {
  const dataRef = ref(database, 'pHSensors/');

  onValue(dataRef, (snapshot) => {
    const data = snapshot.val();
    const keys = Object.keys(data || {});

    // Remove oldest entry if more than 20
    if (keys.length >= 20) {
      const oldestKey = keys.sort()[0]; // Assuming keys are sortable (e.g., entry1, entry2, ...)
      const oldestEntryRef = ref(database, `pHSensors/${oldestKey}`);
      set(oldestEntryRef, null); // Remove the oldest entry
    }

    // Add the new entry
    const newEntryRef = push(dataRef);
    set(newEntryRef, {
      pH: pHValue,
      timestamp: timestamp,
    })
      .then(() => {
        console.log("Data written successfully!");
      })
      .catch((error) => {
        console.error("Error writing data:", error);
      });
  });
}


// Example: Read Data from Firebase Realtime Database
function readData() {
  const dataRef = ref(database, 'pHSensors/');
  onValue(dataRef, (snapshot) => {
    const data = snapshot.val();
    console.log("Data from Firebase:", data);
  });
}

// Example Usage
writeData(7.2, new Date().toISOString()); // Write a sample pH value and timestamp
readData(); // Read all data under "pHSensors"

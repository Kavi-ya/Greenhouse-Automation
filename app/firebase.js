// firebase.js

import { initializeApp } from "firebase/app";
import { getDatabase } from "firebase/database";

const firebaseConfig = {
  apiKey: "####################################",
  authDomain: "esp32-test1-95778.firebaseapp.com",
  projectId: "esp32-test1-95778",
  storageBucket: "esp32-test1-95778.appspot.com",
  messagingSenderId: "689408592145",
  appId: "1:689408592145:web:36aef8eca8e78aedad24b0",
  databaseURL: "https://esp32-test1-95778-default-rtdb.asia-southeast1.firebasedatabase.app/", // Add this if using Realtime Database
};

const app = initializeApp(firebaseConfig);
const database = getDatabase(app);

export { database };

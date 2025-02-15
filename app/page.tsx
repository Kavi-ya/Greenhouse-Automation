'use client';

import React, { useState, useEffect } from 'react';
import { Button } from "./button";
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "./card";
import { Label } from "./label";
import { ref, onValue, set } from "firebase/database";
import { database } from "./firebase"; // Adjust the path if necessary

interface SensorData {
  temperature: number;
  humidity: number;
  lightCondition: string;
  airQuality: number;
  soilMoisture: number;
  airPressure: number;
}

interface SensorOutputs {
  fanStatus: boolean;
  waterPumpStatus: boolean;
  mode: 'AUTO' | 'MANUAL'; // Add mode here
}

const App = () => {
  const [sensorData, setSensorData] = useState<SensorData>({
    temperature: 30,
    humidity: 50,
    lightCondition: "Dark",
    airQuality: 70,
    soilMoisture: 40,
    airPressure: 1015,
  });

  const [sensorOutputs, setSensorOutputs] = useState<SensorOutputs>({
    fanStatus: false,
    waterPumpStatus: false,
    mode: 'AUTO',
  });

  const [mode, setMode] = useState<'AUTO' | 'MANUAL'>('AUTO');
  const [date, setDate] = useState(new Date());
  const [animationClass, setAnimationClass] = useState<string[]>([]);

  useEffect(() => {
    const sensorDataRef = ref(database, 'sensorData');

    const unsubscribe = onValue(sensorDataRef, (snapshot) => {
      if (snapshot.exists()) {
        const data = snapshot.val();
        const prevData = sensorData;

        setSensorData(data);

        const newAnimationClass = [
          getAnimationClass(data.temperature, prevData.temperature),
          getAnimationClass(data.humidity, prevData.humidity),
          getAnimationClass(data.lightCondition, prevData.lightCondition),
          getAnimationClass(data.airQuality, prevData.airQuality),
          getAnimationClass(data.soilMoisture, prevData.soilMoisture),
          getAnimationClass(data.airPressure, prevData.airPressure),
        ];

        setAnimationClass(newAnimationClass);

        setTimeout(() => {
          setAnimationClass(['', '', '', '', '', '']);
        }, 1000);
      }
    });

    return () => {
      unsubscribe();
    };
  }, [sensorData]);

  useEffect(() => {
    const sensorOutputsRef = ref(database, 'sensorOutputs');
    
    const unsubscribe = onValue(sensorOutputsRef, (snapshot) => {
      if (snapshot.exists()) {
        const data = snapshot.val();
        setSensorOutputs(data);
      }
    });

    return () => {
      unsubscribe();
    };
  }, []);

  useEffect(() => {
    const modeRef = ref(database, 'mode');
    const unsubscribe = onValue(modeRef, (snapshot) => {
      if (snapshot.exists()) {
        const data = snapshot.val();
        setMode(data);
      }
    });

    return () => {
      unsubscribe();
    };
  }, []);

  useEffect(() => {
    const intervalId = setInterval(() => {
      setDate(new Date());
    }, 1000);

    return () => clearInterval(intervalId);
  }, []);

  const handleModeToggle = () => {
    const newMode = mode === 'AUTO' ? 'MANUAL' : 'AUTO';
    
    // Update local mode first
    setMode(newMode);
  
    // Prepare the updated outputs based on the new mode
    const updatedOutputs: SensorOutputs = {
      ...sensorOutputs,
      mode: newMode,
      fanStatus: newMode === 'AUTO' ? false : sensorOutputs.fanStatus, // Maintain fan status unless switching to AUTO
      waterPumpStatus: newMode === 'AUTO' ? false : sensorOutputs.waterPumpStatus, // Maintain water pump status unless switching to AUTO
    };
  
    // Update Firebase
    set(ref(database, 'sensorOutputs'), updatedOutputs);
    set(ref(database, 'mode'), newMode);
    
    // Update sensorOutputs state
    setSensorOutputs(updatedOutputs);
  };
  
  

  const handleFanToggle = () => {
    if (mode === 'MANUAL') {
      const newFanStatus = !sensorOutputs.fanStatus;
      const updatedOutputs = { ...sensorOutputs, fanStatus: newFanStatus };
      set(ref(database, 'sensorOutputs'), updatedOutputs);
    }
  };

  const handleWaterPumpToggle = () => {
    if (mode === 'MANUAL') {
      const newWaterPumpStatus = !sensorOutputs.waterPumpStatus;
      const updatedOutputs = { ...sensorOutputs, waterPumpStatus: newWaterPumpStatus };
      set(ref(database, 'sensorOutputs'), updatedOutputs);
    }
  };

  const getAnimationClass = (value: number | string, prevValue: number | string) => {
    if (typeof value === 'number' && typeof prevValue === 'number') {
      if (value > prevValue) {
        return 'increasing';
      } else if (value < prevValue) {
        return 'decreasing';
      }
    } else if (typeof value === 'string' && typeof prevValue === 'string') {
      return ''; // No animation class for strings
    }
    return '';
  };

  const renderSensorValue = (label: string, value: any, unit: string, animationClass: string, color: string, maxValue: number = 100) => {
    let displayValue = 0;
    let formattedValue = 'N/A';

    if (typeof value === 'number') {
      formattedValue = value.toFixed(2);
      displayValue = (value / maxValue) * 100; // Normalize to a percentage
    } else if (typeof value === 'string') {
      formattedValue = value;
      displayValue = value === "Light" ? 100 : 0; // Handle Light Condition
    }

    return (
      <Card>
        <CardHeader>
          <CardTitle>{label}</CardTitle>
          <CardDescription className="mb-2">
            <span className="text-2xl font-bold">{formattedValue}{unit}</span>
          </CardDescription>
        </CardHeader>
        <CardContent>
          <div className={`value-container p-2 rounded-lg shadow-md ${animationClass}`}>
            <div className="w-full h-4 bg-gray-200 rounded-full mb-2 gradient-fade">
              <div className={`h-4 ${color} rounded-full`} style={{ width: `${displayValue}%` }}></div>
            </div>
          </div>
        </CardContent>
      </Card>
    );
  };

  return (
    <div className="max-w-7xl mx-auto p-4 sm:p-6 md:p-8">
      <div className="text-center mt-4">
        <Label>Today's Date and Time </Label>
        <p className="text-3xl">{date.toLocaleString()}</p>
      </div>
      <div className="grid grid-cols-1 gap-4 sm:grid-cols-2 md:grid-cols-3 mt-4">
        {renderSensorValue('Temperature', sensorData.temperature, '°C', animationClass[0], 'bg-red-500')}
        {renderSensorValue('Humidity', sensorData.humidity, '%', animationClass[1], 'bg-blue-500')}
        {renderSensorValue('Light Condition', sensorData.lightCondition, '', animationClass[2], 'bg-yellow-500')}
        {renderSensorValue('Air Quality', sensorData.airQuality, '% PPM', animationClass[3], 'bg-green-500')}
        {renderSensorValue('Soil Moisture', sensorData.soilMoisture, '%', animationClass[4], 'bg-purple-500')}
        {renderSensorValue('Air Pressure', sensorData.airPressure, 'hPa', animationClass[5], 'bg-indigo-500', 1050)}
      </div>
      <div className="flex justify-center mt-4">
        <Button
          className={`${
            sensorOutputs.mode === 'AUTO' ? 'bg-yellow-500 hover:bg-yellow-700' : 'bg-gray-500 hover:bg-yellow-700'
          } text-white font-bold py-2 px-4 rounded ml-4`}
          onClick={handleModeToggle}
        >
          {sensorOutputs.mode === 'AUTO' ? 'Switch to Manual Mode' : 'Switch to Auto Mode'}
        </Button>
        <Button
          className={`${
            sensorOutputs.fanStatus ? 'bg-green-500 hover:bg-green-700' : 'bg-gray-500 hover:bg-green-700'
          } text-white font-bold py-2 px-4 rounded ml-4`}
          onClick={handleFanToggle}
        >
          {sensorOutputs.fanStatus ? 'Fan On' : 'Fan Off'}
        </Button>
        <Button
          className={`${
            sensorOutputs.waterPumpStatus ? 'bg-blue-500 hover:bg-blue-700' : 'bg-gray-500 hover:bg-blue-700'
          } text-white font-bold py-2 px-4 rounded ml-4`}
          onClick={handleWaterPumpToggle}
        >
          {sensorOutputs.waterPumpStatus ? 'Water Pump On' : 'Water Pump Off'}
        </Button>
      </div>
      {/* Footer */}
      <footer
        className="text-center p-4"
        style={{
          background: 'linear-gradient(90deg, #bdbdbd, #757575)',
          color: 'white',
          display: 'flex',
          justifyContent: 'center',
          alignItems: 'center',
          textAlign: 'center',
          position: 'fixed',
          left: 0,
          bottom: 0,
          width: '100%',
        }}
      >
        <p>
          Copyright &copy; 2024
          <a
            href="https://github.com/Kavi-ya"
            target="_blank"
            rel="noopener noreferrer"
            style={{ color: '#ffffff', textDecoration: 'none' }}
          > Kavindu Sahan
          </a>
        </p>
      </footer>
    </div>
  );
};

export default App;

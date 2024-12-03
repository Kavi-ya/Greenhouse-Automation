"use client";

import React, { useState, useEffect } from "react";
import { Button } from "./button";
import {
  Card,
  CardContent,
  CardDescription,
  CardHeader,
  CardTitle,
} from "./card";
import { Label } from "./label";
import { ref, onValue, set } from "firebase/database";
import { database } from "./firebase"; // Adjust the path if necessary
import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
} from "recharts";
import { FaDownload } from "react-icons/fa";

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
  mode: "AUTO" | "MANUAL"; // Add mode here
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
    mode: "AUTO",
  });

  const [mode, setMode] = useState<"AUTO" | "MANUAL">("AUTO");
  const [date, setDate] = useState(new Date());
  const [animationClass, setAnimationClass] = useState<string[]>([]);
  const [showChart, setShowChart] = useState(false);
  const [chartData, setChartData] = useState<any[]>([]); // Storing chart data
  const [pointCount, setPointCount] = useState(0); // Keeps track of number of data points
  const [isFetchingData, setIsFetchingData] = useState(false); // Track if new data is being fetched
  const [prevSensorData, setPrevSensorData] = useState<SensorData>(sensorData); // Track previous data
  const [timestamp, setTimestamp] = useState(null); // State to store clicked timestamp

  const [isPaused, setIsPaused] = useState(false); // New state for pausing chart updates
  const [chartStats, setChartStats] = useState({
    avgTemperature: 0,
    maxTemperature: -Infinity,
    minTemperature: Infinity,
    avgHumidity: 0,
    maxHumidity: -Infinity,
    minHumidity: Infinity,
  });

  // Zoom settings
  const maxZoom = 5;
  const minZoom = 1;
  const pointsPerZoom = 10;
  const [zoomLevel, setZoomLevel] = useState(1);
  const [startIndex, setStartIndex] = useState(0);

  useEffect(() => {
    const sensorDataRef = ref(database, "sensorData");
    const unsubscribe = onValue(sensorDataRef, (snapshot) => {
      if (snapshot.exists() && !isPaused) {
        const data = snapshot.val();

        const currentTime = new Date().toLocaleTimeString();
        setChartData((prevData) => {
          const newData = [...prevData];
          const newPointIndex = pointCount + 1;

          if (newData.length >= 50) newData.shift(); // Keep only the latest 50 points
          newData.push({
            name: `Point ${newPointIndex}`,
            temperature: data.temperature,
            humidity: data.humidity,
            time: currentTime,
          });
          setPointCount(newPointIndex);
          return newData;
        });
      }
    });

    return () => unsubscribe();
  }, [isPaused]); // Depend on `isPaused`

  useEffect(() => {
    const sensorDataRef = ref(database, "sensorData");
    const unsubscribe = onValue(sensorDataRef, (snapshot) => {
      if (snapshot.exists()) {
        setIsFetchingData(true); // Data is being fetched

        const data = snapshot.val();

        // Only update if data is different from previous
        if (JSON.stringify(data) !== JSON.stringify(prevSensorData)) {
          setPrevSensorData(data); // Update previous data with the fetched data
          setSensorData(data);

          const newAnimationClass = [
            getAnimationClass(data.temperature, prevSensorData.temperature),
            getAnimationClass(data.humidity, prevSensorData.humidity),
            getAnimationClass(
              data.lightCondition,
              prevSensorData.lightCondition
            ),
            getAnimationClass(data.airQuality, prevSensorData.airQuality),
            getAnimationClass(data.soilMoisture, prevSensorData.soilMoisture),
            getAnimationClass(data.airPressure, prevSensorData.airPressure),
          ];

          setAnimationClass(newAnimationClass);

          // Update chart data with new point
          const currentTime = new Date().toLocaleTimeString();
          setChartData((prevData) => {
            const newData = [...prevData];
            if (newData.length >= 50) newData.shift(); // Keep only the latest 50 points
            newData.push({
              name: `Point ${prevData.length + 1}`,
              temperature: data.temperature,
              humidity: data.humidity,
              time: currentTime,
            });
            setPointCount(prevData.length + 1);
            return newData;
          });

          if (JSON.stringify(data) !== JSON.stringify(prevSensorData)) {
            setChartStats((prevStats) => {
              const newAvgTemp =
                (prevStats.avgTemperature * chartData.length + data.temperature) /
                (chartData.length + 1);
              const newAvgHumidity =
                (prevStats.avgHumidity * chartData.length + data.humidity) /
                (chartData.length + 1);
              return {
                avgTemperature: newAvgTemp,
                maxTemperature: Math.max(
                  prevStats.maxTemperature,
                  data.temperature
                ),
                minTemperature: Math.min(
                  prevStats.minTemperature,
                  data.temperature
                ),
                avgHumidity: newAvgHumidity,
                maxHumidity: Math.max(prevStats.maxHumidity, data.humidity),
                minHumidity: Math.min(prevStats.minHumidity, data.humidity),
              };
            });
          }
          

          setTimeout(() => {
            setAnimationClass(["", "", "", "", "", ""]);
            setIsFetchingData(false); // Data fetching completed
          }, 1000);
        }
      }
    });

    return () => unsubscribe();
  }, [sensorData, prevSensorData]); // Depend on prevSensorData to avoid unnecessary updates

  useEffect(() => {
    const sensorOutputsRef = ref(database, "sensorOutputs");
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
    const modeRef = ref(database, "mode");
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

  // Update current date and time every second
  useEffect(() => {
    const intervalId = setInterval(() => {
      setDate(new Date());
    }, 1000);

    return () => clearInterval(intervalId);
  }, [showChart, isPaused, pointCount]);

  // Handle Mode toggling
  const handleModeToggle = () => {
    const newMode = mode === "AUTO" ? "MANUAL" : "AUTO";

    setMode(newMode);

    const updatedOutputs: SensorOutputs = {
      ...sensorOutputs,
      mode: newMode,
      fanStatus: newMode === "AUTO" ? false : sensorOutputs.fanStatus,
      waterPumpStatus:
        newMode === "AUTO" ? false : sensorOutputs.waterPumpStatus,
    };

    set(ref(database, "sensorOutputs"), updatedOutputs);
    set(ref(database, "mode"), newMode);
    setSensorOutputs(updatedOutputs);
  };

  // Handle Pause button click
  const handlePauseToggle = () => {
    setIsPaused(!isPaused);
  };

  // Handle Fan toggling in Manual mode
  const handleFanToggle = () => {
    if (mode === "MANUAL") {
      const newFanStatus = !sensorOutputs.fanStatus;
      const updatedOutputs = { ...sensorOutputs, fanStatus: newFanStatus };
      set(ref(database, "sensorOutputs"), updatedOutputs);
    }
  };

  // Handle Water Pump toggling in Manual mode
  const handleWaterPumpToggle = () => {
    if (mode === "MANUAL") {
      const newWaterPumpStatus = !sensorOutputs.waterPumpStatus;
      const updatedOutputs = {
        ...sensorOutputs,
        waterPumpStatus: newWaterPumpStatus,
      };
      set(ref(database, "sensorOutputs"), updatedOutputs);
    }
  };

  const lastUpdate = chartData.length
    ? chartData[chartData.length - 1].time
    : "N/A";

  // Zoom functionality
  const handleZoomIn = () => {
    if (zoomLevel < maxZoom) {
      setZoomLevel(zoomLevel + 1);
      setStartIndex(Math.max(0, startIndex - pointsPerZoom / 2));
    }
  };

  const handlePointClick = (data: {
    payload: { time: React.SetStateAction<null> };
  }) => {
    if (data) {
      setTimestamp(data.payload.time); // Store the clicked timestamp
    }
  };

  const visibleData = chartData.slice(
    startIndex,
    startIndex + pointsPerZoom * zoomLevel
  );

  const handleZoomOut = () => {
    if (zoomLevel > minZoom) {
      setZoomLevel(zoomLevel - 1);
      setStartIndex(
        Math.min(
          startIndex + pointsPerZoom / 2,
          chartData.length - pointsPerZoom
        )
      );
    }
  };

  // Handle download of chart data
  const handleDownloadClick = () => {
    const dataToDownload = JSON.stringify(chartData, null, 2);
    const blob = new Blob([dataToDownload], { type: "application/json" });
    const url = URL.createObjectURL(blob);
    const a = document.createElement("a");
    a.href = url;
    a.download = "chart_data.json";
    a.click();
    URL.revokeObjectURL(url);
  };

  // Get animation class for value change
  const getAnimationClass = (
    value: number | string,
    prevValue: number | string
  ) => {
    if (typeof value === "number" && typeof prevValue === "number") {
      if (value > prevValue) {
        return "increasing";
      } else if (value < prevValue) {
        return "decreasing";
      }
    } else if (typeof value === "string" && typeof prevValue === "string") {
      return ""; // No animation class for strings
    }
    return "";
  };

  const renderDot = (props: { cx: any; cy: any; value: any; dataKey: any; }) => {
    const { cx, cy, value, dataKey } = props;
    const color = dataKey === "temperature" && value > 40 ? "red" : "#8884d8";
    return <circle cx={cx} cy={cy} r={5} fill={color} stroke={color} />;
  };
  const formatTooltip = (value: number, name: string, props: { payload: { temperature: number; humidity: number; time: any; }; }) => {
    const color =
      name === "temperature" ? (value > 40 ? "red" : "#8884d8") : "#82ca9d";
    const temperatureValue = props.payload.temperature.toFixed(2);
    const humidityValue = props.payload.humidity.toFixed(2);
    const timeValue = props.payload.time;

    return [
      <span style={{ color }}>{`${
        name === "temperature" ? temperatureValue : humidityValue
      } ${name === "temperature" ? "°C" : "%"}`}</span>,
      `Time: ${timeValue}`, // This will only show once now
    ];
  };

  // Render sensor values in cards
  const renderSensorValue = (
    label: string,
    value: any,
    unit: string,
    animationClass: string,
    color: string,
    maxValue: number = 100
  ) => {
    let displayValue = 0;
    let formattedValue = "N/A";

    if (typeof value === "number") {
      formattedValue = value.toFixed(2);
      displayValue = (value / maxValue) * 100; // Normalize to a percentage
    } else if (typeof value === "string") {
      formattedValue = value;
      displayValue = value === "Light" ? 100 : 0; // Handle Light Condition
    }

    return (
      <Card>
        <CardHeader>
          <CardTitle>{label}</CardTitle>
          <CardDescription className="mb-2">
            <span className="text-2xl font-bold">
              {formattedValue}
              {unit}
            </span>
          </CardDescription>
        </CardHeader>
        <CardContent>
          <div
            className={`value-container p-2 rounded-lg shadow-md ${animationClass}`}
          >
            <div className="w-full h-4 bg-gray-200 rounded-full mb-2 gradient-fade">
              <div
                className={`h-4 ${color} rounded-full`}
                style={{ width: `${displayValue}%` }}
              ></div>
            </div>
          </div>
        </CardContent>
      </Card>
    );
  };

  return (
    <div className="max-w-7xl mx-auto p-4 sm:p-6 md:p-8">
      <div className="text-center mt-4">
        <Label>Today's Date and Time</Label>
        <p className="text-3xl">{new Date().toLocaleString()}</p>
      </div>

      <div className="grid grid-cols-1 gap-4 sm:grid-cols-2 md:grid-cols-3 mt-4">
        {renderSensorValue(
          "Temperature",
          sensorData.temperature,
          "°C",
          animationClass[0],
          "bg-red-500"
        )}
        {renderSensorValue(
          "Humidity",
          sensorData.humidity,
          "%",
          animationClass[1],
          "bg-blue-500"
        )}
        {renderSensorValue(
          "Light Condition",
          sensorData.lightCondition,
          "",
          animationClass[2],
          "bg-yellow-500"
        )}
        {renderSensorValue(
          "Air Quality",
          sensorData.airQuality,
          "% PPM",
          animationClass[3],
          "bg-green-500"
        )}
        {renderSensorValue(
          "Soil Moisture",
          sensorData.soilMoisture,
          "%",
          animationClass[4],
          "bg-purple-500"
        )}
        {renderSensorValue(
          "Air Pressure",
          sensorData.airPressure,
          "hPa",
          animationClass[5],
          "bg-indigo-500",
          1050
        )}
      </div>

      <div className="flex justify-center mt-4">
        <Button
          className={`${
            sensorOutputs.mode === "AUTO"
              ? "bg-yellow-500 hover:bg-yellow-700"
              : "bg-gray-500 hover:bg-yellow-700"
          } text-white font-bold py-2 px-4 rounded ml-4`}
          onClick={handleModeToggle}
        >
          {sensorOutputs.mode === "AUTO"
            ? "Switch to Manual Mode"
            : "Switch to Auto Mode"}
        </Button>
        <Button
          className={`${
            sensorOutputs.fanStatus
              ? "bg-green-500 hover:bg-green-700"
              : "bg-gray-500 hover:bg-green-700"
          } text-white font-bold py-2 px-4 rounded ml-4`}
          onClick={handleFanToggle}
        >
          {sensorOutputs.fanStatus ? "Fan On" : "Fan Off"}
        </Button>

        <Button
          className={`${
            sensorOutputs.waterPumpStatus
              ? "bg-blue-500 hover:bg-blue-700"
              : "bg-gray-500 hover:bg-blue-700"
          } text-white font-bold py-2 px-4 rounded ml-4`}
          onClick={handleWaterPumpToggle}
        >
          {sensorOutputs.waterPumpStatus ? "Water Pump On" : "Water Pump Off"}
        </Button>

        <Button
          className="bg-blue-500 text-white font-bold py-4 px-4 rounded ml-4"
          onClick={() => setShowChart(!showChart)}
        >
          {showChart ? "Hide Chart" : "Show Chart"}
        </Button>

        <Button
          className="bg-gray-600 text-white font-bold py-2 px-4 rounded ml-4"
          onClick={handleDownloadClick}
        >
          <FaDownload /> Download Data
        </Button>
      </div>

      {showChart && (
        <div className="flex justify-center mt-4">
          <Button
            className={`text-white font-bold py-4 px-4 rounded ml-4 ${
              isPaused ? "bg-red-500" : "bg-cyan-500"
            }`}
            onClick={handlePauseToggle}
          >
            {isPaused ? "Resume" : "Pause"}
          </Button>

          <Button
            className="bg-gray-500 text-white font-bold py-4 px-4 rounded ml-4"
            onClick={handleZoomIn}
            disabled={zoomLevel >= maxZoom}
          >
            Zoom Out
          </Button>
          <Button
            className="bg-gray-500 text-white font-bold py-4 px-4 rounded ml-4"
            onClick={handleZoomOut}
            disabled={zoomLevel <= minZoom}
          >
            Zoom In
          </Button>
        </div>
      )}

      {showChart && (
        <div className="flex">
          {/* Chart Section with Horizontal Scroll */}
          <div
            className="overflow-x-auto"
            style={{ maxWidth: "100%", width: "800px" }} // Set the width for the chart container
          >
            {showChart && (
              <LineChart
                width={1100} // Make the chart wider than the container to enable horizontal scrolling
                height={300}
                data={visibleData}
                margin={{ top: 20, right: 30, left: 20, bottom: 5 }}
                // onClick={handlePointClick} // Handle click event
              >
                <CartesianGrid strokeDasharray="3 3" />
                <XAxis dataKey="name" />
                <YAxis domain={[0, 100]} />
                <Tooltip formatter={formatTooltip} />
                <Line
                  type="monotone"
                  dataKey="temperature"
                  stroke="#8884d8"
                  dot={renderDot}
                 //activeDot={{ r: 8 }}
                 strokeWidth={2}
                  isAnimationActive={false}
                />
                <Line
                  type="monotone"
                  dataKey="humidity"
                  stroke="#82ca9d"
                  dot={renderDot}
                  //activeDot={{ r: 8 }}
                  strokeWidth={2}
                  isAnimationActive={false}
                />
              </LineChart>
            )}
          </div>

          {/* Chart Statistics Section */}
          <div
            className="flex flex-col items-start ml-6 p-4 border rounded-lg shadow-md bg-white"
            style={{ height: "235px", width: "280px" }}
          >
            <h3 className="text-lg font-bold">Chart Statistics</h3>
            <p>
              Average Temperature:{" "}
              <span
                className={`font-bold ${
                  chartStats.avgTemperature > 40 ? "text-red-600" : ""
                }`}
              >
                {chartStats.avgTemperature.toFixed(2)} °C
              </span>
            </p>
            <p>
              Max Temperature:{" "}
              <span
                className={`font-bold ${
                  chartStats.maxTemperature > 40 ? "text-red-600" : ""
                }`}
              >
                {chartStats.maxTemperature.toFixed(2)} °C
              </span>
            </p>
            <p>
              Min Temperature:{" "}
              <span className="font-bold">
                {chartStats.minTemperature.toFixed(2)} °C
              </span>
            </p>
            <p>
              Average Humidity:{" "}
              <span className="font-bold">
                {chartStats.avgHumidity.toFixed(2)} %
              </span>
            </p>
            <p>
              Max Humidity:{" "}
              <span className="font-bold">
                {chartStats.maxHumidity.toFixed(2)} %
              </span>
            </p>
            <p>
              Min Humidity:{" "}
              <span className="font-bold">
                {chartStats.minHumidity.toFixed(2)} %
              </span>
            </p>
            {timestamp && <p>Clicked Time: {timestamp}</p>}{" "}
            {/* Show timestamp only once */}
            <p>Last Updated: {lastUpdate}</p>
          </div>
        </div>
      )}

      <div className="w-full h-4 bg-gray-200 rounded-full mb-2 gradient-fade"></div>

      {/* Footer */}
      <footer
        className="text-center p-4"
        style={{
          background: "linear-gradient(90deg, #bdbdbd, #757575)",
          color: "white",
          display: "flex",
          justifyContent: "center",
          alignItems: "center",
          textAlign: "center",
          position: "fixed",
          left: 0,
          bottom: 0,
          width: "100%",
        }}
      >
        <p>
          Copyright &copy; 2024
          <a
            href="https://github.com/Kavi-ya"
            target="_blank"
            rel="noopener noreferrer"
            style={{ color: "#ffffff", textDecoration: "none" }}
          >
            {" "}
            Kavindu Sahan
          </a>
        </p>
      </footer>
    </div>
  );
};

export default App;

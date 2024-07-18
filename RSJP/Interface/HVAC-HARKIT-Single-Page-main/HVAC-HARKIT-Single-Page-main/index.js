// JAVASCRIPT INDEX PAGE
function updateDateTime() {
  const now = new Date();
  const hour = String(now.getHours()).padStart(2, "0");
  const minute = String(now.getMinutes()).padStart(2, "0");
  const second = String(now.getSeconds()).padStart(2, "0");

  document.getElementById(
    "currentDateTime"
  ).innerHTML = `<b>${now.toLocaleDateString("en-US", {
    weekday: "long",
    year: "numeric",
    month: "long",
    day: "numeric",
  })}</b>`;

  document.getElementById(
    "currentTime"
  ).innerHTML = `<b>${hour}:${minute}:${second}</b>`;
}

// Call updateDateTime initially and then every second
updateDateTime();
setInterval(updateDateTime, 1000);

// document.addEventListener("DOMContentLoaded", function () {
//   setTimeout(function () {
//     const dashboard = document.getElementById("dashboard");
//     dashboard.classList.add("active");
//   }, 2);
// });

let config = {};
let passwordhvac = {};
let UnitID = {};
// Function to fetch configuration
function getConfig() {
  // Replace 'config.json' with the path to your configuration file
  const response = fetch("../config.json")
    .then((response) => response.json())
    .then((data) => {
      console.log(data.serverIP);
      config = data.serverIP;
      passwordhvac = data.password;
      // unitID = data.unitID;
      document.getElementById("OutdoorUnitz").textContent = data.unitID;
      console.log(data.unitID);
    })
    .catch((error) => {
      console.error("Error fetching configuration:", error);
      return {}; // Return an empty object if there's an error
    });

  return response;
}

// console.log(config);
// [[[[[[[[[FUNCTION TO GET DATA]]]]]]]]]]]
let GV_automanual;

function updateInfoValue() {
  fetch("http://" + config + ":1880/getInfoData")
    .then((response) => response.json())
    .then((data) => {
      const modeValue = data[0].Mode;
      document.getElementById("modeValue").textContent = modeValue;
      const turboValue = data[0].Turbo;
      document.getElementById("turboValue").textContent = turboValue;
      const swingValue = data[0].Swing;
      document.getElementById("swingValue").textContent = swingValue;
      const quietValue = data[0].Quiet;
      document.getElementById("quietValue").textContent = quietValue;
      const sleepValue = data[0].Sleep;
      document.getElementById("sleepValue").textContent = sleepValue;
      const timeron1Value = data[0].TimerON1;
      document.getElementById("timeron1Value").textContent = timeron1Value;
      const timeron2Value = data[0].TimerON2;
      document.getElementById("timeron2Value").textContent = timeron2Value;
      const timeroff1Value = data[0].TimerOFF1;
      document.getElementById("timeroff1Value").textContent = timeroff1Value;
      const timeroff2Value = data[0].TimerOFF2;
      document.getElementById("timeroff2Value").textContent = timeroff2Value;
      const delayValue = data[0].DelayTimer;
      document.getElementById("delayValue").textContent = delayValue;
      const fanValue = data[0].Fan;
      document.getElementById("fanValue").textContent = fanValue;
      const unitstatValue = data[0].UnitStatus;
      document.getElementById("unitstatValue").textContent = unitstatValue;
      if (unitstatValue === "ON") {
        document.getElementById("btn_ON").style.backgroundColor = "#76e810";
        document.getElementById("btn_OFF").style.backgroundColor = "gray";
      } else {
        document.getElementById("btn_ON").style.backgroundColor = "gray";
        document.getElementById("btn_OFF").style.backgroundColor = "orangered";
      }

      const automanual = data[0].AutoManual;
      GV_automanual = data[0].AutoManual;
      console.log("Test Global " + GV_automanual);
      if (automanual === 1) {
        document.getElementById("bgstatAM").style.backgroundColor =
          "rgb(77, 12, 182)";
        document.getElementById("statusAM").textContent = "AUTO";
        updateStatusAM("Auto");
      } else {
        document.getElementById("bgstatAM").style.backgroundColor = "orangered";
        document.getElementById("statusAM").textContent = "MANUAL";
        // document.getElementById("statusAM").style.fontSize = "2vh";
        updateStatusAM("Manual");
      }
    });
}
updateInfoValue();
getConfig();
setInterval(getConfig, 1000);
setInterval(updateInfoValue, 5000); // Refresh every 5 seconds (adjust as needed)

// INFORMATION

let tempcathlab1;

// Fetch data from Node-RED server and update JustGage value

function updateElementStatus(
  element,
  icon,
  isRunning,
  runningColor,
  stoppedColor
) {
  element.textContent = isRunning ? "ON" : "OFF";
  element.style.color = isRunning ? runningColor : stoppedColor;
  icon.style.color = isRunning ? "" : stoppedColor;
  isRunning
    ? icon.classList.add("bx-flashing", "bx-spin")
    : icon.classList.remove("bx-flashing", "bx-spin");
}

function updateGaugeValue() {
  fetch("http://" + config + ":1880/getData")
    .then((response) => response.json())
    .then((data) => {
      const {
        Temp_Cathlab: tempCathlab,
        Temp_Machine: tempMachine,
        RH_Cathlab: rhCathlab,
        RH_Machine: rhMachine,
        Pre_Filter: preFilter,
        HEPA_Filter: hepaFilter,
        Outdoor_Unit: outdoorUnit,
        AHU_Status: ahuStatus,
        Booster_Fan: bFan,
        Exhaust_Fan: eFan,
        Heater_1: heat1,
        Heater_2: heat2,
      } = data[0];

      console.log(data[0]); // Log the data object to inspect its structure

      // Update gauge values
      chartraw.refresh(tempCathlab);
      chartraw1.refresh(tempMachine);
      chartraww.refresh(rhCathlab);
      chartraww2.refresh(rhMachine);
      chartfm1.refresh(preFilter);
      chartfm3.refresh(hepaFilter);

      // Update pre-filter status
      const preFilterElement = document.getElementById("prefil");
      preFilterElement.style.background = preFilter >= 180 ? "orangered" : "";
      preFilterElement.textContent = preFilter >= 180 ? "Warning" : "Good";
      preFilterElement.style.color = preFilter >= 180 ? "yellow" : "darkgreen";

      // Update HEPA filter status
      const hepaFilterElement = document.getElementById("hepafil");
      hepaFilterElement.style.backgroundColor = hepaFilter >= 180 ? "blue" : "";
      hepaFilterElement.textContent = hepaFilter >= 180 ? "Warning" : "Good";
      hepaFilterElement.style.color = hepaFilter >= 180 ? "blue" : "darkgreen";

      // Update temperature and humidity display
      document.getElementById("tempCat").textContent = tempCathlab + " °C";
      tempcathlab1 = tempCathlab;
      document.getElementById("tempMac").textContent = tempMachine + " °C";
      document.getElementById("rhCath").textContent = rhCathlab + " %";
      document.getElementById("rhMac").textContent = rhMachine + " %";

      // Update statuses
      const statusElements = [
        {
          element: document.getElementById("OutdoorUnit"),
          icon: document.getElementById("outdooricon"),
          status: outdoorUnit,
        },
        {
          element: document.getElementById("AHU"),
          icon: document.getElementById("ahuicon"),
          status: ahuStatus,
        },
        {
          element: document.getElementById("Booster"),
          icon: document.getElementById("boostericon"),
          status: bFan,
        },
        {
          element: document.getElementById("Exhaust"),
          icon: document.getElementById("exhausticon"),
          status: eFan,
        },
        {
          element: document.getElementById("Heat1"),
          icon: document.getElementById("heat1icon"),
          status: heat1,
        },
        {
          element: document.getElementById("Heat2"),
          icon: document.getElementById("heat2icon"),
          status: heat2,
        },
      ];

      statusElements.forEach(({ element, icon, status }) => {
        updateElementStatus(
          element,
          icon,
          parseFloat(status) === 1,
          "#0085cb",
          "red"
        );
      });

      removeNotification();
    })
    .catch((error) => {
      console.error("Error:", error);
      // Show notification for connection error
      showNotification("Connecting...");
    });
}

// Initial call to update values
updateGaugeValue();
updateValue();
// Function to show notification
function showNotification(message) {
  let notificationElement = document.querySelector(".notification");

  // If notification already exists, update its content
  if (notificationElement) {
    notificationElement.textContent = message;
  } else {
    // Create new notification div
    notificationElement = document.createElement("div");
    notificationElement.textContent = message;
    notificationElement.classList.add("notification");

    // Create refresh button
    const refreshButton = document.createElement("button");
    refreshButton.textContent = "Refresh";
    refreshButton.classList.add("refresh-button");

    // Add click event listener to refresh button
    refreshButton.addEventListener("click", function () {
      // Refresh the page
      window.location.reload();
    });

    // Append refresh button to notification div
    notificationElement.appendChild(refreshButton);

    // Append notification div to body
    document.body.appendChild(notificationElement);
  }
}

// Function to remove notification
function removeNotification() {
  const notificationElement = document.querySelector(".notification");
  if (notificationElement) {
    notificationElement.remove();
  }
}

// Call the function to update the gauge value initially and periodically
updateGaugeValue();
setInterval(updateGaugeValue, 5000);
setInterval(updateValue, 5000); // Refresh every 5 seconds (adjust as needed)

// [[[[[[[[[[END OF FUNCTION GET DATA]]]]]]]]]]

let chartraw = new JustGage({
  id: "gauRaw",
  value: 0,
  valueFontColor: "#003b7d",
  min: 0,
  max: 50,
  hideMinMax: true,
  title: "Temperature",
  titleFontColor: "#003b7d",
  label: "°C",
  labelFontColor: "#003b7d",
  levelColors: [
    "#00B6D4", // Blue (low)
    "#00FF50", // Green (medium)
    "#FF0000", // Red (high)
  ],
  gaugeWidthScale: 0.6, // Set to 1 for a full circle
  gaugeColor: "rgba(115, 255, 202, 1)", // Adjust the alpha value for transparency
  counter: true,
  pointer: true,
  pointerOptions: {
    toplength: 0,
    bottomlength: 10,
    bottomwidth: 0,
    color: "#ffffff",
    stroke: "#64748b",
    stroke_width: 3,
    stroke_linecap: "round",
  },
  donut: true,
  donutStartAngle: -30,
  shadowSize: 2,
  shadowVerticalOffset: 7,
  relativeGaugeSize: true,
  showInnerShadow: true,
  shadowOpacity: 0.5,
  customSectors: [
    {
      color: "#2b59fb", // Light blue
      lo: 0,
      hi: 20,
    },
    {
      color: "#40bb45", // Lime green
      lo: 20,
      hi: 23,
    },
    {
      color: "#FFFF00", // Yellow
      lo: 23,
      hi: 26,
    },
    {
      color: "#FF0000", // Red
      lo: 27,
      hi: 40,
    },
  ],
  levelColorsGradient: true,
});

let chartraww = new JustGage({
  id: "gauRaww",
  value: 0,
  valueFontColor: "#003b7d",
  min: 0,
  max: 100,
  hideMinMax: true,
  title: "Relative Humidity",
  titleFontColor: "#003b7d",
  label: "%",
  labelFontColor: "#003b7d",
  levelColors: [
    "#00B6D4", // Blue (low)
    "#00FF50", // Green (medium)
    "#FF0000", // Red (high)
  ],
  gaugeWidthScale: 0.6, // Set to 1 for a full circle
  gaugeColor: "rgba(115, 255, 202, 1)", // Adjust the alpha value for transparency
  counter: true,
  pointer: true,
  pointerOptions: {
    toplength: 0,
    bottomlength: 10,
    bottomwidth: 0,
    color: "#ffffff",
    stroke: "#64748b",
    stroke_width: 3,
    stroke_linecap: "round",
  },
  donut: true,
  donutStartAngle: -30,
  shadowSize: 2,
  shadowVerticalOffset: 7,
  relativeGaugeSize: true,
  showInnerShadow: true,
  shadowOpacity: 0.5,
  customSectors: [
    {
      color: "#2b59fb", // Light blue
      lo: 50,
      hi: 60,
    },
    {
      color: "#bf8c11", // Lime green
      lo: 60,
      hi: 65,
    },
    {
      color: "#FFFF00", // Yellow
      lo: 65,
      hi: 70,
    },
    {
      color: "#FF0000", // Red
      lo: 71,
      hi: 85,
    },
  ],
  levelColorsGradient: true,
});

let chartraw1 = new JustGage({
  id: "gauRaw1",
  value: 0,
  valueFontColor: "#003b7d",
  min: 0,
  max: 50,
  hideMinMax: true,
  title: "Temperature",
  titleFontColor: "#003b7d",
  label: "°C",
  labelFontColor: "#003b7d",
  levelColors: [
    "#00B6D4", // Blue (low)
    "#00FF50", // Green (medium)
    "#FF0000", // Red (high)
  ],
  gaugeWidthScale: 0.6, // Set to 1 for a full circle
  gaugeColor: "rgba(115, 255, 202, 1)", // Adjust the alpha value for transparency
  counter: true,
  pointer: true,
  pointerOptions: {
    toplength: 0,
    bottomlength: 10,
    bottomwidth: 0,
    color: "#ffffff",
    stroke: "#64748b",
    stroke_width: 3,
    stroke_linecap: "round",
  },
  donut: true,
  donutStartAngle: -30,
  shadowSize: 2,
  shadowVerticalOffset: 7,
  relativeGaugeSize: true,
  showInnerShadow: true,
  shadowOpacity: 0.5,
  customSectors: [
    {
      color: "#2b59fb", // Light blue
      lo: 0,
      hi: 20,
    },
    {
      color: "#40bb45", // Lime green
      lo: 20,
      hi: 23,
    },
    {
      color: "#FFFF00", // Yellow
      lo: 23,
      hi: 26,
    },
    {
      color: "#FF0000", // Red
      lo: 27,
      hi: 40,
    },
  ],
  levelColorsGradient: true,
});

let chartraww2 = new JustGage({
  id: "gauRaww2",
  value: 0,
  valueFontColor: "#003b7d",
  min: 0,
  max: 100,
  hideMinMax: true,
  title: "Relative Humidity",
  titleFontColor: "#003b7d",
  label: "%",
  labelFontColor: "#003b7d",
  levelColors: [
    "#00B6D4", // Blue (low)
    "#00FF50", // Green (medium)
    "#FF0000", // Red (high)
  ],
  gaugeWidthScale: 0.6, // Set to 1 for a full circle
  gaugeColor: "rgba(115, 255, 202, 1)", // Adjust the alpha value for transparency
  counter: true,
  pointer: true,
  pointerOptions: {
    toplength: 0,
    bottomlength: 10,
    bottomwidth: 0,
    color: "#ffffff",
    stroke: "#64748b",
    stroke_width: 3,
    stroke_linecap: "round",
  },
  donut: true,
  donutStartAngle: -30,
  shadowSize: 2,
  shadowVerticalOffset: 7,
  relativeGaugeSize: true,
  showInnerShadow: true,
  shadowOpacity: 0.5,
  customSectors: [
    {
      color: "#2b59fb", // Light blue
      lo: 50,
      hi: 60,
    },
    {
      color: "#bf8c11", // Lime green
      lo: 60,
      hi: 65,
    },
    {
      color: "#FFFF00", // Yellow
      lo: 65,
      hi: 70,
    },
    {
      color: "#FF0000", // Red
      lo: 71,
      hi: 85,
    },
  ],
  levelColorsGradient: true,
});

let chartfm1 = new JustGage({
  id: "gaufm1",
  value: 0,
  valueFontColor: "#003b7d",
  min: 0,
  max: 500,
  hideMinMax: true,
  title: "Pre & Medium Filter",
  titleFontColor: "#003b7d",
  label: "Pa",
  labelFontColor: "#003b7d",
  levelColors: [
    "#00B6D4", // Blue (low)
    "#00FF50", // Green (medium)
    "#FF0000", // Red (high)
  ],
  gaugeWidthScale: 0.6, // Set to 1 for a full circle
  gaugeColor: "rgba(115, 255, 202, 1)", // Adjust the alpha value for transparency
  counter: true,
  pointer: true,
  pointerOptions: {
    toplength: 0,
    bottomlength: 10,
    bottomwidth: 0,
    color: "#ffffff",
    stroke: "#64748b",
    stroke_width: 3,
    stroke_linecap: "round",
  },
  // donut: true,
  // donutStartAngle: -90,
  shadowSize: 2,
  shadowVerticalOffset: 7,
  // relativeGaugeSize: true,
  showInnerShadow: true,
  shadowOpacity: 0.5,
  customSectors: [
    {
      color: "#00B6D4", // Light blue
      lo: 0,
      hi: 100,
    },
    {
      color: "#00FF50", // Lime green
      lo: 100,
      hi: 180,
    },
    {
      color: "#FFFF00", // Yellow
      lo: 180,
      hi: 250,
    },
    {
      color: "#FF0000", // Red
      lo: 250,
      hi: 500,
    },
  ],
  levelColorsGradient: true,
});

let chartfm2 = new JustGage({
  id: "gaufm2",
  value: 0,
  valueFontColor: "#003b7d",
  min: 0,
  max: 500,
  hideMinMax: true,
  titleFontColor: "#64748b",
  label: "Pa",
  levelColors: [
    "#00B6D4", // Blue (low)
    "#00FF50", // Green (medium)
    "#FF0000", // Red (high)
  ],
  gaugeWidthScale: 0.6, // Set to 1 for a full circle
  gaugeColor: "rgba(115, 255, 202, 1)", // Adjust the alpha value for transparency
  counter: true,
  pointer: true,
  pointerOptions: {
    toplength: 0,
    bottomlength: 10,
    bottomwidth: 0,
    color: "#ffffff",
    stroke: "#64748b",
    stroke_width: 3,
    stroke_linecap: "round",
  },
  // donut: true,
  // donutStartAngle: -90,
  shadowSize: 2,
  shadowVerticalOffset: 7,
  // relativeGaugeSize: true,
  showInnerShadow: true,
  shadowOpacity: 0.5,
  customSectors: [
    {
      color: "#00B6D4", // Light blue
      lo: 0,
      hi: 100,
    },
    {
      color: "#00FF50", // Lime green
      lo: 100,
      hi: 180,
    },
    {
      color: "#FFFF00", // Yellow
      lo: 180,
      hi: 250,
    },
    {
      color: "#FF0000", // Red
      lo: 250,
      hi: 500,
    },
  ],
  levelColorsGradient: true,
});

let chartfm3 = new JustGage({
  id: "gaufm3",
  value: 0,
  valueFontColor: "#003b7d",
  min: 0,
  max: 500,
  hideMinMax: true,
  title: "HEPA Filter",
  titleFontColor: "#003b7d",
  label: "Pa",
  labelFontColor: "#003b7d", // Set the color of the label
  labelFontSize: 148, // Set the font size of the label
  levelColors: [
    "#00B6D4", // Blue (low)
    "#00FF50", // Green (medium)
    "#FF0000", // Red (high)
  ],
  gaugeWidthScale: 0.6, // Set to 1 for a full circle
  gaugeColor: "rgba(115, 255, 202, 1)", // Adjust the alpha value for transparency
  counter: true,
  pointer: true,
  pointerOptions: {
    toplength: 0,
    bottomlength: 10,
    bottomwidth: 0,
    color: "#ffffff",
    stroke: "#64748b",
    stroke_width: 3,
    stroke_linecap: "round",
  },
  // donut: true,
  // donutStartAngle: -90,
  shadowSize: 2,
  shadowVerticalOffset: 7,
  // relativeGaugeSize: true,
  showInnerShadow: true,
  shadowOpacity: 0.5,
  customSectors: [
    {
      color: "#00B6D4", // Light blue
      lo: 0,
      hi: 100,
    },
    {
      color: "#00FF50", // Lime green
      lo: 100,
      hi: 180,
    },
    {
      color: "#FFFF00", // Yellow
      lo: 180,
      hi: 250,
    },
    {
      color: "#FF0000", // Red
      lo: 250,
      hi: 500,
    },
  ],
  levelColorsGradient: true,
});

// Get the modal
const modal = document.getElementById("myModal");
// Get the elements inside the modal
const modalTitle = document.getElementById("modalTitle");
const modalContent = document.getElementById("modalContent");
const passwordInput = document.getElementById("passwordInput");
const modalActionButton = document.getElementById("modalActionButton");
let modalstatus = "";
// Function to open the modal
function openModal(data) {
  modal.style.display = "block";
  console.log(data);
  modalstatus = data;
}
function openModalinfo() {
  var modal = document.getElementById("infoModal");
  modal.style.display = "block";
}

// Function to close the modal
function closeModal() {
  modal.style.display = "none";
}

function closeModalinfo() {
  var modal = document.getElementById("infoModal");
  modal.style.display = "none";
}

// Function to handle modal action (password verification or close)
function verifyPassword() {
  const passwordInput = document.getElementById("passwordInput").value;
  // Replace 'your-password' with the actual password
  if (passwordInput === passwordhvac) {
    alert(
      "Password is correct. Proceed with " + modalstatus + " functionality."
    );
    closeModal(); // Close the modal if password is correct
    document.getElementById("passwordInput").value = ""; // Reset password input

    if (modalstatus === "SET") {
      showbtn();
      // Skip the rest of the code
    } else if (modalstatus === "DONE") {
      hidebtn();
      // Skip the rest of the code
    } else {
      let stat = 0;
      if (modalstatus === "ON") {
        stat = 1;
      } else {
        stat = 0;
      }

      if (GV_automanual === 0) {
        console.error(
          "Manual Mode Still Active, Switch to AUTO on the Panel to Gain Remote Access"
        );
        alert(
          "Manual Mode Still Active, Switch to AUTO on the Panel to Gain Remote Access"
        );
      } else {
        fetch(`http://${config}:1880/setUnitStat?value=${stat}`)
          .then((response) => {
            if (!response.ok) {
              throw new Error("Network response was not ok");
            }
            return response.json();
          })
          .then((data) => {
            console.log("updated successfully:", data);
          })
          .catch((error) => {
            console.error("There was a problem updating:", error);
          });
      }
    }
  } else {
    alert("Incorrect password. Please try again.");
    document.getElementById("passwordInput").value = ""; // Reset password input
    closeModal(); // Close the modal if password is correct
  }
}

// JAVASCRIPT FOR CATHLAB PAGE

function updateElementStatus1(element, isRunning) {
  element.style.backgroundColor = isRunning
    ? element.style.backgroundColor === "lime"
      ? "#CCFF99"
      : "lime"
    : "red";
  element.style.color = "white";
  element.textContent = isRunning ? "Running" : "Stopped";
}

function updateValue() {
  fetch("http://" + config + ":1880/getData")
    .then((response) => response.json())
    .then((data) => {
      const {
        Outdoor_Unit: outdoorUnit,
        AHU_Status: ahuStatus,
        Booster_Fan: bFan,
        Exhaust_Fan: eFan,
        Heater_1: heat1,
        Heater_2: heat2,
        Temp_Cathlab: tempCathlab,
        RH_Cathlab: rhCathlab,
        Temp_Machine: tempMachine,
        RH_Machine: rhMachine,
      } = data[0];

      console.log("from updatevalue");

      const elements = {
        // CATHLAB
        ahuStatus: document.getElementById("AHU_status_"),
        exhaustStatus: document.getElementById("Exhaust_status_"),
        outdoorStatus: document.getElementById("Outdoor_status_"),
        heat1Status: document.getElementById("Heater1_status_"),
        heat2Status: document.getElementById("Heater2_status_"),
        boosterStatus: document.getElementById("Booster_status_"),
        // MACHINE
        ahuStatusM: document.getElementById("AHU_status_M"),
        exhaustStatusM: document.getElementById("Exhaust_status_M"),
        outdoorStatusM: document.getElementById("Outdoor_status_M"),
        heat1StatusM: document.getElementById("Heater1_status_M"),
        heat2StatusM: document.getElementById("Heater2_status_M"),
        boosterStatusM: document.getElementById("Booster_status_M"),
        // STATUS
        tempElement: document.getElementById("TBDSAa"), // CATHLAB
        rhElement: document.getElementById("TBDSBb"), // CATHLAB
        tempElementM: document.getElementById("TBDSAaM"), // MACHINE
        rhElementM: document.getElementById("TBDSBbM"), // MACHINE
      };

      //
      //CATHLAB
      updateElementStatus1(elements.ahuStatus, parseFloat(ahuStatus) === 1);
      updateElementStatus1(elements.exhaustStatus, parseFloat(eFan) === 1);
      updateElementStatus1(
        elements.outdoorStatus,
        parseFloat(outdoorUnit) === 1
      );
      updateElementStatus1(elements.heat1Status, parseFloat(heat1) === 1);
      updateElementStatus1(elements.heat2Status, parseFloat(heat2) === 1);
      updateElementStatus1(elements.boosterStatus, parseFloat(bFan) === 1);

      //MACHINE
      updateElementStatus1(elements.ahuStatusM, parseFloat(ahuStatus) === 1);
      updateElementStatus1(elements.exhaustStatusM, parseFloat(eFan) === 1);
      updateElementStatus1(
        elements.outdoorStatusM,
        parseFloat(outdoorUnit) === 1
      );
      updateElementStatus1(elements.heat1StatusM, parseFloat(heat1) === 1);
      updateElementStatus1(elements.heat2StatusM, parseFloat(heat2) === 1);
      updateElementStatus1(elements.boosterStatusM, parseFloat(bFan) === 1);

      elements.tempElement.textContent = tempCathlab + "°C";
      elements.rhElement.textContent = rhCathlab + "%";
      elements.tempElementM.textContent = tempMachine + "°C";
      elements.rhElementM.textContent = rhMachine + "%";

      removeNotification();
    })
    .catch((error) => {
      console.error("Error:", error);
      showNotification("Connecting...");
    });

  fetch("http://" + config + ":1880/getDataSetting")
    .then((response) => response.json())
    .then((data) => {
      const { TempCathlab: currentTemperature, RHCathlab: currentRH } = data[0];
      const { TempMachine: currentTemperature_M, RHMachine: currentRH_M } =
        data[0];

      const TBDSAElement = document.getElementById("TBDSA");
      const TBDSBRHElement = document.getElementById("TBDSBRH");
      const TBDSAElement_M = document.getElementById("TBDSA_M");
      const TBDSBRHElement_M = document.getElementById("TBDSBRH_M");

      TBDSAElement.textContent = currentTemperature + "°C";
      TBDSBRHElement.textContent = currentRH + "%";

      TBDSAElement_M.textContent = currentTemperature_M + "°C";
      TBDSBRHElement_M.textContent = currentRH_M + "%";
    })
    .catch((error) => {
      console.error("Error:", error);
    });
}

function changeTemperature(delta) {
  const TBDSAElement = document.getElementById("TBDSA");
  let currentTemperature = parseInt(TBDSAElement.textContent); // Get current temperature as an integer
  // currentTemperature += delta; // Increment or decrement the temperature based on the delta
  const newTemperature = currentTemperature + delta;
  // Check if the new temperature is within the acceptable range (18 to 26 degrees Celsius)
  if (newTemperature < 18 || newTemperature > 26) {
    console.log(
      "Temperature is outside the acceptable range (18 to 26 degrees Celsius). Skipping update."
    );
    return; // Exit the function without updating the temperature
  }

  TBDSAElement.textContent = newTemperature + "°C"; // Update the temperature value in the HTML

  // Send the updated temperature value to the server via HTTP GET request
  fetch(`http://` + config + `:1880/setDataTemp?value=${newTemperature}`)
    .then((response) => {
      if (!response.ok) {
        throw new Error("Network response was not ok");
      }
      return response.json();
    })
    .then((data) => {
      console.log("Temperature updated successfully:", data);
    })
    .catch((error) => {
      console.error("There was a problem updating the temperature:", error);
    });
}
function changeRH(delta) {
  const TBDSBRHElement = document.getElementById("TBDSBRH");
  let currentRH = parseInt(TBDSBRHElement.textContent); // Get current temperature as an integer
  // currentRH += delta; // Increment or decrement the temperature based on the delta
  const newRH = currentRH + delta;
  // Check if the new temperature is within the acceptable range (18 to 26 degrees Celsius)
  if (newRH < 50 || newRH > 60) {
    console.log(
      "RH is outside the acceptable range (50 to 60). Skipping update."
    );
    return; // Exit the function without updating the temperature
  }

  TBDSBRHElement.textContent = newRH + "%"; // Update the temperature value in the HTML

  // Send the updated temperature value to the server via HTTP GET request
  fetch(`http://` + config + `:1880/setDataRH?value=${newRH}`)
    .then((response) => {
      if (!response.ok) {
        throw new Error("Network response was not ok");
      }
      return response.json();
    })
    .then((data) => {
      console.log("Temperature updated successfully:", data);
    })
    .catch((error) => {
      console.error("There was a problem updating the temperature:", error);
    });
}

function changeTemperature_M(delta) {
  const TBDSAElement = document.getElementById("TBDSA_M");
  let currentTemperature = parseInt(TBDSAElement.textContent); // Get current temperature as an integer
  // currentTemperature += delta; // Increment or decrement the temperature based on the delta
  const newTemperature = currentTemperature + delta;
  // Check if the new temperature is within the acceptable range (18 to 26 degrees Celsius)
  if (newTemperature < 18 || newTemperature > 26) {
    console.log(
      "Temperature is outside the acceptable range (18 to 26 degrees Celsius). Skipping update."
    );
    return; // Exit the function without updating the temperature
  }

  TBDSAElement.textContent = newTemperature + "°C"; // Update the temperature value in the HTML

  // Send the updated temperature value to the server via HTTP GET request
  fetch(`http://` + config + `:1880/setDataTempM?value=${newTemperature}`)
    .then((response) => {
      if (!response.ok) {
        throw new Error("Network response was not ok");
      }
      return response.json();
    })
    .then((data) => {
      console.log("Temperature updated successfully:", data);
    })
    .catch((error) => {
      console.error("There was a problem updating the temperature:", error);
    });
}
function changeRH_M(delta) {
  const TBDSBRHElement = document.getElementById("TBDSBRH_M");
  let currentRH = parseInt(TBDSBRHElement.textContent); // Get current temperature as an integer
  // currentRH += delta; // Increment or decrement the temperature based on the delta
  const newRH = currentRH + delta;
  // Check if the new temperature is within the acceptable range (18 to 26 degrees Celsius)
  if (newRH < 50 || newRH > 60) {
    console.log(
      "RH is outside the acceptable range (50 to 60). Skipping update."
    );
    return; // Exit the function without updating the temperature
  }

  TBDSBRHElement.textContent = newRH + "%"; // Update the temperature value in the HTML

  // Send the updated temperature value to the server via HTTP GET request
  fetch(`http://` + config + `:1880/setDataRHM?value=${newRH}`)
    .then((response) => {
      if (!response.ok) {
        throw new Error("Network response was not ok");
      }
      return response.json();
    })
    .then((data) => {
      console.log("Temperature updated successfully:", data);
    })
    .catch((error) => {
      console.error("There was a problem updating the temperature:", error);
    });
}

function hidebtn() {
  document.getElementById("btntempup").style.display = "none";
  document.getElementById("btnrhup").style.display = "none";
  document.getElementById("btntempdown").style.display = "none";
  document.getElementById("btnrhdown").style.display = "none";
  document.getElementById("btndone").style.display = "none";
  document.getElementById("btnset").style.display = "block";

  document.getElementById("btntempup_M").style.display = "none";
  document.getElementById("btnrhup_M").style.display = "none";
  document.getElementById("btntempdown_M").style.display = "none";
  document.getElementById("btnrhdown_M").style.display = "none";
  document.getElementById("btndone_M").style.display = "none";
  document.getElementById("btnset_M").style.display = "block";
}
hidebtn();
setInterval(updateValue, 3000);

function showbtn() {
  document.getElementById("btntempup").style.display = "block";
  document.getElementById("btnrhup").style.display = "block";
  document.getElementById("btntempdown").style.display = "block";
  document.getElementById("btnrhdown").style.display = "block";
  document.getElementById("btndone").style.display = "block";
  document.getElementById("btnset").style.display = "none";

  document.getElementById("btntempup_M").style.display = "block";
  document.getElementById("btnrhup_M").style.display = "block";
  document.getElementById("btntempdown_M").style.display = "block";
  document.getElementById("btnrhdown_M").style.display = "block";
  document.getElementById("btndone_M").style.display = "block";
  document.getElementById("btnset_M").style.display = "none";
}

function toggleControlButtons(isAuto) {
  const controlButtons = document.getElementById("controlButtons");
  if (isAuto) {
    controlButtons.classList.remove("slide-up");
    controlButtons.classList.add("slide-down");
  } else {
    controlButtons.classList.remove("slide-down");
    controlButtons.classList.add("slide-up");
  }
}

// Example function to update the status and toggle buttons
function updateStatusAM(status) {
  const statusAM = document.getElementById("statusAM");
  statusAM.textContent = status;

  if (status === "Auto") {
    toggleControlButtons(true);
  } else {
    toggleControlButtons(false);
  }
}

// Example of how you might call the updateStatusAM function based on some status

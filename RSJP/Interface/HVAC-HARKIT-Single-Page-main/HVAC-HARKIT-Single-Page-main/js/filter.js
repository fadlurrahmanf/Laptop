function hidebtn() {
  document.getElementById("btntempup").style.display = "none";
  document.getElementById("btnrhup").style.display = "none";
  document.getElementById("btntempdown").style.display = "none";
  document.getElementById("btnrhdown").style.display = "none";
  document.getElementById("btndone").style.display = "none";
  document.getElementById("btnset").style.display = "block";
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
}

let passwordhvac = {};
let config = {};
getConfig();
setInterval(getConfig, 3000);
// Function to fetch configuration
function getConfig() {
  // Replace 'config.json' with the path to your configuration file
  const response = fetch("../config.json")
    .then((response) => response.json())
    .then((data) => {
      console.log(data.serverIP);
      config = data.serverIP;
      passwordhvac = data.password;
    })
    .catch((error) => {
      console.error("Error fetching configuration:", error);
      return {}; // Return an empty object if there's an error
    });

  return response;
}

function updateElementStatus(element, isRunning) {
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
      } = data[0];

      const elements = {
        ahuStatus: document.getElementById("AHU_status"),
        exhaustStatus: document.getElementById("Exhaust_status"),
        outdoorStatus: document.getElementById("Outdoor_status"),
        heat1Status: document.getElementById("Heater1_status"),
        heat2Status: document.getElementById("Heater2_status"),
        boosterStatus: document.getElementById("Booster_status"),
        tempElement: document.getElementById("TBDSAa"),
        rhElement: document.getElementById("TBDSBb"),
      };

      updateElementStatus(elements.ahuStatus, parseFloat(ahuStatus) === 1);
      updateElementStatus(elements.exhaustStatus, parseFloat(eFan) === 1);
      updateElementStatus(
        elements.outdoorStatus,
        parseFloat(outdoorUnit) === 1
      );
      updateElementStatus(elements.heat1Status, parseFloat(heat1) === 1);
      updateElementStatus(elements.heat2Status, parseFloat(heat2) === 1);
      updateElementStatus(elements.boosterStatus, parseFloat(bFan) === 1);

      elements.tempElement.textContent = tempCathlab + "°C";
      elements.rhElement.textContent = rhCathlab + "%";

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

      const TBDSAElement = document.getElementById("TBDSA");
      const TBDSBRHElement = document.getElementById("TBDSBRH");

      TBDSAElement.textContent = currentTemperature + "°C";
      TBDSBRHElement.textContent = currentRH + "%";
    })
    .catch((error) => {
      console.error("Error:", error);
    });
}

// Initial call to update values
updateValue();

// Function to show notification
function showNotification(message) {
  const notificationElement = document.createElement("div");
  notificationElement.textContent = message;
  notificationElement.classList.add("notification");
  document.body.appendChild(notificationElement);
}

// Function to remove notification
function removeNotification() {
  const notificationElement = document.querySelector(".notification");
  if (notificationElement) {
    notificationElement.remove();
  }
}
updateValue();
setInterval(updateValue, 1000);

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
    let stat = 0;
    if (modalstatus === "SET") {
      showbtn();
    }
    if (modalstatus === "DONE") {
      hidebtn();
    }
  } else {
    alert("Incorrect password. Please try again.");
    document.getElementById("passwordInput").value = ""; // Reset password input
    closeModal(); // Close the modal if password is correct
  }
}

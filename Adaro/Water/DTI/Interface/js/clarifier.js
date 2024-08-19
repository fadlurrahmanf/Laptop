document.addEventListener('DOMContentLoaded', function () {
    setTimeout(function () {
        const clarifier = document.getElementById('clarifier');
        clarifier.classList.add('active');
    }, 2);
});

var win = navigator.platform.indexOf('Win') > -1;
var intervalcounter = "Hari";
if (win && document.querySelector('#sidenav-scrollbar')) {
    var options = {
        damping: '0.5'
    }
    Scrollbar.init(document.querySelector('#sidenav-scrollbar'), options);
}
function updateElement(elementId, value) {
    var element = document.getElementById(elementId);
    try {
        if (value === 1) {
            element.textContent = "ON";
            element.style.backgroundColor = "#0000FF";
            element.style.borderColor = "#FFF";
            element.style.color = "#FFF";
            element.style.placeholder = "#FFF";
        } else {
            element.textContent = "OFF";
            element.style.backgroundColor = "#FF0000";
            element.style.borderColor = "#FFF";
            element.style.color = "#FFF";
        }
    } catch (x) {
        // alert([elementId,value]);
    }
}

function UpdateDataDisplay() {
    var http = new XMLHttpRequest();
    http.open("GET", "https://api-dti.azycloud.my.id/StatusRelay?data=" + String(intervalcounter), true);
    http.onreadystatechange = function () {
        if (this.readyState === 4 && this.status === 200) {
            var data1 = JSON.parse(this.responseText)[0];
            for (var i = 1; i <= 9; i++) {
                if (i != 9) {
                    updateElement("manualSA" + i, data1["SA" + i]);
                    updateElement("manualSB" + i, data1["SB" + i]);
                    updateElement("SADA" + i, data1["SA" + i]);
                    updateElement("SBDB" + i, data1["SB" + i]);
                }
                updateElement("manualFAD" + i, data1["FAD" + i]);
                updateElement("manualFBD" + i, data1["FBD" + i]);
                updateElement("FADA" + i, data1["FAD" + i]);
                updateElement("FBDB" + i, data1["FBD" + i]);
            }
            var data2 = JSON.parse(this.responseText)[4];
            var elementsA = document.querySelectorAll('[id^="count"]');
            var elementsM = document.querySelectorAll('[id^="mcount"]');
            for (var i = 0; i < elementsA.length; i++) {
                var elementA = elementsA[i];
                var elementM = elementsM[i];
                if (i < 9) { //FA
                    elementA.placeholder = data2.countFAD[i];
                    elementM.placeholder = data2.countFAD[i];
                } else if (i >= 9 && i < 18) { //FB
                    elementA.placeholder = data2.countFBD[i - 9];
                    elementM.placeholder = data2.countFBD[i - 9];
                } else if (i >= 18 && i < 26) { //SA                
                    elementA.placeholder = data2.countSA[i - 18];
                    elementM.placeholder = data2.countSA[i - 18];
                } else if (i >= 26 && i < 34) { //SB
                    elementA.placeholder = data2.countSB[i - 26];
                    elementM.placeholder = data2.countSB[i - 26];
                }
            }
            var data3 = JSON.parse(this.responseText)[5];
            document.getElementById("tbdraw").innerHTML = data3[0].Turbidity_Raw.toFixed(2);
            document.getElementById("phraw").innerHTML = data3[0].pH_Raw.toFixed(2);
            document.getElementById("Flow_Rate_Raw").innerHTML = data3[0].Flow_Rate_Raw.toFixed(2);
            document.getElementById("TBDSA").innerHTML = data3[0].Turbidity_Sedimentation_A.toFixed(2);
            document.getElementById("TBDSB").innerHTML = data3[0].Turbidity_Sedimentation_B.toFixed(2);
            document.getElementById("tbdproduct").innerHTML = data3[0].Turbidity_Product.toFixed(2);
            document.getElementById("phproduct").innerHTML = data3[0].pH_Product.toFixed(2);
            document.getElementById("Flow_Rate_Product").innerHTML = data3[0].Flow_Rate_Product.toFixed(2);
        }
    };
    http.send();

}

setInterval(UpdateDataDisplay, 2000);

function SubmitAuto(x) {

    if (document.getElementById(x).innerText == "Submit") {
        var val = 0
        var arr = [];
        if (x.substring(0, 1) == "F") {
            for (i = 0; i < 9; i++) {
                val = document.getElementById("F" + String(x.substring(1, 2) + "DA" + String(i + 1))).value;
                if (val <= 0 || val == null) {
                    // alert("No Value at " + String(x.substring(0, 3)));
                    return;
                }
                arr.push(val);
            }
        } else if (x.substring(0, 1) == "S") {
            for (i = 0; i < 8; i++) {
                val = document.getElementById("S" + String(x.substring(1, 2) + "A" + String(i + 1))).value;
                if (val <= 0 || val == null) {
                    // alert("No Value at " + String(x.substring(0, 3)));
                    return;
                }
                arr.push(val);
            }
        }
    } else if (document.getElementById(x).innerText == "GET") {

    }
}
function Radio(x) {
    if (document.getElementById("AutoF" + String(x) + "D").checked) {

        // alert("Checked");
        document.getElementById("F" + String(x) + "D").removeAttribute('Hidden');
        document.getElementById("F" + String(x) + "DA").setAttribute('Hidden', 'Hidden');
    } else {

        // alert("Not Checked");
        document.getElementById("F" + String(x) + "DA").removeAttribute('Hidden');
        document.getElementById("F" + String(x) + "D").setAttribute('Hidden', 'Hidden');
    }
}
function RadioS(x) {
    if (document.getElementById("AutoS" + String(x) + "").checked) {
        document.getElementById("S" + String(x) + "A").removeAttribute('Hidden');
        document.getElementById("S" + String(x) + "").setAttribute('Hidden', 'Hidden');
        // alert("Checked");
    } else {
        document.getElementById("S" + String(x) + "").removeAttribute('Hidden');
        document.getElementById("S" + String(x) + "A").setAttribute('Hidden', 'Hidden');
        // alert("Not Checked");
    }
}
function takedatarelay() {
    var http = new XMLHttpRequest();
    http.open("GET", "https://api-dti.azycloud.my.id/GetStatusRelay", true);
    http.send();
    http.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            var data = JSON.parse(this.responseText);
            datarelay = data;
            createbutton();
        }
    }
}
function createbutton() {
    var column = Object.keys(datarelay[0]);

    for (i = 0; i < column.length; i++) {
        if (column[i].substring(0, 2) == "FC") {
            continue;
        }
        var color = ""
        if (datarelay[0][column[i]] == 0) {
            color = "#FF0000";
        } else {
            color = "#0000FF";
        }
        document.getElementById(String(column[i])).style.background = color;
    }
}
function SendCommand(address) {
    var http = new XMLHttpRequest();
    var color = document.getElementById(address).style.background;
    var cmd = "0";
    if (color == "rgb(255, 0, 0)") {
        cmd = "1";
    } else if (color == "rgb(0, 0, 255)") {
        cmd = "0";
    }
    http.open("GET", "https://api-dti.azycloud.my.id/SendCommand?address=" + String(address) + "&cmd=" + cmd, true);
    http.send();
    http.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            var data = JSON.parse(this.responseText);
            // takedatarelay();
        }
    }

}
function datasensor() {
    var http = new XMLHttpRequest();
    http.open("GET", "https://api-dti.azycloud.my.id/GetCurrent", true);
    http.send();
    http.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            var data = JSON.parse(this.responseText);
            var len = data.length;
            main(data, len);
        }
    }
}
function Upper(data, len) {
    document.getElementById("tbdraw").innerText = data[data.length - 1].Turbidity_Raw + " NTU";
    document.getElementById("phraw").innerText = data[data.length - 1].pH_Raw;
    document.getElementById("Flow_Rate_Raw").innerText = data[data.length - 1].Flow_Rate_Raw + " LPS"
    document.getElementById("TBDSA").innerText = data[data.length - 1].Turbidity_Sedimentation_A + " NTU";
    document.getElementById("TBDSB").innerText = data[data.length - 1].Turbidity_Sedimentation_B + " NTU";
}
function main(data, len) {
    Upper(data, len);
}
function start() {
    takedatarelay();
    datasensor()
    setTimeout(() => {
        start();
    }, 1000);
}
function toggleInterval(selectedId) {
    const container = document.getElementById('invCount');
    const radioButtons = container.querySelectorAll('.radio');
    for (const radio of radioButtons) {
        if (radio.id !== selectedId) {
            radio.checked = false;
        }
    }

    if (selectedId === 'invBulan') {
        intervalcounter = "Bulan";
    } else if (selectedId === 'invMinggu') {
        intervalcounter = "Minggu";
    } else if (selectedId === 'invHari') {
        intervalcounter = "Hari";
    }
}
function toggleRadio(thisID, otherID) {
    const clickedCheckbox = document.getElementById(thisID);
    const otherCheckbox = document.getElementById(otherID);

    if (clickedCheckbox.checked) {
        otherCheckbox.checked = false;
        var url = "https://api-dti.azycloud.my.id/getMode?";

        if (thisID.substr(-7) === "paralel") {
            url = url + "ID=" + thisID + "&AutoMode" + "=1";
        } else if (thisID.substr(-8) === "sequence") {
            url = url + "ID=" + thisID + "&AutoMode" + "=0";
        }

        var http = new XMLHttpRequest();
        http.open("GET", url, true);
        http.send();
    }
}
function toggleMode(option, thisID, otherID, nID) {
    var url = "https://api-dti.azycloud.my.id/getMode?";
    if (option === 'A') {
        document.getElementById(otherID).checked = false;
        document.getElementById("autoFlocc" + nID).style.display = "block";
        document.getElementById("manualFlocc" + nID).style.display = "none";
        url = url + "ID=" + thisID + "&Mode" + "=1"
    } else if (option === 'B') {
        document.getElementById(otherID).checked = false;
        document.getElementById("autoFlocc" + nID).style.display = "none";
        document.getElementById("manualFlocc" + nID).style.display = "block";
        url = url + "ID=" + thisID + "&Mode" + "=0"
    }

    var http = new XMLHttpRequest();
    http.open("GET", url, true);
    http.send();
}

function safetyTombol() {
    const initialButtonStates = {};
    document.querySelectorAll("button").forEach(function (button) {
        initialButtonStates[button.id] = button.disabled;
        button.disabled = true;
    });
    setTimeout(() => {
        document.querySelectorAll("button").forEach(function (button) {
            button.disabled = initialButtonStates[button.id];
        });
    }, 5000);
}

let clickStartTimes = {};
let currentTime = {};
function buttonManual(buttonId) {
    safetyTombol();
    const button = document.getElementById(buttonId);
    var http = new XMLHttpRequest();
    if (button.textContent === "OFF") {
        // button.textContent = "ON";
        // button.style.backgroundColor = "#00ff26";
        button.style.color = "white";
        button.style.paddingLeft = "1.5vh";
        button.style.paddingRight = "1.5vh";
        clickStartTimes[buttonId] = Date.now();
        const clickIntervalId = setInterval(() => updateElapsedTime(buttonId), 1000);
        clickIntervalIds[buttonId] = clickIntervalId;
        // alert("on");
        http.open("GET", "https://api-dti.azycloud.my.id/getTombol?" + buttonId + "=1", true);
        http.send();
    } else {
        clearInterval(clickIntervalIds[buttonId]);
        // button.textContent = "OFF";
        // button.style.backgroundColor = "red";
        button.style.color = "white";
        button.style.paddingLeft = "";
        button.style.paddingRight = "";
        // alert("off");
        http.open("GET", "https://api-dti.azycloud.my.id/getTombol?" + buttonId + "=0&Durasi=" + currentTime[buttonId], true);
        http.send();
    }
}
function updateElapsedTime(buttonId) {
    const elapsedTime = Math.floor((Date.now() - clickStartTimes[buttonId]) / 1000);
    const button = document.getElementById(buttonId);
    // button.textContent = "ON: " + elapsedTime;
    currentTime[buttonId] = elapsedTime;
}
const clickIntervalIds = {};

function btnCheck(buttonId) {
    safetyTombol();
    document.getElementById(buttonId.substring(0, 3) + "_status").placeholder = "Getting Data";
    var buttonElement = document.getElementById(buttonId);
    var originalColor = buttonElement.style.backgroundColor;
    var originalText = buttonElement.textContent;
    var http = new XMLHttpRequest();
    http.open("GET", "https://api-dti.azycloud.my.id/getCheck?id=" + String(buttonId), true);
    http.send();
    http.onreadystatechange = function () {
        if (this.readyState === 4 && this.status === 200) {
            var data = JSON.parse(this.responseText);
            if (data.length > 0) {
                document.getElementById(buttonId.substring(0, 3) + "_status").placeholder = "Check Done";
            } else {
                document.getElementById(buttonId.substring(0, 3) + "_status").placeholder = "Check Failed";
            }
            function setElementPlaceholder(elementId, placeholderValue) {
                const element = document.getElementById(elementId);
                if (element) {
                    element.value = placeholderValue;
                }
            }

            function setAutoModeCheckboxes(sequenceCheckboxId, paralelCheckboxId, sequenceChecked) {
                document.getElementById(sequenceCheckboxId).checked = sequenceChecked;
                document.getElementById(paralelCheckboxId).checked = !sequenceChecked;
            }

            for (let i = 0; i < data.length; i++) {
                const idPrefix = data[i][0].ID.substring(0, 2);
                const durationPrefix = idPrefix === "SA" ? "DA" : idPrefix === "FA" ? "DA" : "DB";
                const durationPlaceholderId = idPrefix + durationPrefix + (i + 1);
                const intervalPlaceholderId = idPrefix === "SA" ? "SADA" + (data.length + 1) : idPrefix === "FA" ? "FADA" + (data.length + 1) : idPrefix + "DB" + (data.length + 1);

                setElementPlaceholder(durationPlaceholderId, data[i][0].Duration);
                setElementPlaceholder(intervalPlaceholderId, data[i][0].Intervalx);

                const sequenceCheckboxId = idPrefix === "SA" ? "AutoSAD_sequence" : idPrefix === "FA" ? "AutoFAD_sequence" : "AutoFBD_sequence";
                setAutoModeCheckboxes(sequenceCheckboxId, sequenceCheckboxId.replace("sequence", "paralel"), data[i][0].Mode === "sequence");
            }

            document.getElementById(buttonId.substring(0, 5) + "submit").disabled = false;
            document.getElementById(buttonId.substring(0, 5) + "stop").disabled = false;
            document.getElementById(buttonId.substring(0, 5) + "run").disabled = true;
        }
    }

    buttonElement.style.backgroundColor = "#945e21";
    buttonElement.textContent = "Done!";
    setTimeout(function () {
        buttonElement.style.backgroundColor = originalColor;
        buttonElement.textContent = originalText;
    }, 5000);
}

function btnSubmit(buttonId) {
    safetyTombol();
    var buttonElement = document.getElementById(buttonId);
    var originalColor = buttonElement.style.backgroundColor;
    var originalText = buttonElement.textContent;
    var inputIds = [];

    if (buttonId.substr(0, 3) === "FAD") {
        var prefix = "FADA";
        var numInputs = 10;
    } else if (buttonId.substr(0, 3) === "FBD") {
        var prefix = "FBDB";
        var numInputs = 10;
    } else if (buttonId.substr(0, 3) === "SAD") {
        var prefix = "SADA";
        var numInputs = 9;
    } else if (buttonId.substr(0, 3) === "SBD") {
        var prefix = "SBDB";
        var numInputs = 9;
    }

    for (var i = 1; i <= numInputs; i++) {
        inputIds.push(prefix + i);
    }

    var validationFailed = false;
    inputIds.forEach(function (id) {
        var inputValue = document.getElementById(id).value;
        if (!inputValue || parseFloat(inputValue) === 0) {
            validationFailed = true;
            return;
        }
    });

    if (validationFailed) {
        Swal.fire({
            icon: 'error',
            title: 'Submit Failed!',
            text: 'Ensure that the duration and interval inputs are not empty or zero.',
        });
        return;
    }

    document.getElementById(buttonId.substring(0, 3) + "_status").placeholder = "Submitting";
    var currmode = "";
    if (buttonId.substring(0, 3) == "SAD") {
        if (document.getElementById("AutoSAD_paralel").checked) {
            currmode = "paralel";
        } else if (document.getElementById("AutoSAD_sequence").checked) {
            currmode = "sequence";
        }
    }
    if (buttonId.substring(0, 3) == "SBD") {
        if (document.getElementById("AutoSBD_paralel").checked) {
            currmode = "paralel";
        } else if (document.getElementById("AutoSBD_sequence").checked) {
            currmode = "sequence";
        }
    }
    if (buttonId.substring(0, 3) == "FAD") {
        if (document.getElementById("AutoFAD_paralel").checked) {
            currmode = "paralel";
        } else if (document.getElementById("AutoFAD_sequence").checked) {
            currmode = "sequence";
        }
    }
    if (buttonId.substring(0, 3) == "FBD") {
        if (document.getElementById("AutoFBD_paralel").checked) {
            currmode = "paralel";
        } else if (document.getElementById("AutoFBD_sequence").checked) {
            currmode = "sequence";
        }
    }
    var data = {};

    inputIds.forEach(function (id) {
        var inputValue = document.getElementById(id).value;
        data[id] = inputValue;
    })
    data['mode'] = currmode;
    buttonElement.style.backgroundColor = "rgb(50 78 255)";
    buttonElement.textContent = "Done!";

    var http = new XMLHttpRequest();
    var url = "https://api-dti.azycloud.my.id/getExecute?ID=" + buttonId + "&Data=" + JSON.stringify(data);
    http.open("GET", url, true);
    http.send();
    http.onreadystatechange = function () {
        if (this.readyState === 4 && this.status === 200) {
            var data = JSON.parse(this.responseText);
            if (data.Stat) {
                document.getElementById(buttonId.substring(0, 3) + "_status").placeholder = "Submitted";
                document.getElementById(buttonId.substring(0, 5) + "stop").disabled = true;
                document.getElementById(buttonId.substring(0, 5) + "run").disabled = false;
            } else {
                document.getElementById(buttonId.substring(0, 3) + "_status").placeholder = "Submit Failed";
            }
        }
    }

    setTimeout(function () {
        buttonElement.style.backgroundColor = originalColor;
        buttonElement.textContent = originalText;
    }, 5000);
}

function btnStop(buttonId) {
    safetyTombol();
    document.getElementById(buttonId.substring(0, 3) + "_status").placeholder = "Stopping";
    var buttonElement = document.getElementById(buttonId);
    var originalColor = buttonElement.style.backgroundColor;
    var originalText = buttonElement.textContent;
    buttonElement.style.backgroundColor = "#75195e";
    buttonElement.textContent = "Done!";
    var http = new XMLHttpRequest();
    var url = "https://api-dti.azycloud.my.id/getExecute?stop=0&ID=" + buttonId;
    http.open("GET", url, true);
    http.send();
    http.onreadystatechange = function () {
        if (this.readyState === 4 && this.status === 200) {
            var data = JSON.parse(this.responseText);
            if (data.Stat) {
                document.getElementById(buttonId.substring(0, 3) + "_status").placeholder = "Stop Success";
                document.getElementById(buttonId.substring(0, 5) + "run").disabled = false;
                document.getElementById(buttonId.substring(0, 5) + "stop").disabled = true;
            } else {
                document.getElementById(buttonId.substring(0, 3) + "_status").placeholder = "Stop Failed";
            }
        }
    }
    setTimeout(function () {
        buttonElement.style.backgroundColor = originalColor;
        buttonElement.textContent = originalText;
    }, 5000);
}

function btnRun(buttonId) {
    safetyTombol();
    document.getElementById(buttonId.substring(0, 3) + "_status").placeholder = "Starting";
    var buttonElement = document.getElementById(buttonId);
    var originalColor = buttonElement.style.backgroundColor;
    var originalText = buttonElement.textContent;
    buttonElement.style.backgroundColor = "#21612c";
    buttonElement.textContent = "Running!";
    var http = new XMLHttpRequest();
    var url = "https://api-dti.azycloud.my.id/getExecute?run=1&ID=" + buttonId;
    http.open("GET", url, true);
    http.send();
    http.onreadystatechange = function () {
        if (this.readyState === 4 && this.status === 200) {
            var data = JSON.parse(this.responseText);
            if (data.Stat) {
                document.getElementById(buttonId.substring(0, 3) + "_status").placeholder = "Running";
                document.getElementById(buttonId.substring(0, 5) + "stop").disabled = false;
                document.getElementById(buttonId.substring(0, 5) + "submit").disabled = true;
            } else {
                document.getElementById(buttonId.substring(0, 3) + "_status").placeholder = "Run Failed";
            }
        }
    }
    setTimeout(function () {
        buttonElement.style.backgroundColor = originalColor;
        buttonElement.textContent = originalText;
    }, 5000);
}


// start();
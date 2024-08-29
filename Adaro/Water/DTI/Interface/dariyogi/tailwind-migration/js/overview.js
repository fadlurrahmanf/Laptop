document.addEventListener('DOMContentLoaded', function () {
    setTimeout(function () {
        const overview = document.getElementById('overview');
        overview.classList.add('active');
    }, 2);
});

let totalizerData = [
    { diffElementId: "diffTRW", elementId: "Totalizer_Raw_Water", dataKey: "Totalizer_Raw", unit: " m<sup>3</sup>" },
    { diffElementId: "diffTPW0", elementId: "Totalizer_Product_Water0", dataKey: "Totalizer_Product", unit: " m<sup>3</sup>" },
    { diffElementId: "diffTPW1", elementId: "Totalizer_Product_Water1", dataKey: "TOTALIZER_PRODUCT_1", unit: " m<sup>3</sup>" },
    { diffElementId: "diffTPW2", elementId: "Totalizer_Product_Water2", dataKey: "TOTALIZER_PRODUCT_2", unit: " m<sup>3</sup>" },
    { diffElementId: "diffTBW", elementId: "Totalizer_Backwash_Water", dataKey: "TOTALIZER_TBW", unit: " m<sup>3</sup>" },
    { diffElementId: "diffreservoir", elementId: "Reservoir_Volume", dataKey: "Reservoir_Volume", unit: " m<sup>3</sup>" }
];

let elementsToUpdate = [
    { id: "tbdraw", key: "Turbidity_Raw", unit: " NTU" },
    { id: "phraw", key: "pH_Raw", unit: "" },
    { id: "tbdproduct", key: "Turbidity_Product", unit: " NTU" },
    { id: "phproduct", key: "pH_Product", unit: "" },
    { id: "TBDSA", key: "Turbidity_Sedimentation_A", unit: " NTU" },
    { id: "TBDSB", key: "Turbidity_Sedimentation_B", unit: " NTU" },
    { id: "Chlorine", key: "Chlorine_Product", unit: " ppm" },
    { id: "Flow_Rate_Raw", key: "Flow_Rate_Raw", unit: " LPS" },
    { id: "Totalizer_Raw_Water", key: "Totalizer_Raw", unit: " m<sup>3</sup>" },
    { id: "Flow_Rate_Product", key: "Flow_Rate_Product", unit: " LPS" },
    { id: "Totalizer_Product_Water", key: "Totalizer_Product", unit: " m<sup>3</sup>" },
    { id: "Reservoir_Volume", key: "Reservoir_Volume", unit: " m<sup>3</sup>" },
    { id: "Reservoir", key: "Reservoir_Level", unit: " cm" },
    { id: "Flow_Rate_Product1", key: "FLOWRATE_PRODUCT_1", unit: " LPS" },
    { id: "Totalizer_Product_Water1", key: "TOTALIZER_PRODUCT_1", unit: " m<sup>3</sup>" },
    { id: "Flow_Rate_Product2", key: "FLOWRATE_PRODUCT_2", unit: " LPS" },
    { id: "Totalizer_Product_Water2", key: "TOTALIZER_PRODUCT_2", unit: " m<sup>3</sup>" }
];

function addSeparator(x) {
    return x.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ",");
}

function updateDiffElement(diffElement, diffValue) {
    let diffplusmin = diffValue < 0 ? "-" : "+";
    let formattedDiff = addSeparator(Math.abs(diffValue).toFixed(2));
    diffElement.classList.add(diffValue < 0 ? "text-danger" : "text-success");
    diffElement.innerHTML = `${diffplusmin}${formattedDiff}`;
}

function updateElementValues(data) {
    let lastData = data[data.length - 1];

    for (let i = 0; i < elementsToUpdate.length; i++) {
        let elementInfo = elementsToUpdate[i];
        let value = lastData[elementInfo.key];
        let formattedValue = 0;
        if (elementInfo.key == "Turbidity_Sedimentation_A" || elementInfo.key == "Turbidity_Sedimentation_B" || elementInfo.key == "Turbidity_Product" || elementInfo.key == "Reservoir_Level") {
            formattedValue = `${value.toFixed(2)}${elementInfo.unit}`;
        } else {
            formattedValue = `${addSeparator(value)}${elementInfo.unit}`;
        } 
        document.getElementById(elementInfo.id).innerHTML = formattedValue;
    }
}

function processData(data, dataIndex) {
    let totalizer = totalizerData[dataIndex];
    let diffElement = document.getElementById(totalizer.diffElementId);
    totalizer.data = data.map(item => item[totalizer.dataKey]);
    let diffPercent = totalizer.data[totalizer.data.length - 1] - totalizer.data[0];
    document.getElementById(totalizer.elementId).innerHTML = addSeparator(totalizer.data[totalizer.data.length - 1]);

    updateDiffElement(diffElement, diffPercent);
}

function start() {
    let http = new XMLHttpRequest();
    http.open("GET", "http://192.168.0.101:1880/GetCurrent", true);
    http.send();
    http.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            let data = JSON.parse(this.responseText);

            updateElementValues(data);
            for (let i = 0; i < totalizerData.length; i++) {
                processData(data, i);
            }
            setTimeout(start, 1000);
        }
    };
}

setTimeout(start, 500);
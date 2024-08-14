document.addEventListener('DOMContentLoaded', function () {
    setTimeout(function () {
        const dashboard = document.getElementById('dashboard');
        dashboard.classList.add('active');
    }, 2);
});

let chartraw = new JustGage({
    id: "gauRaw",
    value: 157,
    valueFontColor: "#64748b",
    min: 0,
    max: 450,
    title: "Flow Rate",
    titleFontColor: "#64748b",
    label: "LPS",
    levelColors: ["rgb(6, 182, 212)"],
    gaugeWidthScale: 0.7,
    gaugeColor: "rgb(203, 213, 225)",
    counter: true,
    pointer: true,
    pointerOptions: {
        toplength: 0,
        bottomlength: 10,
        bottomwidth: 0,
        color: "#ffffff",
        stroke: "#64748b",
        stroke_width: 3,
        stroke_linecap: "round"
    }
});

let chartreservoir = new JustGage({
    id: "gaurs1",
    value: 172,
    valueFontColor: "#64748b",
    min: 0,
    max: 350,
    titleFontColor: "#64748b",
    label: "cm",
    levelColors: ["rgb(6, 182, 212)"],
    gaugeWidthScale: 0.7,
    gaugeColor: "rgb(203, 213, 225)",
    counter: true,
    pointer: true,
    pointerOptions: {
        toplength: 0,
        bottomlength: 10,
        bottomwidth: 0,
        color: "#ffffff",
        stroke: "#64748b",
        stroke_width: 3,
        stroke_linecap: "round"
    }
});

let chartfm1 = new JustGage({
    id: "gaufm1",
    value: 172,
    valueFontColor: "#64748b",
    min: 0,
    max: 450,
    titleFontColor: "#64748b",
    label: "LPS",
    levelColors: ["rgb(6, 182, 212)"],
    gaugeWidthScale: 0.7,
    gaugeColor: "rgb(203, 213, 225)",
    counter: true,
    pointer: true,
    pointerOptions: {
        toplength: 0,
        bottomlength: 10,
        bottomwidth: 0,
        color: "#ffffff",
        stroke: "#64748b",
        stroke_width: 3,
        stroke_linecap: "round"
    }
});

let chartfm2 = new JustGage({
    id: "gaufm2",
    value: 172,
    valueFontColor: "#64748b",
    min: 0,
    max: 450,
    titleFontColor: "#64748b",
    label: "LPS",
    levelColors: ["rgb(6, 182, 212)"],
    gaugeWidthScale: 0.7,
    gaugeColor: "rgb(203, 213, 225)",
    counter: true,
    pointer: true,
    pointerOptions: {
        toplength: 0,
        bottomlength: 10,
        bottomwidth: 0,
        color: "#ffffff",
        stroke: "#64748b",
        stroke_width: 3,
        stroke_linecap: "round"
    }
});

let chartfm3 = new JustGage({
    id: "gaufm3",
    value: 172,
    valueFontColor: "#64748b",
    min: 0,
    max: 450,
    titleFontColor: "#64748b",
    label: "LPS",
    levelColors: ["rgb(6, 182, 212)"],
    gaugeWidthScale: 0.7,
    gaugeColor: "rgb(203, 213, 225)",
    counter: true,
    pointer: true,
    pointerOptions: {
        toplength: 0,
        bottomlength: 10,
        bottomwidth: 0,
        color: "#ffffff",
        stroke: "#64748b",
        stroke_width: 3,
        stroke_linecap: "round"
    }
});

let totalizerData = [
    // { data: [], diffElementId: "diffTRW", diffElementSuffix: "TRW", elementId: "Totalizer_Raw_Water", dataKey: "Totalizer_Raw" },
    // { data: [], diffElementId: "diffTPW0", diffElementSuffix: "TPW0", elementId: "Totalizer_Product_Water0", dataKey: "Totalizer_Product" },
    { data: [], diffElementId: "diffTPW1", diffElementSuffix: "TPW1", elementId: "Totalizer_Product_Water1", dataKey: "TOTALIZER_PRODUCT_1" },
    { data: [], diffElementId: "diffTPW2", diffElementSuffix: "TPW2", elementId: "Totalizer_Product_Water2", dataKey: "TOTALIZER_PRODUCT_2" },
    { data: [], diffElementId: "diffTBW", diffElementSuffix: "TBW", elementId: "Totalizer_Backwash_Water", dataKey: "TOTALIZER_TBW" },
    { data: [], diffElementId: "diffreservoir", diffElementSuffix: "reservoir", elementId: "Reservoir_Volume", dataKey: "Reservoir_Volume" }
];
let elementsToUpdate = [
    { id: "traw1", key: "Totalizer_Raw", unit: " m&sup3" },
    { id: "traw2", key: "Totalizer_Raw", unit: " m&sup3" },
    { id: "frraw", key: "Flow_Rate_Raw", unit: " LPS" },
    { id: "tbdraw", key: "Turbidity_Raw", unit: " NTU" },
    { id: "phraw", key: "pH_Raw", unit: "" },
    { id: "rvol", key: "Reservoir_Volume", unit: " m&sup3" },
    { id: "fmp1", key: "TOTALIZER_PRODUCT_1", unit: " m&sup3" },
    { id: "fmp2", key: "TOTALIZER_PRODUCT_2", unit: " m&sup3" },
    // { id: "fmp3", key: "Totalizer_Product", unit: " m&sup3" },
    { id: "tbdp", key: "Turbidity_Product", unit: " NTU" },
    { id: "phproduct", key: "pH_Product", unit: "" },
    { id: "cproduct", key: "Chlorine_Product", unit: " ppm" },
    { id: "tbds1", key: "Turbidity_Sedimentation_A", unit: " NTU" },
    { id: "tbds2", key: "Turbidity_Sedimentation_B", unit: " NTU" }
];
function addSeparator(x) {
    return x.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ",");
}

function updateDiffElement(diffElement, diffValue) {
    let diffplusmin = diffValue < 0 ? "-" : "+";
    let formattedDiff = addSeparator(Math.abs(diffValue).toFixed(0));
    diffElement.classList.add(diffValue < 0 ? "text-red-500" : "text-green-500");
    diffElement.innerHTML = `${diffplusmin}${formattedDiff}`;
}

function processData(data, len, dataIndex) {
    let totalizer = totalizerData[dataIndex];
    let diffElement = document.getElementById(totalizer.diffElementId);
    totalizer.data = data.map(item => item[totalizer.dataKey]);
    let diffPercent = totalizer.data[len - 1] - totalizer.data[0];
    document.getElementById(totalizer.elementId).innerHTML = addSeparator(totalizer.data[len - 1]);
    updateDiffElement(diffElement, diffPercent);
}

function updateElementValues(data, len) {
    let lastData = data[len - 1];
    for (let i = 0; i < elementsToUpdate.length; i++) {
        let elementInfo = elementsToUpdate[i];
        let value = lastData[elementInfo.key];
        let formattedValue = 0;
        if (elementInfo.key == "Turbidity_Sedimentation_A" || elementInfo.key == "Turbidity_Sedimentation_B") {
            formattedValue = `${value.toFixed(2)}${elementInfo.unit}`;
        } else if (elementInfo.key == "TOTALIZER_PRODUCT_1" || elementInfo.key == "TOTALIZER_PRODUCT_2" || elementInfo.key == "Totalizer_Product") {
            let myupdate = parseFloat(value).toFixed(0);
            formattedValue = `${addSeparator(myupdate)}${elementInfo.unit}`;
        } else {
            let myupdate = parseFloat(value).toFixed(2);
            formattedValue = `${addSeparator(myupdate)}${elementInfo.unit}`;
        }
        document.getElementById(elementInfo.id).innerHTML = formattedValue;
    }
}

function start() {
    let http = new XMLHttpRequest();
    http.open("GET", "http://api-dti.azycloud.my.id/GetCurrent", true);
    http.send();
    http.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            let data = JSON.parse(this.responseText);
            let len = data.length;

            updateElementValues(data, len);
            chartraw.refresh(data[len - 1].Flow_Rate_Raw);
            chartreservoir.refresh(data[len - 1].Reservoir_Level);
            chartfm1.refresh(data[len - 1].FLOWRATE_PRODUCT_1);
            chartfm2.refresh(data[len - 1].FLOWRATE_PRODUCT_2);
            chartfm3.refresh(data[len - 1].Flow_Rate_Product);
            for (let i = 0; i < totalizerData.length; i++) {
                processData(data, len, i);
            }
            setTimeout(start, 1000);
        }
    };
}

start();
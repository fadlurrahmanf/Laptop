document.addEventListener('DOMContentLoaded', function () {
    const sidebarToggle = document.querySelector('.app-sidebar__toggle');
    if (sidebarToggle && window.innerWidth > 768) {
        sidebarToggle.click();
    }

    let tab26Element = document.getElementById('tab26');
    tab26Element.classList.remove('active');
    let tab27Element = document.getElementById('tab27');
    tab27Element.classList.remove('active');
    let tab28Element = document.getElementById('tab28');
    tab28Element.classList.remove('active');

    $('body').addClass('dark-mode');

    $('#myonoffswitch5').prop('checked', true);
    $('#myonoffswitch8').prop('checked', true);
    $('body').removeClass('light-mode');
    $('body').removeClass('transparent-mode');

    $('body')?.removeClass('color-menu');
    $('body')?.removeClass('gradient-menu');
    $('body')?.removeClass('light-menu');
    $('body')?.removeClass('color-header');
    $('body')?.removeClass('gradient-header');
    $('body')?.removeClass('header-light');

    // remove light theme properties
    localStorage.removeItem('sashprimaryColor')
    localStorage.removeItem('sashprimaryHoverColor')
    localStorage.removeItem('sashprimaryBorderColor')
    localStorage.removeItem('sashdarkPrimary')
    document.querySelector('html').style.removeProperty('--primary-bg-color', localStorage.darkPrimary);
    document.querySelector('html').style.removeProperty('--primary-bg-hover', localStorage.darkPrimary);
    document.querySelector('html').style.removeProperty('--primary-bg-border', localStorage.darkPrimary);
    document.querySelector('html').style.removeProperty('--dark-primary', localStorage.darkPrimary);

    // removing light theme data 
    localStorage.removeItem('sashprimaryColor')
    localStorage.removeItem('sashprimaryHoverColor')
    localStorage.removeItem('sashprimaryBorderColor')
    localStorage.removeItem('sashprimaryTransparent');

    $('#myonoffswitch1').prop('checked', false);
    $('#myonoffswitch2').prop('checked', true);
    $('#myonoffswitchTransparent').prop('checked', false);
    //
    checkOptions();

    localStorage.removeItem('sashtransparentBgColor');
    localStorage.removeItem('sashtransparentThemeColor');
    localStorage.removeItem('sashtransparentPrimary');
    localStorage.removeItem('sashtransparentBgImgPrimary');
    localStorage.removeItem('sashtransparentBgImgprimaryTransparent');


    localStorage.removeItem('sashcolormenu');
    localStorage.removeItem('sashgradientmenu');
    localStorage.removeItem('sashlightmenu');
    localStorage.removeItem('sashcolorheader');
    localStorage.removeItem('sashgradientheader');
    localStorage.removeItem('sashlightheader');

    const root = document.querySelector(':root');
    root.style = "";
    names()
});

// CHART FLOWMETER
function createChartFlow(id, data, timeseries, bColor, bgColor, enableMin, min, enableMax, max) {
    var ctx = document.getElementById(id);
    return new Chart(ctx, {
        type: 'line',
        data: {
            labels: timeseries,
            datasets: [{
                label: id,
                borderColor: bColor,
                borderWidth: 3,
                lineTension: 0,
                backgroundColor: bgColor,
                fill: true,
                data: data
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            scales: {
                x: {
                    ticks: {
                        color: "#9ba6b5",
                    },
                    grid: {
                        color: 'rgba(119, 119, 142, 0.2)'
                    }
                },
                y: {
                    min: enableMin ? min : undefined,
                    max: enableMax ? max : undefined,
                    ticks: {
                        color: "#9ba6b5",
                    },
                    grid: {
                        color: 'rgba(119, 119, 142, 0.2)'
                    },
                }
            },
            plugins: {
                legend: {
                    display: false
                },
            },
            elements: {
                point: {
                    pointStyle: "circle",
                    radius: 3,
                }
            }
        }
    });
}




var timeseries = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23];
var Data1 = [14, 10, 7, 4, 10, 8, 11, 5, 11, 8, 5, 4, 8, 7, 0, 10, 5, 2, 14, 19, 9, 14, 5, 13];
var color1 = ["#00ddff", "rgba(0, 221, 255, .2)"];
var color2 = ["#ff00ea", "rgba(255, 0, 234, .2)"];
var color3 = ["#00ff1a", "rgba(0, 255, 26, .2)"];
var color4 = ["#ffb700", "rgba(255, 183, 0, .2)"];

var chartFlow1 = createChartFlow('chart_flowrate_in', Data1, timeseries, color1[0], color1[1], true, 0, false, 0);
var chartFlow2 = createChartFlow('chart_totalizer_in', Data1, timeseries, color1[0], color1[1], true, 0, false, 0);
var chartFlow3 = createChartFlow('chart_density_in', Data1, timeseries, color1[0], color1[1], true, 0.7, true, 0.9);
var chartFlow4 = createChartFlow('chart_temp_in', Data1, timeseries, color1[0], color1[1], true, 20, true, 60);

var ALLDATA = [];
var timeAll = [];
var paramAll = [];
var valueAll = [];

function updateFlow(chart, time, data) {
    chart.data.labels = time;
    chart.data.datasets[0].data = data;
    chart.update(); // Memperbarui chart
}
function chartFlow() {
    var http = new XMLHttpRequest();
    http.open("GET", "http://192.168.50.101:1880/RequestFlowmeterINData?", true);
    http.send();
    http.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            var data = JSON.parse(this.responseText);
            var timeFlowmeter = [];
            var valueFlowmeter = [];
            ALLDATA = data[4];

            // Data Flowmeter
            for (let i = 0; i < data[0].length; i++) {
                const Flowmeter = data[0][i];
                timeFlowmeter.push(Flowmeter.time);
                valueFlowmeter.push(Flowmeter.Value);
            }

            // Data Totalizer
            var timeTotalizer = [];
            var valueTotalizer = [];
            for (let i = 0; i < data[1].length; i++) {
                const Totalizer = data[1][i];
                timeTotalizer.push(Totalizer.time);
                valueTotalizer.push(Totalizer.Value);
            }

            // Data Density
            var timeDensity = [];
            var valueDensity = [];
            for (let i = 0; i < data[2].length; i++) {
                const Density = data[2][i];
                timeDensity.push(Density.time);
                valueDensity.push(Density.Value);
            }

            // Data Temp
            var timeTemp = [];
            var valueTemp = [];
            for (let i = 0; i < data[3].length; i++) {
                const Temp = data[3][i];
                timeTemp.push(Temp.time);
                valueTemp.push(Temp.Value);
            }



            document.getElementById("flowrate_in").innerHTML = valueFlowmeter[valueFlowmeter.length - 1] + " t/h";
            document.getElementById("totalizer_in").innerHTML = valueTotalizer[valueTotalizer.length - 1] + " m³";
            document.getElementById("density_in").innerHTML = valueDensity[valueDensity.length - 1] + " g/cm³";
            document.getElementById("temp_in").innerHTML = valueTemp[valueTemp.length - 1] + " °C";

            document.getElementById("add_flow_in").innerHTML = (valueFlowmeter[valueFlowmeter.length - 1] - valueFlowmeter[valueFlowmeter.length - 2]).toFixed(2);
            document.getElementById("add_totalizer_in").innerHTML = (valueTotalizer[valueTotalizer.length - 1] - valueTotalizer[valueTotalizer.length - 2]).toFixed(2);
            document.getElementById("add_dens_in").innerHTML = (valueDensity[valueDensity.length - 1] - valueDensity[valueDensity.length - 2]).toFixed(2);
            document.getElementById("add_temp_in").innerHTML = (valueTemp[valueTemp.length - 1] - valueTemp[valueTemp.length - 2]).toFixed(2);

            document.getElementById("time_flow_in").innerHTML = " - " + timeFlowmeter[timeFlowmeter.length - 1];
            document.getElementById("time_totalizer_in").innerHTML = " - " + timeTotalizer[timeTotalizer.length - 1];
            document.getElementById("time_dens_in").innerHTML = " - " + timeDensity[timeDensity.length - 1];
            document.getElementById("time_temp_in").innerHTML = " - " + timeTemp[timeTemp.length - 1];
            
            



            updateFlow(chartFlow1, timeFlowmeter, valueFlowmeter);
            updateFlow(chartFlow2, timeTotalizer, valueTotalizer);
            updateFlow(chartFlow3, timeDensity, valueDensity);
            updateFlow(chartFlow4, timeTemp, valueTemp);

        }
    }
    setTimeout(chartFlow, 5000);
};

document.getElementById('parametertable').addEventListener('change', function () {
    const selectedValue = this.value;
    const tableBody = document.getElementById('tabelFlow');

    timeAll = [];
    paramAll = [];
    valueAll = [];
    var parameterselect = selectedValue;
    for (let i = 0; i < ALLDATA.length; i++) {
        var AllData = ALLDATA[i];
        if (parameterselect == "All") {
            timeAll.push(AllData.time);
            paramAll.push(AllData.deviceID);
            valueAll.push(AllData.Value);
        } else {
            if (parameterselect == "FlowRate") {
                if (AllData.deviceID == "FlowRate") {
                    timeAll.push(AllData.time);
                    paramAll.push(AllData.deviceID);
                    valueAll.push(AllData.Value);
                }
            } else if (parameterselect == "Totalizer") {
                if (AllData.deviceID == "Totalizer") {
                    timeAll.push(AllData.time);
                    paramAll.push(AllData.deviceID);
                    valueAll.push(AllData.Value);
                }
            } else if (parameterselect == "Density") {
                if (AllData.deviceID == "Density") {
                    timeAll.push(AllData.time);
                    paramAll.push(AllData.deviceID);
                    valueAll.push(AllData.Value);
                }
            } else if (parameterselect == "Temp") {
                if (AllData.deviceID == "Temp") {
                    timeAll.push(AllData.time);
                    paramAll.push(AllData.deviceID);
                    valueAll.push(AllData.Value);
                }
            }
        }
    }

    // Clear existing rows
    tableBody.innerHTML = '';
    for (let i = 0; i < timeAll.length; i++) {
        const row = document.createElement('tr');
        row.innerHTML = `
            <td>${timeAll[i]}</td>
            <td>${paramAll[i]}</td>
            <td>${valueAll[i]}</td>
        `;
        tableBody.appendChild(row);
    }
});

// Initialize table with all data
document.getElementById('parametertable').dispatchEvent(new Event('change'));

chartFlow();

function tabelFlow() {

};

tabelFlow();

// function addFlowData() {
//     var http = new XMLHttpRequest();
//     http.open("GET", "http://192.168.50.101:1880/getDataFlow?", true);
//     http.send();

//     http.onreadystatechange = function () {
//         if (this.readyState == 4 && this.status == 200) {
//             var data = JSON.parse(this.responseText);
//             document.getElementById("gantry1").textContent = data.gentry1.toLocaleString();
//             document.getElementById("gantry2").textContent = data.gentry2.toLocaleString();
//             document.getElementById("gantry3").textContent = data.gentry3.toLocaleString();
//             document.getElementById("gantry4").textContent = data.gentry4.toLocaleString();
//             document.getElementById("timeFlow1").innerHTML = '<span id="addFlow1" class="text-secondary"><i class="fe fe-arrow-up-circle  text-secondary"></i> 0</span>' + data.time1;
//             document.getElementById("timeFlow2").innerHTML = '<span id="addFlow2" class="text-pink"><i class="fe fe-arrow-up-circle  text-pink"></i> 0</span>' + data.time2;
//             document.getElementById("timeFlow3").innerHTML = '<span id="addFlow3" class="text-success"><i class="fe fe-arrow-up-circle  text-success"></i> 0</span>' + data.time3;
//             document.getElementById("timeFlow4").innerHTML = '<span id="addFlow4" class="text-warning"><i class="fe fe-arrow-up-circle  text-warning"></i> 0</span>' + data.time4;
//             document.getElementById("addFlow1").innerHTML = '<i class="fe fe-arrow-up-circle  text-secondary"></i>' + data.add1.toLocaleString() + '&nbsp;&nbsp;&nbsp;';
//             document.getElementById("addFlow2").innerHTML = '<i class="fe fe-arrow-up-circle  text-pink"></i>' + data.add2.toLocaleString() + '&nbsp;&nbsp;&nbsp;';
//             document.getElementById("addFlow3").innerHTML = '<i class="fe fe-arrow-up-circle  text-success"></i>' + data.add3.toLocaleString() + '&nbsp;&nbsp;&nbsp;';
//             document.getElementById("addFlow4").innerHTML = '<i class="fe fe-arrow-up-circle  text-warning"></i>' + data.add4.toLocaleString() + '&nbsp;&nbsp;&nbsp;';
//         }
//     }
//     setTimeout(addFlowData, 5000);
// };

// addFlowData();

// function updateGantryStatus(gantryNumber, status) {
//     let gantryElement = document.getElementById("staG" + gantryNumber);
//     let progressBarElement = document.getElementById("staP" + gantryNumber);

//     if (status === "Run") {
//         gantryElement.innerHTML = '<span class="text-success">Running</span>';
//         progressBarElement.innerHTML = '<div class="progress-bar progress-bar-indeterminate bg-green"></div>';
//     } else if (status === "Idle") {
//         gantryElement.innerHTML = '<span class="text-danger">Stopped</span>';
//         progressBarElement.innerHTML = '<div class="progress-bar bg-green"></div>';
//     }
// }

// function addFlowStatus() {
//     var http = new XMLHttpRequest();
//     http.open("GET", "http://192.168.50.101:1880/getStatusGentry?", true);
//     http.send();

//     http.onreadystatechange = function () {
//         if (this.readyState == 4) {
//             if (this.status == 200) {
//                 var data = JSON.parse(this.responseText);

//                 // Memanggil fungsi updateGantryStatus untuk masing-masing gantry
//                 updateGantryStatus(1, data[0].Gantry1);
//                 updateGantryStatus(2, data[0].Gantry2);
//                 updateGantryStatus(3, data[0].Gantry3);
//                 updateGantryStatus(4, data[0].Gantry4);
//             } else {
//                 console.error('Gagal memuat data: ' + this.status);
//             }
//         }
//     }
//     setTimeout(addFlowStatus, 2000);
// };

// addFlowStatus();

function convertToEpoch(startDate, endDate) {
    var startParts = startDate.split('/');
    var endParts = endDate.split('/');
    var startDay = parseInt(startParts[0], 10);
    var startMonth = parseInt(startParts[1], 10) - 1;
    var startYear = parseInt(startParts[2], 10);
    var endDay = parseInt(endParts[0], 10);
    var endMonth = parseInt(endParts[1], 10) - 1;
    var endYear = parseInt(endParts[2], 10);
    var startDateTime = new Date(startYear, startMonth, startDay, 0, 0, 0, 0);
    var endDateTime = new Date(endYear, endMonth, endDay, 23, 59, 59, 999);
    return {
        start_time: Math.floor(startDateTime.getTime() / 1000),
        end_time: Math.floor(endDateTime.getTime() / 1000)
    };
}


function btnDownload() {
    var startDateInput = document.getElementById("min").value;
    var endDateInput = document.getElementById("max").value;

    if (!startDateInput || !endDateInput) {
        Swal.fire({
            icon: 'error',
            title: 'Oops...',
            text: 'Please fill in both start date and end date!',
        });
        return;
    } else if (startDateInput > endDateInput) {
        Swal.fire({
            icon: 'error',
            title: 'Oops...',
            text: 'Start date must be smaller than end date!',
        });
        return;
    }

    var epochTime = convertToEpoch(startDateInput, endDateInput);
    var a = epochTime.start_time;
    var b = epochTime.end_time;

    localStorage.setItem('startTimeFlow', a);
    localStorage.setItem('endTimeFlow', b);
    localStorage.setItem('startFlow', startDateInput);
    localStorage.setItem('endFlow', endDateInput);
    // location.reload();
}

// load awal
window.onload = function () {
    var storedStartTime = localStorage.getItem('startTimeFlow');
    var storedEndTime = localStorage.getItem('endTimeFlow');
    var start1 = localStorage.getItem('startFlow');
    var end1 = localStorage.getItem('endFlow');

    if (storedStartTime && storedEndTime) {
        document.getElementById('min').value = start1;
        document.getElementById('max').value = end1;

        var http = new XMLHttpRequest();
        http.open("GET", "http://192.168.50.101:1880/getTabelFlowmeter?data=2&start=" + storedStartTime + "&end=" + storedEndTime, true);
        http.send();

        http.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
                var data = JSON.parse(this.responseText);

                var existingContainer = document.getElementById('dataFlowmeterAll');
                existingContainer.innerHTML = '';

                var table = document.createElement('table');
                table.id = 'file-datatable';
                table.classList.add('table', 'table-bordered', 'text-nowrap', 'key-buttons', 'border-bottom');

                var thead = document.createElement('thead');
                thead.innerHTML = `
                        <tr>
                            <th class="border-bottom-0">Date</th>
                            <th class="border-bottom-0">Gantry</th>
                            <th class="border-bottom-0">Start</th>
                            <th class="border-bottom-0">Finish</th>
                            <th class="border-bottom-0">Duration</th>
                            <th class="border-bottom-0">Start Value (Raw)</th>
                            <th class="border-bottom-0">End Value (Raw)</th>
                            <th class="border-bottom-0">Refueling Value</th>
                        </tr>
                    `;

                var tbody = document.createElement('tbody');
                tbody.setAttribute('id', 'tabelFlow');
                var gentry1 = data.gentry1;
                var gentry2 = data.gentry2;
                var gentry3 = data.gentry3;
                var gentry4 = data.gentry4;
                //gentry 1
                for (var i = 0; i < gentry1.length; i++) {
                    var tr = document.createElement('tr');
                    for (var j = 0; j < 8; j++) {
                        var td = document.createElement('td');
                        switch (j) {
                            case 0:
                                td.textContent = gentry1[i].Date;
                                break;
                            case 1:
                                td.textContent = gentry1[i].ID;
                                break;
                            case 2:
                                td.textContent = gentry1[i].EPO_BEFORE;
                                break;
                            case 3:
                                td.textContent = gentry1[i].EPO_AFTER;
                                break;
                            case 4:
                                td.textContent = gentry1[i].DURATION;
                                break;
                            case 5:
                                td.textContent = gentry1[i].VAL_BEFORE.toLocaleString();
                                break;
                            case 6:
                                td.textContent = gentry1[i].VAL_AFTER.toLocaleString();
                                break;
                            case 7:
                                td.textContent = gentry1[i].Total_Val.toLocaleString();
                                break;
                            default:
                                break;
                        }
                        tr.appendChild(td);
                    }
                    tbody.appendChild(tr);
                }

                //gentry 2
                for (var i = 0; i < gentry2.length; i++) {
                    var tr = document.createElement('tr');
                    for (var j = 0; j < 8; j++) {
                        var td = document.createElement('td');
                        switch (j) {
                            case 0:
                                td.textContent = gentry2[i].Date;
                                break;
                            case 1:
                                td.textContent = gentry2[i].ID;
                                break;
                            case 2:
                                td.textContent = gentry2[i].EPO_BEFORE;
                                break;
                            case 3:
                                td.textContent = gentry2[i].EPO_AFTER;
                                break;
                            case 4:
                                td.textContent = gentry2[i].DURATION;
                                break;
                            case 5:
                                td.textContent = gentry2[i].VAL_BEFORE.toLocaleString();
                                break;
                            case 6:
                                td.textContent = gentry2[i].VAL_AFTER.toLocaleString();
                                break;
                            case 7:
                                td.textContent = gentry2[i].Total_Val.toLocaleString();
                                break;
                            default:
                                break;
                        }
                        tr.appendChild(td);
                    }
                    tbody.appendChild(tr);
                }

                //gentry 3
                for (var i = 0; i < gentry3.length; i++) {
                    var tr = document.createElement('tr');
                    for (var j = 0; j < 8; j++) {
                        var td = document.createElement('td');
                        switch (j) {
                            case 0:
                                td.textContent = gentry3[i].Date;
                                break;
                            case 1:
                                td.textContent = gentry3[i].ID;
                                break;
                            case 2:
                                td.textContent = gentry3[i].EPO_BEFORE;
                                break;
                            case 3:
                                td.textContent = gentry3[i].EPO_AFTER;
                                break;
                            case 4:
                                td.textContent = gentry3[i].DURATION;
                                break;
                            case 5:
                                td.textContent = gentry3[i].VAL_BEFORE.toLocaleString();
                                break;
                            case 6:
                                td.textContent = gentry3[i].VAL_AFTER.toLocaleString();
                                break;
                            case 7:
                                td.textContent = gentry3[i].Total_Val.toLocaleString();
                                break;
                            default:
                                break;
                        }
                        tr.appendChild(td);
                    }
                    tbody.appendChild(tr);
                }

                //gentry 4
                for (var i = 0; i < gentry4.length; i++) {
                    var tr = document.createElement('tr');
                    for (var j = 0; j < 8; j++) {
                        var td = document.createElement('td');
                        switch (j) {
                            case 0:
                                td.textContent = gentry4[i].Date;
                                break;
                            case 1:
                                td.textContent = gentry4[i].ID;
                                break;
                            case 2:
                                td.textContent = gentry4[i].EPO_BEFORE;
                                break;
                            case 3:
                                td.textContent = gentry4[i].EPO_AFTER;
                                break;
                            case 4:
                                td.textContent = gentry4[i].DURATION;
                                break;
                            case 5:
                                td.textContent = gentry4[i].VAL_BEFORE.toLocaleString();
                                break;
                            case 6:
                                td.textContent = gentry4[i].VAL_AFTER.toLocaleString();
                                break;
                            case 7:
                                td.textContent = gentry4[i].Total_Val.toLocaleString();
                                break;
                            default:
                                break;
                        }
                        tr.appendChild(td);
                    }
                    tbody.appendChild(tr);
                }
                table.appendChild(thead);
                table.appendChild(tbody);
                existingContainer.appendChild(table);
            }
            var script = document.createElement('script');
            script.src = './assets/js/table-data.js';
            document.head.appendChild(script);
        }
    }
};


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
function createChartFlow(id, data, timeseries, bColor, bgColor) {
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
                    // suggestedMin: 500,
                    beginAtZero: false,
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

var chartFlow1 = createChartFlow('flwm1', Data1, timeseries, color1[0], color1[1]);
var chartFlow2 = createChartFlow('flwm2', Data1, timeseries, color2[0], color2[1]);
var chartFlow3 = createChartFlow('flwm3', Data1, timeseries, color3[0], color3[1]);
var chartFlow4 = createChartFlow('flwm4', Data1, timeseries, color4[0], color4[1]);

function updateFlow(chart, time, data, max) {
    chart.data.labels = time;
    chart.data.datasets[0].data = data;
    // chart.options.scales.y.suggestedMax = max;
    chart.update(); // Memperbarui chart
}

function chartFlow() {
    var http = new XMLHttpRequest();
    http.open("GET", "http://192.168.50.101:1880/chartFlowmeter?", true);
    http.send();

    http.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            var data = JSON.parse(this.responseText);
            updateFlow(chartFlow1, data.timeA, data.valA, 6000);
            updateFlow(chartFlow2, data.timeB, data.valB, 6000);
            updateFlow(chartFlow3, data.timeC, data.valC, 4000);
            updateFlow(chartFlow4, data.timeD, data.valD, 4000);
        }
    }
    setTimeout(chartFlow, 5000);
};

chartFlow();

// function tabelFlow() {

// };

// tabelFlow();

function addFlowData() {
    var http = new XMLHttpRequest();
    http.open("GET", "http://192.168.50.101:1880/getDataFlow?", true);
    http.send();

    http.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            var data = JSON.parse(this.responseText);
            document.getElementById("gantry1").textContent = data.gentry1.toLocaleString();
            document.getElementById("gantry2").textContent = data.gentry2.toLocaleString();
            document.getElementById("gantry3").textContent = data.gentry3.toLocaleString();
            document.getElementById("gantry4").textContent = data.gentry4.toLocaleString();
            document.getElementById("timeFlow1").innerHTML = '<span id="addFlow1" class="text-secondary"><i class="fe fe-arrow-up-circle  text-secondary"></i> 0</span>' + data.time1;
            document.getElementById("timeFlow2").innerHTML = '<span id="addFlow2" class="text-pink"><i class="fe fe-arrow-up-circle  text-pink"></i> 0</span>' + data.time2;
            document.getElementById("timeFlow3").innerHTML = '<span id="addFlow3" class="text-success"><i class="fe fe-arrow-up-circle  text-success"></i> 0</span>' + data.time3;
            document.getElementById("timeFlow4").innerHTML = '<span id="addFlow4" class="text-warning"><i class="fe fe-arrow-up-circle  text-warning"></i> 0</span>' + data.time4;
            document.getElementById("addFlow1").innerHTML = '<i class="fe fe-arrow-up-circle  text-secondary"></i>' + data.add1.toLocaleString() + '&nbsp;&nbsp;&nbsp;';
            document.getElementById("addFlow2").innerHTML = '<i class="fe fe-arrow-up-circle  text-pink"></i>' + data.add2.toLocaleString() + '&nbsp;&nbsp;&nbsp;';
            document.getElementById("addFlow3").innerHTML = '<i class="fe fe-arrow-up-circle  text-success"></i>' + data.add3.toLocaleString() + '&nbsp;&nbsp;&nbsp;';
            document.getElementById("addFlow4").innerHTML = '<i class="fe fe-arrow-up-circle  text-warning"></i>' + data.add4.toLocaleString() + '&nbsp;&nbsp;&nbsp;';
        }
    }
    setTimeout(addFlowData, 5000);
};

addFlowData();

function updateGantryStatus(gantryNumber, status) {
    let gantryElement = document.getElementById("staG" + gantryNumber);
    let progressBarElement = document.getElementById("staP" + gantryNumber);

    if (status === "Run") {
        gantryElement.innerHTML = '<span class="text-success">Running</span>';
        progressBarElement.innerHTML = '<div class="progress-bar progress-bar-indeterminate bg-green"></div>';
    } else if (status === "Idle") {
        gantryElement.innerHTML = '<span class="text-danger">Stopped</span>';
        progressBarElement.innerHTML = '<div class="progress-bar bg-green"></div>';
    }
}

function addFlowStatus() {
    var http = new XMLHttpRequest();
    http.open("GET", "http://192.168.50.101:1880/getStatusGentry?", true);
    http.send();

    http.onreadystatechange = function () {
        if (this.readyState == 4) {
            if (this.status == 200) {
                var data = JSON.parse(this.responseText);

                // Memanggil fungsi updateGantryStatus untuk masing-masing gantry
                updateGantryStatus(1, data[0].Gantry1);
                updateGantryStatus(2, data[0].Gantry2);
                updateGantryStatus(3, data[0].Gantry3);
                updateGantryStatus(4, data[0].Gantry4);
            } else {
                console.error('Gagal memuat data: ' + this.status);
            }
        }
    }
    setTimeout(addFlowStatus, 2000);
};

addFlowStatus();

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
    location.reload();
}

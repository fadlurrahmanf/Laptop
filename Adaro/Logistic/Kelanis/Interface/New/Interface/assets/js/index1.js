// load awal
document.addEventListener('DOMContentLoaded', function () {
    const sidebarToggle = document.querySelector('.app-sidebar__toggle');
    const isMobileDevice = /Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Ope/i.test(navigator.userAgent);
    if (sidebarToggle && window.innerWidth > 768) {
        sidebarToggle.click();
    }
    var http = new XMLHttpRequest();
    http.open("GET", "http://192.168.50.101:1880/getArrayData?data=0", true);
    http.send();
    chartTank();

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

// CHART LINE
function createTankChart(id, data, maxY) {
    var ctx = document.getElementById(id);
    return new Chart(ctx, {
        type: 'line',
        data: {
            labels: timeseries,
            datasets: [{
                label: id,
                borderColor: "#00ff04",
                borderWidth: 4,
                lineTension: 0,
                backgroundColor: "rgba(64, 255, 0, .2)",
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
                    suggestedMax: maxY,
                    beginAtZero: true,
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
var tank1Data = [14, 10, 7, 4, 10, 8, 11, 5, 11, 8, 5, 4, 8, 7, 0, 10, 5, 2, 14, 19, 9, 14, 5, 13];

var chartTank1 = createTankChart('tank1', tank1Data, 1500000);
var chartTank2 = createTankChart('tank2', tank1Data, 1500000);
var chartTank3 = createTankChart('tank3', tank1Data, 600000);
var chartTank4 = createTankChart('tank4', tank1Data, 600000);

function updateChart(chart, time, data, max) {
    chart.data.labels = time;
    chart.data.datasets[0].data = data;
    // chart.options.scales.y.suggestedMax = max;
    chart.update(); // Memperbarui chart
}

function chartTank() {
    var http = new XMLHttpRequest();
    http.open("GET", "http://192.168.50.101:1880/getArrayData?data=1", true);
    http.send();

    http.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            var data = JSON.parse(this.responseText);
            timeseries = data.time;
            updateChart(chartTank1, timeseries, data.tank1, 6000);
            updateChart(chartTank2, timeseries, data.tank2, 6000);
            updateChart(chartTank3, timeseries, data.tank3, 4000);
            updateChart(chartTank4, timeseries, data.tank4, 4000);
        }
    }
    setTimeout(chartTank, 5000);
};

// button dropdown
function btn_sub(ID) {
    const dropdownItems = document.querySelectorAll(`#${ID}Dropdown .dropdown-item`);
    var http = new XMLHttpRequest();
    dropdownItems.forEach((item) => {
        item.addEventListener('click', () => {
            const selectedValue = item.getAttribute('data-value');
            http.open("GET", "http://192.168.50.101:1880/getArrayData?data=" + selectedValue, true);
            http.send();
            http.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    var data = JSON.parse(this.responseText);
                    timeseries = data.time;
                    updateChart(chartTank1, timeseries, data.tank1);
                    updateChart(chartTank2, timeseries, data.tank2);
                    updateChart(chartTank3, timeseries, data.tank3);
                    updateChart(chartTank4, timeseries, data.tank4);
                }
            }
            document.getElementById(ID).textContent = selectedValue;
        });
    });
}


// CHART GAUGE
function createGauge(ID, data, max, color, label, dci, judul) {
    return new JustGage({
        id: ID,
        value: data,
        levelColors: color,
        min: 0,
        max: max,
        height: 150,
        width: 150,
        label: label,
        decimals: dci,
        gaugeWidthScale: 0.5,
        counter: true,
        formatNumber: true,
        titleMinFontSize: 16,
        title: judul,
        labelMinFontSize: 14
    });
}

var colorFuel = ["#40ff00"];
var colorTemp = ["#ffd500"];
var colorDens = ["#00eaff"];

// var chartFuel1 = createGauge('fuel1', 372, 1500000, colorFuel, "Liter", 0, "Fuel");
// var chartFuel2 = createGauge('fuel2', 720, 1500000, colorFuel, "Liter", 0, "Fuel");
// var chartFuel3 = createGauge('fuel3', 576, 600000, colorFuel, "Liter", 0, "Fuel");
// var chartFuel4 = createGauge('fuel4', 105, 600000, colorFuel, "Liter", 0, "Fuel");

// var chartTemp1 = createGauge('temp1', 27, 100, colorTemp, "Celcius", 0, "Temp");
// var chartTemp2 = createGauge('temp2', 38, 100, colorTemp, "Celcius", 0, "Temp");
// var chartTemp3 = createGauge('temp3', 47, 100, colorTemp, "Celcius", 0, "Temp");
// var chartTemp4 = createGauge('temp4', 72, 100, colorTemp, "Celcius", 0, "Temp");

// var chartDens1 = createGauge('dens1', 0.95, 2, colorDens, "kg/liter", 2, "Density");
// var chartDens2 = createGauge('dens2', 0.85, 2, colorDens, "kg/liter", 2, "Density");
// var chartDens3 = createGauge('dens3', 0.72, 2, colorDens, "kg/liter", 2, "Density");
// var chartDens4 = createGauge('dens4', 1.32, 2, colorDens, "kg/liter", 2, "Density");

function refreshGauge(gauge, value) {
    gauge.refresh(value);
}

// get data
var fuel = ['Tangki1', 'Tangki2', 'Tangki3', 'Tangki4'];
var temp = ['TempTangki1', 'TempTangki2', 'TempTangki3', 'TempTangki4'];
var dsg = ['DSG1', 'DSG2', 'DSG3', 'DSG4'];

function start() {
    var http = new XMLHttpRequest();
    http.open("GET", "http://192.168.50.101:1880/getCurrentData", true);
    http.send();

    http.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            var data = JSON.parse(this.responseText);
            for (var i = 0; i < 4; i++) {
                if (data[0][fuel[i]] == -1) {
                    data[0][fuel[i]] = 0;
                } if (data[0][temp[i]] == -1) {
                    data[0][temp[i]] = 0;
                } if (data[0][dsg[i]] == -1) {
                    data[0][dsg[i]] = 0;
                }
                if (i > 1) {
                    document.getElementById('fuel' + [i + 1]).style.setProperty('--myheigt', `${literToPx(data[0][fuel[i]], 600000)}px`)
                    document.getElementById('fuel' + [i + 1]).textContent = data[0][fuel[i]].toLocaleString() + ' L'
                } else if (i <= 1) {
                    document.getElementById('fuel' + [i + 1]).style.setProperty('--myheigt', `${literToPx(data[0][fuel[i]], 1500000)}px`)
                    document.getElementById('fuel' + [i + 1]).textContent = data[0][fuel[i]].toLocaleString() + ' L'
                }
                document.getElementById('temp' + [i + 1]).innerText = String(data[0][temp[i]]) + "°C";
                document.getElementById('dens' + [i + 1]).innerHTML = String(data[0][dsg[i]]) + "g/cm<sup>3</sup>";
            }
            setTimeout(start, 5000);
        }
    };
}

start();

//FUNCTION ADD
function ltopx(liters) {
    var myheight = Math.round(-0.00007 * liters - 70);
    return myheight;
}
function literToPx(liter, literMax) {
    var pxMin = -70;
    var pxMax = -175;
    var literMin = 0;
    var pxValue = pxMin + ((liter - literMin) / (literMax - literMin)) * (pxMax - pxMin);
    return pxValue;
}

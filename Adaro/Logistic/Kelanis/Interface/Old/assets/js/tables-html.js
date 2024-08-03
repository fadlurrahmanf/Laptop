// load awal
window.onload = function () {
    var storedStartTime = localStorage.getItem('startTime');
    var storedEndTime = localStorage.getItem('endTime');
    var start1 = localStorage.getItem('start');
    var end1 = localStorage.getItem('end');

    if (storedStartTime && storedEndTime) {
        document.getElementById('min').value = start1;
        document.getElementById('max').value = end1;

        var http = new XMLHttpRequest();
        http.open("GET", "http://192.168.50.101:1880/getTabelData?data=2&start=" + storedStartTime + "&end=" + storedEndTime, true);
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
                        <th class="border-bottom-0">Time</th>
                        <th class="border-bottom-0">Fuel Tank 1</th>
                        <th class="border-bottom-0">Fuel Tank 2</th>
                        <th class="border-bottom-0">Fuel Tank 3</th>
                        <th class="border-bottom-0">Fuel Tank 4</th>
                        <th class="border-bottom-0">Temperatur 1</th>
                        <th class="border-bottom-0">Temperatur 2</th>
                        <th class="border-bottom-0">Temperatur 3</th>
                        <th class="border-bottom-0">Temperatur 4</th>
                        <th class="border-bottom-0">Density 1</th>
                        <th class="border-bottom-0">Density 2</th>
                        <th class="border-bottom-0">Density 3</th>
                        <th class="border-bottom-0">Density 4</th>
                    </tr>
                `;

                var tbody = document.createElement('tbody');
                tbody.setAttribute('id', 'dataTabel');

                for (var i = 0; i < data.length; i++) {
                    var tr = document.createElement('tr');
                    for (var j = 0; j < 14; j++) {
                        var td = document.createElement('td');
                        switch (j) {
                            case 0:
                                td.textContent = data[i].Date;
                                break;
                            case 1:
                                td.textContent = data[i].Waktu;
                                break;
                            case 2:
                                td.textContent = data[i].Tangki1;
                                break;
                            case 3:
                                td.textContent = data[i].Tangki2;
                                break;
                            case 4:
                                td.textContent = data[i].Tangki3;
                                break;
                            case 5:
                                td.textContent = data[i].Tangki4;
                                break;
                            case 6:
                                td.textContent = data[i].TempTangki1;
                                break;
                            case 7:
                                td.textContent = data[i].TempTangki2;
                                break;
                            case 8:
                                td.textContent = data[i].TempTangki3;
                                break;
                            case 9:
                                td.textContent = data[i].TempTangki4;
                                break;
                            case 10:
                                td.textContent = data[i].DSG1;
                                break;
                            case 11:
                                td.textContent = data[i].DSG2;
                                break;
                            case 12:
                                td.textContent = data[i].DSG3;
                                break;
                            case 13:
                                td.textContent = data[i].DSG4;
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
            script.src = './assets/js/table-data.js'; // Sesuaikan dengan nama file yang benar
            document.head.appendChild(script);
        }
    }
}

document.addEventListener('DOMContentLoaded', function () {
    const sidebarToggle = document.querySelector('.app-sidebar__toggle');
    const isMobileDevice = /Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Ope/i.test(navigator.userAgent);
    if (sidebarToggle && window.innerWidth > 768) {
        sidebarToggle.click();
    }

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

    localStorage.setItem('startTime', a);
    localStorage.setItem('endTime', b);
    localStorage.setItem('start', startDateInput);
    localStorage.setItem('end', endDateInput);
    location.reload();
}


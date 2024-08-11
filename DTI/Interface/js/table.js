document.addEventListener('DOMContentLoaded', function () {
    setTimeout(function () {
        const tables = document.getElementById('tables');
        tables.classList.add('active');
    }, 2);
});

let jsontabel = [];
function gm(mon) {
    let d = Date.parse(mon + "1, 2012");
    if (!isNaN(d)) {
        let res = "";
        if (String(new Date(d).getMonth() + 1).length < 2) {
            res = "0" + String(new Date(d).getMonth() + 1);
        } else {
            res = String(new Date(d).getMonth() + 1);
        }
        return res;
    }
    return -1;
}

function Search() {
    let btnSearch = document.getElementById('btn-search');
    let from = ((new Date(document.getElementById("from").value)).getTime());
    let to = ((new Date(document.getElementById("to").value)).getTime());
    if (from < to) {
        btnSearch.innerHTML = "<i class='bx bx-loader-alt bx-spin bx-md-3'></i> Searching";
        let http = new XMLHttpRequest();
        http.open("GET", "https://api-dti.azycloud.my.id/GetDataFromTo?From=" + String(from) + "&To=" + String(to), true);
        http.send();
        http.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
                let data = JSON.parse(this.responseText);
                jsontabel = data;
                let len = data.length;
                let column = Object.keys(data[0]);
                let table = "<table class='table-auto border-collapse text-center text-[0.8vw]'>";
                table += "<tr>";

                for (i = 0; i < column.length; i++) {

                    table += "<th class='p-[0.5vw] border-y-[0.2vw] border-x-[0.2vw] border-slate-500'>";
                    if (i == 0) {
                        table += "Date";
                    } else {
                        table += String(column[i]);
                    }
                    table += "</th>";
                }
                table += "</tr>";
                for (i = 0; i < data.length; i++) {
                    table += "<tr>";
                    for (j = 0; j < column.length; j++) {
                        table += "<td class='p-[0.5vw] border-y-[0.2vw] border-x-[0.2vw] border-slate-500'>";
                        if (j == 0) {
                            table += String(new Date(data[i][column[j]])).split(" ")[2] + "/" + String(gm(String(new Date(data[i][column[j]])).split(" ")[1])) + "/" + String(new Date(data[i][column[j]])).split(" ")[3] + " " + String(new Date(data[i][column[j]])).split(" ")[4].substring(0, 5);
                        } else {
                            table += (data[i][column[j]]);
                        }
                        table += "</td>";
                    }
                    table += "</tr>";
                }
                table += "</table>";
                document.getElementById("tableku").innerHTML = table;
            }
            btnSearch.innerHTML = "Search";
        }
    } else {
        Swal.fire({
            icon: 'error',
            title: 'Invalid Date!',
        });
    }
}
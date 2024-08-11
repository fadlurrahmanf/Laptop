function toExcel(tableID, filename = '') {
    const btnDownload= document.getElementById('btn-download');
    btnDownload.innerHTML = "<i class='bx bx-loader-alt bx-spin bx-md-3'></i> Downloading"
    let tableSelect = document.getElementById(tableID);

    if (!tableSelect) {
        console.error("Table element with ID '" + tableID + "' not found.");
        return;
    }

    let tableHTML = tableSelect.outerHTML;
    let workbook = XLSX.read(tableHTML, { type: 'string' });
    let excelData = XLSX.write(workbook, { bookType: 'xlsx', type: 'binary' });

    filename = filename ? filename + '.xlsx' : 'excel_data.xlsx';

    let downloadLink = document.createElement("a");
    document.body.appendChild(downloadLink);

    let blob = new Blob([s2ab(excelData)], { type: 'application/vnd.openxmlformats-officedocument.spreadsheetml.sheet' });
    downloadLink.href = window.URL.createObjectURL(blob);
    downloadLink.download = filename;
    downloadLink.click();
    document.body.removeChild(downloadLink);
    btnDownload.innerHTML = 'Downloaded';
    setTimeout(function() {
        btnDownload.innerHTML = 'Download';
    }, 2000);
}

function s2ab(s) {
    let buf = new ArrayBuffer(s.length);
    let view = new Uint8Array(buf);
    for (let i = 0; i != s.length; ++i) view[i] = s.charCodeAt(i) & 0xFF;
    return buf;
}

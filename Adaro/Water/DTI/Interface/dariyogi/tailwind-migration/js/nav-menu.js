document.addEventListener("DOMContentLoaded", function () {
    const navMenu = document.getElementById('nav-menu');
    const myHeader = document.getElementById('my-header');

    const hdContent = `
        <a href="../index.html" class="flex justify-center items-center">
            <img src="../img/logo.png" class="h-6 mr-3 border-r-2 pr-3 border-slate-300" alt="logo-wtp">
            <h1 class="flex justify-center items-center h-6 mr-3 border-r-2 pr-3 border-slate-300 font-semibold text-slate-500 text-xs">
                Drupadi Tirta Intan</h1>
            <h1 class="flex justify-center items-center h-6 p-3 rounded-md bg-cyan-500 text-white text-center font-medium text-xs">
                WTP</h1>
        </a>`;

    const navContent = `
        <nav class="bg-white absolute z-50 h-[450px] w-[200px] m-[3vh] rounded-md flex items-center pl-6 shadow-md">
            <ul class="block">
                <li class="group">
                    <a href="../index.html" class="list-menu" id="dashboard">
                        <span class="pr-3">
                            <svg xmlns="http://www.w3.org/2000/svg" class="fill-slate-500" width="24" height="24"
                                viewBox="0 0 24 24">
                                <path d="M4 13h6a1 1 0 0 0 1-1V4a1 1 0 0 0-1-1H4a1 1 0 0 0-1 1v8a1 1 0 0 0 1
                                    1zm-1 7a1 1 0 0 0 1 1h6a1 1 0 0 0 1-1v-4a1 1 0 0 0-1-1H4a1 1 0 0 0-1
                                    1v4zm10 0a1 1 0 0 0 1 1h6a1 1 0 0 0 1-1v-7a1 1 0 0 0-1-1h-6a1 1 0 0 0-1
                                    1v7zm1-10h6a1 1 0 0 0 1-1V4a1 1 0 0 0-1-1h-6a1 1 0 0 0-1 1v5a1 1 0 0 0 1 1z" />
                            </svg>
                        </span> Dashboard</a>
                </li>
                <li class="group">
                    <a href="../pages/overview.html" class="list-menu" id="overview">
                        <span class="pr-3">
                            <svg xmlns="http://www.w3.org/2000/svg" class="fill-slate-500" width="24" height="24"
                                viewBox="0 0 24 24">
                                <path d="M19 3H5c-1.103 0-2 .897-2 2v14c0 1.103.897 2 2 2h8a.996.996 0 0 0
                                    .707-.293l7-7a.997.997 0 0 0 .196-.293c.014-.03.022-.061.033-.093a.991.991
                                    0 0 0 .051-.259c.002-.021.013-.041.013-.062V5c0-1.103-.897-2-2-2zM5
                                    5h14v7h-6a1 1 0 0 0-1 1v6H5V5zm9 12.586V14h3.586L14 17.586z" />
                            </svg>
                        </span> Overview</a>
                </li>
                <li class="group">
                    <a href="../pages/dosing.html" class="list-menu" id="dosing">
                        <span class="pr-3">
                            <svg xmlns="http://www.w3.org/2000/svg" class="fill-slate-500" width="24" height="24"
                                viewBox="0 0 24 24">
                                <path d="M12 22c5.131 0 9-1.935 9-4.5V7h-.053c.033-.164.053-.33.053-.5C21 3.935
                                17.131 2 12 2 7.209 2 3.52 3.688 3.053 6H3v11.5c0 2.565 3.869 4.5 9 4.5zm0-2c-4.273
                                0-7-1.48-7-2.5V9.394C6.623 10.387 9.111 11 12 11s5.377-.613 7-1.606V17.5c0 1.02-2.727
                                2.5-7 2.5zm0-16c4.273 0 7 1.48 7 2.5S16.273 9 12 9 5 7.52 5 6.5 7.727 4 12 4z" />
                            </svg>
                        </span> Dosing</a>
                </li>
                <li class="group">
                    <a href="../pages/clarifier.html" class="list-menu" id="clarifier">
                        <span class="pr-3">
                            <svg xmlns="http://www.w3.org/2000/svg" class="fill-slate-500" width="24" height="24"
                                viewBox="0 0 24 24">
                                <path d="M18.277 8c.347.596.985 1 1.723 1a2 2 0 0 0 0-4c-.738 0-1.376.404-1.723
                                    1H16V4a1 1 0 0 0-1-1H9a1 1 0 0 0-1 1v2H5.723C5.376 5.404 4.738 5 4 5a2 2 0 0 0 0
                                    4c.738 0 1.376-.404 1.723-1H8v.369C5.133 9.84 4.318 12.534 4.091 14H3a1 1 0 0 0-1
                                    1v4a1 1 0 0 0 1 1h4a1 1 0 0 0 1-1v-4a1 1 0 0 0-1-1h-.877c.197-.959.718-2.406
                                    2.085-3.418A.984.984 0 0 0 9 11h6a.98.98 0 0 0 .792-.419c1.373 1.013 1.895 2.458
                                    2.089 3.419H17a1 1 0 0 0-1 1v4a1 1 0 0 0 1 1h4a1 1 0 0 0 1-1v-4a1 1 0 0
                                    0-1-1h-1.092c-.227-1.466-1.042-4.161-3.908-5.632V8h2.277zM6 18H4v-2h2v2zm14
                                    0h-2v-2h2v2zm-6-9h-4V5h4v4z" />
                            </svg>
                        </span> Clarifier</a>
                </li>
                <li class="group">
                    <a href="../pages/filter.html" class="list-menu" id="filter">
                        <span class="pr-3">
                            <svg xmlns="http://www.w3.org/2000/svg" class="fill-slate-500" width="24" height="24"
                                viewBox="0 0 24 24">
                                <path d="M3 4v5h2V5h4V3H4a1 1 0 0 0-1 1zm18 5V4a1 1 0 0 0-1-1h-5v2h4v4h2zm-2
                                10h-4v2h5a1 1 0 0 0 1-1v-5h-2v4zM9 21v-2H5v-4H3v5a1 1 0 0 0 1 1h5zM2 11h20v2H2z" />
                            </svg>
                        </span> Filter</a>
                </li>
                <li class="group">
                    <a href="../pages/reservoir.html" class="list-menu" id="reservoir">
                        <span class="pr-3">
                            <svg xmlns="http://www.w3.org/2000/svg" class="fill-slate-500" width="24" height="24"
                                viewBox="0 0 24 24">
                                <path d="M20 3H4a2 2 0 0 0-2 2v2a2 2 0 0 0 1 1.72V19a2 2 0 0 0 2 2h14a2 2 0 0 0 2-2V8.72A2
                                    2 0 0 0 22 7V5a2 2 0 0 0-2-2zM4 5h16v2H4zm1 14V9h14v10z" />
                                <path d="M8 11h8v2H8z" />
                            </svg>
                        </span> Reservoir</a>
                </li>
                <li class="group">
                    <a href="../pages/table.html" class="list-menu" id="tables">
                        <span class="pr-3">
                            <svg xmlns="http://www.w3.org/2000/svg" class="fill-slate-500" width="24" height="24"
                                viewBox="0 0 24 24">
                                <path d="M19 3H5c-1.103 0-2 .897-2 2v14c0 1.103.897 2 2 2h14c1.103 0 2-.897
                                2-2V5c0-1.103-.897-2-2-2zm0 2 .001 4H5V5h14zM5 11h8v8H5v-8zm10 8v-8h4.001l.001
                                8H15z" />
                            </svg>
                        </span> Tables</a>
                </li>
            </ul>
        </nav>
    `;
    myHeader.innerHTML = hdContent;
    navMenu.innerHTML = navContent;
});

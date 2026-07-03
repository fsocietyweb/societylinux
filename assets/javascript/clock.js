/*

Clock Widget By 404City. Visit https://404city.neocities.org/ For More!
Copyright 404City All Rights Reserved

*/
(function() {
    const scriptTag = document.currentScript;
    scriptTag.insertAdjacentHTML('beforebegin', "<!--Clock Widget By 404City. Visit https://404city.neocities.org/ for more!--><!--Copyright 404City All Rights Reserved-->");
    scriptTag.insertAdjacentHTML('afterend', `
        <div id="clock">
            <div id="clockDisplay" data-seconds="" data-ampm=""></div>
            <div id="dateDisplay"></div>
        </div>
        <style>
            :where(#clock) {
                height: 100px;
                width: 300px;
                box-sizing: border-box;
                display: flex;
                align-items: center;
                justify-content: center;
                flex-direction: column;
                border: 1px solid black;
                font-size: 16px;
                font-family: Arial, Helvetica, sans-serif;
                user-select: none
            }
            :where(#clockDisplay) {
                box-sizing: border-box;
                font-size: 3em;
            }
            :where(#clockDisplay)::after {
                content: attr(data-seconds) attr(data-ampm);
            }
            :where(#dateDisplay) {
                box-sizing: border-box;
            }
        </style>
        <!--End Of The Clock Widget-->
    `);
    renderClock();
    setInterval(renderClock, 1000);
    function checkTime(i) {if (i < 10) {i = "0" + i;} return i;}
    function renderClock() {
        var today = new Date();
        var hr = today.getHours();
        var clockDisplay = document.getElementById("clockDisplay");
        if(scriptTag.getAttribute("data-seconds") == "true") {
            clockDisplay.setAttribute("data-seconds", ":" + checkTime(today.getSeconds()));
        }
        if(scriptTag.getAttribute("data-format") == "12h") {
            clockDisplay.setAttribute("data-ampm", (hr < 12) ? " AM" : " PM");
            hr = (hr == 0) ? 12 : hr;
            hr = (hr > 12) ? hr - 12 : hr;
        }
        hr = checkTime(hr);
        clockDisplay.innerText = hr + ":" + checkTime(today.getMinutes());
        document.getElementById("dateDisplay").innerText = today.getDate() + " " + ['January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October', 'November', 'December'][today.getMonth()] + " " + today.getFullYear() + " " + ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'][today.getDay()];
    }
})();
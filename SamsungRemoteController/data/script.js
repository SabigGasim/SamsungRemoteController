const ModeIndices = {
    Auto: 0,
    Cooling: 1,
    Dry: 2,
    Fan: 3,
    Heating: 4
}

const Modes = ["AUTO", "COOL", "DRY", "FAN", "HEAT"];

const FanSpeeds = {
    Auto: 0,
    Low: 1,
    Medium: 2,
    High: 3,
    VeryHigh: 4
}

// Initial settings
let powerOn = true;
let currentTemp = 20;
let currentWindSpeed = FanSpeeds.VeryHigh;
let currentMode = ModeIndices.Cooling;
let isDragging = false;
let sliderRect;

const esp32Url = "http://192.168.1.169";

// Get elements
const tempValueEl = document.getElementById('temp-value');
const coolingStatusEl = document.getElementById('cooling-status');
const powerButtonEl = document.getElementById('power-button');
const windSpeedButtonEl = document.getElementById('wind-speed-button');
const autoSwingButtonEl = document.getElementById('auto-swing-button');
const modeButtonEl = document.getElementById('mode-button');
const slider = document.querySelector('.slider');
const sliderFill = document.getElementById('sliderFill');
const sliderLabel = document.getElementById('sliderLabel');
const bars = [
    document.getElementById('bar-1'),
    document.getElementById('bar-2'),
    document.getElementById('bar-3'),
    document.getElementById('bar-4')
];

// Function to update temperature display
function updateTemperature() {
    tempValueEl.textContent = currentTemp;
}

// Function to toggle power
function togglePower() {
    powerOn = !powerOn;
    postUpdates("power", {"powerOn": powerOn}).then(updatePower);
}

function updatePower(){
    console.log("power is: " + powerOn);
    if (powerOn) {
        updateMode();
    } else {
        coolingStatusEl.textContent = 'OFF';
        coolingStatusEl.style.color = '#ff4f4f';
    }
}

// Function to change wind speed
function changeWindSpeed() {
    if(currentWindSpeed == FanSpeeds.VeryHigh)
        currentWindSpeed = FanSpeeds.Auto;
    else
        currentWindSpeed++;

    postUpdates("fan", {"fan": currentWindSpeed}).then(updateWindSpeed);
}

function changeMode(){
    if(!powerOn)
        return

    if(currentMode == Modes.length - 1)
        currentMode = 0;
    else
        currentMode++;
    
    postUpdates("mode", {"mode": currentMode}).then(updateMode);
}

function toggleSwing(){
    postUpdates("toggle-swing", null);
}

// Function to update wind speed display
function updateWindSpeed() {
    bars.forEach((bar, index) => {
        if (index < currentWindSpeed) {
            bar.classList.add('active');
        } else {
            bar.classList.remove('active');
        }
    });
}

// Function to update slider fill based on sliding percentage
function updateSliderFill(clientY) {
    let newY = clientY - sliderRect.top;

    let fillPercentage = Math.floor(((sliderRect.height - newY) / sliderRect.height * 100)/ 5.71) * 5.71;
    if (fillPercentage < 20) fillPercentage = 20;
    if (fillPercentage > 100) fillPercentage = 100;
    currentTemp = Math.floor((fillPercentage-20)/80*14+16);

    sliderFill.style.height = `${fillPercentage}%`;
    sliderLabel.textContent = `${currentTemp}°C`
    tempValueEl.textContent = currentTemp;
}

function updateSliderFillByTemp(temp) {
    let fillPercentage = Math.floor(sliderRect.height * 0.2 + (temp - 16) * 5.71);
    if (fillPercentage > 100)
         fillPercentage = 100;

    sliderFill.style.height = `${fillPercentage}%`;
    sliderLabel.textContent = `${temp}°C`
    tempValueEl.textContent = currentTemp;
}

function updateMode(){
    if(currentMode == ModeIndices.Heating)
        coolingStatusEl.style.color = '#ff7700'
    else
        coolingStatusEl.style.color = '#4ade80'
    coolingStatusEl.textContent = Modes[currentMode];
}

function onSliderMouseDown(e){
    sliderRect = slider.getBoundingClientRect();
    let y = e.clientY ? e.clientY : e.touches[0].clientY;
    isDragging = true;
    updateSliderFill(y);
    updateTemperature();
}

function onSliderMouseMove(e){
    if (!isDragging) 
        return;
    let y = e.clientY ? e.clientY : e.touches[0].clientY;
    updateSliderFill(y);
    updateTemperature();
}

function onSliderMouseUp(){
    if(isDragging){
        postUpdates("temp", {"temp": currentTemp});
        isDragging = false;
    }

}

function modeString(modeNum){
    switch(modeNum){
        case 0: return Modes.Auto;
        case 1: return Modes.Cooling;
        case 2: return Modes.Dry;
        case 3: return Modes.Fan;
        case 4: return Modes.Heating;
    }
}

function UpdateUI(data) {
    sliderRect ??= slider.getBoundingClientRect();
    powerOn = data.PowerOn;
    currentTemp = data.Temp;
    currentWindSpeed = data.Fan;
    currentMode = data.Mode;

    updateMode();
    updateSliderFillByTemp(currentTemp);
    updateWindSpeed();
    updatePower();
}

async function fetchData() {
    try {
        const response = await fetch(`${esp32Url}/data`);
        if (response.ok) {
            const data = await response.json();
            return data;
        } else {
            console.error("Failed to fetch data:", response.status);
        }
    } catch (error) {
        console.error("Error:", error);
    }

    return null;
}

async function postUpdates(endpoint, body) {
    let url = `${esp32Url}/${endpoint}`;
    console.log(url);
    try {
        await fetch(`${esp32Url}/${endpoint}`, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(body)
        });
    } catch (error) {
        console.error("Error:", error);
    }
}



// Event Listeners
powerButtonEl.addEventListener('click', togglePower);
windSpeedButtonEl.addEventListener('click', changeWindSpeed);
autoSwingButtonEl.addEventListener('click', toggleSwing)
modeButtonEl.addEventListener('click', changeMode);
slider.addEventListener('mousedown', onSliderMouseDown);
window.addEventListener('mousemove', onSliderMouseMove);
window.addEventListener('mouseup', onSliderMouseUp);
slider.addEventListener('touchstart', onSliderMouseDown);
window.addEventListener('touchmove', onSliderMouseMove);
window.addEventListener('touchend', onSliderMouseUp);



// Initial update
fetchData()
    .then(data => {
        if (data == null) {
            return;
        }
        UpdateUI(data);
    })
    .catch(error => {
        console.error('Error fetching data:', error);
    });
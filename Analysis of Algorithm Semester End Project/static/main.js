const canvas = document.getElementById('gameCanvas');
const ctx = canvas.getContext('2d');
const generateBtn = document.getElementById('generateBtn');
const bruteForceBtn = document.getElementById('bruteForceBtn');
const dncBtn = document.getElementById('dncBtn');
const showClosestBtn = document.getElementById('showClosestBtn');
const pointCountSpan = document.getElementById('pointCount');
const bfTimeSpan = document.getElementById('bfTime');
const dncTimeSpan = document.getElementById('dncTime');
const minDistSpan = document.getElementById('minDist');
const resultMessage = document.getElementById('resultMessage');

let points = [];
let closestPairData = null;

// Canvas settings
const POINT_RADIUS = 4;
const POINT_COLOR = '#3b82f6'; // blue-500
const HIGHLIGHT_COLOR = '#ef4444'; // red-500
const LINE_COLOR = '#10b981'; // emerald-500

function clearCanvas() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
}

function drawPoint(x, y, color = POINT_COLOR, radius = POINT_RADIUS) {
    ctx.beginPath();
    ctx.arc(x, y, radius, 0, Math.PI * 2);
    ctx.fillStyle = color;
    ctx.fill();
    ctx.closePath();
}

function drawLine(p1, p2, color = LINE_COLOR) {
    ctx.beginPath();
    ctx.moveTo(p1[0], p1[1]);
    ctx.lineTo(p2[0], p2[1]);
    ctx.strokeStyle = color;
    ctx.lineWidth = 2;
    ctx.stroke();
    ctx.closePath();
}

function drawPoints() {
    clearCanvas();
    points.forEach(p => drawPoint(p[0], p[1]));
}

function resetUI() {
    closestPairData = null;
    bfTimeSpan.textContent = '-- ms';
    dncTimeSpan.textContent = '-- ms';
    minDistSpan.textContent = '--';
    resultMessage.textContent = '';
    bruteForceBtn.disabled = false;
    dncBtn.disabled = false;
    showClosestBtn.disabled = true;
    drawPoints();
}

generateBtn.addEventListener('click', async () => {
    try {
        const response = await fetch('/generate', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ num_points: 50 })
        });
        points = await response.json();
        pointCountSpan.textContent = points.length;
        resetUI();
    } catch (error) {
        console.error('Error generating points:', error);
        resultMessage.textContent = 'Error generating points.';
    }
});

async function runAlgorithms() {
    if (points.length < 2) return;

    try {
        const response = await fetch('/closest', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ points: points })
        });
        closestPairData = await response.json();
        
        // Enable show button
        showClosestBtn.disabled = false;
        
        return closestPairData;
    } catch (error) {
        console.error('Error running algorithms:', error);
        resultMessage.textContent = 'Error running algorithms.';
        return null;
    }
}

bruteForceBtn.addEventListener('click', async () => {
    const data = await runAlgorithms();
    if (data) {
        bfTimeSpan.textContent = `${data.time_bruteforce.toFixed(4)} ms`;
        resultMessage.textContent = 'Brute Force calculation complete.';
    }
});

dncBtn.addEventListener('click', async () => {
    const data = await runAlgorithms();
    if (data) {
        dncTimeSpan.textContent = `${data.time_dnc.toFixed(4)} ms`;
        resultMessage.textContent = 'Divide & Conquer calculation complete.';
    }
});

showClosestBtn.addEventListener('click', () => {
    if (!closestPairData) return;

    const pair = closestPairData.closest_pair_dnc.pair; // Both should be same
    const dist = closestPairData.closest_pair_dnc.distance;

    drawPoints(); // Redraw to clear previous highlights
    
    // Highlight closest pair
    drawPoint(pair[0][0], pair[0][1], HIGHLIGHT_COLOR, 6);
    drawPoint(pair[1][0], pair[1][1], HIGHLIGHT_COLOR, 6);
    drawLine(pair[0], pair[1]);

    minDistSpan.textContent = dist.toFixed(2);
    resultMessage.textContent = `Closest pair found! Distance: ${dist.toFixed(2)}`;
});

// Initial load
window.addEventListener('load', () => {
    // Optional: Auto-generate on load
    generateBtn.click();
});

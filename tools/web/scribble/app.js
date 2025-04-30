/**
 * Author: Paul Wareing
 * Version: 1.0
 */

const canvas = document.getElementById('scribbleCanvas');
const ctx = canvas.getContext('2d');

canvas.width = window.innerWidth;
canvas.height = window.innerHeight;

let drawing = false;
let currentContacts = {};

function getPointerColor(pointerType) {
    switch (pointerType) {
        case 'mouse':
            return 'blue';
        case 'touch':
            return 'green';
        case 'pen':
            return 'red';
        default:
            return 'black';
    }
}

canvas.addEventListener('pointerdown', (event) => {
    drawing = true;
    const contactId = event.pointerId;
    currentContacts[contactId] = {
        x: event.clientX,
        y: event.clientY,
        color: getPointerColor(event.pointerType)
    };
});

canvas.addEventListener('pointermove', (event) => {
    if (!drawing) return;

    const contactId = event.pointerId;
    if (currentContacts[contactId]) {
        const prevX = currentContacts[contactId].x;
        const prevY = currentContacts[contactId].y;

        currentContacts[contactId].x = event.clientX;
        currentContacts[contactId].y = event.clientY;

        ctx.lineWidth = event.pointerType === 'pen' ? Math.max(1, event.pressure * 20) : 2; // Adjust width for pen pressure
        ctx.lineWidth = event.pointerType === 'touch' ? Math.max(1, event.radiusX * 2) : 2; // Adjust width for touch size
        ctx.lineCap = 'round';
        ctx.strokeStyle = currentContacts[contactId].color;

        ctx.beginPath();
        ctx.moveTo(prevX, prevY);
        ctx.lineTo(event.clientX, event.clientY);
        ctx.stroke();
    }
});

canvas.addEventListener('pointerup', (event) => {
    delete currentContacts[event.pointerId];
    if (Object.keys(currentContacts).length === 0) {
        drawing = false;
    }
});

canvas.addEventListener('pointerleave', () => {
    drawing = false;
    currentContacts = {};
});